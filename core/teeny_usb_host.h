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


#ifndef __TEENY_USB_HOST_H__
#define __TEENY_USB_HOST_H__
#include "stdint.h"
#include "teeny_usb_config.h"
#include "teeny_usb_def.h"
#include "teeny_usb_host_driver.h"



/** Type define for USB host */

typedef struct _tusbh_periodic_xfer tusbh_periodic_xfer_t;

typedef struct _tusb_host{
    tusb_host_driver_t*    host_drv;           /**< host driver */
    void*                  user_data;          /**< User data for host */
    tusbh_periodic_xfer_t* periodic_queue;     /**< periodic xfer queue */
    uint16_t               last_frame;         /**< last frame number for periodic xfer */
    uint16_t               periodic_pending;   /**< periodic action pending count */
}tusb_host_t;


typedef struct _tusbh_transfer tusbh_transfer_t;
typedef struct _tusbh_drv_xfer_data tusbh_drv_xfer_data_t;

#define  SPLIT_ISO_MIDDLE       0x00
#define  SPLIT_ISO_BEGIN        0x02
#define  SPLIT_ISO_END          0x01
#define  SPLIT_ISO_FULL         0x03

typedef enum
{
  TS_IDLE = 0,
  TS_ACK,
  TS_NAK,
  TS_NYET,
  TS_STALL,
  TS_ERROR,
  TS_TIMEOUT,
  TS_NO_CHANNEL,
} tusb_transfer_state_t;

struct _tusbh_transfer{
    void*      data;                              /**< transfer data pointer */
    uint32_t   total_length;                      /**< total data length */
    uint32_t   actual_length;                     /**< actual transfer data length */
    tusbh_drv_xfer_data_t*   drv_data;            /**< transfer data used by driver */
    void* context;                                /**< user data */
    void  (*cb_complete)(tusbh_transfer_t*);      /**< transfer complete callback */
    
    uint8_t  hub_addr;                            /**< hub address for split transfer */
    uint8_t  hub_port;                            /**< hub port for split transfer */
    uint8_t  is_split:1;                          /**< is split transfer or not */
    uint8_t  is_sc:1;                             /**< is split complete transfer or not */
    uint8_t  is_setup:1;                          /**< is setup packet or not */
    uint8_t  st_stage:5;                          /**< split trasnfer stage */
    uint8_t  split_iso_type;                      /**< split ISO out transfer type,
                                                       SPLIT_ISO_MIDDLE,SPLIT_ISO_BEGIN,SPLIT_ISO_END,SPLIT_ISO_FULL */
    
    uint8_t  dev_addr;                            /**< device address */
    uint8_t  dev_speed;                           /**< device speed, PORT_SPEED_HIGH, PORT_SPEED_FULL, PORT_SPEED_LOW */
    uint8_t  ep_num;                              /**< endpoint number, bit0-6: endpoint address, bit 7: direction*/
    uint8_t  ep_type;                             /**< endpoint type, 
                                                   USBD_EP_TYPE_CTRL, USBD_EP_TYPE_ISOC, USBD_EP_TYPE_BULK, USBD_EP_TYPE_INTR*/
    
    uint16_t ep_mps;                              /**< endpoint max packet size */
    uint16_t padding2;                            /**< padding to 32bit boundary */
    
    tusb_transfer_state_t   state;                /**< transfer state */
};


/* Enums: */
/** Enums for \ref tusb_host_port_changed  new_state parameter
 *  indicating the port state
 */
typedef enum {
  TUSB_HOST_PORT_DUMMY = 0,     /**< Initial state */
  TUSB_HOST_PORT_DISCONNECTED,  /**< Host port is disconnected */
  TUSB_HOST_PORT_CONNECTED,     /**< Host port connected */
  TUSB_HOST_PORT_ENABLED,       /**< Host port enabled, this is done by fire a reset when in connected state */
  TUSB_HOST_PORT_DISABLED,      /**< Host port disabled */
}host_port_state_t;

