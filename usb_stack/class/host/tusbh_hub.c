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

#include "tusbh_hub.h"
#include "string.h"

// USB 2.0 11.24.2.7
typedef struct _usb_hub_port_status
{
  struct _port_status{
    uint16_t     PORT_CONNECTION      : 1;
    uint16_t     PORT_ENABLE          : 1;
    uint16_t     PORT_SUSPEND         : 1;
    uint16_t     PORT_OVER_CURRENT    : 1;
    uint16_t     PORT_RESET           : 1;
    uint16_t     RESERVED_1           : 3;
    uint16_t     PORT_POWER           : 1;
    uint16_t     PORT_LOW_SPEED       : 1;
    uint16_t     PORT_HIGH_SPEED      : 1;
    uint16_t     PORT_TEST            : 1;
    uint16_t     PORT_INDICATOR       : 1;
    uint16_t     RESERVED_2           : 3;
  } wPortStatus;
  struct _port_change{
    uint16_t     C_PORT_CONNECTION    : 1;
    uint16_t     C_PORT_ENABLE        : 1;
    uint16_t     C_PORT_SUSPEND       : 1;
    uint16_t     C_PORT_OVER_CURRENT  : 1;
    uint16_t     C_PORT_RESET         : 1;
    uint16_t     RESERVED             : 11;
  } wPortChange;
} usb_hub_port_status_t;


typedef struct _tusbh_hub_info
{
    uint8_t  ep_in;
    uint8_t  ep_in_type;
    uint16_t ep_in_mps;
    tusbh_ep_info_t* ep;
    usb_hub_descriptor_t hub_desc;
}tusbh_hub_info_t;

#define  USB_REQ_RECIPIENT_OTHER       0x03


#define HUB_FEATURE_SEL_PORT_CONN         		0x00
#define HUB_FEATURE_SEL_PORT_ENABLE             0x01
#define HUB_FEATURE_SEL_PORT_SUSPEND            0x02
#define HUB_FEATURE_SEL_PORT_OVER_CURRENT       0x03
#define HUB_FEATURE_SEL_PORT_RESET              0x04
#define HUB_FEATURE_SEL_PORT_POWER              0x08
#define HUB_FEATURE_SEL_PORT_LOW_SPEED          0x09
#define HUB_FEATURE_SEL_C_PORT_CONNECTION       0x10
#define HUB_FEATURE_SEL_C_PORT_ENABLE           0x11
#define HUB_FEATURE_SEL_C_PORT_SUSPEND          0x12
#define HUB_FEATURE_SEL_C_PORT_OVER_CURRENT     0x13
#define HUB_FEATURE_SEL_C_PORT_RESET            0x14
#define HUB_FEATURE_SEL_PORT_INDICATOR          0x16

static inline int tusbh_get_hub_port_staus(tusbh_device_t* dev, uint8_t port, usb_hub_port_status_t* status)
{
    return tusbh_control_xfer(
       dev,
       USB_D2H | USB_REQ_RECIPIENT_OTHER | USB_REQ_TYPE_CLASS,
       USB_REQ_GET_STATUS,
       0,
       port,
       status, sizeof(usb_hub_port_status_t));
}

static inline int tusbh_set_hub_port_feature(tusbh_device_t* dev, uint8_t port, uint16_t feature)
{
    return tusbh_control_xfer(
       dev,
       USB_H2D | USB_REQ_RECIPIENT_OTHER | USB_REQ_TYPE_CLASS,
       USB_REQ_SET_FEATURE,
       feature,
       port,
       0, 0);
}

static inline int tusbh_clear_hub_port_feature(tusbh_device_t* dev, uint8_t port, uint16_t feature)
{
    return tusbh_control_xfer(
       dev,
       USB_H2D | USB_REQ_RECIPIENT_OTHER | USB_REQ_TYPE_CLASS,
       USB_REQ_CLEAR_FEATURE,
       feature,
       port,
       0, 0);
}

static inline int tusbh_get_hub_descriptor(tusbh_device_t* dev, void* data, uint32_t len)
{
    return tusbh_control_xfer(
       dev,
       USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_CLASS,
       USB_REQ_GET_DESCRIPTOR,
       USB_HUB_DESCRIPTOR_TYPE<<8,
       0,
       data, len);
}

