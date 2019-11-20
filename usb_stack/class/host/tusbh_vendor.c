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

#include "tusbh_vendor.h"
#include "string.h"

typedef struct _tusbh_vendor_info
{
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t ep_in_type;
    uint8_t ep_out_type;
    uint16_t ep_in_mps;
    uint16_t ep_out_mps;
}tusbh_vendor_info_t;

static int tusbh_vendor_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+cfg_offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    interface->ep_num = itf->bNumEndpoints;
    create_info_pool(interface, tusbh_vendor_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_vendor_info_t));
    tusbh_vendor_info_t* info = tusbh_get_info(interface, tusbh_vendor_info_t);
    memset(interface->endpoints, 0, sizeof(interface->endpoints));
    
    while(cfg_offset < wTotalLength){
        uint8_t len = dev->config_desc[cfg_offset];
        uint8_t t = dev->config_desc[cfg_offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+cfg_offset);
            if(ep->bEndpointAddress & 0x80){
                if(!info->ep_in){
                    info->ep_in = ep->bEndpointAddress;
                    info->ep_in_type = ep->bmAttributes;
                    info->ep_in_mps = EP_MPS(ep);
                }
            }else{
                if(!info->ep_out){
                    info->ep_out = ep->bEndpointAddress;
                    info->ep_out_type = ep->bmAttributes;
                    info->ep_out_mps = EP_MPS(ep);
                }
            }
            if(ep_index < TUSBH_MAX_EP){
                setup_ep_info(&interface->endpoints[ep_index], ep, interface);
            }else{
                TUSB_ITF_INFO("Endpoint count large than TUSBH_MAX_EP in this interface\n");
            }
            ep_index++;
        }else if(t == USB_INTERFACE_DESCRIPTOR_TYPE){
            if(itf_cnt > 0){
                break;
            }
            itf_cnt++;
        }
        cfg_offset+=len;
    }
    if(ep_index != interface->ep_num){
        TUSB_ITF_INFO("Endpoint count error espect %d, got %d\n", interface->ep_num, ep_index);
        return -1;
    }
    TUSB_ITF_INFO("Vendor interface init\n");
    TUSB_ITF_INFO("Ep Info: %d eps, in_ep:[%02x,%d,%d], out_ep:[%02x,%d,%d],\n",
    ep_index, 
    info->ep_in, info->ep_in_type, info->ep_in_mps, 
    info->ep_out, info->ep_out_type, info->ep_out_mps);
    return cfg_offset;
}

static int tusbh_vendor_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    TUSB_ITF_INFO("Vendor interface deinit\n");
    tusbh_free_info(interface);
    return 0;
}

int tusbh_vendor_xfer_data(tusbh_ep_info_t* ep, void* data, uint32_t len)
{
    int pipe_num = ep->pipe_num;
    if(pipe_num<0){
        pipe_num = tusbh_ep_allocate_pipe(ep);
    }
    if( pipe_num < 0 ){
        return pipe_num;
    }
    tusb_host_xfer_data(ep_host(ep), pipe_num, 1, data, len);
    return 0;
}

WEAK int tusbh_vendor_xfer_done(tusbh_ep_info_t* ep, channel_state_t state)
{
    (void)ep;
    (void)state;
    return 0;
}

static int vendor_xfer_done(tusbh_ep_info_t* ep)
{
    channel_state_t state = TUSB_CS_UNKNOWN_ERROR;
    if(ep->pipe_num>=0){
        state = (channel_state_t)ep_host(ep)->hc[ep->pipe_num].state;
    }
    TUSB_ASSERT(ep->interface->cls->backend == &tusbh_vendor_backend);
    if( ((tusbh_vendor_class_t*)ep->interface->cls)->transfer_done){
        ((tusbh_vendor_class_t*)ep->interface->cls)->transfer_done(ep, state);
    }
    return 0;
}

const tusbh_interface_backend_t  tusbh_vendor_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = 0xff,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .init = tusbh_vendor_init,
    .deinit = tusbh_vendor_deinit,
    .data_xfered = vendor_xfer_done,
    .desc = "vendor class",
};

