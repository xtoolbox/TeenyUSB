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

#ifndef __TUSBD_CDC_H__
#define __TUSBD_CDC_H__
#include "tusbd.h"
#include "tusb_cdc.h"

typedef struct _tusb_cdc_device tusb_cdc_device_t;

/** Type define for CDC device
 *  Add capacity D3 in CDC ACM descriptor to enable the set break feature
 */
struct _tusb_cdc_device
{
    tusb_device_t* dev;
    const tusb_device_backend_t* backend;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t ep_int;
    uint8_t ep_int_busy;
    int(* on_recv_data)(tusb_cdc_device_t* cdc, const void* data, uint16_t len);
    int(* on_send_done)(tusb_cdc_device_t* cdc, const void* data, uint16_t len);
    void*    rx_buf;
    uint32_t rx_size;
    void(* on_line_coding_change)(tusb_cdc_device_t* cdc);
    void(* on_line_state_change)(tusb_cdc_device_t* cdc, uint16_t state);
    void(* on_set_break)(tusb_cdc_device_t* cdc, uint16_t break_time_in_ms);
    void* user_data;
    tusb_cdc_line_coding_t line_coding;
    tusb_cdc_state_t state;
};


/**  send CDC data */
#define tusb_cdc_device_send(cdc, data, len) \
  tusb_send_data( (cdc)->dev, (cdc)->ep_in, data, len, TUSB_TXF_ZLP)

/** send CDC state when the input line state changed or error occurs */
int tusb_cdc_send_state(tusb_cdc_device_t* cdc);

extern const tusb_device_backend_t cdc_device_backend;

#endif
