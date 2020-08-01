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

#include "teeny_usb.h"
#include <string.h>
#include "tusbh.h"

static void tusbh_msg_root_connected(tusbh_message_t* msg);
static void tusbh_msg_root_disconnected(tusbh_message_t* msg);
static void tusbh_msg_root_enable(tusbh_message_t* msg);
static void tusbh_msg_root_disable(tusbh_message_t* msg);
static void tusbh_msg_periodic_hander(tusbh_message_t* msg);

void tusb_host_port_changed(tusb_host_t* host, uint8_t port, host_port_state_t new_state)
{
    // root hub port state changed
    // for STM32 there is only one port for each root hub
    (void)host;
    (void)port;
    (void)new_state;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    (void)root;
    if(new_state == TUSB_HOST_PORT_CONNECTED){
        root->conn_mask |= 1<<0;
        POST_MESSAGE(root->mq, tusbh_msg_root_connected, 0, host, 0);
    }else if(new_state == TUSB_HOST_PORT_ENABLED){
        root->en_mask |= 1<<0;
        POST_MESSAGE(root->mq, tusbh_msg_root_enable, 0, host, 0);
    }else if(new_state == TUSB_HOST_PORT_DISCONNECTED){
        root->conn_mask &= ~(1<<0);
        POST_MESSAGE(root->mq, tusbh_msg_root_disconnected, 0, host, 0);
    }else{
        root->en_mask &= ~(1<<0);
        POST_MESSAGE(root->mq, tusbh_msg_root_disable, 0, host, 0);
    }
}

void tusb_host_sof_event(tusb_host_t* host)
{
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    if(!root->sof_in_progress){
        root->sof_in_progress = 1;
        POST_MESSAGE(root->mq, tusbh_msg_periodic_hander, 0, host, 0);
    }
}

const char* channelState(channel_state_t state)
{
#define SSS(name)  case name: return #name;
    switch(state){
        SSS( TUSB_CS_INIT                  )
        SSS( TUSB_CS_TRANSFER_COMPLETE     )
        SSS( TUSB_CS_NAK                   )
        SSS( TUSB_CS_ACK                   )
        SSS( TSUB_CS_PING_SUCCESS          )
        SSS( TUSB_CS_NYET                  )
        SSS( TUSB_CS_STALL                 )
        SSS( TUSB_CS_INT_NAK               )
        SSS( TUSB_CS_AHB_ERROR             )
        SSS( TUSB_CS_DT_ERROR              )
        SSS( TUSB_CS_TRANSACTION_ERROR     )
        SSS( TUSB_CS_FRAMEOVERRUN_ERROR    )
        SSS( TUSB_CS_BABBLE_ERROR          )
        SSS( TUSB_CS_XFER_ONGOING          )
        SSS( TUSB_CS_XFER_CANCEL           )
        SSS( TUSB_CS_UNKNOWN_ERROR         )
    }
    return "Unknown Channel State";
#undef SSS
}

int tusbh_allocate_pipe(tusbh_device_t* dev, uint8_t ep, uint8_t type, uint16_t mps)
{
    tusb_pipe_t pipe;
    int r = tusb_pipe_open(dev->host, &pipe, dev->address, ep, type, mps, dev->speed);
    if(r < 0) return r;
    return pipe.hc_num;
}

int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num)
{
    tusb_hc_data_t* hc = &host->hc[hc_num];
    if(hc->xfer_done){
        tusbh_xfered_action_t* action = (tusbh_xfered_action_t*)hc->user_data;
        if(action && action->func){
            action->func(host, hc_num, action);
        }
    }
    return 0;
}

static void tusbh_xfered_set_event_handler(tusb_host_t* host, uint8_t hc_num, tusbh_xfered_set_event_t* data)
{
    if(data->event){
        tusbh_evt_set(data->event);
    }
}

void tusbh_close_pipe(tusbh_device_t* dev, int pipe_num)
{
    tusb_pipe_t pipe = {
        .host = dev->host,
        .hc_num = (uint8_t)pipe_num,
    };
    
#if 0
    tusb_hc_data_t* hc = &dev->host->hc[pipe_num];
    
    hc->user_data = &dev->xfer_evt;
    
    tusb_pipe_cancel(&pipe);
    int res = tusbh_evt_wait(dev->xfer_evt.event, timeout);
    hc->user_data = 0;
    
    if(res != 0){
        TUSB_DEV_INFO("Fail to close the pipe\n");
    }
#endif
    
    tusb_pipe_close(&pipe);
}

