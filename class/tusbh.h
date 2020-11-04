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

#ifndef __TUSBH_H__
#define __TUSBH_H__

#include "teeny_usb.h"
#include "teeny_usb_def.h"
#include "teeny_usb_osal.h"
#include <stdio.h>

typedef struct _tusbh_class  tusbh_class_t;
typedef const tusbh_class_t* tusbh_class_cp;
typedef struct _tusbh_device tusbh_device_t;
typedef struct _tusbh_ep_info tusbh_ep_info_t;
typedef struct _tusbh_interface tusbh_interface_t;
typedef struct _tusbh_interface_backend tusbh_interface_backend_t;

#define  TUSBH_ROOT_PORT_COUNT        1
#define  TUSBH_ROOT_DEVICE_COUNT      32
#define  TUSBH_ITF_MAX_EP_COUNT       4
#define  TUSBH_MAX_CHILDREN_DEV_COUNT 4
#define  TUSBH_MAX_ITF_COUNT          4

typedef struct _tusbh_root_hub{
    tusb_mq_t*      mq;                                /**<  message q for use event */
    tusbh_device_t* children[TUSBH_ROOT_PORT_COUNT];    /**<  root hub direct connect devices */
    uint32_t        addr_mask;                         /**<  address usage, max 32 devices */
    uint32_t        conn_mask:8;                       /**<  port connect mask, max 8 port on root hub */
    uint32_t        en_mask:8;                         /**<  port enable mask, max 8 port on root hub */
    uint32_t        padding:16;                        /**<  padding to 32bit boundary */
    void(*cb_device_ready)(tusbh_device_t* dev);       /**<  device ready callback */
    void(*cb_device_remove)(tusbh_device_t* dev);      /**<  device remove callback */
    void(*cb_sof)(tusb_host_t* host);                  /**<  SOF callback */
    const tusbh_class_cp*    support_classes;          /**<  supported USB class of this root controller */
    const char*     id;                                /**<  root hub id */
}tusbh_root_hub_t;

struct _tusbh_ep_info
{
    usb_endpoint_descriptor_t* desc;
    tusbh_interface_t*         interface;
};

#define ep_addr(ep)       ((ep)->desc->bEndpointAddress)
#define ep_interface(ep)  ((ep)->interface)
#define ep_device(ep)     ((ep)->interface->device)
#define ep_host(ep)       ((ep)->interface->device->host)
#define ep_root(ep)       ((tusbh_root_hub_t*)(ep)->interface->device->host->user_data)
#define ep_class(ep, class_t)   ((class_t*)(ep)->interface->cls)

#define  setup_ep_info(info, ep_desc, itf)              \
do{                                                     \
    (info)->interface = itf;                            \
    (info)->desc = (ep_desc);                           \
}while(0)


struct _tusbh_interface{
    tusbh_ep_info_t 
        endpoints[TUSBH_ITF_MAX_EP_COUNT];         /**< interface endpoint descriptors */
    usb_interface_descriptor_t* desc;              /**< interface descriptor of this interface */
    void*   info_pool;                             /**< dynamic allocate info */
    tusbh_device_t* device;                        /**< device of this interface */
    const tusbh_class_t* cls;                      /**< interface class */
    uint16_t interface_id;                         /**< inerface id in device */
    uint16_t ep_count;                             /**< endpoint count of this interface */
};

#define create_info_pool(itf, info_type)    itf->info_pool = tusb_malloc(sizeof(info_type))
#define tusbh_free_info(itf)                do{ if(itf->info_pool){ tusb_free(itf->info_pool); itf->info_pool = 0; } }while(0)
#define tusbh_get_info(itf, info_type)      ((info_type*)itf->info_pool)

