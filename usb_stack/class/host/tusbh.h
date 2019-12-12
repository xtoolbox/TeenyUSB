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

#ifndef __TUSBH_H__
#define __TUSBH_H__

#include "teeny_usb.h"
#include "tusb_def.h"
#include "tusbh_os.h"
#include <stdio.h>

#define  USB_REQ_DIR_MASK                                  0x80U
#define  USB_H2D                                           0x00U
#define  USB_D2H                                           0x80U


#define TUSHH_ROOT_CHILD_COUNT   1
#define TUSBH_MAX_CHILD          4
#define TUSBH_MAX_CONFIG_LENGTH  256
#define TUSBH_MAX_INTERFACE      8
#define TUSBH_MAX_EP             4


typedef __PACK_BEGIN struct _usb_hub_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bNumPorts;
    uint16_t wHubCharacteristics;
    uint8_t  bPowerOnToPowerGood;
    uint8_t  bHubControlCurrent;
    uint8_t  bRemoveAndPowerMask[4];
} __PACK_END usb_hub_descriptor_t;


typedef struct _tusbh_device tusbh_device_t;
typedef struct _tusbh_interface tusbh_interface_t;
typedef struct _tusbh_ep_info tusbh_ep_info_t;
typedef struct _tusbh_class tusbh_class_t;

typedef struct _tusbh_interface_backend
{
    uint16_t vid;
    uint16_t pid;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t padding;
    int(*init)(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset);
    int(*deinit)(tusbh_device_t* dev, tusbh_interface_t* interface);
    int(*data_xfered)(tusbh_ep_info_t* ep);
    const char* desc;
}tusbh_interface_backend_t;

struct _tusbh_class
{
    const tusbh_interface_backend_t* backend;
};

typedef const tusbh_class_t* tusbh_class_reg_t;

///////////////////////////////////////////////////////////
/// Channel xfer done action handler struct
typedef struct _tusbh_xfered_action tusbh_xfered_action_t;
struct _tusbh_xfered_action{
    void(*func)(tusb_host_t* host, uint8_t hc_num, void* data);
};
// xfer done set event
typedef struct _tusbh_xfered_set_event tusbh_xfered_set_event_t;
struct _tusbh_xfered_set_event{
    void(*func)(tusb_host_t* host, uint8_t hc_num, tusbh_xfered_set_event_t* data);
    tusbh_evt_t* event;
};
// xfer done notify ep
typedef struct _tusbh_xfered_notify_ep tusbh_xfered_notify_ep_t;
struct _tusbh_xfered_notify_ep{
    void(*func)(tusb_host_t* host, uint8_t hc_num, tusbh_xfered_notify_ep_t* data);
    tusbh_ep_info_t* ep;
};


struct _tusbh_ep_info
{
    usb_endpoint_descriptor_t* desc;               /**< endpoint descriptor */
    tusbh_interface_t* interface;                  /**< interface of this ep */
    void*   data;                                  /**< data buffer for this endpoint */
    uint32_t data_len;                             /**< actual xfered data length of this endpoint */
    tusbh_xfered_notify_ep_t  ep_notify;           /**< endpoint data xfer done send a notify message */
    uint8_t remain_interval;                       /**< remain interval for periodic endpoint */
    uint8_t xfer_in_progress;                      /**< endpoint transfer in progress */
    int8_t pipe_num;                               /**< pipe number for this endpoint */
    uint8_t pipe_leak;                             /**< pipe leak count */
};

#define ep_addr(ep)       ((ep)->desc->bEndpointAddress)
#define ep_interface(ep)  ((ep)->interface)
#define ep_device(ep)     ((ep)->interface->device)
#define ep_host(ep)       ((ep)->interface->device->host)
#define ep_root(ep)       ((tusbh_root_hub_t*)(ep)->interface->device->host->user_data)
#define ep_class(ep, class_t)   ((class_t*)(ep)->interface->cls)

// Some device not support un-aligned access
#define EP_MPS(ep)  (\
    ((uint16_t)(((uint8_t*)(&((ep)->wMaxPacketSize)))[0]) << 0) \
  | ((uint16_t)(((uint8_t*)(&((ep)->wMaxPacketSize)))[1]) << 8))

#define  setup_ep_info(info, ep_desc, itf)              \
do{                                                     \
    (info)->interface = itf;                            \
    (info)->desc = (ep_desc);                           \
    (info)->xfer_in_progress = 0;                       \
    (info)->pipe_num = -1;                              \
    (info)->pipe_leak = 0;                              \
    (info)->data = 0;                                   \
    (info)->data_len = 0;                               \
    (info)->remain_interval = (ep_desc)->bInterval;     \
}while(0)

struct _tusbh_interface{
    tusbh_ep_info_t    endpoints[TUSBH_MAX_EP];
    usb_interface_descriptor_t* desc;              /**< interface descriptor of this interface */
    void*   info_pool;                             /**< dynamic allocate info */
    tusbh_device_t* device;                        /**< device of this interface */
    const tusbh_class_t* cls;                      /**< interface class */
    uint16_t interface_id;                         /**< inerface id in device */
    uint16_t ep_num;                               /**< endpoint number for this interface */
};