static channel_state_t tusbh_pipe_xfer_and_wait(tusbh_device_t* dev, int pipe_num, uint8_t is_data, void* data, uint16_t len, uint32_t timeout)
{   
    void* bak = dev->host->hc[pipe_num].user_data;
    dev->host->hc[pipe_num].user_data = &dev->xfer_evt;
    
    tusb_host_xfer_data(dev->host, pipe_num, is_data, data, len);
    
    channel_state_t r = TUSB_CS_UNKNOWN_ERROR;
    
    //int res = 
    tusbh_evt_wait(dev->xfer_evt.event, timeout);
    
    //if(res == 0){
        r = (channel_state_t)dev->host->hc[pipe_num].state;
    //}
    dev->host->hc[pipe_num].user_data = bak;
    
    return r;
}

int tusbh_ep_xfer(tusbh_ep_info_t* ep, void* data, uint16_t len, uint32_t timeout)
{
    return tusbh_ep_xfer_with_event(ep, data, len, &ep_device(ep)->xfer_evt, timeout);
}

static int tusbh_ep_in_xfer_with_event(tusbh_ep_info_t* ep, void* data, uint16_t len, tusbh_xfered_set_event_t* action, uint32_t timeout)
{
    void* bak = ep_host(ep)->hc[ep->pipe_num].user_data;
    ep_host(ep)->hc[ep->pipe_num].user_data = action;
    uint16_t remain = len;
    uint8_t* p = (uint8_t*)data;
    int res = 0;
    do{
        uint16_t xfer_len = EP_MPS(ep->desc);
        if(xfer_len > remain){
            xfer_len = remain;
        }
        tusb_host_xfer_data(ep_host(ep), ep->pipe_num, 1, p, xfer_len);
        tusbh_evt_wait(action->event, timeout);
        tusb_hc_data_t* hc = &ep_device(ep)->host->hc[ep->pipe_num];
        channel_state_t s = (channel_state_t)hc->state;
        if(s != TUSB_CS_TRANSFER_COMPLETE){
            res = -(int)s;
            goto error;
        }
        remain -= hc->count;
        p += hc->count;
        if(hc->count != xfer_len){
            // short packet
            break;
        }
    }while(remain);
    res = p - ((uint8_t*)data);
error:
    ep_host(ep)->hc[ep->pipe_num].user_data = bak;
    return res;
    
    
    //tusb_host_xfer_data(ep_host(ep), ep->pipe_num, 1, data, len);
    //tusbh_evt_wait(action->event, timeout);
    //tusb_hc_data_t* hc = &ep_device(ep)->host->hc[ep->pipe_num];
    //channel_state_t s = (channel_state_t)hc->state;
    //if(s != TUSB_CS_TRANSFER_COMPLETE){
    //    return -s;
    //}
    //return hc->count;
}

static int tusbh_ep_out_xfer_with_event(tusbh_ep_info_t* ep, void* data, uint16_t len, tusbh_xfered_set_event_t* action, uint32_t timeout)
{
    void* bak = ep_host(ep)->hc[ep->pipe_num].user_data;
    ep_host(ep)->hc[ep->pipe_num].user_data = action;
    uint16_t remain = len;
    uint8_t* p = (uint8_t*)data;
    int res = 0;
    do{
        uint16_t xfer_len;
        if(remain > EP_MPS(ep->desc)){
            xfer_len = EP_MPS(ep->desc);
        }else{
            xfer_len = remain;
        }
        tusb_host_xfer_data(ep_host(ep), ep->pipe_num, 1, p, xfer_len);
        tusbh_evt_wait(action->event, timeout);
        tusb_hc_data_t* hc = &ep_device(ep)->host->hc[ep->pipe_num];
        channel_state_t s = (channel_state_t)hc->state;
        if(s != TUSB_CS_TRANSFER_COMPLETE){
            res = -(int)s;
            goto error;
        }
        if(hc->count != xfer_len){
            res = -1;
            goto error;
        }
        remain -= xfer_len;
        p += xfer_len;
    }while(remain);
    res = len;
error:
    ep_host(ep)->hc[ep->pipe_num].user_data = bak;
    return res;
}

int tusbh_ep_xfer_with_event(tusbh_ep_info_t* ep, void* data, uint16_t len, tusbh_xfered_set_event_t* action, uint32_t timeout)
{
    if(ep->desc->bEndpointAddress & 0x80){
        return tusbh_ep_in_xfer_with_event(ep, data, len, action, timeout);
    }else{
        return tusbh_ep_out_xfer_with_event(ep, data, len, action, timeout);
    }
}

