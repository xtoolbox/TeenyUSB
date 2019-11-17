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

#include "teeny_usb.h"
#include "tusbh.h"
#include "tusbh_vendor.h"
#include "tusbh_hub.h"
#include "tusbh_hid.h"

extern uint32_t SystemCoreClock;

static __IO uint32_t tick_ms;
void SysTick_Handler(void)
{
  tick_ms++;
}

// host need accurate delay
void tusb_delay_ms(uint32_t ms)
{
  uint32_t t = tick_ms;
  while( tick_ms - t < ms);
}


static tusbh_root_hub_t root_fs;
static tusbh_root_hub_t root_hs;
static const tusbh_boot_key_class_t cls_boot_key = {
    .backend = &tusbh_boot_keyboard_backend,
    // .on_key = process_key
};

static const tusbh_boot_mouse_class_t cls_boot_mouse = {
    .backend = &tusbh_boot_mouse_backend,
    // .on_mouse = process_mouse
};

static const tusbh_hid_class_t cls_hid = {
    .backend = &tusbh_hid_backend,
    //.on_recv_data = process_hid_recv,
    //.on_send_done = process_hid_sent,
};

static const tusbh_hub_class_t cls_hub = {
    .backend = &tusbh_hub_backend,
};

static const tusbh_vendor_class_t cls_vendor = {
    .backend = &tusbh_vendor_backend,
    //.transfer_done = process_vendor_xfer_done
};

static const tusbh_class_reg_t class_table[] = {
    (tusbh_class_reg_t)&cls_boot_key,
    (tusbh_class_reg_t)&cls_boot_mouse,
    (tusbh_class_reg_t)&cls_hub,
    (tusbh_class_reg_t)&cls_hid,
    (tusbh_class_reg_t)&cls_vendor,
    0,
};

void open_console(uint32_t baud);

int main()
{
    SystemCoreClockUpdate();
    
    open_console(115200);
  
    SysTick_Config(SystemCoreClock/1000);
  
    TUSB_OS_INFO("Start USB host\n");
    tusb_host_t* fs = tusb_get_host(USB_CORE_ID_FS);
    tusb_host_t* hs = tusb_get_host(USB_CORE_ID_HS);
    tusbh_msg_q_t* mq = tusbh_mq_create();
    tusbh_mq_init(mq);
  
    HOST_PORT_POWER_ON_FS();
    HOST_PORT_POWER_ON_HS();
  
    // two host core use same message queue, 
    // so we can manage them in single thread
    root_fs.mq = mq;
    root_fs.id = "FS";
    root_fs.support_classes = class_table;
  
    root_hs.mq = mq;
    root_hs.id = "HS";
    root_hs.support_classes = class_table;
  
    tusb_host_init(fs, &root_fs);
    tusb_host_init(hs, &root_hs);

    tusb_open_host(fs);
    tusb_open_host(hs);

    while(1){
        // there is only one message q for every thing
        tusbh_msg_loop(mq);
        
        //tusbh_msg_loop(root_fs.mq);
        //tusbh_msg_loop(root_hs.mq);
    }
}

#ifdef DEBUG

#define LOG_SIZE   1024
static int hc_log_index;
static channel_state_t hc_log_buf[LOG_SIZE];
static USB_OTG_HostChannelTypeDef hc_info;
static USB_OTG_HostChannelTypeDef* hc_reg;
static tusb_hc_data_t* hc_data;
static uint8_t hc_no;
void hc_log_begin(tusb_host_t* host, uint8_t hc_num)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
    USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
    tusb_hc_data_t* hc = &host->hc[hc_num];
    hc_reg = HC;
    hc_data = hc;
    hc_no = hc_num;
    hc_info.HCCHAR   = HC->HCCHAR;
    hc_info.HCSPLT   = HC->HCSPLT;
    hc_info.HCINTMSK = HC->HCINTMSK;
    hc_info.HCTSIZ   = HC->HCTSIZ;
    hc_info.HCDMA    = HC->HCDMA;
    hc_log_index = 0;
}

void hc_log_data(tusb_host_t* host, uint8_t hc_num, uint32_t data)
{
    if(hc_log_index<LOG_SIZE){
        hc_log_buf[hc_log_index] = (channel_state_t)data;
        hc_log_index++;
    }
}

void hc_log_end(tusb_host_t* host, uint8_t hc_num)
{
}

#endif


#ifdef STM32F723xx

#define MODE(x, pin)  ((x) << ((pin)*2))
#define TYPE(x, pin)  ((x) << ((pin)))
#define AF(x,  pin)    (((pin)<8) ? ((uint32_t)(x) << ((pin)*4)) : ((uint32_t)(x) << ( ((pin)-8)*4)) )

void open_console(uint32_t baud)
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
    
    USART6->BRR = UART_DIV_SAMPLING16(SystemCoreClock, baud);
    
  
    CLEAR_BIT(USART6->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(USART6->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    
    USART6->CR1 |=  USART_CR1_UE;
}

int tusb_putchar(int ch){
    // TODO putchar
    while ((USART6->ISR & UART_FLAG_TC) == RESET);
    USART6->ICR = UART_CLEAR_TCF;
    USART6->TDR = (uint32_t)ch;
    while ((USART6->ISR & UART_FLAG_TC) == RESET);
    return 0;
}

#else
#warning Device print channel not set

#endif
