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

#include "teeny_usb_device.h"
#include "teeny_usb_driver.h"
#include "teeny_usb_util.h"
#include "teeny_usb_def.h"
#include "string.h"

/**
 * TeenyUSB device driver need implement these functions:
 * 
 * tusb_dev_drv_open
 * tusb_dev_drv_close
 * tusb_dev_drv_setup_endpoint
 * tusb_dev_drv_get_context
 * tusb_dev_drv_get_speed
 * tusb_dev_drv_set_address
 * tusb_dev_drv_send_data
 * tusb_dev_drv_cancel_send
 * tusb_dev_drv_set_stall
 * tusb_dev_drv_clear_stall
 * tusb_dev_drv_set_recv_buffer
 * tusb_dev_drv_set_rx_valid
 * 
 * TeenyUSB device driver need call these functions:
 * 
 * tusb_device_ep_xfer_done
 * tusb_device_reset
 * 
 */

struct _tusb_device_driver{
    void* context;
};

static tusb_device_driver_t  stm32_dev_drv;

int tusb_dev_drv_open(tusb_device_driver_t** drv, const tusb_device_driver_param_t* param, void* context)
{
    // TODO: parse the param
    if(drv){
        stm32_dev_drv.context = context;
        *drv = &stm32_dev_drv;
    }
}

void* tusb_dev_drv_get_context(tusb_device_driver_t* drv)
{
    return drv->context;
}

int tusb_dev_drv_close(tusb_device_driver_t* drv)
{
    return 0;
}

int tusb_dev_drv_setup_endpoint(tusb_device_driver_t* drv, const tusb_ep_config* ep_config, int count)
{
    return 0;
}

int tusb_dev_drv_get_speed(tusb_device_driver_t* drv)
{
    return PORT_SPEED_FULL;
}

int tusb_dev_drv_set_address(tusb_device_driver_t* drv, uint8_t addr, uint8_t flag)
{
    if(flag){
        // FS device must set address after status out

    }
    return 0;
}

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
int tusb_dev_drv_send_data(tusb_device_driver_t* drv, uint8_t EPn, const void* data, uint16_t len, uint8_t option);

/** USB device cancel send, make the EP in NAK status
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number
 *
 *  \return  remain data count
 */
int tusb_dev_drv_cancel_send(tusb_device_driver_t* drv, uint8_t EPn);

/** USB device set endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number and direction, when endpoint is 0, both IN/OUT will be stalled
 */
void tusb_dev_drv_set_stall(tusb_device_driver_t* drv, uint8_t EPn);

/** USB device clear endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device driver handle, init by \ref tusb_dev_drv_open
 *  \param[in]  EPn          endpoint number and direction
 */
void tusb_dev_drv_clear_stall(tusb_device_driver_t* drv, uint8_t EPn);


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
int tusb_dev_drv_set_recv_buffer(tusb_device_driver_t* drv, uint8_t EPn, void* data, uint16_t len);

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
void tusb_dev_drv_set_rx_valid(tusb_device_driver_t* drv, uint8_t EPn);