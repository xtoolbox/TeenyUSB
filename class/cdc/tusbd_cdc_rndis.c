/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for micro controllers
 * 
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "tusbd_cdc_rndis.h"
#include "teeny_usb_util.h"

const uint32_t OIDSupportedList[] = 
{
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_MAXIMUM_SEND_PACKETS,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_MAC_OPTIONS
};

STATIC_ASSERT( sizeof( ((tusb_rndis_device_t*)0)->encapsulated_buffer ) > sizeof(OIDSupportedList) + 32 )


static int tusb_rndis_device_init(tusb_rndis_device_t* cdc)
{
    tusb_set_recv_buffer(cdc->dev, cdc->ep_out, cdc->rx_buf, sizeof(cdc->rx_buf));
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    cdc->ep_int_busy = 0;
    if(cdc->ep_int && cdc->ep_int < TUSB_MAX_EP_PAIR_COUNT){
      tusb_device_config_t* dev_config = (tusb_device_config_t*)cdc->dev->user_data;
      dev_config->ep_in_interface[ cdc->ep_int-1 ] = (tusb_device_interface_t*)cdc;
    }
    if(!cdc->link_speed){
        cdc->link_speed = 12ul * 1000ul * 1000ul;
    }
    cdc->txok = 1;
    cdc->rxok = 1;
    cdc->txbad = 0;
    cdc->rxbad = 0;
    cdc->link_status = 1;
    return 0;
}

static void rndis_notify(tusb_rndis_device_t* cdc)
{
    memset(cdc->notify_buffer,0,8);
    cdc->notify_buffer[0] = 1;
    cdc->ep_int_busy = 1;
    tusb_send_data(cdc->dev, cdc->ep_int, cdc->notify_buffer, 8, 0);
}

static void rndis_query_complete(tusb_rndis_device_t* cdc, int status, const void *data, int size)
{
    rndis_query_cmplt_t *c = (rndis_query_cmplt_t *)cdc->encapsulated_buffer;
    c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
    c->MessageLength = sizeof(rndis_query_cmplt_t) + size;
    c->InformationBufferLength = size;
    c->InformationBufferOffset = 16;
    c->Status = status;
    memcpy(c + 1, data, size);
    rndis_notify(cdc);
}

#define rndis_query_cmplt(s,d,l)            \
do{                                         \
    rndis_query_complete(cdc, s, d, l);     \
}while(0)

#define rndis_query_cmplt32(s, d)            \
do{                                          \
    uint32_t dd = d;                         \
    rndis_query_complete(cdc, s, &dd, 4);    \
}while(0)

static const char def_vendor[] = "TeenyUSB";