struct _tusbh_device
{
    usb_device_descriptor_t device_desc;               /**< device descriptor */
    uint16_t pad;                                      /**< pad to 32 bit boundart */
    uint8_t* config_desc;                              /**< config descriptor */
    tusbh_interface_t 
        interfaces[TUSBH_MAX_ITF_COUNT];               /**< Interfaces of this device */
    tusbh_device_t*
        children[TUSBH_MAX_CHILDREN_DEV_COUNT];        /**< children device */
    tusbh_device_t* parent;                            /**< parent device */
    tusb_host_t*    host;                              /**< host controller for this device */
    uint8_t address;                                   /**< device address */
    uint8_t speed;                                     /**< device speed   */
    uint8_t config;                                    /**< device current configuartion */
    uint8_t interface_count;                           /**< Interface count, interfaces under IAD treat as 1 interface */
};

struct _tusbh_interface_backend
{
    uint16_t vid;                       /**< match VID, 0 ignore */
    uint16_t pid;                       /**< match PID, 0 ignore */
    uint8_t bInterfaceClass;            /**< match class, 0 ignore */
    uint8_t bInterfaceSubClass;         /**< match subclass, 0 ignore */
    uint8_t bInterfaceProtocol;         /**< match protocol, 0 ignore */
    uint8_t padding;                    /**< padding to 32bit boundary */
    int(*init)(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset);
    int(*deinit)(tusbh_device_t* dev, tusbh_interface_t* interface);
    int(*data_xfered)(tusbh_ep_info_t* ep, const void* data, int length);
    const char* desc;
};

struct _tusbh_class
{
    const tusbh_interface_backend_t* backend;
};

void tusbh_post_msg(tusb_mq_t* mq, void* context, uint32_t param1, uint32_t param2, void(*handler)(tusb_msg_t*));

int tusbh_init(tusb_host_t* host, tusbh_root_hub_t* root_hub);

int tusbh_host_handler(tusb_mq_t* mq);

tusbh_device_t* tusbh_new_device(void);

void tusbh_free_device(tusbh_device_t* host_dev);


#define RTHUB_LOGD(fmt, ...)   TUSB_LOGD("[HOST] %s RHB:%02d "  fmt, root->id, port, ##__VA_ARGS__)
#define RTHUB_LOGW(fmt, ...)   TUSB_LOGW("[HOST] %s RHB:%02d "  fmt, root->id, port, ##__VA_ARGS__)
#define RTHUB_LOGE(fmt, ...)   TUSB_LOGE("[HOST] %s RHB:%02d "  fmt, root->id, port, ##__VA_ARGS__)

#define GetDevRoot(dev)       ((tusbh_root_hub_t*)((dev)->host->user_data))
#define DEV_LOGD(fmt, ...)     TUSB_LOGD("[HOST] %s DEV:%02d "  fmt, GetDevRoot(dev)->id, dev->address, ##__VA_ARGS__)
#define DEV_LOGW(fmt, ...)     TUSB_LOGW("[HOST] %s DEV:%02d "  fmt, GetDevRoot(dev)->id, dev->address, ##__VA_ARGS__)
#define DEV_LOGE(fmt, ...)     TUSB_LOGE("[HOST] %s DEV:%02d "  fmt, GetDevRoot(dev)->id, dev->address, ##__VA_ARGS__)

#define ITF_LOGD(fmt, ...)     TUSB_LOGD("[HOST] %s ITF:%02d.%d-%d "  fmt, GetDevRoot(interface->device)->id, interface->device->address, interface->interface_id, interface->desc->bInterfaceNumber, ##__VA_ARGS__)
#define ITF_LOGW(fmt, ...)     TUSB_LOGW("[HOST] %s ITF:%02d.%d-%d "  fmt, GetDevRoot(interface->device)->id, interface->device->address, interface->interface_id, interface->desc->bInterfaceNumber, ##__VA_ARGS__)
#define ITF_LOGE(fmt, ...)     TUSB_LOGE("[HOST] %s ITF:%02d.%d-%d "  fmt, GetDevRoot(interface->device)->id, interface->device->address, interface->interface_id, interface->desc->bInterfaceNumber, ##__VA_ARGS__)

#endif
