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

void console_uart_init(uint32_t baud)
{
    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
  
    __HAL_RCC_USART6_CONFIG(RCC_USART6CLKSOURCE_SYSCLK);
  
     /**USART6 GPIO Configuration
        PC7     ------> USART6_RX
        PC6     ------> USART6_TX
     */
    GPIOC->MODER &= ~ (MODE(3,6) | MODE(3,7));
    GPIOC->MODER |=   (MODE(2,6) | MODE(2,7));  
    GPIOC->OTYPER &= ~(TYPE(1,6) | TYPE(1,7));
    GPIOC->OSPEEDR |= (MODE(3,6) | MODE(3,7));
    GPIOC->AFR[0] &= ~(AF(0xf,6) | AF(0xf,7));
    GPIOC->AFR[0] |=  (AF(GPIO_AF8_USART6,6) | AF(GPIO_AF8_USART6,7));
  
    USART6->CR1 &=  ~USART_CR1_UE;
  
#define UART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                     USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8))

    MODIFY_REG(USART6->CR1, UART_CR1_FIELDS, 
      UART_WORDLENGTH_8B|UART_PARITY_NONE|UART_MODE_TX_RX|UART_OVERSAMPLING_16);
  
    MODIFY_REG(USART6->CR2, USART_CR2_STOP, UART_STOPBITS_1);
    
    // No flow control, 1 bit oversample
    MODIFY_REG(USART6->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), UART_ONE_BIT_SAMPLE_ENABLE);
    
    SystemCoreClockUpdate();
    
    USART6->BRR = UART_DIV_SAMPLING16(SystemCoreClock, baud);
    
    NVIC_SetPriority(USART6_IRQn, 0);
    NVIC_EnableIRQ(USART6_IRQn);
    
    USART6->CR1 |= USART_CR1_RXNEIE;
  
    CLEAR_BIT(USART6->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(USART6->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    
    USART6->CR1 |=  USART_CR1_UE;
}

void USART6_IRQHandler(void)
{
    if ( USART6->ISR & UART_FLAG_RXNE){
        char ch = USART6->RDR & 0xff;
        stdin_recvchar(ch);
        USART6->ICR = UART_FLAG_RXNE;
    }
    if( USART6->ISR & UART_FLAG_ORE){
        USART6->ICR = UART_FLAG_ORE;
    }
}

WEAK void stdin_recvchar(int ch)
{
    (void)ch;
}

void stdout_sendchar(int ch)
{
    while ((USART6->ISR & UART_FLAG_TC) == RESET);
    USART6->ICR = UART_CLEAR_TCF;
    USART6->TDR = (uint32_t)ch;
}

void stdio_init(void)
{
    console_uart_init(115200);
}






