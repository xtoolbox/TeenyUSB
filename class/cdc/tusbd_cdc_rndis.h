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

#ifndef __TUSBD_CDC_RNDIS_H__
#define __TUSBD_CDC_RNDIS_H__
#include "tusbd.h"
#include "tusb_cdc.h"
#include "rndis_protocol.h"

#define RNDIS_MTU        1500                           /* MTU value */

#define ETH_HEADER_SIZE             24
#ifndef ETH_MAX_PACKET_SIZE
#define ETH_MAX_PACKET_SIZE         ETH_HEADER_SIZE + RNDIS_MTU
#endif
#define ETH_MIN_PACKET_SIZE         60
#define RNDIS_RX_BUFFER_SIZE        (ETH_MAX_PACKET_SIZE + sizeof(rndis_data_packet_t))
    

typedef struct _tusb_rndis_device tusb_rndis_device_t;

struct _tusb_rndis_device
{
    tusb_device_t* dev;
    const tusb_device_backend_t* backend;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t ep_int;
    uint8_t ep_int_busy;
    int(* on_recv_data)(tusb_rndis_device_t* cdc, const void* data, int len);
    int(* on_send_done)(tusb_rndis_device_t* cdc, const void* data, int len);
    int(* eth_linkchange)(tusb_rndis_device_t* cdc, int linkup);
    int(* config_param)(tusb_rndis_device_t* cdc, const char* name, const char* data, int name_len, int data_len);
    uint8_t    rx_buf[ ((sizeof(rndis_data_packet_t) + ETH_MAX_PACKET_SIZE+3) / 4) * 4];
    uint8_t    tx_buf[ ((sizeof(rndis_data_packet_t) + ETH_MAX_PACKET_SIZE+3) / 4) * 4];
    uint8_t    notify_buffer[8];
    uint8_t    encapsulated_buffer[256];
    uint32_t  link_speed;
    uint32_t  oid_packet_filter;
    void*     user_data;
    uint8_t   mac_addr[6];
    uint8_t   link_status:1;
    uint8_t   txok:1;
    uint8_t   rxok:1;
    uint8_t   txbad:1;
    uint8_t   rxbad:1;
    uint8_t   pad:3;
    rndis_state_t state;
};


int tusb_rndis_send_packet(tusb_rndis_device_t* cdc, const void* data, int len);


extern const tusb_device_backend_t rndis_device_backend;

#endif
