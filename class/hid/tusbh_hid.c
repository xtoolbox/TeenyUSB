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

#include "tusbh_hid.h"
#include "tusbh_xfer.h"
#include "string.h"
#include "teeny_usb_util.h"

#define HID_BOOT_CODE                                  0x01    
#define HID_KEYBRD_BOOT_CODE                           0x01
#define HID_MOUSE_BOOT_CODE                            0x02


#define USB_HID_GET_REPORT           0x01
#define USB_HID_GET_IDLE             0x02
#define USB_HID_GET_PROTOCOL         0x03
#define USB_HID_SET_REPORT           0x09
#define USB_HID_SET_IDLE             0x0A
#define USB_HID_SET_PROTOCOL         0x0B    

#define USB_HID_INPUT_REPORT    0x01
#define USB_HID_OUTPUT_REPORT   0x02
#define USB_HID_FEATURE_REPORT  0x03

typedef __PACK_BEGIN struct _usb_hid_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;
    __PACK_BEGIN struct {
        uint8_t  bDescriptorType;
        uint16_t wDescriptorLength;
    } __PACK_END report_desc[1];
} __PACK_END usb_hid_descriptor_t;


static int tusbh_hid_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+cfg_offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    interface->ep_count = itf->bNumEndpoints;
    create_info_pool(interface, tusbh_hid_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_hid_info_t));
    tusbh_hid_info_t* info = tusbh_get_info(interface, tusbh_hid_info_t);
    
    memset(interface->endpoints, 0, sizeof(interface->endpoints));
    
    while(cfg_offset < wTotalLength){
        uint8_t len = dev->config_desc[cfg_offset];
        uint8_t t = dev->config_desc[cfg_offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+cfg_offset);
            if(ep_index < TUSBH_ITF_MAX_EP_COUNT){
                setup_ep_info(&interface->endpoints[ep_index], ep, interface);
                if(ep->bEndpointAddress & 0x80){
                    if(!info->ep_in){
                        info->ep_in = &interface->endpoints[ep_index];
                    }
                }else{
                    if(!info->ep_out){
                        info->ep_out = &interface->endpoints[ep_index];
                    }
                }
            }else{
                ITF_LOGE("Endpoint count large than TUSBH_ITF_MAX_EP_COUNT in HUB interface\n");
            }
            ep_index++;
        }else if(t == USB_HID_DESCRIPTOR_TYPE){
            usb_hid_descriptor_t* hid = (usb_hid_descriptor_t*)(dev->config_desc+cfg_offset);
            if(hid->bNumDescriptors > 0){
                if(hid->report_desc[0].bDescriptorType != USB_REPORT_DESCRIPTOR_TYPE){
                    ITF_LOGW("HID class descriptor type is not REPORT, %02x\n", hid->report_desc[0].bDescriptorType);
                }
                uint8_t* p = (uint8_t*) &hid->report_desc[0].wDescriptorLength;;
                info->report_desc_len = p[0] | (p[1]<<8);
            }
            if(hid->bNumDescriptors > 1){
                ITF_LOGW("HID class descriptor count large than 1\n");
            }
        }else if(t == USB_INTERFACE_DESCRIPTOR_TYPE){
            if(itf_cnt > 0){
                break;
            }
            itf_cnt++;
        }else if(t == USB_IAD_DESCRIPTOR_TYPE){
            break;
        }
        cfg_offset+=len;
    }
    if(ep_index != interface->ep_count){
        ITF_LOGE("Endpoint count error espect %d, got %d\n", interface->ep_count, ep_index);
        return -1;
    }
    if(!info->ep_in){
        ITF_LOGE("Fail to get hid in endpoint\n");
        return -1;
    }
    
    if( (info->ep_in->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR){
        ITF_LOGE("HID in ep attr error %d\n", info->ep_in->desc->bmAttributes);
        return -1;
    }
    
    if( info->ep_out){
        if( (info->ep_out->desc->bmAttributes & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR){
            ITF_LOGE("HID out ep attr error %d\n", info->ep_out->desc->bmAttributes);
            return -1;
        }
    }
    
    if(itf->bInterfaceSubClass == 1){        
        int r = tusbh_control_xfer(
           dev,
           USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
           USB_HID_SET_PROTOCOL,
           0,
           interface->desc->bInterfaceNumber,
           0, 0);
        if(r<0){
            ITF_LOGE("Fail to set boot protocol\n");
            return -1;
        }
    }else{
        if(info->report_desc_len > 0){
            info->report_desc = tusb_malloc(info->report_desc_len);
            if(info->report_desc){
                   int r = tusbh_control_xfer(
                    dev,
                    USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD,
                    USB_REQ_GET_DESCRIPTOR,
                    (USB_REPORT_DESCRIPTOR_TYPE<<8) | 0,
                    interface->desc->bInterfaceNumber,
                    info->report_desc,
                    info->report_desc_len
                );
                if(r<0){
                    ITF_LOGE("Fail to get report descriptor\n");
                    return -1;
                }
            }else{
                ITF_LOGE("Fail to allocate memory for report descriptor\n");
            }
        }
    }
    
    ITF_LOGD("HID interface init\n");
    ITF_LOGD("Ep in %02x\n", info->ep_in->desc->bEndpointAddress);
    if(info->ep_out){
        ITF_LOGD("Ep out %02x\n", info->ep_out->desc->bEndpointAddress);
    }
    return cfg_offset;
}

static int tusbh_hid_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    ITF_LOGD("HID interface deinit\n");
    tusbh_hid_info_t* info = tusbh_get_info(interface, tusbh_hid_info_t);
    if(info->report_desc){
        tusb_free(info->report_desc);
        info->report_desc = 0;
    }
    tusbh_free_info(interface);
    return 0;
}