//////////////////////////////////////////////
// Host functions,  used in host only mode
//////////////////////////////////////////////

/** Open USB host
 *
 *  \ingroup Group_Device
 * 
 *  \param[in] host             USB host handle
 *  \param[in] driver_param     parameter pass to the driver
 *
 *  \return 0 for success, otherwise fail
 */
int tusb_open_host(tusb_host_t* host, const tusb_hardware_param_t* driver_param);

/** Close USB host 
 *
 *  \ingroup Group_Host
 *
 *  \param  host          USB host handle, init by \ref tusb_open_host
 * 
 *  \return 0 for success, otherwise fail
 */
static inline int tusb_close_host(tusb_host_t* host)
{ return tusb_host_drv_close(host->host_drv); }

/** Reset USB host port
 *
 *  \ingroup Group_Host
 *
 *  \param  host          USB host handle, init by \ref tusb_open_host
 *  \param  port          port id
 * 
 *  \return 0 for success, otherwise fail
 */
static inline int tusb_host_reset(tusb_host_t* host, int port)
{ return tusb_host_drv_reset(host->host_drv, port); }


/** Get USB host port speed
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, init by \ref tusb_open_host
 *  \param[in]  port           port id
 *
 *  \return   Port attached device speed: \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
static inline uint8_t tusb_host_get_speed(tusb_host_t* host, uint8_t port)
{ return tusb_host_drv_get_speed(host->host_drv, port); }

/** Get USB host frame number
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, init by \ref tusb_open_host
 *
 *  \return   current frame number
 */
static inline uint32_t tusb_host_get_frame_number(tusb_host_t* host)
{ return tusb_host_drv_get_frame_number(host->host_drv); }

/** Add a transfer to host process queue
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, init by \ref tusb_open_host
 *  \param[in]  transfer       transfer info
 *
 *  \return  0 success,  other error
 *  
 *  \remark  we can add a same transfer more than one time, if we don't remove it, the driver
 *           will try to use the same transfer pipe
 *           e.g.
 *           transfer = malloc(...);
 *           fill_the_transfer
 *           tusb_host_transfer_add(transfer)
 *           // add again without remove, driver will use the same pipe
 *           tusb_host_transfer_add(transfer)
 *           // remove the transfer
 *           tusb_host_transfer_remove(transfer)
 *           re fill_the_transfer
 *           // driver may use new pipe to handle the transfer
 *           tusb_host_transfer_add(transfer)
 *           // free the transfer from driver
 *           tusb_host_transfer_remove(transfer)
 * 
 */
static inline int tusb_host_transfer_add(tusb_host_t* host, tusbh_transfer_t* transfer)
{ return tusb_host_drv_transfer_add(host->host_drv, transfer); }

/** Remove a transfer from host process queue
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, init by \ref tusb_open_host
 *  \param[in]  transfer       transfer info
 *
 *  \return  0 success,  other error
 */
static inline int tusb_host_transfer_remove(tusb_host_t* host, tusbh_transfer_t* transfer)
{ return tusb_host_drv_transfer_remove(host->host_drv, transfer); }


//////////////////////////////////
// host callback functions
//////////////////////////////////

/** Called by driver when host port state changed
 *
 *  \ingroup Group_Host
 *  \ingroup Group_Diver_Event
 *
 *  \param[in]  drv          USB host driver handle
 *  \param[in]  port         port number
 *  \param[in]  new_state    new port state
 * 
 *  \return     0 for success, other fail
 */
int tusb_host_port_changed(tusb_host_driver_t* drv, int port, host_port_state_t new_state);

/** Called by driver when SOF event fired
 *
 *  \ingroup Group_Host
 *  \ingroup Group_Diver_Event
 *
 *  \param[in]  drv          USB host driver handle
 * 
 *  \return     0 for success, other fail
 */
int tusb_host_sof_event(tusb_host_driver_t* drv);

#endif

