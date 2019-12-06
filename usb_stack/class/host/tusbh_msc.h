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

#ifndef __TUSBH_MSC_H__
#define __TUSBH_MSC_H__

#include "tusbh.h"
#include "tusb_msc.h"

typedef struct _tusbh_block_info
{
    uint32_t block_count;
    uint32_t block_size;
    scsi_inquiry_std_response_t inquiry;
}tusbh_block_info_t;

typedef struct _tusbh_msc_info
{
    tusbh_ep_info_t* in_ep;
    tusbh_ep_info_t* out_ep;
    uint8_t max_lun;
    uint8_t pad1;
    uint8_t pad2;
    uint8_t pad3;
    tusbh_block_info_t* blocks;
    scsi_sense_fixed_resp_t sense;
}tusbh_msc_info_t;

typedef struct _tusbh_msc_class
{
    const tusbh_interface_backend_t* backend;    
    int (*mount)(tusbh_interface_t* itf, int max_lun, const tusbh_block_info_t* blocks);
    int (*unmount)(tusbh_interface_t* itf);
}tusbh_msc_class_t;

extern const tusbh_interface_backend_t  tusbh_msc_bot_backend;

int tusbh_msc_block_read(tusbh_interface_t* itf, int lun, uint32_t blockAddr, uint32_t blockCount, void* buffer);
int tusbh_msc_block_write(tusbh_interface_t* itf, int lun, uint32_t blockAddr, uint32_t blockCount, void* buffer);
// return  1: success,  0 : not ready
int tusbh_msc_is_unit_ready(tusbh_interface_t* itf, int lun);

#endif
