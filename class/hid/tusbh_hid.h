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

#ifndef __TUSBH_HID_H__
#define __TUSBH_HID_H__

#include "tusbh.h"

typedef struct _tusbh_hid_class
{
    const tusbh_interface_backend_t* backend;    
    int(*on_recv_data)(tusbh_ep_info_t* ep, const uint8_t* data, int len);
    int(*on_send_done)(tusbh_ep_info_t* ep, const uint8_t* data, int len);
}tusbh_hid_class_t;

typedef struct _tusbh_boot_key_class
{
    const tusbh_interface_backend_t* backend;    
    int(*on_key)(tusbh_ep_info_t* ep, const uint8_t* key);
}tusbh_boot_key_class_t;

typedef struct _tusbh_boot_mouse_class
{
    const tusbh_interface_backend_t* backend;    
    int(*on_mouse)(tusbh_ep_info_t* ep, const uint8_t* mouse);
}tusbh_boot_mouse_class_t;

typedef struct _tusbh_hid_info
{
    tusbh_ep_info_t* ep_in;
    tusbh_ep_info_t* ep_out;
    uint8_t*         report_desc;
    uint32_t         report_desc_len;
}tusbh_hid_info_t;


int tusbh_hid_send_data(tusbh_ep_info_t* ep, void* data, int len);

int tusbh_set_keyboard_led(tusbh_ep_info_t* ep, uint8_t leds);

extern const tusbh_interface_backend_t  tusbh_hid_backend;
extern const tusbh_interface_backend_t  tusbh_boot_mouse_backend;
extern const tusbh_interface_backend_t  tusbh_boot_keyboard_backend;


#endif
