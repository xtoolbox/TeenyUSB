/*
 * tiny usb - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2018 XToolBox  - admin@xtoolbox.org
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

// tiny usb platform header for STM32F1/STM32F0 device
#ifndef __TEENY_USB_PLATFORM_H__
#define __TEENY_USB_PLATFORM_H__

#if defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F072xB) || defined(STM32F078xx) || defined(STM32F070xB)|| defined(STM32F070x6)
// HAL header for STM32F0xx devices
#include "stm32f0xx_hal.h"

#define USB_CLOCK_SOURCE_CRS
#define INTERNAL_PULLUP

#include "stm32f0f1_platform.h"

#elif defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || \
    defined(STM32F103xE) || defined(STM32F103xG) || \
    defined(STM32F105xC) || defined(STM32F107xC)

// HAL header for STM32F1xx devices
#include "stm32f1xx_hal.h"

#include "stm32f0f1_platform.h"

#elif defined (STM32F756xx) || defined (STM32F746xx) || defined (STM32F745xx) || defined (STM32F767xx) || \
      defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx) || defined (STM32F722xx) || \
      defined (STM32F723xx) || defined (STM32F732xx) || defined (STM32F733xx)

#include "stm32f7xx_hal.h"
#include "stm32f7_platform.h"

#if defined(STM32F723xx)
#define  OTG_HS_EMBEDDED_PHY
#else
#define  OTG_HS_EXTERNAL_PHY
#endif
#define  OTG_FS_EMBEDDED_PHY


#else

// Device unknown
#error  Device not supported
#endif



#endif

