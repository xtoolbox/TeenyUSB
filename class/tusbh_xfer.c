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

#include "teeny_usb.h"
#include "teeny_usb_util.h"
#include <string.h>
#include "tusbh_xfer.h"


#define  USB_REQ_DIR_MASK                                  0x80U
#define  USB_H2D                                           0x00U
#define  USB_D2H                                           0x80U

#define TUSB_XFER_LOGD(fmt, ...)  TUSB_LOGD("[XFER] " fmt, ##__VA_ARGS__)
#define TUSB_XFER_LOGW(fmt, ...)  TUSB_LOGW("[XFER] " fmt, ##__VA_ARGS__)
#define TUSB_XFER_LOGE(fmt, ...)  TUSB_LOGE("[XFER] " fmt, ##__VA_ARGS__)

static void xfer_complete_set_event(tusbh_transfer_t* xfer)
{
    tusb_ev_t* ev = (tusb_ev_t*)xfer->context;
    tusb_ev_set(ev);
}

tusb_transfer_state_t tusbh_transaction(tusbh_transfer_t* xfer, tusbh_device_t* dev, void* data, uint32_t length, uint8_t is_setup, uint8_t ep_num, uint8_t ep_type)
{
    // TODO: check the device parent, to enable split transfer
    tusb_transfer_state_t res = TS_IDLE;
    memset(xfer, 0, sizeof(tusbh_transfer_t));
    xfer->data = data;
    xfer->total_length = length;
    xfer->context = tusb_ev_create();
    if(!xfer->context){
        TUSB_XFER_LOGE("Fail to create transaction event\n");
        return TS_ERROR;
    }
    xfer->cb_complete = xfer_complete_set_event;
    xfer->is_setup = is_setup;
    
    xfer->dev_addr = dev->address;
    xfer->dev_speed = dev->speed;
    xfer->ep_num = ep_num;
    xfer->ep_type = ep_type;
    xfer->ep_mps = dev->device_desc.bMaxPacketSize;
    xfer->state = TS_IDLE;
    
    if(tusb_host_transfer_add(dev->host, xfer) < 0){
        res = TS_NO_CHANNEL;
    }else{
        if(tusb_ev_wait((tusb_ev_t*)xfer->context, 5000) < 0){
            TUSB_XFER_LOGE("Transaction event timeout\n");
            res = TS_TIMEOUT;
        }else{
            res = xfer->state;
        }
    }
    tusb_host_transfer_remove(dev->host, xfer);
    tusb_ev_delete((tusb_ev_t*)xfer->context);
    xfer->context = 0;
    xfer->cb_complete = 0;
    return res;
}

