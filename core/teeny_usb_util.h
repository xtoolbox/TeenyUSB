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

#ifndef __TEENY_USB_UTIL_H__
#define __TEENY_USB_UTIL_H__

#include "teeny_usb_def.h"
#include "teeny_usb_config.h"
#include "stdio.h"

/** printf function in teeny usb stack */
#ifndef TUSB_PRINTF
#define TUSB_PRINTF printf
#endif // #ifndef TUSB_PRINTF

/** debug info output function */
#ifndef TUSB_DBG
#define TUSB_DBG(level, fmt, ...)          \
    do                                     \
    {                                      \
        if (level >= TUSB_DBG_LEVEL)       \
        {                                  \
            TUSB_PRINTF(fmt, ##__VA_ARGS__); \
        }                                  \
    } while (0)
#endif // #ifndef TUSB_DBG

#define TUSB_LOGD(fmt, ...) TUSB_DBG(TUSB_DBG_LEVEL_DEBUG,   "[D] " fmt, ##__VA_ARGS__)
#define TUSB_LOGW(fmt, ...) TUSB_DBG(TUSB_DBG_LEVEL_WARNING, "[W] " fmt, ##__VA_ARGS__)
#define TUSB_LOGE(fmt, ...) TUSB_DBG(TUSB_DBG_LEVEL_ERROR,   "[E] " fmt, ##__VA_ARGS__)

void tusb_delay_ms(uint32_t ms);

#endif