#define TUSB_HID_LOGD(fmt, ...)  TUSB_LOGD("[HID ] " fmt, ##__VA_ARGS__)
#define TUSB_HID_LOGW(fmt, ...)  TUSB_LOGW("[HID ] " fmt, ##__VA_ARGS__)
#define TUSB_HID_LOGE(fmt, ...)  TUSB_LOGE("[HID ] " fmt, ##__VA_ARGS__)


static int tusbh_on_boot_key(tusbh_ep_info_t* ep, const uint8_t* keys)
{
    TUSB_HID_LOGD("Key: %02x %02x %02x %02x %02x %02x %02x %02x\n",
    keys[0],keys[1],keys[2],keys[3],keys[4],keys[5],keys[6],keys[7] );
    return 0;
}

static int tusbh_on_boot_mouse(tusbh_ep_info_t* ep, const uint8_t* mouse)
{
    uint8_t btn = mouse[0];
    int8_t x = ((int8_t*)mouse)[1];
    int8_t y = ((int8_t*)mouse)[2];
    TUSB_HID_LOGD("Mouse:%02x (%d,%d)\n", btn, x, y);
    return 0;
}

static int keyboard_xfered(tusbh_ep_info_t* ep, const void* data, int len)
{
    tusbh_hid_info_t* info = tusbh_get_info(ep->interface, tusbh_hid_info_t);    
    if(ep->desc->bEndpointAddress != info->ep_in->desc->bEndpointAddress){
        TUSB_HID_LOGE("HID KBD Wrong ep xfered handler, espect %02x, got %02x\n", info->ep_in->desc->bEndpointAddress, ep->desc->bEndpointAddress);
        return -1;
    }
    
    if(ep_class(ep,tusbh_boot_key_class_t)->on_key){
        ep_class(ep,tusbh_boot_key_class_t)->on_key(ep, (const uint8_t*)data);
    }else{
        tusbh_on_boot_key(ep, (const uint8_t*)data);
    }
    
    return 0;
}

