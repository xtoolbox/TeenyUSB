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
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
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

#include "tusbh_cdc_rndis.h"
#include "string.h"
#define KEEP_ALIVE_FREQ   20

typedef struct _tusbh_cdc_rndis_info
{
    tusbh_ep_info_t* ep_in;
    tusbh_ep_info_t* ep_out;
    tusbh_ep_info_t* ep_notify;
    uint32_t         last_oid;
    uint32_t         request_id;
    uint8_t          enc_buffer[256];
    uint32_t         oid_packet_filter;
    uint8_t          tx_buffer[2048];
    uint8_t          rx_buffer[2048];
    uint8_t          mac[6];
    uint8_t          link_status;
    uint8_t          keep_alive;
}tusbh_cdc_rndis_info_t;

static int tusbh_rndis_set_enc_data(tusbh_interface_t* interface)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    uint32_t len = ((rndis_generic_msg_t*)info->enc_buffer)->MessageLength;
    int r = tusbh_control_xfer(
       interface->device,
       USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_SEND_ENCAPSULATED_COMMAND,
       0,
       interface->desc->bInterfaceNumber,
       info->enc_buffer, len);
    if(r < 0){
        TUSB_ITF_INFO("CDC RNDIS Fail set command\n");
    }
    return r;
}

static int tusbh_rndis_get_enc_data(tusbh_interface_t* interface)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    int r = tusbh_control_xfer(
       interface->device,
       USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_GET_ENCAPSULATED_RESPONSE,
       0,
       interface->desc->bInterfaceNumber,
       info->enc_buffer, sizeof(info->enc_buffer));
    if(r < 0){
        TUSB_ITF_INFO("CDC RNDIS Fail get response\n");
    }
    return r;
}

int tusbh_rndis_send_message(tusbh_interface_t* interface, rndis_generic_msg_t* msg)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    if( (void*)msg != (void*)info->enc_buffer){
        memcpy(info->enc_buffer, msg, msg->MessageLength);
    }
    return tusbh_rndis_set_enc_data(interface);
}

static int prepare_tx_packet(tusbh_interface_t* interface, void* data, uint32_t len)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    rndis_data_packet_t* hdr = (rndis_data_packet_t*)info->tx_buffer;
    memset(hdr, 0, sizeof(rndis_data_packet_t));
    hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
    hdr->MessageLength = sizeof(rndis_data_packet_t) + len;
    hdr->DataOffset = sizeof(rndis_data_packet_t) - 8;
    hdr->DataLength = len;
    if(  info->tx_buffer +  sizeof(rndis_data_packet_t) != (uint8_t*)data ){
        memcpy(info->tx_buffer + sizeof(rndis_data_packet_t), data, len);
    }
    return hdr->MessageLength;
}

int tusbh_rndis_send_packet(tusbh_interface_t* interface, void* data, uint32_t len, uint32_t timeout)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    rndis_data_packet_t* hdr = (rndis_data_packet_t*)info->tx_buffer;
    memset(hdr, 0, sizeof(rndis_data_packet_t));
    hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
    hdr->MessageLength = sizeof(rndis_data_packet_t) + len;
    hdr->DataOffset = sizeof(rndis_data_packet_t) - 8;
    hdr->DataLength = len;
    if(  info->tx_buffer +  sizeof(rndis_data_packet_t) != (uint8_t*)data ){
        memcpy(info->tx_buffer + sizeof(rndis_data_packet_t), data, len);
    }
    return tusbh_ep_xfer(info->ep_out, hdr, hdr->MessageLength, timeout);
}

