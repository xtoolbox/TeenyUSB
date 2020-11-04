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

#ifndef __TEENY_USB_HOST_DRIVER_H__
#define __TEENY_USB_HOST_DRIVER_H__

#include "stdint.h"

typedef struct _tusb_host_driver tusb_host_driver_t;
typedef struct _tusbh_transfer tusbh_transfer_t;

/** Host driver open
 *
 *  \ingroup Group_Host_Driver
 * 
 *  \param[out] drv        Pointer to host driver handle
 *  \param[in]  param      hardware parameter
 *  \return                0 success, otherwise fail
 */
int tusb_host_drv_open(tusb_host_driver_t **drv, const tusb_hardware_param_t *param, void *context);

/** Host driver get context
 *
 *  \ingroup Group_Device_Driver
 * 
 *  \param[in] drv         Device driver handle, init by \ref tusb_dev_drv_open
 *
 *  \return                driver context
 */
void* tusb_host_drv_get_context(tusb_host_driver_t *drv);

/** Host driver close
 *
 *  \ingroup Group_Host_Driver
 * 
 *  \param[in] drv         Pointer to host driver handle
 *  \return                0 success, otherwise fail
 */
int tusb_host_drv_close(tusb_host_driver_t *drv);

/** Host driver reset
 *
 *  \ingroup Group_Host_Driver
 * 
 *  \param[in] drv        Pointer to host driver handle
 *  \param[in] port       port id
 * 
 *  \return                0 success, otherwise fail
 */
int tusb_host_drv_reset(tusb_host_driver_t *drv, int port);

/** Host driver get port speed
 *
 *  \ingroup Group_Host_Driver
 *
 *  \param[in]  host           Pointer to host driver handle
 *  \param[in]  port           port id
 *
 *  \return   Port attached device speed: \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
uint8_t tusb_host_drv_get_speed(tusb_host_driver_t* drv, uint8_t port);

/** Host driver get frame number
 *
 *  \ingroup Group_Host_Driver
 *
 *  \param[in]  host           Pointer to host driver handle
 *
 *  \return   current frame number
 */
uint32_t tusb_host_drv_get_frame_number(tusb_host_driver_t* drv);

/** Host driver add transfer
 *
 *  \ingroup Group_Host_Driver
 * 
 *  \param[in] drv        Pointer to host driver handle
 *  \param[in] transfer   transfer info
 * 
 *  \return                >= 0 transfer id, otherwise fail
 */
int tusb_host_drv_transfer_add(tusb_host_driver_t *drv, tusbh_transfer_t* transfer);

/** Host driver cancel transfer
 *
 *  \ingroup Group_Host_Driver
 * 
 *  \param[in] drv        Pointer to host driver handle
 *  \param[in] transfer   transfer info
 * 
 *  \return                0 success, otherwise fail
 */
int tusb_host_drv_transfer_remove(tusb_host_driver_t *drv, tusbh_transfer_t* transfer);

#endif
