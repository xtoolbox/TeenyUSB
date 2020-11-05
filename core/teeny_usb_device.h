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


#ifndef __TEENY_USB_DEVICE_H__
#define __TEENY_USB_DEVICE_H__
#include "stdint.h"
#include "teeny_usb_config.h"
#include "teeny_usb_def.h"
#include "teeny_usb_device_driver.h"


/* Type Defines: */
/** Type define for setup packet
 */
typedef struct _tusb_setup_packet{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} tusb_setup_packet;

/**< forward declaration of \ref tusb_device_driver_t */
typedef struct _tusb_device_driver tusb_device_driver_t;

/** Type define for descriptor type
 */
typedef const uint8_t* desc_t;

/** Type define for extra descriptor
 */
typedef struct _tusb_extra_desc
{
    desc_t          wcid_desc;         /**< WCID 1.0 descriptor for windows campatible device */
    const desc_t*   wcid_properties;   /**< WCID 1.0 properties descriptor */

    desc_t          wcid_bos;          /**< WCID 2.0 descriptor for BOS */
    desc_t          wcid_desc_set;     /**< WCID 2.0 descriptor set */
}tusb_extra_desc_t;



/** Type define for device descriptor
 */
typedef struct _tusb_descriptors
{
    desc_t  device;                   /**< device descriptor */
    const desc_t*  configs;           /**< configuration descriptor array */
    uint32_t config_count;            /**< configuration descriptor count */
    const desc_t*  strings;           /**< string descriptor array */
    uint32_t string_cnt;              /**< string descriptor count */
    const tusb_extra_desc_t* extra;   /**< extra descriptors */
}tusb_descriptors_t;

typedef struct _tusb_device tusb_device_t; /**< forward declaration of \ref tusb_device_t */

/** Type define for endpoint 0 callback function
 */
typedef void(*tusb_callback_t)(tusb_device_t* dev, const void* data, int len);

#define SetBusPowered(dev)       do{ (dev)->status |= 0x01; }while(0)
#define ClearBusPowered(dev)     do{ (dev)->status &= ~0x01; }while(0)
#define IsBusPowered(dev)        (((dev)->status & 0x01) != 0)
#define SetRemoteWakeup(dev)     do{ (dev)->status |= USB_STATUS_REMOTE_WAKEUP; }while(0)
#define ClearRemoteWakeup(dev)   do{ (dev)->status &= ~USB_STATUS_REMOTE_WAKEUP; }while(0)
#define IsRemoteWakeup(dev)      (((dev)->status & USB_STATUS_REMOTE_WAKEUP) != 0)

/** Type define for usb device
 */
struct _tusb_device{
    tusb_device_driver_t*      dev_drv;           /**< device driver                                  */
    void*                      user_data;         /**<  User data for device                          */
    tusb_callback_t            ep0_tx_done;       /**< endpoint 0 transmit done callback              */
    tusb_callback_t            ep0_rx_done;       /**< endpoint 0 receive done callback               */
    tusb_setup_packet          setup;             /**< setup packet buffer                            */
    const tusb_descriptors_t*  descriptors;       /**< device descriptors, can be override at runtime */
    uint32_t                   addr:8;            /**< device address                                 */
    volatile uint32_t          config:8;          /**< device current configurtaion ID                */
    uint32_t                   b_hnp:1;           /**< OTG only, hnp for B device                     */
    uint32_t                   a_hnp:1;           /**< OTG only, hnp for A device                     */
    uint32_t                   a_alt_hnp:1;       /**< OTG only, hnp for A device on other port       */
    uint32_t                   flag_padding:13;   /**< padding the bit flags                          */
    uint16_t                   temp_buffer;       /**< temp buffer to hold data <= 2 bytes, 
                                                       must align at 32bit boundary                   */
    uint16_t                   status;            /**< device status                                  */
    uint8_t    alt_cfg[TUSB_MAX_INTERFACE_COUNT]; /**< device interface alernate configuration        */
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
    uint8_t  desc_buffer[DESCRIPTOR_BUFFER_SIZE]; /**< descriptor buffer, used for qualifier or DMA   */
#endif
};


#define GetDriver(dev)  (dev->dev_drv)
#define SetDescriptor(dev, desc)  do { (dev)->descriptors = (desc); } while(0)

//////////////////////////////////////////////
// Device functions,  used in device only mode
//////////////////////////////////////////////

/** Open USB device
 *
 *  \ingroup Group_Device
 * 
 *  \param[in] dev              USB device handle
 *  \param[in] driver_param     parameter pass to the driver
 *
 *  \return 0 for success, otherwise fail
 */
static inline int tusb_open_device(tusb_device_t* dev, const tusb_hardware_param_t* driver_param)
{ return tusb_dev_drv_open(&dev->dev_drv, driver_param, dev); }

/** close USB device 
 *
 *  \ingroup Group_Device
 *
 *  \param  dev          USB device handle, init by \ref tusb_open_device
 * 
 *  \return 0 for success, otherwise fail
 */
static inline int tusb_close_device(tusb_device_t* dev)
{ return tusb_dev_drv_close(GetDriver(dev)); }

/** get USB device current speed
 *
 *  \ingroup Group_Device
 *
 *  \param  dev          USB device handle, init by \ref tusb_open_device
 *
 *   \return Actual device speed, \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
static inline int tusb_get_device_speed(tusb_device_t* dev)
{ return tusb_dev_drv_get_speed(GetDriver(dev)); }

/** USB device send data
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         data pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          data length, set len to 0 to send zero length packet
 *  \param[in]  option       data send options, each of \c TUSB_TXF_* mask.
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
static inline int tusb_send_data(tusb_device_t* dev, uint8_t EPn, const void* data, int len, uint8_t option)
{ return tusb_dev_drv_send_data(GetDriver(dev), EPn, data, len, option); }

/** USB device cancel send, make the EP in NAK status
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number
 *
 *  \return  remain data count
 */