int tusbh_ep_clear_feature(tusbh_ep_info_t* ep)
{
    return tusbh_control_xfer(
        ep_device(ep),
        USB_H2D | USB_REQ_RECIPIENT_ENDPOINT | USB_REQ_TYPE_STANDARD,
        USB_REQ_CLEAR_FEATURE,
        USB_FEATURE_EP_HALT,
        ep->desc->bEndpointAddress,
        0, 0);
}

#define SETUP_DELAY  1000

int tusbh_control_xfer(tusbh_device_t* dev, uint8_t bmRequest, uint8_t bRequest, uint32_t value, uint32_t index, void* data, uint32_t len)
{
    int free_ctrl_in = 0;
    int free_ctrl_out = 0;
    int res = 0;
    if(dev->ctrl_in<0){
        free_ctrl_in = 1;
        dev->ctrl_in = tusbh_allocate_pipe(dev, 0x80, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    }
    if(dev->ctrl_out<0){
        free_ctrl_out = 1;
        dev->ctrl_out = tusbh_allocate_pipe(dev, 0x00, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    }
    
    TUSB_ASSERT( dev && dev->host && dev->ctrl_in>=0 && dev->ctrl_out>=0);
    channel_state_t state;

    tusb_setup_packet setup;
    setup.bmRequestType = bmRequest;
    setup.bRequest = bRequest;
    setup.wValue = value;
    setup.wIndex = index;
    setup.wLength = len;
    state = tusbh_pipe_xfer_and_wait(dev, dev->ctrl_out, 0, &setup, sizeof(setup), SETUP_DELAY);
    if( state != TUSB_CS_TRANSFER_COMPLETE){
        TUSB_DEV_INFO("Setup stage fail, %s\n", channelState(state));
        res = -1;
        goto error;
    }

    if(bmRequest & USB_D2H){
        // data IN
        state = tusbh_pipe_xfer_and_wait(dev, dev->ctrl_in, 1, data, len, SETUP_DELAY);
        if( state != TUSB_CS_TRANSFER_COMPLETE){
            TUSB_DEV_INFO("Data IN stage fail, %s\n", channelState(state));
            res = -1;
            goto error;
        }
        // status out
        state = tusbh_pipe_xfer_and_wait(dev, dev->ctrl_out, 1, 0, 0, SETUP_DELAY);
        if( state != TUSB_CS_TRANSFER_COMPLETE){
            TUSB_DEV_INFO("Status OUT stage fail, %s\n", channelState(state));
            res = -1;
            goto error;
        }
    }else{
        // data OUT
        if(data && len){
            state = tusbh_pipe_xfer_and_wait(dev, dev->ctrl_out, 1, data, len, SETUP_DELAY);
            if( state != TUSB_CS_TRANSFER_COMPLETE){
                TUSB_DEV_INFO("Data OUT stage fail, %s\n", channelState(state));
                res = -1;
                goto error;
            }
        }
        // status in
        state = tusbh_pipe_xfer_and_wait(dev, dev->ctrl_in, 1, 0, 0, SETUP_DELAY);
        if( state != TUSB_CS_TRANSFER_COMPLETE){
            TUSB_DEV_INFO("Data IN stage fail, %s\n", channelState(state));
            res = -1;
            goto error;
        }
    }
error:
    if(free_ctrl_in && dev->ctrl_in > 0){
        tusbh_close_pipe(dev, dev->ctrl_in);
        dev->ctrl_in = -1;
    }
    if(free_ctrl_out && dev->ctrl_out > 0){
        tusbh_close_pipe(dev, dev->ctrl_out);
        dev->ctrl_out = -1;
    }
    
    return res;
}

int tusbh_get_descriptor(tusbh_device_t* dev, uint8_t type, uint8_t index, void* data, int len)
{
    return tusbh_control_xfer(
        dev,
        USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
        USB_REQ_GET_DESCRIPTOR,
        (type<<8) | index,
        0,
        data,
        len
    );
}

int tusbh_set_address(tusbh_device_t* dev, uint8_t addr)
{
    return tusbh_control_xfer(
        dev,
        USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
        USB_REQ_SET_ADDRESS,
        addr,
        0,
        0,
        0
    );
}

int tusbh_set_config(tusbh_device_t* dev, uint8_t config)
{
    return tusbh_control_xfer(
        dev,
        USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
        USB_REQ_SET_CONFIGURATION,
        config,
        0,
        0,
        0
    );
}


static uint8_t alloc_addr(tusb_host_t* host)
{
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    uint32_t t = 1;
    uint8_t addr = 1;
    while(t && (root->addr_mask & t) ){
        t<<=1;
        addr++;
    }
    if(t){
        root->addr_mask |= t;
        return addr;
    }
    TUSB_HOST_INFO("Error: Fail to get free addr\n");
    return 0;
}

static void free_addr(tusb_host_t* host, uint8_t addr)
{
    if(addr == 0 || addr > 32){
        TUSB_HOST_INFO("Error: Device address out of range\n");
        return;
    }
    uint32_t t = 1<<(addr-1);
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    if(root->addr_mask &t){
        root->addr_mask ^= t;
    }else{
        TUSB_HOST_INFO("Warn: Device address not allocate\n");
    }
}

static tusbh_class_reg_t tusbh_find_class(tusbh_device_t* dev, uint8_t bClass, uint8_t bSubClass, uint8_t bProtocol)
{
    const tusbh_class_reg_t* p = dev_root(dev)->support_classes;
    if(!p){
      return 0;
    }
    tusbh_class_reg_t reg;
    while( (reg = *p++) != 0 ){
        const tusbh_interface_backend_t* t = reg->backend;
        if(  (t->vid == 0 || t->vid == dev->device_desc.idVendor)
          && (t->pid == 0 || t->pid == dev->device_desc.idProduct)
          && (t->bInterfaceClass == 0 || t->bInterfaceClass == bClass)
          && (t->bInterfaceSubClass == 0 || t->bInterfaceSubClass == bSubClass)
          && (t->bInterfaceProtocol == 0 || t->bInterfaceProtocol == bProtocol)
         ){
             return reg;
         }
    }
    return 0;
}

// get the interface's endpoint
static int tusbh_get_interface_ep(tusbh_device_t* dev, tusbh_interface_t* interface, int offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    if(itf->bDescriptorType == USB_IAD_DESCRIPTOR_TYPE){
        offset += itf->bLength;
        itf = (usb_interface_descriptor_t*)(dev->config_desc+offset);
    }
    if(itf->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE){
        TUSB_DEV_INFO("Error: Wrong interface desc type %d\n", itf->bDescriptorType);
        return -1;
    }
    offset += itf->bLength;
    while(offset < wTotalLength){
        uint8_t len = dev->config_desc[offset];
        uint8_t t = dev->config_desc[offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+offset);
            if(ep_index < TUSBH_MAX_EP){
                if(interface){
                    setup_ep_info(&interface->endpoints[ep_index], ep, interface);
                }
            }else{
                TUSB_ITF_INFO("Endpoint count large than TUSBH_MAX_EP (%d), ignore it\n", TUSBH_MAX_EP);
            }
            ep_index++;
        }
        if(t == USB_INTERFACE_DESCRIPTOR_TYPE || t == USB_IAD_DESCRIPTOR_TYPE){
            break;
        }
        offset+=len;
    }
    if(ep_index != itf->bNumEndpoints){
        TUSB_ITF_INFO("Endpoint count error espect %d, got %d\n", itf->bNumEndpoints, ep_index);
    }
    if(interface){
        interface->ep_num = itf->bNumEndpoints;
        TUSB_ITF_INFO("Unknown interface (%02x,%02x,%02x) %d eps, init\n", 
            itf->bInterfaceClass, itf->bInterfaceSubClass, itf->bInterfaceProtocol, itf->bNumEndpoints);
        for(uint8_t i=0;i<ep_index;i++){
            usb_endpoint_descriptor_t* desc = interface->endpoints[i].desc;
            TUSB_ITF_INFO("  Ep  addr:%02x, attr:%02x, mps:%d\n", desc->bEndpointAddress, desc->bmAttributes, EP_MPS(desc));
        }
    }
    
    return offset;
}

static int tusbh_parse_config(tusbh_device_t* dev)
{
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    int offset = sizeof(usb_config_descriptor_t);
    int r = 0;
    dev->interface_num = 0;
    
    while(offset < wTotalLength){
        uint8_t bClass, bSubClass, bProtocol;
        uint8_t t = dev->config_desc[offset+1];
        usb_interface_descriptor_t* itf;
        if(t == USB_INTERFACE_DESCRIPTOR_TYPE){
            itf = (usb_interface_descriptor_t*)(dev->config_desc+offset);
            bClass = itf->bInterfaceClass;
            bSubClass = itf->bInterfaceSubClass;
            bProtocol = itf->bInterfaceProtocol;
            if(itf->bAlternateSetting > 0){
                TUSB_DEV_INFO("Warning: interface alternate setting (%d > 0) is not support, ignore it\n", itf->bAlternateSetting);
                r = tusbh_get_interface_ep(dev, 0, offset);
                if(r < 0){ return r; }
                offset = r;
                continue;
            }
        }else if(t == USB_IAD_DESCRIPTOR_TYPE){
            usb_interface_association_descriptor_t* iad = (usb_interface_association_descriptor_t*)(dev->config_desc+offset);
            bClass = iad->bFunctionClass;
            bSubClass = iad->bFunctionSubClass;
            bProtocol = iad->bFunctionProtocol;
            itf = (usb_interface_descriptor_t*)(dev->config_desc+offset+iad->bLength);
        }else{
            TUSB_DEV_INFO("Error: interface descriptor type %d, offset=%d\n", dev->config_desc[offset+1], offset+1);
            return -1;
        }
        if(dev->interface_num>=TUSBH_MAX_INTERFACE){
            TUSB_DEV_INFO("Warning: interface count large than TUSBH_MAX_INTERFACE (%d), ignore it\n", TUSBH_MAX_INTERFACE);
            r = tusbh_get_interface_ep(dev, 0, offset);
            if(r < 0){ return r; }
            offset = r;
            continue;
        }
        tusbh_class_reg_t cls = tusbh_find_class(dev, bClass, bSubClass, bProtocol);
        tusbh_interface_t* dev_itf = &dev->interfaces[dev->interface_num];
        dev_itf->cls = cls;
        dev_itf->device = dev;
        dev_itf->desc = itf;
        dev_itf->interface_id = dev->interface_num;
        if(cls){
            r = cls->backend->init(dev, dev_itf, offset);
        }else{
            r = tusbh_get_interface_ep(dev, dev_itf, offset);
        }
        if(r<0){
            TUSB_DEV_INFO("Fail to init interface %d\n", dev->interface_num);
            return -1;
        }
        dev->interface_num++;
        offset = r;
    }
    TUSB_DEV_INFO("Device has %d interfaces\n", dev->interface_num);
    return 0;
}

static const char* device_speed_str(uint8_t speed)
{
    switch(speed){
        case 0: return "HS";
        case 1: return "FS";
        case 2: return "LS";
    }
    return "?S";
}

// a new device attached, enum it's type
int tusbh_device_attach(tusbh_device_t* dev)
{
    TUSB_ASSERT(dev && dev->host);
    int res = 0;
    uint8_t addr;
    usb_config_descriptor_t* cfg;
    dev->address = 0;
    dev->ctrl_in = tusbh_allocate_pipe(dev, 0x80, EP_TYPE_CTRL, 8);
    dev->ctrl_out = tusbh_allocate_pipe(dev, 0x00, EP_TYPE_CTRL, 8);
    
    TUSB_DEV_INFO("Allocate pipe, in = %d, out = %d\n", dev->ctrl_in, dev->ctrl_out);
    if(dev->ctrl_in<0 || dev->ctrl_out<0){
        TUSB_DEV_INFO("Fail to allocate pipe for Control\n");
        res = -1;
        goto error;
    }
    
    dev->xfer_evt.func = tusbh_xfered_set_event_handler;
    dev->xfer_evt.event = tusbh_evt_create();
    if(!dev->xfer_evt.event){
        TUSB_DEV_INFO("Fail to allocate device control event\n");
        res = -1;
        goto error;
    }

    res = tusbh_get_descriptor(dev, USB_DEVICE_DESCRIPTOR_TYPE, 0, &dev->device_desc, 8);
    if(res != 0){
        TUSB_DEV_INFO("Fail to get device descriptor\n");
        goto error;
    }
    addr = alloc_addr(dev->host);
    if(!addr){
        TUSB_DEV_INFO("Fail to allocate device address\n");
        goto error;
    }

    res = tusbh_set_address(dev, addr);
    if(res != 0){
        TUSB_DEV_INFO("Fail to set device address\n");
        goto error;
    }
    

    tusbh_close_pipe(dev, dev->ctrl_in);
    tusbh_close_pipe(dev, dev->ctrl_out);

    dev->address = addr;

    dev->ctrl_in = tusbh_allocate_pipe(dev, 0x80, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    dev->ctrl_out = tusbh_allocate_pipe(dev, 0x00, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    
    TUSB_DEV_INFO("Re-allocate pipe, in = %d, out = %d\n", dev->ctrl_in, dev->ctrl_out);

    res = tusbh_get_descriptor(dev, USB_DEVICE_DESCRIPTOR_TYPE, 0, &dev->device_desc, sizeof(dev->device_desc));
    if(res != 0){
        TUSB_DEV_INFO("Fail to get full device descriptor\n");
        goto error;
    }

    TUSB_DEV_INFO("%s device, VID:%04x PID:%04x\n", device_speed_str(dev->speed), dev->device_desc.idVendor, dev->device_desc.idProduct);
    
    if(dev->device_desc.bNumConfigurations > 1){
        TUSB_DEV_INFO("TeenyUSB only support the first configuration, total = %d\n", dev->device_desc.bNumConfigurations);
    }

    res = tusbh_get_descriptor(dev, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, dev->config_desc, 9);
    if(res != 0){
        TUSB_DEV_INFO("Fail to get config descriptor\n");
        goto error;
    }
    cfg = (usb_config_descriptor_t*)dev->config_desc;
    if( cfg->wTotalLength> sizeof(dev->config_desc)){
        TUSB_DEV_INFO("Config descriptor longer than buffer\n");
        goto error;
    }

    res = tusbh_get_descriptor(dev, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, dev->config_desc, cfg->wTotalLength);
    if(res != 0){
        TUSB_DEV_INFO("Fail to get full config descriptor\n");
        goto error;
    }

    dev->config = 1;
    res = tusbh_set_config(dev, 1);

    if(res != 0){
        TUSB_DEV_INFO("Error: Fail to set config\n");
        goto error;
    }
    
    res = tusbh_parse_config(dev);

    if(res != 0){
        TUSB_DEV_INFO("Error: Fail to parse config descriptor\n");
        goto error;
    }
   
    if(dev_root(dev)->cb_device_ready){
        dev_root(dev)->cb_device_ready(dev);
    }
    
    //TUSB_DEV_INFO("Device attached addr=%d\n", dev->address);
    
    if(dev->ctrl_in>=0){ tusbh_close_pipe(dev, dev->ctrl_in); }
    if(dev->ctrl_out>=0){ tusbh_close_pipe(dev, dev->ctrl_out); }
    dev->ctrl_in = dev->ctrl_out = -1;
    return res;
error:
    if(dev->ctrl_in>=0){ tusbh_close_pipe(dev, dev->ctrl_in); }
    if(dev->ctrl_out>=0){ tusbh_close_pipe(dev, dev->ctrl_out); }
    dev->ctrl_in = dev->ctrl_out = -1;
    
    if(dev->xfer_evt.event){
        tusbh_evt_free(dev->xfer_evt.event);
        dev->xfer_evt.event = 0;
    }
        
    if(dev->address){
        free_addr(dev->host, dev->address);
        dev->address = 0;
    }
    
    return res;
}

int tusbh_device_deinit(tusbh_device_t* dev)
{
    for(int i=0;i<TUSBH_MAX_CHILD;i++){
        if(dev->children[i]){
            tusbh_device_deinit(dev->children[i]);
            dev->children[i] = 0;
        }
    }
    
    if(dev_root(dev)->cb_device_remove){
        dev_root(dev)->cb_device_remove(dev);
    }
    
    for(uint8_t i=0;i<dev->interface_num;i++){
        
        if(dev->interfaces[i].cls && dev->interfaces[i].cls->backend->deinit){
            dev->interfaces[i].cls->backend->deinit(dev, &dev->interfaces[i]);
            dev->interfaces[i].cls = 0;
        }
        
        for(uint32_t j=0;j<dev->interfaces[i].ep_num;j++){
            tusbh_ep_info_t* ep = &dev->interfaces[i].endpoints[j];
            if(ep->pipe_num>=0){
                TUSB_EP_INFO("Warning: pipe leak %d\n", ep->pipe_num);
                tusbh_ep_free_pipe(ep);
            }
        }
    }
    
    if(dev->xfer_evt.event){
        tusbh_evt_free(dev->xfer_evt.event);
        dev->xfer_evt.event = 0;
    }
    
    if(dev->address){
        free_addr(dev->host, dev->address);
        dev->address = 0;
    }
    tusbh_free_device(dev);
    return 0;
}


void tusb_host_init(tusb_host_t* host, tusbh_root_hub_t* root_hub_info)
{
    root_hub_info->addr_mask = 0;
    root_hub_info->sof_in_progress = 0;
    host->user_data = (void*)root_hub_info;
}

static void tusbh_msg_root_connected(tusbh_message_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->data;
    uint8_t port = msg->param;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    if(port<TUSHH_ROOT_CHILD_COUNT){
        tusbh_device_t* dev = root->children[port];
        if(dev){
            if(root->en_mask & (1<<port)){
                TUSB_ROOT_INFO("Already enabled\n");
                return;
            }
        }else{
            dev = tusbh_new_device();
            root->children[port] = dev;
        }
        if(!dev){
            TUSB_ROOT_INFO("No Device memory\n");
        }
        TUSB_ROOT_INFO("Connect\n");
        tusb_delay_ms(200);
        tusb_port_set_reset(host, port, 1);
        tusb_delay_ms(100);
        tusb_port_set_reset(host, port, 0);
        tusb_delay_ms(10);
    }
}

static void tusbh_msg_root_disconnected(tusbh_message_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->data;
    uint8_t port = msg->param;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    
    // free all child on root device
    if(port<TUSHH_ROOT_CHILD_COUNT){
        if(root->children[port]){
            tusbh_device_deinit(root->children[port]);
            root->children[port] = 0;
        }else{
            TUSB_ROOT_INFO("Device not exsit\n");
        }
    }else{
        TUSB_ROOT_INFO("Wrong Port DISCONNECT\n");
    }
    
    TUSB_ROOT_INFO("Disconnect\n");
#if defined(USB_OTG_HS)
    if(GetUSB(host) == USB_OTG_HS){
        // TODO: some un-document feature for HS core
        // close the high speed core when root device disconnected
        //TUSB_DEV_INFO("Close the HS core\n");
        tusb_close_host(host);
        tusb_delay_ms(100);
        //TUSB_DEV_INFO("Re-open the HS core\n");
        tusb_open_host(host);
        tusb_delay_ms(50);
    }
#endif
}

static void tusbh_msg_root_enable(tusbh_message_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->data;
    uint8_t port = msg->param;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    TUSB_ROOT_INFO("Enable\n");
    if(port<TUSHH_ROOT_CHILD_COUNT){
        tusbh_device_t* dev = root->children[port];
        if(!dev){
            TUSB_ROOT_INFO("Device not exist\n");
        }else{
            int res;
            dev->host = host;
            dev->speed = tusb_port_get_speed(host, port);
            tusb_delay_ms(100);
            res = tusbh_device_attach(dev);
            if(res!=0){
                TUSB_ROOT_INFO("Device attach failed\n");
            }
        }
    }else{
        TUSB_ROOT_INFO("Wrong Port ENABLE\n");
    }
}

static void tusbh_msg_root_disable(tusbh_message_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->data;
    uint8_t port = (uint8_t)msg->param;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    (void)port;
    (void)root;
    TUSB_ROOT_INFO("Disable\n");
}

static void tusbh_ep_data_xfered(tusbh_message_t* msg)
{
    uint8_t hc_num = (uint8_t)msg->param;
    tusbh_xfered_notify_ep_t* data = (tusbh_xfered_notify_ep_t*) msg->data;
    tusbh_ep_info_t* ep = data->ep;
    tusb_hc_data_t* hc = &ep->interface->device->host->hc[hc_num];
    ep->data_len = hc->count;
    if(ep->interface->cls && ep->interface->cls->backend->data_xfered){
        ep->interface->cls->backend->data_xfered(ep);
    }
    ep->xfer_in_progress = 0;
    //tusbh_ep_free_pipe(ep);
}

static void tusbh_xfered_notify_ep_handler(tusb_host_t* host, uint8_t hc_num, tusbh_xfered_notify_ep_t* data)
{
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    POST_MESSAGE(root->mq, tusbh_ep_data_xfered, hc_num, data, 0);
}

void tusbh_ep_free_pipe(tusbh_ep_info_t* ep)
{
    if(ep->pipe_num>=0){
        tusbh_close_pipe(ep->interface->device, ep->pipe_num);
        ep->pipe_num = -1;
    }
}

int tusbh_ep_allocate_pipe(tusbh_ep_info_t* ep)
{
    uint16_t mps = EP_MPS(ep->desc);
    tusbh_device_t* dev = ep->interface->device;
    if(ep->pipe_num >= 0){
        ep->pipe_leak++;
        if(ep->pipe_leak == 10){
            TUSB_EP_INFO("Pipe leak detected\n");
        }
        if(ep->pipe_leak > 12){
            ep->pipe_leak = 12;
        }
        tusbh_ep_free_pipe(ep);
    }
    
    int pipe_num = tusbh_allocate_pipe(
        dev,
        ep->desc->bEndpointAddress,
        ep->desc->bmAttributes & USBD_EP_TYPE_MASK,
        mps
    );
    
    if(pipe_num<0){
        TUSB_EP_INFO("Fail to allocate pipe\n");
        return -1;
    }
    
    ep->pipe_num = (int8_t)pipe_num;
    ep->ep_notify.func = tusbh_xfered_notify_ep_handler;
    ep->ep_notify.ep = ep;
    dev->host->hc[pipe_num].user_data = &ep->ep_notify;
    
    return pipe_num;
    
}

static void start_period_in(tusbh_device_t* dev, tusbh_ep_info_t* ep)
{
    //if(tusbh_ep_allocate_pipe(ep) >= 0){
    if(ep->pipe_num >= 0){
        uint16_t mps = EP_MPS(ep->desc);
        ep->xfer_in_progress = 1;
        tusb_host_xfer_data(dev->host, ep->pipe_num, 1, ep->data, mps);
    }
}

static void process_period_ep(tusbh_device_t* dev)
{
    for(uint8_t i=0;i<dev->interface_num;i++){
        tusbh_interface_t* itf = &dev->interfaces[i];
        for(uint32_t j=0;j<itf->ep_num;j++){
            tusbh_ep_info_t* ep = &itf->endpoints[j];
            if( (ep->xfer_in_progress == 0) 
             && (ep->desc->bmAttributes & 1)
             && (ep->desc->bEndpointAddress & 0x80)
             && (ep->data) ){
                // periodic IN endpoint
                if(ep->remain_interval){
                    ep->remain_interval--;
                }
                if(!ep->remain_interval){
                    ep->remain_interval = ep->desc->bInterval;
                    start_period_in(dev, ep);
                }
            }
        }
    }
    for(int i=0;i<TUSBH_MAX_CHILD;i++){
        if(dev->children[i]){
            process_period_ep(dev->children[i]);
        }
    }
}

static void tusbh_msg_periodic_hander(tusbh_message_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->data;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    for(int i=0;i<TUSHH_ROOT_CHILD_COUNT;i++){
        if(root->children[i] && (root->en_mask & (1<<i)) ){
            process_period_ep(root->children[i]);
        }
    }
    if(root->cb_sof){
        root->cb_sof(host);
    }
    root->sof_in_progress = 0;
}

void tusbh_msg_loop(tusbh_msg_q_t* mq)
{
    tusbh_message_t msg;
    if(tusbh_mq_get(mq, &msg)){
        if(msg.handler){
            msg.handler(&msg);
        }else{
            TUSB_OS_INFO("Unknown message handler\n");
        }
    }
}

int tusbh_test_parse_config(const void* cfg, int len)
{
    tusbh_root_hub_t root = {
        .support_classes = 0,
        .id = "FS",
    };
    tusb_host_t* host = tusb_get_host(0);
    tusb_host_init(host, &root);
    tusbh_device_t* dev = tusbh_new_device();
    dev->host = host;
    memcpy(dev->config_desc, cfg, len);
    return tusbh_parse_config(dev);
}

static void ls_device(const tusbh_device_t* dev, int port, const tusbh_device_t* parent)
{
    if(parent){
        TUSB_PRINTF("  Device VID:%04x PID:%04x, Parent: Dev %02x:%d\n", 
            dev->device_desc.idVendor, dev->device_desc.idProduct, parent->address, port);
    }else{
        TUSB_PRINTF("  Device VID:%04x PID:%04x, Parent: ROOT %s:%d\n", 
            dev->device_desc.idVendor, dev->device_desc.idProduct, dev_root(dev)->id, port);
    }
    for(int i=0;i<dev->interface_num;i++){
        const tusbh_interface_t* itf = &dev->interfaces[i];
        if(itf->cls && itf->cls->backend ){
            if(itf->cls->backend->desc){
                TUSB_PRINTF("    Interface %d: %s\n", itf->desc->bInterfaceNumber, itf->cls->backend->desc);
            }else{
                TUSB_PRINTF("    Interface %d: Backend(%p)\n", itf->desc->bInterfaceNumber, itf->cls->backend);
            }
        }else{
            TUSB_PRINTF("    Interface %d: Unknown type\n", itf->desc->bInterfaceNumber);
        }
    }
    
    for(int i=0;i<TUSBH_MAX_CHILD;i++){
        tusbh_device_t* child = dev->children[i];
        if(child){
            ls_device(child, i+1, dev);
        }
    }
}

void ls_usb(tusb_host_t* host)
{
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    TUSB_PRINTF("Device of %s root hub\n", root->id);
    for(int i=0;i<TUSHH_ROOT_CHILD_COUNT;i++){
        tusbh_device_t* dev = root->children[i];
        if(dev){
            ls_device(dev, i, 0);
        }
    }
}


