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
#include "tusbh.h"
#include "tusbh_xfer.h"

int tusbh_init(tusb_host_t* host, tusbh_root_hub_t* root_hub)
{
    memset(root_hub->children,0,sizeof(root_hub->children));
    root_hub->addr_mask = 0;
    root_hub->conn_mask = 0;
    root_hub->en_mask = 0;
    host->user_data = (void*)root_hub;
    return 0;
}

const char* const speed_string[] = {
    "HS","FS","LS","?S",
};

#define SPPED_STRING(x)     speed_string[ (x) & 0x03]

#define  TUSB_ASSERT(exp)                                       \
do{                                                             \
    if(exp){}else{                                              \
        TUSB_LOGE(#exp "%s:%d\n", __FUNCTION__, __LINE__);       \
        while(1);                                               \
}}while(0)


static uint8_t get_free_addr(tusbh_root_hub_t* root)
{
    uint32_t mask = 1;
    for(uint8_t i=1;i<=32;i++){
        if(root->addr_mask & mask){
        }else{
            return i;
        }
    }
    return 0;
}

static void free_device_addr(tusbh_root_hub_t* root, tusbh_device_t* dev)
{
    if(dev && dev->address){
        root->addr_mask &= ~(1<<dev->address);
    }
}

static const tusbh_class_t* tusbh_find_class(tusbh_device_t* dev, uint8_t bClass, uint8_t bSubClass, uint8_t bProtocol)
{
    const tusbh_class_cp* p = GetDevRoot(dev)->support_classes;
    if(!p){
      return 0;
    }
    tusbh_class_cp reg;
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

static const char* const ep_attr_str[] = {
    "CTRL", "ISOC", "BULK" , "INTR"
};
static const char* ep_attr_2_str(uint8_t attr)
{
    return ep_attr_str[attr & USBD_EP_TYPE_MASK];
}

// parse interface descriptor, add the correspond endpoint to the interface
static int tusbh_get_interface_ep(tusbh_device_t* dev, tusbh_interface_t* interface, int offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t ep_count = 0;
    if(itf->bDescriptorType == USB_IAD_DESCRIPTOR_TYPE){
        offset += itf->bLength;
        itf = (usb_interface_descriptor_t*)(dev->config_desc+offset);
    }
    if(itf->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE){
        DEV_LOGE("Wrong interface desc type %d\n", itf->bDescriptorType);
        return -1;
    }
    offset += itf->bLength;
    while(offset < wTotalLength){
        uint8_t len = dev->config_desc[offset];
        uint8_t t = dev->config_desc[offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+offset);
            if(ep_index < TUSBH_ITF_MAX_EP_COUNT){
                if(interface){
                    setup_ep_info(&interface->endpoints[ep_index], ep, interface);
                }
                ep_count++;
            }else{
                DEV_LOGW("Endpoint count(%d) large than TUSBH_ITF_MAX_EP_COUNT(%d), ignore it\n", ep_index, TUSBH_ITF_MAX_EP_COUNT);
            }
            ep_index++;
        }
        if(t == USB_INTERFACE_DESCRIPTOR_TYPE || t == USB_IAD_DESCRIPTOR_TYPE){
            break;
        }
        offset+=len;
    }
    if(ep_index != itf->bNumEndpoints){
        DEV_LOGE("Endpoint count error espect %d, got %d\n", itf->bNumEndpoints, ep_index);
    }
    if(ep_index != ep_count){
        DEV_LOGE("Endpoint count mismatch espect %d, got %d\n", ep_index, ep_count);
    }
    if(interface){
        interface->ep_count = itf->bNumEndpoints;
        DEV_LOGD("Interface (0x%02x,0x%02x,0x%02x) has %d endpoints:\n", 
            itf->bInterfaceClass, itf->bInterfaceSubClass, itf->bInterfaceProtocol, itf->bNumEndpoints);
        for(uint8_t i=0;i<ep_count;i++){
            usb_endpoint_descriptor_t* desc = interface->endpoints[i].desc;
            DEV_LOGD("  Ep  addr:0x%02x, attr:%s, mps:%d\n", desc->bEndpointAddress, ep_attr_2_str(desc->bmAttributes), EP_MPS(desc));
        }
    }
    
    return offset;
}

/**
 * parse configuration descriptor, initial the interface and correspond endpoint
 */
static int tusbh_parse_config(tusbh_device_t* dev)
{
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    int offset = sizeof(usb_config_descriptor_t);
    int r = 0;
    dev->interface_count = 0;
    
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
                DEV_LOGW("Interface alternate setting (%d > 0) is not support, ignore it\n", itf->bAlternateSetting);
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
            DEV_LOGE("Interface descriptor type %d, offset=%d\n", dev->config_desc[offset+1], offset+1);
            return -1;
        }
        if(dev->interface_count>=TUSBH_MAX_ITF_COUNT){
            DEV_LOGW("Interface count large than TUSBH_MAX_INTERFACE (%d), ignore it\n", TUSBH_MAX_ITF_COUNT);
            r = tusbh_get_interface_ep(dev, 0, offset);
            if(r < 0){ return r; }
            offset = r;
            continue;
        }
        const tusbh_class_t* cls = tusbh_find_class(dev, bClass, bSubClass, bProtocol);
        tusbh_interface_t* dev_itf = &dev->interfaces[dev->interface_count];
        dev_itf->cls = cls;
        dev_itf->device = dev;
        dev_itf->desc = itf;
        dev_itf->interface_id = dev->interface_count;
        if(cls){
            r = cls->backend->init(dev, dev_itf, offset);
            DEV_LOGD("Class [%s] init result = %d\n", cls->backend->desc, r);
        }else{
            DEV_LOGD("Class (0x%02x,0x%02x,0x%02x) not found, init it as raw device\n", bClass, bSubClass, bProtocol);
            r = tusbh_get_interface_ep(dev, dev_itf, offset);
        }
        if(r<0){
            DEV_LOGE("Fail to init interface %d\n", dev->interface_count);
            return -1;
        }
        dev->interface_count++;
        offset = r;
    }
    DEV_LOGD("Device has %d interfaces\n", dev->interface_count);
    return 0;
}

