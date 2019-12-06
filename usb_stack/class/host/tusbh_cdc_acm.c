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

#include "tusbh_cdc_acm.h"
#include "string.h"

typedef struct _tusbh_cdc_acm_info
{
    tusbh_ep_info_t* ep_in;
    tusbh_ep_info_t* ep_out;
    tusbh_ep_info_t* ep_notify;
    tusb_cdc_line_coding_t line_coding;
    uint8_t          padding;
    uint8_t          bmCapabilities;
}tusbh_cdc_acm_info_t;

static const char* line_coding_to_str(const tusb_cdc_line_coding_t* line_coding)
{
    static char res[16];
    sprintf(res, "%d ", (int)line_coding->bitrate);
    switch(line_coding->parity){
        case CDC_NONE  : strcat(res,"N"); break;
        case CDC_ODD   : strcat(res,"O"); break;
        case CDC_EVEN  : strcat(res,"E"); break;
        case CDC_MARK  : strcat(res,"M"); break;
        case CDC_SPACE : strcat(res,"S"); break;
        default:         strcat(res,"-"); break;
    }
    char bits[8];
    sprintf(bits, " %d ", line_coding->databits);
    strcat(res, bits);
    switch(line_coding->stopbits){
        case 1: strcat(res,"1.5"); break;
        case 2: strcat(res,"2"); break;
        default: strcat(res,"1"); break;
    }
    return res;
}

int tusbh_cdc_set_line_coding(tusbh_interface_t* interface, tusb_cdc_line_coding_t* line_coding)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    if( !(info->bmCapabilities & CDC_CAP_LINE) ){
        TUSB_ITF_INFO("Not support set line coding\n");
        return -1;
    }
    int r = tusbh_control_xfer(
       interface->device,
       USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_SET_LINE_CODING,
       0,
       interface->desc->bInterfaceNumber,
       line_coding, 7);
    if(r < 0){
        TUSB_ITF_INFO("Fail to set line coding\n");
    }else{
        TUSB_ITF_INFO("Set line coding %s\n", line_coding_to_str(line_coding));
    }
    return r;
}

int tusbh_cdc_get_line_coding(tusbh_interface_t* interface, tusb_cdc_line_coding_t* line_coding)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    if( !(info->bmCapabilities & CDC_CAP_LINE) ){
        TUSB_ITF_INFO("Not support get line coding\n");
        return -1;
    }
    int r = tusbh_control_xfer(
       interface->device,
       USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_GET_LINE_CODING,
       0,
       interface->desc->bInterfaceNumber,
       line_coding, 7);
    if(r < 0){
        TUSB_ITF_INFO("Fail to get line coding\n");
    }else{
        TUSB_ITF_INFO("Get line coding %s\n", line_coding_to_str(line_coding));
    }
    return r;
}

int tusbh_cdc_set_line_state(tusbh_interface_t* interface, uint8_t state)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    if( !(info->bmCapabilities & CDC_CAP_LINE) ){
        TUSB_ITF_INFO("Not support set control line state\n");
        return -1;
    }
    int r = tusbh_control_xfer(
       interface->device,
       USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_SET_CONTROL_LINE_STATE,
       state,
       interface->desc->bInterfaceNumber,
       0, 0);
    if(r < 0){
        TUSB_ITF_INFO("Fail to set line state\n");
    }else{
        TUSB_ITF_INFO("Set line state %02x\n", state);
    }
    return r;
}

int tusbh_cdc_set_break(tusbh_interface_t* interface, uint16_t duration_in_ms)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    if( !(info->bmCapabilities & CDC_CAP_BREAK) ){
        TUSB_ITF_INFO("Not support set break\n");
        return -1;
    }
    int r = tusbh_control_xfer(
       interface->device,
       USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       CDC_SEND_BREAK,
       duration_in_ms,
       interface->desc->bInterfaceNumber,
       0, 0);
    if(r < 0){
        TUSB_ITF_INFO("Fail to set break\n");
    }else{
        TUSB_ITF_INFO("Set break %d ms\n", duration_in_ms);
    }
    return r;
}

