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

#ifndef __TUSBH_XFER_H__
#define __TUSBH_XFER_H__

#include "teeny_usb.h"
#include "teeny_usb_def.h"
#include "teeny_usb_osal.h"
#include "tusbh.h"
#include <stdio.h>

/** USB host start a transaction
 *
 *  \ingroup Group_Host
 *
 *  \param  xfer          transfer parameter
 *  \param  dev           host device
 *  \param  data          data
 *  \param  length        data length
 *  \param  is_setup      0: not setup data, 1: setup data
 *  \param  ep_num        endpoint number, msb is direction
 *  \param  ep_type       endpoint type
 * 
 *  \return  transaction state
 */
tusb_transfer_state_t tusbh_transaction(tusbh_transfer_t* xfer, tusbh_device_t* dev, void* data, uint32_t length, uint8_t is_setup, uint8_t ep_num, uint8_t ep_type);


/** USB host start a control transfer
 *
 *  \ingroup Group_Host
 *
 *  \param  dev           host device
 *  \param  bmRequest     request type
 *  \param  bRequest      request
 *  \param  value         wValue
 *  \param  index         wIndex
 *  \param  data          data pointer
 *  \param  len           control transfer data length
 * 
 *  \return  0:success, other fail
 */
int tusbh_control_xfer(tusbh_device_t* dev, uint8_t bmRequest, uint8_t bRequest, uint32_t value, uint32_t index, void* data, uint32_t len);

/** USB host get descriptor
 *
 *  \ingroup Group_Host
 *
 *  \param  dev           host device
 *  \param  type          descriptor type
 *  \param  index         descriptor index
 *  \param  data          data pointer
 *  \param  len           control transfer data length
 * 
 *  \return  0:success, other fail
 */
int tusbh_get_descriptor(tusbh_device_t* dev, uint8_t type, uint8_t index, void* data, int len);

/** USB host set address
 *
 *  \ingroup Group_Host
 *
 *  \param  dev           host device
 *  \param  addr          new device address
 * 
 *  \return  0:success, other fail
 */
int tusbh_set_address(tusbh_device_t* dev, uint8_t addr);

/** USB host set configuration
 *
 *  \ingroup Group_Host
 *
 *  \param  dev           host device
 *  \param  config        new device configuration
 * 
 *  \return  0:success, other fail
 */
int tusbh_set_config(tusbh_device_t* dev, uint8_t config);

/** USB host clear endpoint feature
 *
 *  \ingroup Group_Host
 *
 *  \param  dev           host device
 *  \param  ep_num        endpoint number
 * 
 *  \return  0:success, other fail
 */
int tusbh_clear_ep_feature(tusbh_device_t* dev, uint8_t ep_num);


int tusbh_init_periodic_xfer(tusbh_ep_info_t* ep);

int tusbh_deinit_periodic_xfer(tusbh_ep_info_t* ep);

int tusbh_handle_periodic_xfer(tusb_host_t* host);

#endif