static void traverse_periodic_in(tusbh_device_t* dev, int (*action)(tusbh_ep_info_t* ep))
{
    for(int iItf=0;iItf<dev->interface_count;iItf++){
        tusbh_interface_t* itf = &dev->interfaces[iItf];
        if(!itf){
            DEV_LOGE("Interface not found in list\n");
            continue;
        }
        for(int iEp=0; iEp<itf->ep_count; iEp++){
            tusbh_ep_info_t* ep = &itf->endpoints[iEp];
            if(!ep){
                DEV_LOGE("Endpoint not found in list\n");
                continue;
            }
            if( ep->desc->bmAttributes & 1 ){
                // ISOC or INTR endpoints
                if(ep->desc->bEndpointAddress & 0x80){
                    // IN endpoint
                    action(ep);
                }
            }
        }
    }
}

static int tusbh_device_init(tusbh_device_t* dev)
{
    TUSB_ASSERT(dev && dev->host);
    int res = 0;
    tusb_host_t* host = dev->host;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    dev->address = 0;
    dev->device_desc.bMaxPacketSize = 8;
    res = tusbh_get_descriptor(dev, USB_DEVICE_DESCRIPTOR_TYPE, 0, (uint8_t*)&dev->device_desc, 8);
    if(res != 0){
        DEV_LOGE("Fail to get device descriptor\n");
        return -1;
    }
    uint8_t addr = get_free_addr(root);
    if(addr == 0){
        DEV_LOGE("Fail to find a free device address\n");
        return -1;
    }
    
    res = tusbh_set_address(dev, addr);
    if(res != 0){
        DEV_LOGE("Fail to set device address\n");
        return -1;
    }
    
    DEV_LOGD("Set device address to %02d\n", addr);
    
    dev->address = addr;
    
    res = tusbh_get_descriptor(dev, USB_DEVICE_DESCRIPTOR_TYPE, 0, (uint8_t*)&dev->device_desc, sizeof(dev->device_desc));
    if(res != 0){
        DEV_LOGE("Fail to get full device descriptor\n");
        return -1;
    }
    
    DEV_LOGD("Device VID:%04x PID:%04x\n", dev->device_desc.idVendor, dev->device_desc.idProduct);
    
    uint8_t config_temp[16];
    
    res = tusbh_get_descriptor(dev, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, config_temp, 9);
    if(res != 0){
        DEV_LOGE("Fail to get configuration descriptor header\n");
        return -1;
    }
    
    usb_config_descriptor_t* cfg = (usb_config_descriptor_t*)config_temp;
    dev->config_desc = tusb_malloc(cfg->wTotalLength);
    if(!dev->config_desc){
        DEV_LOGE("Fail to allocate memory for config descriptor\n");
        return -1;
    }
    
    res = tusbh_get_descriptor(dev, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, dev->config_desc, cfg->wTotalLength);
    if(res != 0){
        DEV_LOGE("Fail to get full configuration descriptor\n");
        return -1;
    }
    
    res = tusbh_parse_config(dev);
    if(res != 0){
        DEV_LOGE("Fail to parse configuration descriptor\n");
        return -1;
    }
    dev->config = 1;
    res = tusbh_set_config(dev, dev->config);
    if(res != 0){
        DEV_LOGE("Fail to set configuration\n");
        return -1;
    }
    
    // collect periodic IN endpoints, mannage them it host periodic queue
    traverse_periodic_in(dev, tusbh_init_periodic_xfer);
    DEV_LOGD("Set to configuration %d(%d)\n", dev->config, dev->device_desc.bNumConfigurations);
    
    return res;
}