static void rndis_query(tusb_rndis_device_t* cdc)
{
    switch (((rndis_query_msg_t *)cdc->encapsulated_buffer)->Oid)
    {
        case OID_GEN_SUPPORTED_LIST:         rndis_query_cmplt(RNDIS_STATUS_SUCCESS, OIDSupportedList, sizeof(OIDSupportedList)); return;
        case OID_GEN_VENDOR_DRIVER_VERSION:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00000200);  return;
        case OID_802_3_CURRENT_ADDRESS:      rndis_query_cmplt(RNDIS_STATUS_SUCCESS, cdc->mac_addr, 6); return;
        case OID_802_3_PERMANENT_ADDRESS:    rndis_query_cmplt(RNDIS_STATUS_SUCCESS, cdc->mac_addr, 6); return;
        case OID_GEN_MEDIA_SUPPORTED:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_MEDIA_IN_USE:           rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_PHYSICAL_MEDIUM:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_HARDWARE_STATUS:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_GEN_LINK_SPEED:             rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->link_speed / 100); return;
        case OID_GEN_VENDOR_ID:              rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00FFFFFF); return;
        case OID_GEN_VENDOR_DESCRIPTION:     rndis_query_cmplt(RNDIS_STATUS_SUCCESS, def_vendor, sizeof(def_vendor)); return;
        case OID_GEN_CURRENT_PACKET_FILTER:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->oid_packet_filter); return;
        case OID_GEN_MAXIMUM_FRAME_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE - ETH_HEADER_SIZE); return;
        case OID_GEN_MAXIMUM_TOTAL_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
        case OID_GEN_TRANSMIT_BLOCK_SIZE:    rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
        case OID_GEN_RECEIVE_BLOCK_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
        case OID_GEN_MEDIA_CONNECT_STATUS:   rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->link_status ? NDIS_MEDIA_STATE_CONNECTED : NDIS_MEDIA_STATE_DISCONNECTED); return;
        case OID_GEN_RNDIS_CONFIG_PARAMETER: rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_MAXIMUM_LIST_SIZE:    rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 1); return;
        case OID_802_3_MULTICAST_LIST:       rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0xE000000); return;
        case OID_802_3_MAC_OPTIONS:          rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_GEN_MAC_OPTIONS:            rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, /*MAC_OPT*/ 0); return;
        case OID_802_3_RCV_ERROR_ALIGNMENT:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_XMIT_ONE_COLLISION:   rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_XMIT_MORE_COLLISIONS: rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_GEN_XMIT_OK:                rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->txok); return;
        case OID_GEN_RCV_OK:                 rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->rxok); return;
        case OID_GEN_RCV_ERROR:              rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->rxbad); return;
        case OID_GEN_XMIT_ERROR:             rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, cdc->txbad); return;
        case OID_GEN_RCV_NO_BUFFER:          rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        default:                             rndis_query_cmplt(RNDIS_STATUS_FAILURE, NULL, 0); return;
    }
}

#define INFBUF ((uint32_t *)((uint8_t *)&(m->RequestId) + m->InformationBufferOffset))
static void rndis_handle_set_msg(tusb_rndis_device_t* cdc)
{
    rndis_set_cmplt_t *c;
    rndis_set_msg_t *m;
    rndis_Oid_t oid;

    c = (rndis_set_cmplt_t *)cdc->encapsulated_buffer;
    m = (rndis_set_msg_t *)cdc->encapsulated_buffer;

    /* Never have longer parameter names than PARM_NAME_LENGTH */
    /*
    char parmname[PARM_NAME_LENGTH+1];
    uint8_t i;
    int8_t parmlength;
    */

    /* The parameter name seems to be transmitted in uint16_t, but */
    /* we want this in uint8_t. Hence have to throw out some info... */

    /*
    if (CFGBUF->ParameterNameLength > (PARM_NAME_LENGTH*2))
    {
        parmlength = PARM_NAME_LENGTH * 2;
    }
    else
    {
        parmlength = CFGBUF->ParameterNameLength;
    }
    i = 0;
    while (parmlength > 0)
    {
        // Convert from uint16_t to char array. 
        parmname[i] = (char)*(PARMNAME + 2*i); // FSE! FIX IT!
        parmlength -= 2;
        i++;
    }
    */

    oid = m->Oid;
    c->MessageType = REMOTE_NDIS_SET_CMPLT;
    c->MessageLength = sizeof(rndis_set_cmplt_t);
    c->Status = RNDIS_STATUS_SUCCESS;

    switch (oid)
    {
        /* Parameters set up in 'Advanced' tab */
        case OID_GEN_RNDIS_CONFIG_PARAMETER:
            {
                rndis_config_parameter_t *p;
                char *ptr = (char *)m;
                ptr += sizeof(rndis_generic_msg_t);
                ptr += m->InformationBufferOffset;
                p = (rndis_config_parameter_t *)ptr;
                if(cdc->config_param){
                    cdc->config_param(cdc, ptr+p->ParameterNameOffset, ptr+p->ParameterValueOffset, p->ParameterNameLength, p->ParameterValueLength);
                }
            }
            break;

        /* Mandatory general OIDs */
        case OID_GEN_CURRENT_PACKET_FILTER:
            cdc->oid_packet_filter = *INFBUF;
            if (cdc->oid_packet_filter)
            {
                //rndis_packetFilter(cdc->oid_packet_filter);
                cdc->state = rndis_data_initialized;
            } 
            else 
            {
                cdc->state = rndis_initialized;
            }
            if(cdc->eth_linkchange){
                cdc->eth_linkchange(cdc, 1);
            }
            break;

        case OID_GEN_CURRENT_LOOKAHEAD:
            break;

        case OID_GEN_PROTOCOL_OPTIONS:
            break;

        /* Mandatory 802_3 OIDs */
        case OID_802_3_MULTICAST_LIST:
            break;

        /* Power Managment: fails for now */
        case OID_PNP_ADD_WAKE_UP_PATTERN:
        case OID_PNP_REMOVE_WAKE_UP_PATTERN:
        case OID_PNP_ENABLE_WAKE_UP:
        default:
            c->Status = RNDIS_STATUS_FAILURE;
            break;
    }

    /* c->MessageID is same as before */
    rndis_notify(cdc);
    return;
}