int tusbh_rndis_recv_packet(tusbh_interface_t* interface, void* data, uint32_t len, uint32_t timeout)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    int r = tusbh_ep_xfer(info->ep_in, info->rx_buffer, sizeof(info->rx_buffer), timeout);
    if(r < 0) return r;
    rndis_data_packet_t* hdr = (rndis_data_packet_t*)info->rx_buffer;
    if(hdr->MessageType != REMOTE_NDIS_PACKET_MSG){
        return -1;
    }
    uint32_t recv_len = hdr->MessageLength - sizeof(rndis_data_packet_t);
    if(hdr->DataLength != recv_len){
        return -1;
    }
    if(  info->rx_buffer +  sizeof(rndis_data_packet_t) != (uint8_t*)data ){
        if(len >= recv_len){
            memcpy(data, info->rx_buffer + sizeof(rndis_data_packet_t), recv_len);
        }
    }
    return recv_len;
}

int tusbh_rndis_send_packet_with_event(tusbh_interface_t* interface, void* data, uint32_t len, tusbh_xfered_set_event_t* action, uint32_t timeout)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    int size = prepare_tx_packet(interface, data, len);
    return tusbh_ep_xfer_with_event(info->ep_out, info->tx_buffer, size, action, timeout);
}

int tusbh_rndis_recv_packet_with_event(tusbh_interface_t* interface, void* data, uint32_t len, tusbh_xfered_set_event_t* action, uint32_t timeout)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    int recv_len = tusbh_ep_xfer_with_event(info->ep_in, info->rx_buffer, sizeof(info->rx_buffer), action, timeout);
    if(recv_len <= 0) return recv_len;
    
    if(!interface->info_pool) return -1;
    rndis_data_packet_t* hdr = (rndis_data_packet_t*)info->rx_buffer;
    if(hdr->MessageType != REMOTE_NDIS_PACKET_MSG){
        return -1;
    }
    recv_len = hdr->MessageLength - sizeof(rndis_data_packet_t);
    if(hdr->DataLength != recv_len){
        return -1;
    }
    if(  info->rx_buffer +  sizeof(rndis_data_packet_t) != (uint8_t*)data ){
        if(len >= recv_len){
            memcpy(data, info->rx_buffer + sizeof(rndis_data_packet_t), recv_len);
        }
    }
    return (int)recv_len;
}

int tusbh_rndis_start_recv_packet(tusbh_interface_t* interface, tusbh_xfered_set_event_t* action)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    return tusbh_ep_xfer_with_event(info->ep_in, info->rx_buffer, sizeof(info->rx_buffer), action, 0);
}

int tusbh_rndis_keepalive(tusbh_interface_t* interface)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    rndis_keepalive_msg_t* msg = (rndis_keepalive_msg_t*)info->enc_buffer;
    info->request_id++;
    msg->MessageType     = REMOTE_NDIS_KEEPALIVE_MSG;
	msg->MessageLength   = sizeof(rndis_keepalive_msg_t);
	msg->RequestId       = info->request_id;
    return tusbh_rndis_set_enc_data(interface);
}

static int tusbh_rndis_init(tusbh_interface_t* interface)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    rndis_initialize_msg_t* msg = (rndis_initialize_msg_t*)info->enc_buffer;
    info->request_id++;
    msg->MessageType    = REMOTE_NDIS_INITIALIZE_MSG;
	msg->MessageLength  = sizeof(rndis_initialize_msg_t);
	msg->RequestId      = info->request_id;
	msg->MajorVersion   = RNDIS_MAJOR_VERSION;
	msg->MinorVersion   = RNDIS_MINOR_VERSION;
	msg->MaxTransferSize = sizeof(info->tx_buffer);
    return tusbh_rndis_set_enc_data(interface);
}

static int tusbh_rndis_set(tusbh_interface_t* interface, uint32_t oid, void* data, int len)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    info->last_oid = oid;
    info->request_id++;
    rndis_set_msg_t* msg = (rndis_set_msg_t*)info->enc_buffer;
	msg->MessageType    = REMOTE_NDIS_SET_MSG;
	msg->MessageLength  = sizeof(rndis_query_msg_t);
	msg->RequestId      = info->request_id;
    msg->Oid            = oid;
    msg->InformationBufferLength = len;
    msg->InformationBufferOffset = sizeof(rndis_set_msg_t) - 8;
    msg->DeviceVcHandle = 0;
    memcpy(msg+1, data, len);
    return tusbh_rndis_set_enc_data(interface);
}