static void tusb_port_changed_handler(tusb_msg_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->context;
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    int port = (int)msg->param1;
    host_port_state_t new_state = (host_port_state_t)msg->param2;
    if(port > TUSBH_ROOT_PORT_COUNT){
        RTHUB_LOGE("port large equal than %d\n", TUSBH_ROOT_PORT_COUNT);
        return;
    }
    uint8_t port_mask = 1<<port;
    switch(new_state){
        case TUSB_HOST_PORT_DISCONNECTED:
            RTHUB_LOGD("Disconntected\n");
            if(root->conn_mask & port_mask){
            }else{
                RTHUB_LOGD("Not connected, ignore it\n");
            }
            root->conn_mask &= ~port_mask;
            root->en_mask &= ~port_mask;
            if(root->children[port]){
                free_device_addr(root, root->children[port]);
                tusbh_free_device(root->children[port]);
                root->children[port] = 0;
            }
            break;
        case TUSB_HOST_PORT_CONNECTED:
            if(root->conn_mask & port_mask){
                RTHUB_LOGD("Already conntected\n");
            }
            RTHUB_LOGD("Conntected\n");
            root->conn_mask |= port_mask;
            if(root->en_mask & port_mask){
                RTHUB_LOGD("Already enabled, [Connect]\n");
                break;
            }
            tusb_delay_ms(200);
            tusb_host_reset(host, port);
            break;
        case TUSB_HOST_PORT_ENABLED:{
            if(root->en_mask & port_mask){
                RTHUB_LOGD("Already enabled, [Enabled]\n");
            }
            // wait for device ready to operate
            tusb_delay_ms(100);
            
            uint8_t speed = tusb_host_get_speed(host, port);
            RTHUB_LOGD("Enabled, %s device\n", SPPED_STRING(speed));
            root->en_mask |= port_mask;
            if(root->children[port]){
                RTHUB_LOGD("Device already ready\n");
            }else{
                root->children[port] = tusbh_new_device();
            }
            root->children[port]->host = host;
            root->children[port]->speed = speed;
            if(tusbh_device_init(root->children[port]) != 0){
                free_device_addr(root, root->children[port]);
                RTHUB_LOGE("Fail to init device\n");
                tusbh_free_device(root->children[port]);
                root->children[port] = 0;
                root->en_mask &= ~port_mask;
                root->conn_mask &= ~port_mask;
            }
            break;
        }
        case TUSB_HOST_PORT_DISABLED:
            RTHUB_LOGD("Disabled\n");
            if(root->en_mask & port_mask){
                root->en_mask &= ~port_mask;
            }else{
                RTHUB_LOGD("Not enabled, ingore it\n");
            }
            break;
        default:
            break;
    }
}

