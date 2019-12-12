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

#ifndef __TUSBH_CDC_RNDIS_H__
#define __TUSBH_CDC_RNDIS_H__

#include "tusbh.h"
#include "tusb_cdc.h"
#include "ndis.h"
#include "rndis_protocol.h"

typedef struct _tusbh_cdc_rndis_class
{
    const tusbh_interface_backend_t* backend;
    
    // return 1 will break the default rndis init sequence
    int(* mount)(tusbh_interface_t* itf);
    int(* unmount)(tusbh_interface_t* itf);
    // return 1 means the msg has been processed
    int(* on_response)(tusbh_interface_t* itf, rndis_generic_msg_t* msg);
}tusbh_cdc_rndis_class_t;

int tusbh_rndis_send_message(tusbh_interface_t* itf, rndis_generic_msg_t* msg);

int tusbh_rndis_send_packet(tusbh_interface_t* itf, void* data, uint32_t len, uint32_t timeout);

int tusbh_rndis_recv_packet(tusbh_interface_t* itf, void* data, uint32_t len, uint32_t timeout);

int tusbh_rndis_send_packet_with_event(tusbh_interface_t* itf, void* data, uint32_t len, tusbh_xfered_set_event_t* action, uint32_t timeout);

int tusbh_rndis_recv_packet_with_event(tusbh_interface_t* itf, void* data, uint32_t len, tusbh_xfered_set_event_t* action, uint32_t timeout);

int tusbh_rndis_keepalive(tusbh_interface_t* interface);
    

extern const tusbh_interface_backend_t  tusbh_cdc_rndis_backend;

#endif
