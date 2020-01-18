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


////////////////////////////////////////////////
/// TeenyUSB board related API
////////////////////////////////////////////////

#include "teeny_usb.h"
#include <errno.h>
#include <stdio.h>

#define MODE(x, pin)  ((x) << ((pin)*2))
#define TYPE(x, pin)  ((x) << ((pin)))
#define AF(x,  pin)    (((pin)<8) ? ((uint32_t)(x) << ((pin)*4)) : ((uint32_t)(x) << ( ((pin)-8)*4)) )
void SystemCoreClockUpdate(void);
void console_uart_init(uint32_t baud)
{
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
  
    //__HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_SYSCLK);
  
     /**USART3 GPIO Configuration
        PC11    ------> USART6_RX
        PC10    ------> USART6_TX
     */
    GPIOC->MODER &= ~ (MODE(3,10) | MODE(3,11));
    GPIOC->MODER |=   (MODE(2,10) | MODE(2,11));  
    GPIOC->OTYPER &= ~(TYPE(1,10) | TYPE(1,11));
    GPIOC->OSPEEDR |= (MODE(3,10) | MODE(3,11));
    GPIOC->AFR[1] &= ~(AF(0xf,10) | AF(0xf,11));
    GPIOC->AFR[1] |=  (AF(GPIO_AF7_USART3,10) | AF(GPIO_AF7_USART3,11));
  
    USART3->CR1 &= ~USART_CR1_UE;
  
    MODIFY_REG(USART3->CR2, USART_CR2_STOP, UART_STOPBITS_1);
    
    MODIFY_REG(USART3->CR1, (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | \
                                   USART_CR1_RE | USART_CR1_OVER8),
                                   UART_WORDLENGTH_8B|UART_PARITY_NONE|UART_MODE_TX_RX|UART_OVERSAMPLING_16);
                                   
    MODIFY_REG(USART3->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), UART_HWCONTROL_NONE);
    
    SystemCoreClockUpdate();
    
    USART3->BRR = UART_BRR_SAMPLING16(SystemCoreClock/4, baud);
    
    NVIC_SetPriority(USART3_IRQn, 0);
    NVIC_EnableIRQ(USART3_IRQn);
    
    USART3->CR1 |= USART_CR1_RXNEIE;
  
    CLEAR_BIT(USART3->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(USART3->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    
    USART3->CR1 |=  USART_CR1_UE;
}

void USART3_IRQHandler(void)
{
    uint32_t sr = USART3->SR;
    if ( (sr & UART_FLAG_RXNE) ){
        char ch = USART3->DR & 0xff;
        stdin_recvchar(ch);
    }
    if( sr & UART_FLAG_ORE){
        char ch = USART3->DR & 0xff;
        (void)ch;
    }
}

WEAK void stdin_recvchar(int ch)
{
    (void)ch;
}

void stdout_sendchar(int ch)
{
    while ((USART3->SR & UART_FLAG_TC) == RESET);
    USART3->DR = (uint32_t)ch;
}

void stdio_init(void)
{
    console_uart_init(115200);
}