int tusbh_control_xfer(tusbh_device_t* dev, uint8_t bmRequest, uint8_t bRequest, uint32_t value, uint32_t index, void* data, uint32_t len)
{
    tusb_setup_packet setup;
    setup.bmRequestType = bmRequest;
    setup.bRequest = bRequest;
    setup.wValue = value;
    setup.wIndex = index;
    setup.wLength = len;
    
    tusbh_transfer_t  xfer;
    tusb_transfer_state_t state;
    state = tusbh_transaction(&xfer, dev, &setup, sizeof(setup), 1, 0x00, USBD_EP_TYPE_CTRL);
    if(state != TS_ACK){
        TUSB_XFER_LOGD("Setup stage fail %d\n", state);
        return -1;
    }
    
    if(bmRequest & USB_D2H){
        // data IN
        state = tusbh_transaction(&xfer, dev, data, len, 0, 0x80, USBD_EP_TYPE_CTRL);
        if( state != TS_ACK){
            TUSB_XFER_LOGD("Data IN stage fail, %d\n", state);
            return -1;
        }
        // status out
        state = tusbh_transaction(&xfer, dev, data, 0, 0, 0x00, USBD_EP_TYPE_CTRL);
        if( state != TS_ACK){
            TUSB_XFER_LOGD("Status OUT stage fail, %d\n", state);
            return -1;
        }
    }else{
        // data OUT
        if(data && len){
            state = tusbh_transaction(&xfer, dev, data, len, 0, 0x00, USBD_EP_TYPE_CTRL);
            if( state != TS_ACK){
                TUSB_XFER_LOGD("Data OUT stage fail, %d\n", state);
                return -1;
            }
        }
        // status in
        state = tusbh_transaction(&xfer, dev, data, 0, 0, 0x80, USBD_EP_TYPE_CTRL);
        if( state != TS_ACK){
            TUSB_XFER_LOGD("Data IN stage fail, %d\n", state);
            return -1;
        }
    }
    return 0;
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

int tusbh_clear_ep_feature(tusbh_device_t* dev, uint8_t ep_num)
{
    return tusbh_control_xfer(
        dev,
        USB_H2D | USB_REQ_RECIPIENT_ENDPOINT | USB_REQ_TYPE_STANDARD,
        USB_REQ_CLEAR_FEATURE,
        USB_FEATURE_EP_HALT,
        ep_num,
        0, 0);
}


struct _tusbh_periodic_xfer
{
    tusbh_periodic_xfer_t* next;
    tusbh_ep_info_t*       ep;
    tusbh_transfer_t       xfer;
    void*                  buffer;
    int                    buffer_len;
    uint32_t               remain_interval;
    uint32_t               interval;
    uint16_t               busy_count;
    uint16_t               busy_max;
};

static uint32_t get_interval(tusbh_ep_info_t* ep)
{
    // only interrupt and isoc ep will enter here
    if(ep->interface->device->speed == PORT_SPEED_HIGH || (!(ep->desc->bmAttributes & 0x02)) ){
        // high speed or full speed ISOC
        if(ep->desc->bInterval > 0 && ep->desc->bInterval <= 16){
            return 1<<(ep->desc->bInterval-1);
        }
    }else{
        if(ep->desc->bInterval){
            return ep->desc->bInterval;
        }
    }
    TUSB_XFER_LOGE("Device(%d) Ep(0x%02x) interval(%d) is wrong\n", 
                ep->interface->device->address, ep->desc->bEndpointAddress, ep->desc->bInterval);
    return 0;
}



static void periodic_xfer_complete(tusbh_transfer_t* xfer)
{
    tusbh_periodic_xfer_t* p_xfer = (tusbh_periodic_xfer_t*)xfer->context;
    p_xfer->busy_count = 0;
    if(xfer->state == TS_ACK){
        if(p_xfer->ep->interface->cls && p_xfer->ep->interface->cls->backend->data_xfered){
            p_xfer->ep->interface->cls->backend->data_xfered(p_xfer->ep, p_xfer->buffer, xfer->actual_length);
        }else{
            TUSB_XFER_LOGD("device(%d) ep(0x%02x) got %ld bytes periodic data\n", 
                ep_device(p_xfer->ep)->address, p_xfer->ep->desc->bEndpointAddress, xfer->actual_length);
        }
    }
}

static int start_periodic_xfer(tusb_host_t* host, tusbh_periodic_xfer_t* p_xfer)
{
    // TODO: check the device parent, to enable split transfer
    if(p_xfer->busy_count){
        p_xfer->busy_count++;
        if(p_xfer->busy_max < p_xfer->busy_count){
            p_xfer->busy_max = p_xfer->busy_count;
        }
        if(p_xfer->busy_count > 500){
            TUSB_XFER_LOGE("Periodic xfer busy more than 500 times\n");
            p_xfer->busy_count = 1;
        }
        return -1;
    }
    tusb_transfer_state_t res = TS_IDLE;
    tusbh_transfer_t* xfer = &p_xfer->xfer;
    xfer->data = p_xfer->buffer;
    xfer->total_length = p_xfer->buffer_len;
    xfer->context = p_xfer;
    xfer->cb_complete = periodic_xfer_complete;
    xfer->is_setup = 0;
    
    xfer->dev_addr = ep_device(p_xfer->ep)->address;
    xfer->dev_speed = ep_device(p_xfer->ep)->speed;
    xfer->ep_num = p_xfer->ep->desc->bEndpointAddress;
    xfer->ep_type = p_xfer->ep->desc->bmAttributes & USBD_EP_TYPE_MASK;
    xfer->ep_mps = EP_MPS(p_xfer->ep->desc);
    xfer->state = TS_IDLE;
    p_xfer->busy_count = 1;
    if(tusb_host_transfer_add(host, xfer) < 0){
        TUSB_XFER_LOGE("No channel for periodic xfer\n");
        res = TS_NO_CHANNEL;
    }
    return res;
}


int tusbh_handle_periodic_xfer(tusb_host_t* host)
{
    tusbh_periodic_xfer_t* p_xfer = host->periodic_queue;
    uint16_t cur_frame = (uint16_t)tusb_host_get_frame_number(host);
    uint32_t cost = (uint32_t)((cur_frame - host->last_frame) & 0x3fff);
    host->last_frame = cur_frame;
    while(p_xfer){
        if(p_xfer->remain_interval < cost){
            if(cost > p_xfer->interval){
                p_xfer->remain_interval = p_xfer->interval;
            }else{
                p_xfer->remain_interval = p_xfer->remain_interval + p_xfer->interval - cost;
            }
            start_periodic_xfer(host, p_xfer);
        }else{
            p_xfer->remain_interval -= cost;
        }
        p_xfer = p_xfer->next;
    }
    return 0;
}

int tusbh_init_periodic_xfer(tusbh_ep_info_t* ep)
{
    tusb_host_t* host = ep->interface->device->host;
    uint32_t interval = get_interval(ep);
    if(!interval){
        TUSB_XFER_LOGE("Periodic xfer interval error\n");
        return -1;
    }
    tusbh_periodic_xfer_t* p_xfer = (tusbh_periodic_xfer_t*)tusb_malloc(sizeof(tusbh_periodic_xfer_t));
    if(!p_xfer){
        TUSB_XFER_LOGE("Fail to allocate periodic xfer\n");
        return -1;
    }
    p_xfer->next = 0;
    p_xfer->ep = ep;
    p_xfer->busy_count = 0;
    p_xfer->busy_max = 0;
    memset(&p_xfer->xfer, 0, sizeof(tusbh_transfer_t));
    p_xfer->remain_interval = interval;
    p_xfer->interval = interval;
    p_xfer->buffer_len = EP_MPS(ep->desc);
    p_xfer->buffer = tusb_malloc( EP_MPS(ep->desc));
    if(!p_xfer->buffer){
        TUSB_XFER_LOGE("Fail to allocate periodic xfer buffer\n");
        tusb_free(p_xfer);
        return -1;
    }
    p_xfer->next = host->periodic_queue;
    if(!host->periodic_queue){
        host->last_frame = (uint16_t)tusb_host_get_frame_number(host);
    }
    host->periodic_queue = p_xfer;
    return 0;
}

int tusbh_deinit_periodic_xfer(tusbh_ep_info_t* ep)
{
    tusb_host_t* host = ep->interface->device->host;
    tusbh_periodic_xfer_t** p_xfer = &host->periodic_queue;
    while(*p_xfer){
        tusbh_periodic_xfer_t* t = (*p_xfer);
        if(t->ep == ep){
            *p_xfer = t->next;
            tusb_host_transfer_remove(host, &t->xfer);
            if(t->buffer){
                tusb_free(t->buffer);
            }
            tusb_free(t);
            return 0;
        }
        p_xfer = &(t->next);
    }
    TUSB_XFER_LOGW("Periodic endpoint not found in periodic queue\n");
    return 0;
}