static int tusbh_hub_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+cfg_offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    int r = 0;
    interface->ep_num = itf->bNumEndpoints;
    create_info_pool(interface, tusbh_hub_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_hub_info_t));
    tusbh_hub_info_t* info = tusbh_get_info(interface, tusbh_hub_info_t);
    
    memset(interface->endpoints, 0, sizeof(interface->endpoints));
    
    while(cfg_offset < wTotalLength){
        uint8_t len = dev->config_desc[cfg_offset];
        uint8_t t = dev->config_desc[cfg_offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+cfg_offset);
            if(ep_index < TUSBH_MAX_EP){
                setup_ep_info(&interface->endpoints[ep_index], ep, interface);
                if(ep->bEndpointAddress & 0x80){
                    if(!info->ep_in){
                        info->ep_in = ep->bEndpointAddress;
                        info->ep_in_type = ep->bmAttributes;
                        info->ep_in_mps = EP_MPS(ep);
                        info->ep = &interface->endpoints[ep_index];
                    }
                }
            }else{
                TUSB_ITF_INFO("Endpoint count large than TUSBH_MAX_EP in HUB interface\n");
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
        goto error;
    }
    if(!info->ep_in){
        TUSB_ITF_INFO("Fail to get hub in endpoint\n");
        goto error;
    }else{
        // allocate a buffer for the IN ep, so it will start automatically
        info->ep->data = tusbh_malloc( info->ep_in_mps );
        if(!info->ep->data){
            TUSB_ITF_INFO("Fail to allocate memory for hub in endpoint\n");
            goto error;
        }
        if(tusbh_ep_allocate_pipe(info->ep) < 0){
            TUSB_ITF_INFO("Fail to allocate pipe for HUB in endpoint\n");
            goto error;
        }else{
            tusbh_ep_info_t* ep = info->ep;
            TUSB_EP_INFO("HUB IN ep pipe = %d\n", ep->pipe_num);
        }
    }
    
    if( (info->ep_in_type & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR){
        TUSB_ITF_INFO("Hub in endpoint attr error %d\n", info->ep_in_type);
        goto error;
    }
    
    r = tusbh_get_hub_descriptor(dev, &info->hub_desc, 8);//tusbh_get_descriptor(dev, USB_HUB_DESCRIPTOR_TYPE, 0, &info->hub_desc, 8);
    if(r < 0){
        TUSB_ITF_INFO("Fail to get hub descriptor\n");
        goto error;
    }
    
    r = tusbh_get_hub_descriptor(dev, &info->hub_desc, info->hub_desc.bLength);
    if(r < 0){
        TUSB_ITF_INFO("Fail to get full hub descriptor\n");
        goto error;
    }
    
    for(uint8_t port=1;port<=info->hub_desc.bNumPorts;port++){
        r = tusbh_set_hub_port_feature(dev, port, HUB_FEATURE_SEL_PORT_POWER);
        if(r < 0){
            TUSB_HUB_INFO("Fail to set port power\n");
        }
        tusb_delay_ms(info->hub_desc.bPowerOnToPowerGood*2);
    }
    
    TUSB_ITF_INFO("HUB interface init, port num = %d\n", info->hub_desc.bNumPorts);
    if(info->hub_desc.bNumPorts > TUSBH_MAX_CHILD){
        TUSB_ITF_INFO("Warning, HUB port large than TUSBH_MAX_CHILD (%d)\n", TUSBH_MAX_CHILD);
    }
error:
    return cfg_offset;
}

static int tusbh_hub_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    TUSB_ITF_INFO("HUB interface deinit\n");
    tusbh_hub_info_t* info = tusbh_get_info(interface, tusbh_hub_info_t);
    if(info->ep->data){
        tusbh_free(info->ep->data);
    }
    tusbh_ep_free_pipe(info->ep);
    tusbh_free_info(interface);
    return 0;
}
            