struct _tusbh_device{
    usb_device_descriptor_t device_desc;               /**< device descriptor */
    uint16_t pad;                                      /**< pad to 32 bit boundart */
    uint8_t config_desc[TUSBH_MAX_CONFIG_LENGTH];      /**< config descriptor */
    tusbh_interface_t interfaces[TUSBH_MAX_INTERFACE]; /**< Interfaces of this device */
    tusbh_device_t* children[TUSBH_MAX_CHILD];         /**< child device */
    tusbh_device_t* parent;                            /**< parent device */
    tusb_host_t*    host;                              /**< host controller for this device */
    int             ctrl_in;                           /**< Control In pipe */
    int             ctrl_out;                          /**< Control Out pipe */
    tusbh_xfered_set_event_t  xfer_evt;                /**< device endpoint xfer event handler */
    uint8_t address;                                   /**< device address */
    uint8_t speed;                                     /**< device speed   */
    uint8_t config;                                    /**< device current configuartion */
    uint8_t interface_num;                             /**< Interface number, interfaces under IAD treat as 1 interface */
};

#define dev_root(dev)  ((tusbh_root_hub_t*)(dev->host->user_data))

typedef struct _tusbh_root_hub{
    tusbh_msg_q_t*  mq;                                /**<  message q for use event */
    tusbh_device_t* children[TUSHH_ROOT_CHILD_COUNT];  /**<  root hub devices */
    uint32_t        addr_mask;                         /**<  address usage */
    uint8_t         conn_mask;                         /**<  port connect mask */
    uint8_t         en_mask;                           /**<  port enable mask */
    uint8_t         sof_in_progress;                   /**<  indicate the SOF message has been processed or not */
    uint8_t         padding;                           /**<  padding to 32bit boundary */
    void(*cb_device_ready)(tusbh_device_t* dev);       /**<  device ready callback */
    void(*cb_device_remove)(tusbh_device_t* dev);      /**<  device remove callback */
    void(*cb_sof)(tusb_host_t* host);                  /**<  SOF callback */
    const tusbh_class_reg_t* support_classes;          /**<  supported USB class of this root controller */
    const char*     id;                                /**<  root hub id */
    
}tusbh_root_hub_t;
#define host_root(host)  ((tusbh_root_hub_t*)(host->user_data))

#define create_info_pool(itf, info_type)    itf->info_pool = tusbh_malloc(sizeof(info_type))
#define tusbh_free_info(itf)                do{ if(itf->info_pool){ tusbh_free(itf->info_pool); itf->info_pool = 0; } }while(0)
#define tusbh_get_info(itf, info_type)      ((info_type*)itf->info_pool)


void tusb_host_init(tusb_host_t* host, tusbh_root_hub_t* root_hub_info);

void ls_usb(tusb_host_t* host);

void tusbh_msg_loop(tusbh_msg_q_t* mq);

int tusbh_control_xfer(tusbh_device_t* dev, uint8_t bmRequest, uint8_t bRequest, uint32_t value, uint32_t index, void* data, uint32_t len);

int tusbh_get_descriptor(tusbh_device_t* dev, uint8_t type, uint8_t index, void* data, int len);

int tusbh_allocate_pipe(tusbh_device_t* dev, uint8_t ep, uint8_t type, uint16_t mps);

void tusbh_close_pipe(tusbh_device_t* dev, int pipe_num);

void tusbh_ep_free_pipe(tusbh_ep_info_t* ep);

int tusbh_ep_allocate_pipe(tusbh_ep_info_t* ep);

int tusbh_ep_xfer(tusbh_ep_info_t* ep, void* data, uint16_t len, uint32_t timeout);

int tusbh_ep_xfer_with_event(tusbh_ep_info_t* ep, void* data, uint16_t len, tusbh_xfered_set_event_t* action, uint32_t timeout);

int tusbh_ep_clear_feature(tusbh_ep_info_t* ep);

#ifndef LOG_INFO
#define LOG_INFO 1
#endif

#ifndef TUSB_PRINTF
#define TUSB_PRINTF printf
#endif

#if LOG_INFO
#define  TUSB_DBG(fmt,  ...)           TUSB_PRINTF(fmt, ## __VA_ARGS__)
#else
static inline int dummy_printf(const char* fmt, ...) { (void)fmt; return 0; }
#define  TUSB_DBG(fmt,  ...)           dummy_printf(fmt, ## __VA_ARGS__)
#endif

#define  TUSB_ASSERT(exp)                                       \
do{                                                             \
    if(exp){}else{                                              \
        TUSB_DBG(#exp "%s:%d\n", __FUNCTION__, __LINE__);       \
        while(1);                                               \
}}while(0)


// LOG for better format
#define TUSB_OS_INFO(msg, ...)    TUSB_DBG("OS             " msg , ## __VA_ARGS__)
#define TUSB_HOST_INFO(msg, ...)  TUSB_DBG("HOST %s:       " msg , host_root(host)->id, ## __VA_ARGS__)
#define TUSB_ROOT_INFO(msg, ...)  TUSB_DBG("RHUB %s:%d      " msg , root->id, port, ## __VA_ARGS__)
#define TUSB_DEV_INFO(msg, ...)   TUSB_DBG("DEV  %s.%02x     " msg, dev_root(dev)->id, dev->address, ## __VA_ARGS__)
#define TUSB_ITF_INFO(msg, ...)   TUSB_DBG("ITF  %s.%02x.%d-%d " msg, dev_root(interface->device)->id, interface->device->address, interface->interface_id, interface->desc->bInterfaceNumber, ## __VA_ARGS__)
#define TUSB_HUB_INFO(msg, ...)   TUSB_DBG("SHUB %s.%02x:%d   " msg, dev_root(dev)->id, dev->address, port, ## __VA_ARGS__)
#define TUSB_EP_INFO(msg, ...)    TUSB_DBG("ENDP %s.%02x.%02x  " msg, ep_root(ep)->id, ep_device(ep)->address, ep_addr(ep), ## __VA_ARGS__)

#endif