static int rndis_handle_msg(tusb_rndis_device_t* cdc)
{
    rndis_generic_msg_t * msg = (rndis_generic_msg_t *)cdc->encapsulated_buffer;
    switch(msg->MessageType){
        case REMOTE_NDIS_INITIALIZE_MSG:{
            rndis_initialize_cmplt_t *m;
            m = ((rndis_initialize_cmplt_t *)cdc->encapsulated_buffer);
            /* m->MessageID is same as before */
            m->MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
            m->MessageLength = sizeof(rndis_initialize_cmplt_t);
            m->MajorVersion = RNDIS_MAJOR_VERSION;
            m->MinorVersion = RNDIS_MINOR_VERSION;
            m->Status = RNDIS_STATUS_SUCCESS;
            m->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
            m->Medium = RNDIS_MEDIUM_802_3;
            m->MaxPacketsPerTransfer = 1;
            m->MaxTransferSize = RNDIS_RX_BUFFER_SIZE;
            m->PacketAlignmentFactor = 0;
            m->AfListOffset = 0;
            m->AfListSize = 0;
            cdc->state = rndis_initialized;
            rndis_notify(cdc);
            break;
        }
        case REMOTE_NDIS_HALT_MSG:{
            cdc->state = rndis_uninitialized;
            if(cdc->eth_linkchange){
                cdc->eth_linkchange(cdc, 0);
            }
            break;
        }
        case REMOTE_NDIS_QUERY_MSG:
            rndis_query(cdc);
            break;
        case REMOTE_NDIS_SET_MSG:
            rndis_handle_set_msg(cdc);
            break;
        case REMOTE_NDIS_RESET_MSG:{
            rndis_reset_cmplt_t * m;
            m = ((rndis_reset_cmplt_t *)cdc->encapsulated_buffer);
            cdc->state = rndis_uninitialized;
            m->MessageType = REMOTE_NDIS_RESET_CMPLT;
            m->MessageLength = sizeof(rndis_reset_cmplt_t);
            m->Status = RNDIS_STATUS_SUCCESS;
            m->AddressingReset = 1; /* Make it look like we did something */
            /* m->AddressingReset = 0; - Windows halts if set to 1 for some reason */
            rndis_notify(cdc);
            break;
        }
        case REMOTE_NDIS_KEEPALIVE_MSG:{
            rndis_keepalive_cmplt_t * m;
            m = (rndis_keepalive_cmplt_t *)cdc->encapsulated_buffer;
            m->MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
            m->MessageLength = sizeof(rndis_keepalive_cmplt_t);
            m->Status = RNDIS_STATUS_SUCCESS;
            rndis_notify(cdc);
            break;
        }
        default:
            break;
    }
    return 0;
}

static void rndis_dataout_request(tusb_device_t* dev, const void* data, int len)
{
    tusb_device_config_t* dev_config = (tusb_device_config_t*)dev->user_data;
    int ifn = dev->setup.wIndex;
    if(ifn<dev_config->if_count){
        tusb_rndis_device_t* cdc = (tusb_rndis_device_t*)dev_config->interfaces[ifn];
        rndis_handle_msg(cdc);
    }
    tusb_send_status(dev);
}

