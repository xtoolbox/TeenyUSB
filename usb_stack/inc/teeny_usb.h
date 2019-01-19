/*
 * teeny usb - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2018 XToolBox  - admin@xtoolbox.org
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

#ifndef __TEENY_USB_H__
#define __TEENY_USB_H__

#include "teeny_usb_platform.h"


#define USB_FEATURE_B_HNP_ENABLE                3       // SET FEATURE OTG - Enable B device to perform HNP
#define USB_FEATURE_A_HNP_SUPPORT               4       // SET FEATURE OTG - A device supports HNP
#define USB_FEATURE_A_ALT_HNP_SUPPORT           5       // SET FEATURE OTG - Another port on the A device supports HNP

typedef struct _tusb_setup_packet{
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} tusb_setup_packet;

#define LO_BYTE(x)   (uint8_t)(x)
#define HI_BYTE(x)  (uint8_t)((x)>>8)

typedef struct _tusb_ep_data
{
  const uint8_t*  tx_buf;       // tx buffer
  uint8_t*        rx_buf;       // rx buffer
  uint16_t        tx_size;      // data remain to transfer
  uint16_t        rx_size;      // rx buffer size
  uint16_t        rx_count;     // current received data
  uint16_t        tx_last_size; // last tx data length
#if defined(USB)
  uint32_t        tx_pushed;    // data packet number in the PMA buffer
#endif
}tusb_ep_data;

typedef const uint8_t* desc_t;

typedef struct _tusb_descriptors
{
  desc_t  device;
  desc_t  config;
  const desc_t*  strings;
  uint32_t string_cnt;
#if defined(HAS_WCID)
  desc_t  wcid_desc;
  desc_t  wcid_properties;
#endif
}tusb_descriptors;

typedef struct _tusb_core
{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           // USB hardware handle
#else
  void*                 handle_not_used;  // fix compile issue
#endif
}tusb_core_t;


#ifndef EP_NUM
#define EP_NUM 1
#endif

typedef struct _tusb_device tusb_device_t;
typedef void(*tusb_callback_t)(tusb_device_t* dev);
struct _tusb_device{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           // USB hardware handle
#endif
  uint8_t   addr;                         // device addr
  uint8_t   config;                       // device config
  uint8_t   alt_cfg;                      // device alg config
  uint8_t   remote_wakeup:1;              // device feature of remote wakeup
  uint8_t   b_hnp:1;                      // OTG only, hnp for b device
  uint8_t   a_hnp:1;                      // OTG only, hnp for a device
  uint8_t   a_alt_hnp:1;                  // OTG only, hnp for a device on other port
  uint8_t   flag_padding:4;               // padding the bit flags
  uint16_t  status;                       // device status
  uint16_t  padding;                      // padding to 32bit boundary
  tusb_callback_t ep0_tx_done;            // ep0 tx done callback
  tusb_callback_t ep0_rx_done;            // ep0 rx done callback
#if defined(NEED_MAX_PACKET)
  const uint8_t* rx_max_size;             // RX max size buffer
  const uint8_t* tx_max_size;             // TX max size buffer
#endif
  tusb_setup_packet  setup;               // setup packet buffer
  tusb_ep_data   Ep[EP_NUM];              // end point
  const tusb_descriptors*  descriptors;   // device descriptors, can be override at runtime
};


#define  RETRY_FOREVER              0xffffffff
#define  MAX_HC_NUM                 16

// Host channel data
typedef struct _tusb_hc_data
{
  uint8_t*  ch_buf;
  uint16_t  size;                    // size of the pipe buffer
  uint16_t  count;                   // data count of the pipe buffer
  uint32_t  nak_count;               // channel NAK count
  uint32_t  error_count;             // channel error count
  uint32_t  error_reason;            // channel error reason
  uint8_t   state;                   // channel state
  uint8_t   toggle_in:1;             // toggle bit for IN pipe
  uint8_t   toggle_out:1;            // toggle bit for OUT pipe
  uint8_t   do_ping:1;               // do ping flag
  uint8_t   is_use:1;                // used flag
  uint8_t   is_data:1;               // 1: data packet, 0: setup packet
  uint8_t   auto_free:1;             // 1: auto free when transaction done, 0: do not free
  uint8_t   xfer_done:1;             // 1: xfer done, 0: xfer on goning
  uint8_t   padding:1;               // bit field padding
  uint16_t  padding32;               // padding to 32bit boundary
}tusb_hc_data_t;

typedef struct _tusb_host{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           // USB hardware handle
#endif
  uint32_t  state;
  tusb_hc_data_t  hc[MAX_HC_NUM];         // host channel
}tusb_host_t;


#if defined(USB_CORE_HANDLE_TYPE)
#define  TUSB_DEVICE_SIZE   (sizeof(tusb_device_t) - sizeof(USB_CORE_HANDLE_TYPE))
#else
#define  TUSB_DEVICE_SIZE   sizeof(tusb_device_t)
#endif

typedef union _tusb_otg
{
  tusb_device_t device;
  tusb_host_t   host;
}tusb_otg_t;


typedef enum {
  TUSB_HS_DUMMY = 0,
  TUSB_HOST_PORT_DISCONNECTED,
  TUSB_HOST_PORT_CONNECTED,
  TUSB_HOST_PORT_ENABLED,
  TUSB_HOST_PORT_DISABLED,
}host_port_state_t;

typedef enum {
  TUSB_CS_INIT = 0,
  TUSB_CS_TRANSFER_COMPLETE = 1,
  TUSB_CS_NAK,
  TUSB_CS_ACK,
  TSUB_CS_PING_SUCCESS,
  TUSB_CS_NYET,
  TUSB_CS_STALL,
  TUSB_CS_INT_NAK,
  
  TUSB_CS_AHB_ERROR = 10,
  TUSB_CS_DT_ERROR,
  TUSB_CS_TRANSACTION_ERROR,
  TUSB_CS_FRAMEOVERRUN_ERROR,
  TUSB_CS_BABBLE_ERROR,
  
  TUSB_CS_XFER_ONGOING,
  TUSB_CS_UNKNOWN_ERROR,
}channel_state_t;


//////////////////////////////////////////////
// Device functions,  used in device only mode
//////////////////////////////////////////////
// Get USB device
// Param id:  0 - FS core
//            1 - HS core
tusb_device_t* tusb_get_device(uint8_t id);

// open USB in device mode, implement by platform
void tusb_open_device(tusb_device_t* dev);

// close USB device, implement by platform
void tusb_close_device(tusb_device_t* dev);

// return 0 for success, -1 for end point not ready or data is on going
int tusb_send_data(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len);

// set the recv buffer for end point, should be called in tusb_reconfig
// param  len must large than the end point packet size
// return value: 0 - means success
//              -1 - means fail
int tusb_set_recv_buffer(tusb_device_t* dev, uint8_t EPn, void* data, uint16_t len);

// set rx valid, when tusb_on_rx_done return value is not 0, the rx end point will hang
// use the method to enable it again
void tusb_set_rx_valid(tusb_device_t* dev, uint8_t EPn);

//////////////////////////////////
// user callback functions
//////////////////////////////////

// called when data transfer done
// WEAK function, default do nothing
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn);

// called when data receive done
// there are two conditions to trigger this callback
// 1. Received data is large or equal to the buffer length set by tusb_set_recv_buffer
// 2. Received data packed size is less than the end point max packet size
// return value:  0 - means received data processed, the previous set received buffer can be used
//        otherwise - means the data will be processed later, call tusb_set_rx_valid to enable receive again
//        in case of ISO out endpoint, return 0 will use current receive buffer to rx data again
//                                     otherwise the follow data will be dropped untill call tusb_set_rx_valid
// WEAK function, default return 0
int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len);

// called when USB device reset
// Setup the end point and device initial status here
// WEAK function, default do nothing
void tusb_reconfig(tusb_device_t* dev);

// called when got class specified request
// WEAK function, default send a zero length IN packet (which means status IN)
void tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup_req);

// get device report descripor when need
// WEAK function, default return 0, set length to 0
const uint8_t* tusb_get_report_descriptor(tusb_device_t* dev, tusb_setup_packet *req, uint16_t* len);


//////////////////////////////////////////////
// Host functions,  used in host only mode
//////////////////////////////////////////////
// Get USB host
// Param id:  0 - FS core
//            1 - HS core
tusb_host_t* tusb_get_host(uint8_t id);

// open USB core in host mode, implement by platform
void tusb_open_host(tusb_host_t* host);

// close USB host, implement by platform
void tusb_close_host(tusb_host_t* host);

// Reset the port
void tusb_host_port_reset(tusb_host_t* host, uint8_t port, uint8_t reset);

//////////////////////////////////
// Host callback functions
//////////////////////////////////

// WEAK function
// called when host channel action done or error
// return value: 0 - means the channel should keep
//       otherwise - halt the channel 
int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num);

// WEAK function
// called when port status changed
// when new connection detect, start enum device
void tusb_host_port_changed(tusb_host_t* host, host_port_state_t new_state);



//////////////////////////////////////////////
// OTG functions, used in DRD mode
//////////////////////////////////////////////
// When working in device mode, use the device related functions
// when working in host mode, use the host related functions
// open USB core in DRD mode, implement by platform
void tusb_open_otg(tusb_otg_t* otg);

// close USB core in DRD mode, implement by platform
void tusb_close_otg(tusb_otg_t* otg);


#endif
