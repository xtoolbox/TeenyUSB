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

#ifndef __TEENY_USB_DEVICE_DRIVER_H__
#define __TEENY_USB_DEVICE_DRIVER_H__

#include "stdint.h"

typedef struct _tusb_device_driver tusb_device_driver_t;

typedef struct _tusb_ep_config
{
    uint8_t addr;
    uint8_t attribute;
    uint16_t mps;
} tusb_ep_config;

/** Device driver open
 *
 *  \ingroup Group_Device_Driver
 * 
 *  \param[out] drv        Pointer to device driver handle,
 *  \param[in]  param      driver parameter
 *  \return                0 success, otherwise fail
 */
int tusb_dev_drv_open(tusb_device_driver_t **drv, const tusb_hardware_param_t *param, void *context);

/** Device driver get context
 *
 *  \ingroup Group_Device_Driver
 * 
 *  \param[in] drv         Device driver handle, init by \ref tusb_dev_drv_open
 *
 *  \return                driver context
 */
void *tusb_dev_drv_get_context(tusb_device_driver_t *drv);

/** Device driver close
 *
 *  \ingroup Group_Device_Driver
 * 
 *  \param[in] drv         Device driver handle, init by \ref tusb_dev_drv_open
 *
 *  \return                0 success, otherwise fail
 */
int tusb_dev_drv_close(tusb_device_driver_t *drv);

/** Device driver setup endpoint
 *
 *  \ingroup Group_Device_Driver
 * 
 *  \param[in] drv         Device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in] ep_config   endpoint config array
 *  \param[in] count       endpoint count
 *  \param[in] is_reset    endpoint setup reason, 1 - device reset, 0 - set configuration
 *
 *  \return                0 success, otherwise fail
 */
int tusb_dev_drv_setup_endpoint(tusb_device_driver_t *drv, const tusb_ep_config *ep_config, int count, int is_reset);

/** get USB device current speed
 *
 *  \ingroup Group_Device
 *
 *  \param  drv          USB device driver handle, init by \ref tusb_dev_drv_open
 *
 *  \return Actual device speed, \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
int tusb_dev_drv_get_speed(tusb_device_driver_t *drv);

/** set USB device address
 *
 *  \ingroup Group_Device
 *
 *  \param  drv                USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param  addr               device address
 *  \param  after_status_out   0 - set address before status out, 1 - set address after status out
 *
 *  \return Actual device speed, \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
int tusb_dev_drv_set_address(tusb_device_driver_t *drv, uint8_t addr, uint8_t after_status_out);

/** USB device send data
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  drv          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         data pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          data length, set len to 0 to send zero length packet
 *  \param[in]  option       data send options, each of \c TUSB_TXF_* mask.
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
int tusb_dev_drv_send_data(tusb_device_driver_t *drv, uint8_t EPn, const void *data, int len, uint8_t option);

/** USB device cancel send, make the EP in NAK status
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number
 *
 *  \return  remain data count
 */
int tusb_dev_drv_cancel_send(tusb_device_driver_t *drv, uint8_t EPn);

/** USB device set endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number and direction, when endpoint is 0, both IN/OUT will be stalled
 */
void tusb_dev_drv_set_stall(tusb_device_driver_t *drv, uint8_t EPn);

/** USB device clear endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number and direction
 */
void tusb_dev_drv_clear_stall(tusb_device_driver_t *drv, uint8_t EPn);

/** USB device set the recv buffer for endpoint
 *  Call this function before \ref tusb_set_rx_valid
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number
 *  \param[out] data         receive buffer pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          buffer length, length must be N * MPS, where MPS is endpoint max packet size
 *
 *  \return  0 for success, otherwise fail
 */
int tusb_dev_drv_set_recv_buffer(tusb_device_driver_t *drv, uint8_t EPn, void *data, int len);

/** USB device set the Rx/In endpoint valid
 *  Call \ref tusb_set_recv_buffer before set endpoint receive valid
 *  When the return value of callback function \ref tusb_on_rx_done is other than 0
 *  the endpoint will stop receive data, call this function to set receive endpoint valid
 *  again
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number
 */
void tusb_dev_drv_set_rx_valid(tusb_device_driver_t *drv, uint8_t EPn);

#endif