static int tusbh_rndis_query(tusbh_interface_t* interface, uint32_t oid)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    info->last_oid = oid;
    info->request_id++;
    rndis_query_msg_t* msg = (rndis_query_msg_t*)info->enc_buffer;
	msg->MessageType    = REMOTE_NDIS_QUERY_MSG;
	msg->MessageLength  = sizeof(rndis_query_msg_t);
	msg->RequestId      = info->request_id;
    msg->Oid            = oid;
    msg->InformationBufferLength = 0;
    msg->InformationBufferOffset = 0;
    msg->DeviceVcHandle = 0;
    return tusbh_rndis_set_enc_data(interface);
}

// default init sequence handler, start with REMOTE_NDIS_INITIALIZE_MSG message
static int tusbh_rndis_default_init_handler(tusbh_interface_t* interface)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    rndis_generic_msg_t* msg = (rndis_generic_msg_t*)info->enc_buffer;
    switch(msg->MessageType){
        case REMOTE_NDIS_INITIALIZE_CMPLT:{
            rndis_initialize_cmplt_t* ini = (rndis_initialize_cmplt_t*)msg;
            TUSB_ITF_INFO("RNDIS Ver %d.%d, Medium: %d, xfer size: %d x %d\n", (int)ini->MajorVersion, (int)ini->MinorVersion,
            (int)ini->Medium, (int)ini->MaxTransferSize, (int)ini->MaxPacketsPerTransfer);
            if(ini->MaxTransferSize > sizeof(info->rx_buffer)){
                TUSB_ITF_INFO("RNDIS transfer size error %d vs %d", (int)ini->MaxTransferSize, (int)sizeof(info->rx_buffer));
            }
            // next get vendor description
            tusbh_rndis_query(interface, OID_GEN_VENDOR_DESCRIPTION);
            break;
        }
        case REMOTE_NDIS_QUERY_CMPLT:{
            rndis_query_cmplt_t* c = (rndis_query_cmplt_t*)msg;
            switch(info->last_oid){
                case OID_GEN_VENDOR_DESCRIPTION:{
                    info->enc_buffer[c->MessageLength] = 0;
                    TUSB_ITF_INFO("RNDIS Vendor %s\n", (char*)info->enc_buffer + sizeof(rndis_query_cmplt_t) );
                    // next get MAC address
                    tusbh_rndis_query(interface, OID_802_3_CURRENT_ADDRESS);
                    break;
                }
                case OID_802_3_CURRENT_ADDRESS:{
                    memcpy(info->mac, info->enc_buffer + sizeof(rndis_query_cmplt_t), 6);
                    TUSB_ITF_INFO("RNDIS MAC %02x-%02x-%02x-%02x-%02x-%02x\n", 
                    info->mac[0],info->mac[1],info->mac[2],info->mac[3],info->mac[4],info->mac[5]);
                    // next set filter
                    info->oid_packet_filter = 1;
                    tusbh_rndis_set(interface, OID_GEN_CURRENT_PACKET_FILTER, &info->oid_packet_filter, 4);
                    break;
                }
                default:{
                    TUSB_ITF_INFO("RNDIS Un-process Query Response messsage 0x%x\n", (int)info->last_oid);
                    break;
                }
            }
            break;
        }
        case REMOTE_NDIS_SET_CMPLT:{
            rndis_set_cmplt_t* c = (rndis_set_cmplt_t*)msg;
            if(info->last_oid == OID_GEN_CURRENT_PACKET_FILTER){
                if(c->Status == RNDIS_STATUS_SUCCESS){
                    // now the RNDIS device is ready to use
                    TUSB_ITF_INFO("RNDIS Set filter success\n");
                }else{
                    TUSB_ITF_INFO("RNDIS Set filter fail, 0x%x\n", (int)c->Status);
                }
            }else{
                TUSB_ITF_INFO("RNDIS Un-process Set Response messsage 0x%x\n", (int)info->last_oid);
            }
            break;
        }
        case REMOTE_NDIS_KEEPALIVE_CMPLT:{
            rndis_keepalive_cmplt_t* c = (rndis_keepalive_cmplt_t*)msg;
            info->keep_alive = 0;
            if(c->Status == RNDIS_STATUS_SUCCESS){
            }else if(c->Status == RNDIS_STATUS_MEDIA_DISCONNECT){
                info->link_status = 0;
            }
            break;
        }
        default:{
            TUSB_ITF_INFO("RNDIS Un-process Response messsage 0x%x\n", (int)msg->MessageType);
            break;
        }
                    
    }
    return 0;
}