WEAK void tusb_fix_control_xfer_corrupt_issue(tusb_device_t* dev){}
static int tusb_rndis_device_request(tusb_rndis_device_t* cdc, tusb_setup_packet* setup_req)
{
    tusb_device_t* dev = cdc->dev;
    if( (setup_req->bmRequestType & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_CLASS){
        return 0;
    }
    if(setup_req->wLength == 0){ 
        // should never reach here
        tusb_send_status(dev);
        return 1;
    }
    if(setup_req->bRequest == CDC_GET_ENCAPSULATED_RESPONSE){
        tusb_fix_control_xfer_corrupt_issue(dev);
        tusb_control_send(dev, cdc->encapsulated_buffer, ((rndis_generic_msg_t *)cdc->encapsulated_buffer)->MessageLength );
        return 1;
    }else if(setup_req->bRequest == CDC_SEND_ENCAPSULATED_COMMAND){
        dev->ep0_rx_done = rndis_dataout_request;
        tusb_set_recv_buffer(dev, 0, cdc->encapsulated_buffer, setup_req->wLength);
        tusb_set_rx_valid(dev, 0);
        return 1;
    }
    return 0;
}

static int tusb_rndis_device_send_done(tusb_rndis_device_t* cdc, uint8_t EPn, const void* data, int len)
{
    if(EPn == cdc->ep_in){
        if(cdc->on_send_done){
            return cdc->on_send_done(cdc, data, len);
        }
    }else if(EPn == cdc->ep_int){
      cdc->ep_int_busy = 0;
    }
    return 0;
}

static int tusb_rndis_device_recv_done(tusb_rndis_device_t* cdc, uint8_t EPn, const void* data, int len)
{
    if(data == (const void*)cdc->rx_buf){
        rndis_data_packet_t* hdr = (rndis_data_packet_t*)cdc->rx_buf;
        if (len < sizeof(rndis_data_packet_t)) return 0;
        if (hdr->MessageType != REMOTE_NDIS_PACKET_MSG || hdr->MessageLength != len) return 0;
        if (hdr->DataOffset + 8 + hdr->DataLength != len) return 0;
        if(cdc->on_recv_data){
            return cdc->on_recv_data(cdc, cdc->rx_buf + hdr->DataOffset + 8, hdr->DataLength);
        }
    }
    return 0;
}

int tusb_rndis_send_packet(tusb_rndis_device_t* cdc, const void* data, int len)
{
    if(!cdc->link_status) return -1;
    if(len + sizeof(rndis_data_packet_t) > sizeof(cdc->tx_buf)) return -2;
    rndis_data_packet_t* hdr = (rndis_data_packet_t*)cdc->tx_buf;
    memset(hdr, 0, sizeof(rndis_data_packet_t));
    hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
    hdr->MessageLength = sizeof(rndis_data_packet_t) + len;
    hdr->DataOffset = sizeof(rndis_data_packet_t) - 8;
    hdr->DataLength = len;
    memcpy(cdc->tx_buf + sizeof(rndis_data_packet_t), data, len);
    return tusb_send_data(cdc->dev, cdc->ep_in, cdc->tx_buf, hdr->MessageLength, TUSB_TXF_ZLP);
}

const tusb_device_backend_t rndis_device_backend = {
    .device_init = (int(*)(tusb_device_interface_t*))tusb_rndis_device_init,
    .device_request = (int(*)(tusb_device_interface_t*, tusb_setup_packet*))tusb_rndis_device_request,
    .device_send_done = (int(*)(tusb_device_interface_t*, uint8_t, const void*, int))tusb_rndis_device_send_done,
    .device_recv_done = (int(*)(tusb_device_interface_t*, uint8_t, const void*, int))tusb_rndis_device_recv_done,
};