static int mouse_xfered(tusbh_ep_info_t* ep, const void* data, int len)
{
    tusbh_hid_info_t* info = tusbh_get_info(ep->interface, tusbh_hid_info_t);
    
    if(ep->desc->bEndpointAddress != info->ep_in->desc->bEndpointAddress){
        TUSB_HID_LOGE("HID Mouse Wrong ep xfered handler, espect %02x, got %02x\n", info->ep_in->desc->bEndpointAddress, ep->desc->bEndpointAddress);
        return -1;
    }
    
    if(ep_class(ep,tusbh_boot_mouse_class_t)->on_mouse){
        ep_class(ep,tusbh_boot_mouse_class_t)->on_mouse(ep, (const uint8_t*)data);
    }else{
        tusbh_on_boot_mouse(ep, (const uint8_t*)data);
    }
    
    return 0;
}

static int hid_xfered(tusbh_ep_info_t* ep, const void* data, int len)
{
    tusbh_hid_info_t* info = tusbh_get_info(ep->interface, tusbh_hid_info_t);
    
    if(ep == info->ep_out){
        if(ep->interface->cls->backend == &tusbh_hid_backend){
            TUSB_HID_LOGE("Endpoint backend missmatch\n");
            return -1;
        }
        if(ep_class(ep, tusbh_hid_class_t)->on_send_done){
            ep_class(ep, tusbh_hid_class_t)->on_send_done(ep, (const uint8_t*)data, len);
        }
        return 0;
    }
    
    if(ep->desc->bEndpointAddress != info->ep_in->desc->bEndpointAddress){
        TUSB_HID_LOGE("HID Mouse Wrong ep xfered handler, espect %02x, got %02x\n", info->ep_in->desc->bEndpointAddress, ep->desc->bEndpointAddress);
        return -1;
    }
    
    if(ep == info->ep_in){
        if(ep_class(ep, tusbh_hid_class_t)->on_recv_data){
            ep_class(ep, tusbh_hid_class_t)->on_recv_data(ep, (const uint8_t*)data, len);
        }else{
            TUSB_HID_LOGD("HID: got %d bytes\n", len);
        }
    }
    return 0;
}

int tusbh_hid_send_data(tusbh_ep_info_t* ep, void* data, int len)
{
    tusbh_transfer_t xfer;
    tusb_transfer_state_t res = 
        tusbh_transaction(&xfer, ep_device(ep), data, (uint32_t)len, 0, ep->desc->bEndpointAddress, ep->desc->bmAttributes & USBD_EP_TYPE_MASK);
    if(res != TS_ACK){
        if(res == 0) return -1;
        return -res;
    }
    return 0;
}

int tusbh_set_keyboard_led(tusbh_ep_info_t* ep, uint8_t leds)
{
    uint32_t buf[1];
    buf[0] = leds;
    int r = tusbh_control_xfer(
       ep_device(ep),
       USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       USB_HID_SET_REPORT,
       USB_HID_OUTPUT_REPORT<<8,
       ep->interface->desc->bInterfaceNumber,
       buf, 1);
    return r;
}

const tusbh_interface_backend_t  tusbh_boot_keyboard_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 1,
    .bInterfaceProtocol = HID_KEYBRD_BOOT_CODE,
    .init = tusbh_hid_init,
    .deinit = tusbh_hid_deinit,
    .data_xfered = keyboard_xfered,
    .desc = "HID Boot Keyboard",
};

const tusbh_interface_backend_t  tusbh_boot_mouse_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 1,
    .bInterfaceProtocol = HID_MOUSE_BOOT_CODE,
    .init = tusbh_hid_init,
    .deinit = tusbh_hid_deinit,
    .data_xfered = mouse_xfered,
    .desc = "HID Boot Mouse",
};

const tusbh_interface_backend_t  tusbh_hid_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .init = tusbh_hid_init,
    .deinit = tusbh_hid_deinit,
    .data_xfered = hid_xfered,
    .desc = "HID Device",
};
