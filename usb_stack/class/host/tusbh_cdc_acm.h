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

#ifndef __TUSBH_CDC_ACM_H__
#define __TUSBH_CDC_ACM_H__

#include "tusbh.h"
#include "tusb_cdc.h"

typedef struct _tusbh_cdc_acm_class
{
    const tusbh_interface_backend_t* backend;
    int(*on_line_status_changed)(tusbh_interface_t* itf, tusb_cdc_line_state_t state);
}tusbh_cdc_acm_class_t;

int tusbh_cdc_set_line_coding(tusbh_interface_t* itf, tusb_cdc_line_coding_t* line_coding);

int tusbh_cdc_get_line_coding(tusbh_interface_t* itf, tusb_cdc_line_coding_t* line_coding);

int tusbh_cdc_set_line_state(tusbh_interface_t* itf, uint8_t state);

int tusbh_cdc_set_break(tusbh_interface_t* itf, uint16_t duration_in_ms);

int tusbh_cdc_send_data(tusbh_interface_t* itf, void* data, uint32_t len, uint32_t timeout);

int tusbh_cdc_recv_data(tusbh_interface_t* itf, void* data, uint32_t len, uint32_t timeout);

extern const tusbh_interface_backend_t  tusbh_cdc_acm_backend;


#endif