static void tusb_sof_event_handler(tusb_msg_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->context;
    tusbh_handle_periodic_xfer(host);
    host->periodic_pending = 0;
}

static void tusb_channel_event_handler(tusb_msg_t* msg)
{
    tusb_host_t* host = (tusb_host_t*)msg->context;
    (void)host;
    int ch_num = (int)msg->param1;
    int ch_state = (host_port_state_t)msg->param2;
    TUSB_LOGD("Host channel (%d) change to state %d\n", ch_num, ch_state);
}

int tusb_host_port_changed(tusb_host_driver_t* drv, int port, host_port_state_t new_state)
{
    tusb_host_t* host = (tusb_host_t*)tusb_host_drv_get_context(drv);
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    tusbh_post_msg(root->mq, host, port, new_state, tusb_port_changed_handler);
    return 0;
}

int tusb_host_sof_event(tusb_host_driver_t* drv)
{
    tusb_host_t* host = (tusb_host_t*)tusb_host_drv_get_context(drv);
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    uint16_t pending = host->periodic_pending;
    host->periodic_pending++;
    if(!pending){
        tusbh_post_msg(root->mq, host, 0, 0, tusb_sof_event_handler);
    }
    return 0;
}

int tusb_host_channel_event(tusb_host_driver_t* drv, int ch_num, int ch_state)
{
    tusb_host_t* host = (tusb_host_t*)tusb_host_drv_get_context(drv);
    tusbh_root_hub_t* root = (tusbh_root_hub_t*)host->user_data;
    tusbh_post_msg(root->mq, host, ch_num, ch_state, tusb_channel_event_handler);
    return 0;
}

void tusbh_post_msg(tusb_mq_t* mq, void* context, uint32_t param1, uint32_t param2, void(*handler)(tusb_msg_t*))
{
    tusb_msg_t msg = { context, param1, param2, handler};
    tusb_mq_post(mq, &msg);
}

int tusbh_host_handler(tusb_mq_t* mq)
{
    tusb_msg_t msg;
    if(tusb_mq_get(mq, &msg)){
        if(msg.handler){
            msg.handler(&msg);
        }else{
            TUSB_LOGE("Unknown host message handler\n");
            return -1;
        }
    }
    return 0;
}


#define TUSBH_DEV_TAG  0x1242CDa2
tusbh_device_t* tusbh_new_device(void)
{
    uint32_t* res = tusb_malloc(sizeof(tusbh_device_t)+8);
    res[0] = TUSBH_DEV_TAG;
    return (tusbh_device_t*)(res+1);
}

void tusbh_free_device(tusbh_device_t* host_dev)
{
    uint32_t* res = (uint32_t*)host_dev;
    res -= 1;
    if(res[0] == TUSBH_DEV_TAG){
        if(host_dev->config_desc){
            traverse_periodic_in(host_dev, tusbh_deinit_periodic_xfer);
            tusb_free(host_dev->config_desc);
            host_dev->config_desc = 0;
        }
        tusb_free(res);
    }else{
        TUSB_LOGE("Host device memory leak @ %08lx\n", (uint32_t)host_dev);
    }
}

