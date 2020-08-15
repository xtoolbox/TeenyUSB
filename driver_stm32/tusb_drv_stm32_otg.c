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

#include "teeny_usb_util.h"
#include "teeny_usb_def.h"
#include "teeny_usb_device.h"
#include "tusb_platform_stm32.h"
#include "string.h"

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)

#define AFL(val, pin)   ((val)<< (( (pin))*4))
#define AFH(val, pin)   ((val)<< (( (pin)-8)*4))
void tusb_stm32_init_otg_io(const tusb_stm32_otg_io_cfg_t* cfg)
{
    while(cfg->IO_BASE){
        GPIO_TypeDef* GPIO = (GPIO_TypeDef*)cfg->IO_BASE;
        uint32_t pos = ((uint32_t)GPIO - GPIOA_BASE) / 0x400;
        SET_BIT(RCC->AHB1ENR, (1<<pos));
        
        uint8_t pin = cfg->pin;
        uint8_t af = cfg->af;
        GPIO->MODER &= ~(GPIO_MODER_MODER0 << (pin*2));
        GPIO->MODER |= (GPIO_MODER_MODER0_1 << (pin*2));
        GPIO->OTYPER &= ~(GPIO_OTYPER_OT_0 << pin);
        GPIO->OSPEEDR |= ( (GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR0_1)  << (pin*2) );
        if(pin > 7){
            GPIO->AFR[1] &= ~( AFH(0xf,pin));
            GPIO->AFR[1] |= ( AFH(af,pin));
        }else{
            GPIO->AFR[0] &= ~( AFL(0xf,pin));
            GPIO->AFR[0] |= ( AFL(af,pin));
        }
        cfg++;
    }
}

void tusb_delay_ms(uint32_t ms);

void HAL_Delay(uint32_t ms)
{
    tusb_delay_ms(ms);
}

void stm32_otg_device_handler(int is_hs);
void stm32_otg_host_handler(int is_hs);

#ifdef USB_OTG_FS
void OTG_FS_IRQHandler(void)
{
#ifdef  RTOS_INTERRUPT_ENTER
    RTOS_INTERRUPT_ENTER();
#endif
#ifndef NO_DEVICE
    stm32_otg_device_handler(0);
#endif
#ifndef NO_HOST
    stm32_otg_host_handler(0);
#endif
#ifdef  RTOS_INTERRUPT_LEAVE
    RTOS_INTERRUPT_LEAVE();
#endif
}
#endif

#ifdef USB_OTG_HS
void OTG_HS_IRQHandler(void)
{
#ifdef  RTOS_INTERRUPT_ENTER
    RTOS_INTERRUPT_ENTER();
#endif
#ifndef NO_DEVICE
    stm32_otg_device_handler(1);
#endif
#ifndef NO_HOST
    stm32_otg_host_handler(1);
#endif
#ifdef  RTOS_INTERRUPT_LEAVE
    RTOS_INTERRUPT_LEAVE();
#endif
}
#endif

#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)