static int hub_clear_port_c_features(tusbh_device_t* dev, uint8_t port, usb_hub_port_status_t* status)
{
    int r;
#define FeatureClear(field)                                                       \
    if(status->wPortChange.field){                                                \
        r = tusbh_clear_hub_port_feature(dev, port, HUB_FEATURE_SEL_##field);     \
        if( r < 0 ){                                                              \
            TUSB_HUB_INFO("Error Clear feature: " #field "\n");                   \
        }                                                                         \
    }
    
    FeatureClear(C_PORT_CONNECTION)
    FeatureClear(C_PORT_ENABLE)
    FeatureClear(C_PORT_SUSPEND)
    FeatureClear(C_PORT_OVER_CURRENT)
    FeatureClear(C_PORT_RESET)
#undef FeatureClear
    return r;
}

int tusbh_device_attach(tusbh_device_t* dev);
int tusbh_device_deinit(tusbh_device_t* dev);

static int hub_data_xfered(tusbh_ep_info_t* ep)
{
    tusbh_hub_info_t* info = tusbh_get_info(ep->interface, tusbh_hub_info_t);
    tusb_hc_data_t* hc = &ep_host(ep)->hc[ep->pipe_num];
    tusbh_device_t* dev = ep_device(ep);
    
    if(ep->desc->bEndpointAddress != info->ep_in){
        TUSB_DEV_INFO("HUB Wrong ep xfered handler, espect %02x, got %02x\n", info->ep_in, ep->desc->bEndpointAddress);
        return -1;
    }
    
    if(hc->state != TUSB_CS_TRANSFER_COMPLETE){
        return -1;
    }
    
    uint8_t port_state = *(uint8_t*)ep->data;
    uint8_t port = 0;
    int res = 0;
    
    if(dev->ctrl_in>=0 || dev->ctrl_out>=0){
        TUSB_DEV_INFO("Error control pipe leaks in=%d, out=%d\n", dev->ctrl_in, dev->ctrl_out);
        tusbh_close_pipe(dev, dev->ctrl_in);
        tusbh_close_pipe(dev, dev->ctrl_out);
    }
    dev->ctrl_in = tusbh_allocate_pipe(dev, 0x80, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    dev->ctrl_out = tusbh_allocate_pipe(dev, 0x00, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
    
    if(dev->ctrl_in<0 || dev->ctrl_out<0){
        TUSB_DEV_INFO("HUB interface fail to allocate pipe for Control\n");
        res = -1;
        goto error;
    }
    TUSB_HUB_INFO("HUB port state = %02x\n", port_state);
    for(;port_state; port_state>>=1, port++){
        if( (port_state & 1) == 0 ) continue;
        usb_hub_port_status_t status;
        TUSB_HUB_INFO("HUB get port %d status\n", port);
        res = tusbh_get_hub_port_staus(dev, port, &status);
        if(res < 0){
            TUSB_HUB_INFO("HUB fail to get port status\n");
            goto error;
        }
        
        if(!status.wPortStatus.PORT_POWER){
            int r = r = tusbh_set_hub_port_feature(dev, port, HUB_FEATURE_SEL_PORT_POWER);
            if(r < 0){
                TUSB_HUB_INFO("Fail set power\n");
            }
            tusb_delay_ms(info->hub_desc.bPowerOnToPowerGood*2);
            continue;
        }
        
        hub_clear_port_c_features(dev, port, &status);
        
        if(port>TUSBH_MAX_CHILD){
            TUSB_HUB_INFO("Port large than TUSBH_MAX_CHILD(%d), ingore it\n", TUSBH_MAX_CHILD);
            continue;
        }
        
        // Port state  Disconnect -> Connect -> Enalbed -> Disconnect
        
        if(status.wPortStatus.PORT_CONNECTION){
            if(status.wPortStatus.PORT_ENABLE){
                TUSB_HUB_INFO("Enabled\n");
                tusbh_device_t* child = dev->children[port-1];
                if(!child){
                    TUSB_HUB_INFO("Device not exist\n");
                }
                child->host = dev->host;
                child->speed = PORT_SPEED_FULL;
                if(status.wPortStatus.PORT_HIGH_SPEED){
                    child->speed = PORT_SPEED_HIGH;
                }
                if(status.wPortStatus.PORT_LOW_SPEED){
                    child->speed = PORT_SPEED_LOW;
                }
                tusb_delay_ms(200);
                
                if(dev->ctrl_in>=0){
                    tusbh_close_pipe(dev, dev->ctrl_in);
                }
                if(dev->ctrl_out>=0){
                    tusbh_close_pipe(dev, dev->ctrl_out);
                }
                if(tusbh_device_attach(child) != 0){
                    TUSB_HUB_INFO("Device attach failed\n");
                }
                dev->ctrl_in = tusbh_allocate_pipe(dev, 0x80, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
                dev->ctrl_out = tusbh_allocate_pipe(dev, 0x00, EP_TYPE_CTRL, dev->device_desc.bMaxPacketSize);
                if(dev->ctrl_in<0){
                    TUSB_HUB_INFO("Fail to re-allocate hub ctrl in\n");
                }
                if(dev->ctrl_out<0){
                    TUSB_HUB_INFO("Fail to re-allocate hub ctrl out\n");
                } 
            }else{
                TUSB_HUB_INFO("Connect\n");
                tusbh_device_t* child = dev->children[port-1];
                if(child){
                    TUSB_HUB_INFO("aleary has a child\n");
                    tusbh_device_deinit(child);
                    dev->children[port-1] = 0;
                }
                child = tusbh_new_device();
                dev->children[port-1] = child;
                tusbh_set_hub_port_feature(dev, port, HUB_FEATURE_SEL_PORT_RESET);
            }
        }else{
            TUSB_HUB_INFO("Disconnect\n");
            tusbh_device_t* child = dev->children[port-1];
            if(!child){
                TUSB_HUB_INFO("Device not exsit\n");
            }else{
                tusbh_device_deinit(child);
                dev->children[port-1] = 0;
            }
        }
    }
    
error:
    if(dev->ctrl_in>=0){ tusbh_close_pipe(dev, dev->ctrl_in); }
    if(dev->ctrl_out>=0){ tusbh_close_pipe(dev, dev->ctrl_out); }
    dev->ctrl_in = dev->ctrl_out = -1;
    return res;
}


const tusbh_interface_backend_t  tusbh_hub_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = USB_CLASS_HUB,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .init = tusbh_hub_init,
    .deinit = tusbh_hub_deinit,
    .data_xfered = hub_data_xfered,
    .desc = "HUB",
};

