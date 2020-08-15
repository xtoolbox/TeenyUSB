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
 
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#if defined(USE_RTTHREAD)
#include <rtthread.h>
#define RTOS_INTERRUPT_ENTER()  rt_interrupt_enter()
#define RTOS_INTERRUPT_LEAVE()  rt_interrupt_leave()
#define TUSB_PRINTF  rt_kprintf
#define TUSB_HAS_OS
#endif

#define  HOST_PORT_POWER_ON_HS()

#define  HOST_PORT_POWER_ON_FS() \
do{\
  __HAL_RCC_GPIOG_CLK_ENABLE();\
  GPIOG->MODER &= ~(GPIO_MODER_MODER0 << (6*2));\
  GPIOG->MODER |= (GPIO_MODER_MODER0_0 << (6*2));\
  GPIOG->BSRR = GPIO_PIN_6;\
}while(0)

#define  HOST_PORT_POWER_ON()  HOST_PORT_POWER_ON_FS()

// init the stdio hardware
void stdio_init(void);
// stdin recv char handler
void stdin_recvchar(int ch);
// stdout send char handler 
void stdout_sendchar(int ch);

#endif