static int tusbh_cdc_acm_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    interface->ep_num = 0;
    create_info_pool(interface, tusbh_cdc_acm_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_cdc_acm_info_t));
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    
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
        }else if(t == USB_FUCNTION_DESCRIPTOR_TYPE){
            usb_function_descriptor_t* func = (usb_function_descriptor_t*)(dev->config_desc+cfg_offset);
            if(func->bDescriptorSubtype == 2){
                info->bmCapabilities = dev->config_desc[cfg_offset+3];
            }
        }
        cfg_offset+=len;
    }
    if(ep_index != interface->ep_num){
        TUSB_ITF_INFO("Endpoint count error espect %d, got %d\n", interface->ep_num, ep_index);
        goto error;
    }
    if(!info->ep_in || !info->ep_out){
        TUSB_ITF_INFO("Fail to get cdc endpoint\n");
        goto error;
    }
    
    if( (info->ep_in->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK){
        TUSB_ITF_INFO("CDC ACM in ep attr error %d\n", info->ep_in->desc->bmAttributes);
        goto error;
    }
    
    if( (info->ep_out->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK){
        TUSB_ITF_INFO("CDC ACM out ep attr error %d\n", info->ep_out->desc->bmAttributes);
        goto error;
    }
    if(info->ep_notify){
        if( (info->ep_notify->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR){
            TUSB_ITF_INFO("CDC ACM notify ep attr error %d\n", info->ep_notify->desc->bmAttributes);
            goto error;
        }
    }
    
    // allocate a buffer for the notify ep, so it will start automatically
    if(info->ep_notify){
        info->ep_notify->data = tusbh_malloc( EP_MPS(info->ep_notify->desc) );
        if(!info->ep_notify->data){
            TUSB_ITF_INFO("Fail to allocate memory for CDC ACM notify endpoint\n");
            goto error;
        }
        
        if(tusbh_ep_allocate_pipe(info->ep_notify) < 0){
            TUSB_ITF_INFO("Fail to allocate pipe for CDC ACM notify endpoint\n");
            goto error;
        }
    }
    
    if(tusbh_ep_allocate_pipe(info->ep_in)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for CDC ACM in\n");
        goto error;
    }
    if(tusbh_ep_allocate_pipe(info->ep_out)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for CDC ACM out\n");
        goto error;
    }
    
error:
    TUSB_ITF_INFO("CDC ACM interface init\n");
    TUSB_ITF_INFO("CDC ACM Ep in %02x, out %02x, Cap %02x\n",
        info->ep_in->desc->bEndpointAddress,
        info->ep_out->desc->bEndpointAddress,
        info->bmCapabilities);
    if(info->ep_notify){
        TUSB_ITF_INFO("CDC ACM Notify EP %02x\n", info->ep_notify->desc->bEndpointAddress);
    }
    
    tusbh_cdc_get_line_coding(interface, &info->line_coding);
    info->line_coding.bitrate = 9600;
    info->line_coding.stopbits = CDC_1STOP;
    info->line_coding.parity = CDC_NONE;
    info->line_coding.databits = 8;
    tusbh_cdc_set_line_coding(interface, &info->line_coding);
    
    tusbh_cdc_get_line_coding(interface, &info->line_coding);
    
    tusbh_cdc_set_line_state(interface, 0);
    
    return cfg_offset;
}

static int tusbh_cdc_acm_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    TUSB_ITF_INFO("CDC ACM interface deinit\n");
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
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

static int cdc_acm_xfered(tusbh_ep_info_t* ep)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(ep->interface, tusbh_cdc_acm_info_t);
    if(ep != info->ep_notify){
        return 0;
    }
    tusb_hc_data_t* hc = &ep_host(ep)->hc[ep->pipe_num];
    if(hc->state != TUSB_CS_TRANSFER_COMPLETE){
        return -1;
    }

    tusb_cdc_state_t* cdc_state = (tusb_cdc_state_t*)ep->data;
    if(ep_class(ep,tusbh_cdc_acm_class_t)->on_line_status_changed){
        ep_class(ep,tusbh_cdc_acm_class_t)->on_line_status_changed(ep->interface, cdc_state->line_state);
    }else{
        tusbh_interface_t* interface = ep->interface;
        uint16_t* pstate = (uint16_t*)&cdc_state->line_state;
        TUSB_ITF_INFO("State and line notify %04x\n", *pstate);
    }
    return 0;
}

int tusbh_cdc_send_data(tusbh_interface_t* interface, void* data, uint32_t len, uint32_t timeout)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    return tusbh_ep_xfer(info->ep_out, data, len, timeout);
}

int tusbh_cdc_recv_data(tusbh_interface_t* interface, void* data, uint32_t len, uint32_t timeout)
{
    tusbh_cdc_acm_info_t* info = tusbh_get_info(interface, tusbh_cdc_acm_info_t);
    return tusbh_ep_xfer(info->ep_in, data, len, timeout);
}

const tusbh_interface_backend_t  tusbh_cdc_acm_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_CDC,
    .bInterfaceSubClass = 2,
    .bInterfaceProtocol = 1,
    .init = tusbh_cdc_acm_init,
    .deinit = tusbh_cdc_acm_deinit,
    .data_xfered = cdc_acm_xfered,
    .desc = "CDC ACM class",
};

