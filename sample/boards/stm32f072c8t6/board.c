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

#include "stm32f0xx_hal.h"
#include "teeny_usb_def.h"

static int flash_init;

static void wait_flash(void)
{
  int cnt = 0x100000;
  while( (FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY){
    if(cnt<=0)break;
    cnt--;
  }
  if ( (FLASH->SR & FLASH_FLAG_EOP) == FLASH_FLAG_EOP ){
    FLASH->SR = FLASH_FLAG_EOP;
  }
}


static void erase_page(uint32_t addr)
{
    wait_flash();

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = addr;
    FLASH->CR |= FLASH_CR_STRT;

    wait_flash();
    FLASH->CR &= ~FLASH_CR_PER;
}

static void wirte_flash(uint32_t addr, const uint8_t* buf, uint32_t size)
{

    wait_flash();
    FLASH->CR |= FLASH_CR_PG;
    while(size>=2){
        *((uint16_t*)addr) = (buf[0]) | ((uint16_t)buf[1]<<8);
        wait_flash();
        size-=2;
        buf+=2;
        addr+=2;
    }
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

static void flash_unlock()
{
  if ( FLASH->CR & FLASH_CR_LOCK)
  {
    /* Authorize the FLASH Registers access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

void flash_write(uint32_t addr, const uint8_t* buf, uint32_t size)
{
    if(size % FLASH_PAGE_SIZE){
        while(1);
    }
    if(!flash_init){
        flash_init = 1;
        flash_unlock();
    }
    while(size >= FLASH_PAGE_SIZE){
        erase_page(addr);
        wirte_flash(addr,buf, FLASH_PAGE_SIZE);
        size-=FLASH_PAGE_SIZE;
        addr+=FLASH_PAGE_SIZE;
    }
}


void console_uart_init(uint32_t baud)
{   
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // USART1 TX PA9   AF1
    // USART1 RX PA10  AF1
    GPIOA->OTYPER |= GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10;
    GPIOA->MODER &= ~(GPIO_MODER_MODER9|GPIO_MODER_MODER10);
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10;
    
    
    GPIOA->AFR[1] |= ((1<<GPIO_AFRH_AFSEL9_Pos) | (1<<GPIO_AFRH_AFSEL10_Pos));
    
    SystemCoreClockUpdate();
    USART1->BRR = SystemCoreClock / baud;
    
    
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
    
    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

WEAK void stdin_recvchar(int ch)
{
    (void)ch;
}

void USART1_IRQHandler(void)
{
    if ( USART1->ISR & USART_ISR_RXNE){
        char ch = USART1->RDR & 0xff;
        stdin_recvchar(ch);
    }
    if( USART1->ISR & USART_ISR_ORE){
        USART1->ICR = USART_ICR_ORECF;
    }
}

void stdout_sendchar(int ch)
{
    while ((USART1->ISR & USART_ISR_TC) == RESET);
    //USART1->ICR = USART_ICR_TC;
    USART1->TDR = (uint32_t)ch;
}

void stdio_init(void)
{
    console_uart_init(115200);
}

