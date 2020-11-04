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


////////////////////////////////////////////////
/// TeenyUSB board related API
////////////////////////////////////////////////

#include "stm32f7xx_hal.h"
#include "teeny_usb_def.h"
#include "tusb_dev_drv_stm32_otg.h"
#include <errno.h>
#include <stdio.h>

#define MODE(x, pin)  ((x) << ((pin)*2))
#define TYPE(x, pin)  ((x) << ((pin)))
#define AF(x,  pin)    (((pin)<8) ? ((uint32_t)(x) << ((pin)*4)) : ((uint32_t)(x) << ( ((pin)-8)*4)) )

void console_uart_init(uint32_t baud)
{
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
  
    __HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_SYSCLK);
  
     /**USART3 GPIO Configuration
        PC9     ------> USART3_RX
        PC8     ------> USART3_TX
     */
    GPIOD->MODER &= ~ (MODE(3,8) | MODE(3,9));
    GPIOD->MODER |=   (MODE(2,8) | MODE(2,9));  
    GPIOD->OTYPER &= ~(TYPE(1,8) | TYPE(1,9));
    GPIOD->OSPEEDR |= (MODE(3,8) | MODE(3,9));
    GPIOD->AFR[1] &= ~(AF(0xf,8) | AF(0xf,9));
    GPIOD->AFR[1] |=  (AF(GPIO_AF7_USART3,8) | AF(GPIO_AF7_USART3,9));
  
    USART3->CR1 &=  ~USART_CR1_UE;
  
#define UART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                     USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8))

    MODIFY_REG(USART3->CR1, UART_CR1_FIELDS, 
      UART_WORDLENGTH_8B|UART_PARITY_NONE|UART_MODE_TX_RX|UART_OVERSAMPLING_16);
  
    MODIFY_REG(USART3->CR2, USART_CR2_STOP, UART_STOPBITS_1);
    
    // No flow control, 1 bit oversample
    MODIFY_REG(USART3->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), UART_ONE_BIT_SAMPLE_ENABLE);
    
    SystemCoreClockUpdate();
    
    USART3->BRR = UART_DIV_SAMPLING16(SystemCoreClock, baud);
    
    NVIC_SetPriority(USART3_IRQn, 0);
    NVIC_EnableIRQ(USART3_IRQn);
    
    USART3->CR1 |= USART_CR1_RXNEIE;
  
    CLEAR_BIT(USART3->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(USART3->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    
    USART3->CR1 |=  USART_CR1_UE;
}

WEAK void stdin_recvchar(int ch)
{
    (void)ch;
}

void USART3_IRQHandler(void)
{
    if ( USART3->ISR & UART_FLAG_RXNE){
        char ch = USART3->RDR & 0xff;
        stdin_recvchar(ch);
        USART3->ICR = UART_FLAG_RXNE;
    }
    if( USART3->ISR & UART_FLAG_ORE){
        USART3->ICR = UART_FLAG_ORE;
    }
}

void stdout_sendchar(int ch)
{
    while ((USART3->ISR & UART_FLAG_TC) == RESET);
    USART3->ICR = UART_CLEAR_TCF;
    USART3->TDR = (uint32_t)ch;
}

void stdio_init(void)
{
    console_uart_init(115200);
}

const tusb_stm32_otg_io_cfg_t f767_otg_default_io[] = {
    {GPIOA, 11, GPIO_AF10_OTG_FS},
    {GPIOA, 12, GPIO_AF10_OTG_FS},
    {0,0,0},
};


const tusb_hardware_param_t otg_default_param = {
    .is_hs_core = 0,
    .is_high_speed = 0,
    .is_internal_phy = 1,
    .dma_enable = 0,
    .sof_enable = 0,
    .low_power_enable = 0,
    .lpm_enable = 0,
    .battery_charging_enable = 0,
    
    .vbus_sensing_enable = 0,
    .use_dedicated_ep1 = 0,
    .use_external_vbus = 0,
    .io_cfgs = f767_otg_default_io,
};