static int tusbh_cdc_rndis_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    interface->ep_num = 0;
    create_info_pool(interface, tusbh_cdc_rndis_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_cdc_rndis_info_t));
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    
    memset(interface->endpoints, 0, sizeof(interface->endpoints));
    
    while(cfg_offset < wTotalLength){
        uint8_t len = dev->config_desc[cfg_offset];
        uint8_t t = dev->config_desc[cfg_offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+cfg_offset);
            if(ep_index < TUSBH_MAX_EP){
                setup_ep_info(&interface->endpoints[ep_index], ep, interface);
                if(ep->bEndpointAddress & 0x80){
                    if( (ep->bmAttributes & 0x03) == USB_EP_INTERRUPT){
                        if(!info->ep_notify){
                            info->ep_notify = &interface->endpoints[ep_index];
                        }
                    }
                    if( (ep->bmAttributes & 0x03) == USB_EP_BULK){
                        if(!info->ep_in){
                            info->ep_in = &interface->endpoints[ep_index];
                        }
                    }
                }else{
                    if(!info->ep_out){
                        info->ep_out = &interface->endpoints[ep_index];
                    }
                }
            }else{
                TUSB_ITF_INFO("Endpoint count large than TUSBH_MAX_EP in HUB interface\n");
            }
            ep_index++;
        }else if(t == USB_INTERFACE_DESCRIPTOR_TYPE){
            if(itf_cnt > 1){
                // CDC class need two interface
                break;
            }
            usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+cfg_offset);
            interface->ep_num += itf->bNumEndpoints;
            itf_cnt++;
        }else if(t == USB_IAD_DESCRIPTOR_TYPE){
            if(itf_cnt > 0){
                break;
            }
        }
        cfg_offset+=len;
    }
    if(ep_index != interface->ep_num){
        TUSB_ITF_INFO("Endpoint count error espect %d, got %d\n", interface->ep_num, ep_index);
        goto error;
    }
    if(!info->ep_in || !info->ep_out){
        TUSB_ITF_INFO("Fail to get rndis endpoint\n");
        goto error;
    }
    
    if( (info->ep_in->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK){
        TUSB_ITF_INFO("CDC RNDIS in ep attr error %d\n", info->ep_in->desc->bmAttributes);
        goto error;
    }
    
    if( (info->ep_out->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK){
        TUSB_ITF_INFO("CDC RNDIS out ep attr error %d\n", info->ep_out->desc->bmAttributes);
        goto error;
    }
    if(info->ep_notify){
        if( (info->ep_notify->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR){
            TUSB_ITF_INFO("CDC RNDIS notify ep attr error %d\n", info->ep_notify->desc->bmAttributes);
            goto error;
        }
    }
    
    // allocate a buffer for the notify ep, so it will start automatically
    if(info->ep_notify){
        info->ep_notify->data = tusbh_malloc( EP_MPS(info->ep_notify->desc) );
        if(!info->ep_notify->data){
            TUSB_ITF_INFO("Fail to allocate memory for CDC RNDIS notify endpoint\n");
            goto error;
        }
        
        if(tusbh_ep_allocate_pipe(info->ep_notify) < 0){
            TUSB_ITF_INFO("Fail to allocate pipe for CDC RNDIS notify endpoint\n");
            goto error;
        }
    }
    
    if(tusbh_ep_allocate_pipe(info->ep_in)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for CDC RNDIS in\n");
        goto error;
    }
    if(tusbh_ep_allocate_pipe(info->ep_out)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for CDC RNDIS out\n");
        goto error;
    }
    
    if( ((tusbh_cdc_rndis_class_t*)(interface->cls))->mount){
        if( ((tusbh_cdc_rndis_class_t*)(interface->cls))->mount(interface) ){
            goto error;
        }
    }
    
    tusbh_rndis_init(interface);
    
    info->keep_alive = 0;
    
error:
    TUSB_ITF_INFO("CDC RNDIS interface init\n");
    TUSB_ITF_INFO("CDC RNDIS Ep in %02x, out %02x\n",
        info->ep_in->desc->bEndpointAddress,
        info->ep_out->desc->bEndpointAddress);
    if(info->ep_notify){
        TUSB_ITF_INFO("CDC RNDIS Notify EP %02x\n", info->ep_notify->desc->bEndpointAddress);
    }
    
    return cfg_offset;
}

static int tusbh_cdc_rndis_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    TUSB_ITF_INFO("CDC RNDIS interface deinit\n");
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(interface, tusbh_cdc_rndis_info_t);
    
    if( ((tusbh_cdc_rndis_class_t*)(interface->cls))->unmount){
        ((tusbh_cdc_rndis_class_t*)(interface->cls))->unmount(interface);
    }
    
    if(info->ep_notify){
        if(info->ep_notify->data){
            tusbh_free(info->ep_notify->data);
            info->ep_notify->data = 0;
        }
        tusbh_ep_free_pipe(info->ep_notify);
    }
    if(info->ep_in){
        if(info->ep_in->data){
            tusbh_free(info->ep_in->data);
            info->ep_in->data = 0;
        }
        tusbh_ep_free_pipe(info->ep_in);
    }
    if(info->ep_out){
        if(info->ep_out->data){
            tusbh_free(info->ep_out->data);
            info->ep_out->data = 0;
        }
        tusbh_ep_free_pipe(info->ep_out);
    }
    
    tusbh_free_info(interface);
    return 0;
}

static int cdc_rndis_xfered(tusbh_ep_info_t* ep)
{
    tusbh_cdc_rndis_info_t* info = tusbh_get_info(ep->interface, tusbh_cdc_rndis_info_t);
    if(ep != info->ep_notify){
        return 0;
    }
    tusb_hc_data_t* hc = &ep_host(ep)->hc[ep->pipe_num];
    if(hc->state != TUSB_CS_TRANSFER_COMPLETE){
        return -1;
    }
    if( tusbh_rndis_get_enc_data(ep->interface)<0 ){
        return -1;
    }
    if(ep_class(ep,tusbh_cdc_rndis_class_t)->on_response){
        if(ep_class(ep,tusbh_cdc_rndis_class_t)->on_response(ep->interface, (rndis_generic_msg_t*)info->enc_buffer)){
            return 0;
        }
    }
    tusbh_rndis_default_init_handler(ep->interface);
    return 0;
}

const tusbh_interface_backend_t  tusbh_cdc_rndis_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_CDC,
    .bInterfaceSubClass = 2,
    .bInterfaceProtocol = 0xff,
    .init = tusbh_cdc_rndis_init,
    .deinit = tusbh_cdc_rndis_deinit,
    .data_xfered = cdc_rndis_xfered,
    .desc = "CDC RNDIS class",
};

