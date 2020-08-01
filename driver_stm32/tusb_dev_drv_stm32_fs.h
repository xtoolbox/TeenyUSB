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
 
#ifndef __TUSB_DEV_DRV_STM32_FS_H__
#define __TUSB_DEV_DRV_STM32_FS_H__

#define  TUSB_STM32_FS_INTERNAL_PULLUP    1
#define  TUSB_STM32_FS_HSI_CLOCK          2

#define  TUSB_BUILD_PARAM(flags)    (void*)(flags)

#ifndef  TUSB_DEFAULT_DEV_PARAM
#define  TUSB_DEFAULT_DEV_PARAM      TUSB_BUILD_PARAM(TUSB_STM32_FS_INTERNAL_PULLUP | TUSB_STM32_FS_HSI_CLOCK)
#endif

#define  IS_INTERNAL_PULLUP(param)   ((((int)(param)) & TUSB_STM32_FS_INTERNAL_PULLUP) != 0)
#define  IS_HSI_CLOCK(param)         ((((int)(param)) & TUSB_STM32_FS_HSI_CLOCK) != 0)


#endif