static inline int tusb_cancel_send(tusb_device_t* dev, uint8_t EPn)
{ return tusb_dev_drv_cancel_send(GetDriver(dev), EPn); }

/** USB device send status packet
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
static inline int tusb_send_status(tusb_device_t* dev)
{ return tusb_send_data(dev, 0, 0, 0, 0); }

/** USB device send control data
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  data         control data pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          control data length
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
static inline int tusb_control_send(tusb_device_t* dev, const void* data, int len)
{ return tusb_send_data(dev, 0, data, len,  (len)!=(dev)->setup.wLength ? TUSB_TXF_ZLP:0); }

/** USB device set endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number and direction, when endpoint is 0, both IN/OUT will be stalled
 */
static inline void tusb_set_stall(tusb_device_t* dev, uint8_t EPn)
{ tusb_dev_drv_set_stall(GetDriver(dev), EPn); }

/** USB device clear endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number and direction
 */
static inline void tusb_clear_stall(tusb_device_t* dev, uint8_t EPn)
{ tusb_dev_drv_clear_stall(GetDriver(dev), EPn); }


/** USB device set the recv buffer for endpoint
 *  Call this function before \ref tusb_set_rx_valid
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number
 *  \param[out] data         receive buffer pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          buffer length, length must be N * MPS, where MPS is endpoint max packet size
 *
 *  \return  0 for success, otherwise fail
 */
static inline int tusb_set_recv_buffer(tusb_device_t* dev, uint8_t EPn, void* data, int len)
{ return tusb_dev_drv_set_recv_buffer( GetDriver(dev), EPn, data, len); }

/** USB device set the Rx/In endpoint valid
 *  Call \ref tusb_set_recv_buffer before set endpoint receive valid
 *  When the return value of callback function \ref tusb_on_rx_done is other than 0
 *  the endpoint will stop receive data, call this function to set receive endpoint valid
 *  again
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, init by \ref tusb_open_device
 *  \param[in]  EPn          endpoint number
 */
static inline void tusb_set_rx_valid(tusb_device_t* dev, uint8_t EPn)
{ tusb_dev_drv_set_rx_valid( GetDriver(dev), EPn); }


//////////////////////////////////
// driver callback functions
//////////////////////////////////

/** Called by driver when endpoint transfer complete
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Diver_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the transmit complete event
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         data buffer
 *  \param[in]  len          actual xfer data length
 *  \param[in]  isSetup      0 - normal data, 1 - setup data
 * 
 *  \return          0 - means received data processed, the Rx/OUT endpoint will set to valid again, previous set received buffer will be used
 *           otherwise - means the data will be processed later, call \ref tusb_set_rx_valid to enable the Rx/OUT endpoint again
 *               in case of ISO out endpoint, return 0 will use current receive buffer to rx data again
 *               otherwise the follow data will be dropped untill call tusb_set_rx_valid
 */
int tusb_device_ep_xfer_done(tusb_device_driver_t* drv, uint8_t EPn, const void* data, int len, uint8_t isSetup);

/** Called by driver when a reset event occur
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Diver_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the transmit complete event
 */
void tusb_device_reset(tusb_device_driver_t* drv);


//////////////////////////////////
// user callback functions
//////////////////////////////////

/** Called when data transmit complete
 *  This is a WEAK function, default do nothing
 *  user application can override it to get the data transmit complete event 
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the transmit complete event
 *  \param[in]  EPn          endpoint number
 */
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn, const void* data, int len);

/** called when data receive complete
 *  This is a WEAK function, default always return 0, this will set Rx/OUT endpoint valid again
 *  user application can override it to get the data send done event 
 *  there are two conditions to trigger this callback
 *    1. Received data is large or equal to the buffer length set by tusb_set_recv_buffer
 *    2. Received data packed size is less than the end point max packet size
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the receive complete event
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         receive data point buffer, this is the same of data paramter in \ref tusb_set_recv_buffer function
 *  \param[in]  len          actual receive data length
 *
 *  \return          0 - means received data processed, the Rx/OUT endpoint will set to valid aging, previous set received buffer will be used
 *           otherwise - means the data will be processed later, call \ref tusb_set_rx_valid to enable this endpoint again
 *               in case of ISO out endpoint, return 0 will use current receive buffer to rx data again
 *               otherwise the follow data will be dropped untill call tusb_set_rx_valid        
 */
int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, int len);

/** Called when USB device configured
 *  This is a WEAK function, default do nothing
 *  user application can override it to setup the endpoint and initial device status
 *
 *  \ingroup Group_Device
 * 
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the reset event
 */
void tusb_reconfig(tusb_device_t* dev);

/** Called when USB device need handle class request
 *  This is a WEAK function, default return 0
 *  user application can override it to handle the class request
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device need handle class request
 *  \param[in]  setup        Setup packet pointer
 *
 *  \return       0 - the setup request is not processed by class, will be handle in TeenyUSB stack
 *                1 - the setup request is processed by class, TeenyUSB stack will ignore it
 * 
 *  \remark    the class request not only receive class request, it receive all request before it pass
 *             to the teeny usb stack
 */
int tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup);


#endif
