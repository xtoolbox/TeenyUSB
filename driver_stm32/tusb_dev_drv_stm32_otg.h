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
 * Copyright (c) 2020 XToolBox - admin@xtoolbox.org
 * www.tusb.org
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
 
#ifndef __TUSB_DEV_DRV_STM32_OTG_H__
#define __TUSB_DEV_DRV_STM32_OTG_H__

#include "stdint.h"
#include "teeny_usb_def.h"

#define  TUSB_STM32_OTG_FS_CORE     0
#define  TUSB_STM32_OTG_HS_CORE     1

#define  TUSB_STM32_OTG_INTERNAL_PHY    0x04
#define  TUSB_STM32_OTG_DMA             0x08
#define  TUSB_STM32_OTG_VBUS_DETECT     0x10


typedef struct _tusb_stm32_otg_io_cfg
{
    void* IO_BASE;
    uint16_t pin;
    uint16_t af;
}tusb_stm32_otg_io_cfg_t;


struct _tusb_hardware_param
{
    uint32_t is_hs_core : 1;
    uint32_t is_high_speed : 1;
    uint32_t is_internal_phy:1;
    uint32_t dma_enable:1;
    uint32_t sof_enable:1;
    uint32_t low_power_enable:1;
    uint32_t lpm_enable:1;
    uint32_t battery_charging_enable:1;
    
    uint32_t vbus_sensing_enable:1;
    uint32_t use_dedicated_ep1:1;
    uint32_t use_external_vbus:1;
    
    uint32_t padding:21;
    const tusb_stm32_otg_io_cfg_t* io_cfgs;
};

extern const tusb_hardware_param_t otg_default_param;

#ifndef TUSB_DEFAULT_DEV_PARAM
#define TUSB_DEFAULT_DEV_PARAM    (&otg_default_param)
#endif



#endif
