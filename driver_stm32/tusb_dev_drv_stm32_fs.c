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

/**
 * TeenyUSB device driver need implement these functions:
 * 
 * tusb_dev_drv_open
 * tusb_dev_drv_close
 * tusb_dev_drv_setup_endpoint
 * tusb_dev_drv_get_context
 * tusb_dev_drv_get_speed
 * tusb_dev_drv_set_address
 * tusb_dev_drv_send_data
 * tusb_dev_drv_cancel_send
 * tusb_dev_drv_set_stall
 * tusb_dev_drv_clear_stall
 * tusb_dev_drv_set_recv_buffer
 * tusb_dev_drv_set_rx_valid
 * 
 * TeenyUSB device driver need call these functions:
 * 
 * tusb_device_ep_xfer_done
 * tusb_device_reset
 * 
 */

#include "teeny_usb_device.h"
#include "teeny_usb_device_driver.h"
#include "teeny_usb_util.h"
#include "teeny_usb_def.h"
#include "tusb_platform_stm32.h"
#include "tusb_dev_drv_stm32_fs.h"
#include "string.h"

#ifdef USB

#ifndef PMA_SIZE
#define PMA_SIZE 512
#endif

#ifndef STM32_MAX_EP_COUNT
#define STM32_MAX_EP_COUNT 8
#endif

void tusb_delay_ms(uint32_t ms);

// IN/OUT structure for endpoint
typedef struct _tusb_ep_data
{
    const uint8_t *tx_buf;           /**< data transmit buffer for IN endpoint */
    uint8_t *rx_buf;                 /**< data receive buffer for OUT endpoint */
    int32_t  tx_remain_size;         /**< data reamin size in transmit buffer */
    int32_t  tx_total_size;          /**< total transmit data length */
    int32_t  rx_size;                /**< data buffer total size in receive buffer */
    int32_t  rx_count;               /**< current received data length */
    uint16_t tx_last_size;           /**< last transmit data length */
    uint16_t tx_need_zlp : 1;        /**< need transmit zero length packet */
    uint16_t bit_field_padding : 7;  /**< padding to 16 bit boundary */
    uint16_t tx_pushed : 8;          /**< data packet number in the PMA buffer */
} tusb_ep_data;

struct _tusb_device_driver
{
    void *context;
    tusb_ep_data Ep[STM32_MAX_EP_COUNT];
    uint16_t mps_in[STM32_MAX_EP_COUNT];
    uint16_t mps_out[STM32_MAX_EP_COUNT];
};

#define GetInMaxPacket(drv, EPn) ((drv)->mps_in[(EPn)])
#define GetOutMaxPacket(drv, EPn) ((drv)->mps_out[(EPn)])

// STM32 FS device driver instance
static tusb_device_driver_t stm32_dev_drv;

#define IMR_MSK ((USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM))

#ifndef USB_CLR_DIR
#define USB_CLR_DIR ((uint16_t)~USB_ISTR_DIR)
#endif
#ifndef USB_CLR_ESOF
#define USB_CLR_ESOF ((uint16_t)~USB_ISTR_ESOF)
#endif
#ifndef USB_CLR_SOF
#define USB_CLR_SOF ((uint16_t)~USB_ISTR_SOF)
#endif
#ifndef USB_CLR_RESET
#define USB_CLR_RESET ((uint16_t)~USB_ISTR_RESET)
#endif
#ifndef USB_CLR_SUSP
#define USB_CLR_SUSP ((uint16_t)~USB_ISTR_SUSP)
#endif
#ifndef USB_CLR_WKUP
#define USB_CLR_WKUP ((uint16_t)~USB_ISTR_WKUP)
#endif
#ifndef USB_CLR_ERR
#define USB_CLR_ERR ((uint16_t)~USB_ISTR_ERR)
#endif
#ifndef USB_CLR_PMAOVR
#define USB_CLR_PMAOVR ((uint16_t)~USB_ISTR_PMAOVR)
#endif
#ifndef USB_CLR_CTR
#define USB_CLR_CTR ((uint16_t)~USB_ISTR_CTR)
#endif

#if defined(STM32F3)
#define AFL(val, pin) ((val) << (((pin)) * 4))
#define AFH(val, pin) ((val) << (((pin)-8) * 4))
static void set_io_af_mode(GPIO_TypeDef *GPIO, uint8_t pin, uint8_t af)
{
    GPIO->MODER &= ~(GPIO_MODER_MODER0 << (pin * 2));
    GPIO->MODER |= (GPIO_MODER_MODER0_1 << (pin * 2));
    GPIO->OTYPER &= ~(GPIO_OTYPER_OT_0 << pin);
    GPIO->OSPEEDR |= ((GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR0_1) << (pin * 2));
    if (pin > 7)
    {
        GPIO->AFR[1] &= ~(AFH(0xf, pin));
        GPIO->AFR[1] |= (AFH(af, pin));
    }
    else
    {
        GPIO->AFR[0] &= ~(AFL(0xf, pin));
        GPIO->AFR[0] |= (AFL(af, pin));
    }
}
#endif

static void tusb_disconnect(tusb_device_driver_t *dev)
{
#if defined(STM32F0)
//#ifdef USB_FS_INTERNAL_PULLUP
    if(GetUSB(drv)->BCDR & USB_BCDR_DPPU){
        RCC->APB1ENR |= RCC_APB1ENR_USBEN;
        GetUSB(dev)->BCDR &= ~USB_BCDR_DPPU;
    }else{
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        // PA12 = PushPull = 0
        GPIOA->OTYPER |= GPIO_OTYPER_OT_12;
        GPIOA->MODER &= ~GPIO_MODER_MODER12;
        GPIOA->MODER |= GPIO_MODER_MODER12_0;
        GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR12;
        GPIOA->BRR = GPIO_BRR_BR_12;
    }
#endif

#if defined(STM32F1)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // PA_12 output mode: OD = 0
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
    GPIOA->CRH &= (~GPIO_CRH_CNF12_1);
    GPIOA->CRH |= GPIO_CRH_MODE12; // PA_12 set as: Output mode, max speed 50 MHz.
    GPIOA->BRR = GPIO_BRR_BR12;
#endif

#if defined(STM32F3)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODER12;
    GPIOA->MODER |= GPIO_MODER_MODER12_0;
    GPIOA->BRR = GPIO_BRR_BR_12;
#endif
}

int tusb_dev_drv_open(tusb_device_driver_t **pdrv, const tusb_hardware_param_t *param, void *context)
{
    tusb_device_driver_t * drv = &stm32_dev_drv;
    if (!pdrv){
        return -1;
    }
    drv->context = context;
    *pdrv = drv;

    tusb_disconnect(drv);

    tusb_delay_ms(20);

#if defined(STM32F0)
    if(IS_HSI_CLOCK(param)){
        RCC->CFGR3 &= ~RCC_CFGR3_USBSW;
        RCC->APB1ENR |= RCC_APB1ENR_CRSEN;
        CRS->CFGR &= ~CRS_CFGR_SYNCSRC;
        CRS->CFGR |= RCC_CRS_SYNC_SOURCE_USB;
        CRS->CR |= (CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
    }else{
        RCC->CFGR3 |= RCC_CFGR3_USBSW_PLLCLK;
    }
#else
    if(IS_HSI_CLOCK(param)){
        TUSB_LOGD("HSI clock is not support on this device\n");
    }
    if(IS_INTERNAL_PULLUP(param)){
        TUSB_LOGD("Interanl pullup is not support on this device\n");
    }
#endif

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    GetUSB(drv)->CNTR = (USB_CNTR_FRES);
    GetUSB(drv)->CNTR = (0);

    // wait reset finish
    while (!((GetUSB(drv)->ISTR) & USB_ISTR_RESET))
        ;

    GetUSB(drv)->ISTR = (0);
    GetUSB(drv)->BTABLE = (BTABLE_ADDRESS);
    GetUSB(drv)->CNTR = (IMR_MSK);

#if defined(STM32F0)
    if(IS_INTERNAL_PULLUP(param)){
        // USE the interal pull up resistor
        GetUSB(drv)->BCDR |= USB_BCDR_DPPU;
    }else{
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        // PA12 = In float
        GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12;
        GPIOA->MODER &= ~GPIO_MODER_MODER12;
    }
    NVIC_EnableIRQ(USB_IRQn);
#endif

#if defined(STM32F1)
    // PA12 = Input
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
    GPIOA->CRH &= ~GPIO_CRH_CNF12_1;
    GPIOA->CRH &= ~GPIO_CRH_MODE12;
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif

#if defined(STM32F3)
    // PA12 = Input
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    set_io_af_mode(GPIOA, 11, GPIO_AF14_USB);
    set_io_af_mode(GPIOA, 12, GPIO_AF14_USB);
    NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
#endif
    return 0;
}

static inline void enable_double_buffer_interrupt()
{
#if defined(STM32F1)
    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
#if defined(STM32F3)
    NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
#endif
}

void *tusb_dev_drv_get_context(tusb_device_driver_t *drv)
{
    return drv->context;
}

int tusb_dev_drv_close(tusb_device_driver_t *drv)
{
#if defined(STM32F0)
    NVIC_DisableIRQ(USB_IRQn);
#endif

#if defined(STM32F1)
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif

#if defined(STM32F3)
    NVIC_DisableIRQ(USB_HP_CAN_TX_IRQn);
    NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
#endif

    GetUSB(drv)->ISTR = (0);
    // Turn USB Macrocell off (FRES + PWDN)
    GetUSB(drv)->CNTR = (USB_CNTR_FRES | USB_CNTR_PDWN);

    tusb_disconnect(drv);

    // Disable USB Clock on APB1
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;

    TUSB_LOGD("Close STM32 FS device\n");
    return 0;
}

#ifndef USB_CNTR_LP_MODE
#define USB_CNTR_LP_MODE USB_CNTR_LPMODE
#endif
static void tsub_suspend(tusb_device_driver_t *drv)
{
    uint16_t wCNTR;

    wCNTR = GetUSB(drv)->CNTR;
    wCNTR |= USB_CNTR_FSUSP;
    GetUSB(drv)->CNTR = (wCNTR);

    wCNTR = GetUSB(drv)->CNTR;
    wCNTR |= USB_CNTR_LP_MODE;
    GetUSB(drv)->CNTR = (wCNTR);
}

void tsub_resume(tusb_device_driver_t *drv)
{
    uint16_t wCNTR;
    wCNTR = GetUSB(drv)->CNTR;
    wCNTR &= (~USB_CNTR_LP_MODE);
    GetUSB(drv)->CNTR = (wCNTR);
    GetUSB(drv)->CNTR = (IMR_MSK);
}

// end point data handler, also called in USB_HP IRQ
void tusb_ep_handler(tusb_device_driver_t *drv, uint8_t EPn);

#if defined(STM32F3)
void USB_HP_CAN_TX_IRQHandler(void)
#else
void USB_HP_CAN1_TX_IRQHandler(void)
#endif
{
    uint16_t wIstr;
#ifdef  RTOS_INTERRUPT_ENTER
    RTOS_INTERRUPT_ENTER();
#endif
    tusb_device_driver_t *drv = &stm32_dev_drv;
    while ((wIstr = GetUSB(drv)->ISTR) & USB_ISTR_CTR)
    {
        GetUSB(drv)->ISTR = (uint16_t)(USB_CLR_CTR);
        tusb_ep_handler(drv, wIstr & USB_ISTR_EP_ID);
    }
#ifdef  RTOS_INTERRUPT_LEAVE
    RTOS_INTERRUPT_LEAVE();
#endif
}

#if defined(STM32F0)
void USB_IRQHandler(void)
#elif defined(STM32F3)
void USB_LP_CAN_RX0_IRQHandler(void)
#else
void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
{
    tusb_device_driver_t *drv = &stm32_dev_drv;
    uint16_t wIstr;
#ifdef  RTOS_INTERRUPT_ENTER
    RTOS_INTERRUPT_ENTER();
#endif
    while ((wIstr = GetUSB(drv)->ISTR) & USB_ISTR_CTR)
    {
        GetUSB(drv)->ISTR = (uint16_t)(USB_CLR_CTR);
        tusb_ep_handler(drv, wIstr & USB_ISTR_EP_ID);
    }

    if (wIstr & USB_ISTR_RESET)
    {
        GetUSB(drv)->ISTR = (USB_CLR_RESET);
        GetUSB(drv)->BTABLE = (BTABLE_ADDRESS);
        tusb_device_reset(drv);
        GetUSB(drv)->DADDR = (0 | USB_DADDR_EF);
    }
    if (IMR_MSK & USB_ISTR_PMAOVR)
    {
        if (wIstr & USB_ISTR_PMAOVR)
        {
            GetUSB(drv)->ISTR = (USB_CLR_PMAOVR);
        }
    }
    if (IMR_MSK & USB_ISTR_SUSP)
    {
        if (wIstr & USB_ISTR_SUSP)
        {
            GetUSB(drv)->ISTR = (USB_CLR_SUSP);
            if (GetUSB(drv)->DADDR & 0x007f)
            {
                GetUSB(drv)->DADDR = (0);
                GetUSB(drv)->CNTR &= ~USB_CNTR_SUSPM;
            }
            tsub_suspend(drv);
        }
    }
    if (IMR_MSK & USB_ISTR_ERR)
    {
        if (wIstr & USB_ISTR_ERR)
        {
            GetUSB(drv)->ISTR = (USB_CLR_ERR);
        }
    }

    if (IMR_MSK & USB_ISTR_WKUP)
    {
        if (wIstr & USB_ISTR_WKUP)
        {
            GetUSB(drv)->ISTR = (USB_CLR_WKUP);
            tsub_resume(drv);
        }
    }

    if (IMR_MSK & USB_ISTR_SOF)
    {
        if (wIstr & USB_ISTR_SOF)
        {
            GetUSB(drv)->ISTR = (USB_CLR_SOF);
        }
    }
    if (IMR_MSK & USB_ISTR_ESOF)
    {
        if (wIstr & USB_ISTR_ESOF)
        {
            GetUSB(drv)->ISTR = (USB_CLR_ESOF);
        }
    }

    GetUSB(drv)->ISTR = (0);
#ifdef  RTOS_INTERRUPT_LEAVE
    RTOS_INTERRUPT_LEAVE();
#endif
}

int tusb_dev_drv_setup_endpoint(tusb_device_driver_t *drv, const tusb_ep_config *ep_config, int count, int is_reset)
{    
    uint8_t ep_in[TUSB_MAX_EP_PAIR_COUNT] = {0};
    uint8_t ep_out[TUSB_MAX_EP_PAIR_COUNT] = {0};
    uint32_t used = 0;
    uint32_t used_double = 0;
    uint8_t max_ep_id = 0;
    for (int i = 0; i < count; i++)
    {
        // first calculate the buffer requirement, for both double buffer and single buffer
        const tusb_ep_config *cfg = ep_config + i;
        uint8_t ep = cfg->addr & 0x7f;
        uint8_t type = cfg->attribute & 3;
        if (ep >= STM32_MAX_EP_COUNT)
        {
            TUSB_LOGE("Endpoint 0x%02x large than STM32 max ep\n", cfg->addr);
            continue;
        }
        if (ep > max_ep_id)
        {
            max_ep_id = ep;
        }
        if (cfg->addr & 0x80)
        {
            if (ep_in[ep])
            {
                TUSB_LOGW("Endpoint 0x%02x already used, ignore it\n", cfg->addr);
                continue;
            }
            drv->mps_in[ep] = cfg->mps;
            ep_in[ep] = 1;
        }
        else
        {
            if (ep_out[ep])
            {
                TUSB_LOGW("Endpoint 0x%02x already used, ignore it\n", cfg->addr);
                continue;
            }
            drv->mps_out[ep] = cfg->mps;
            ep_out[ep] = 1;
        }
        if (type == USBD_EP_TYPE_ISOC)
        {
            // ISO must use double buffer
            used += cfg->mps * 2;
            used_double += cfg->mps * 2;
            if (cfg->addr & 0x80)
            {
                if (ep_out[ep])
                {
                    TUSB_LOGE("STM32 FS ISO EP 0x%02x already used by out direction\n", cfg->addr);
                }
            }
            else
            {
                if (ep_in[ep])
                {
                    TUSB_LOGE("STM32 FS ISO EP 0x%02x already used by in direction\n", cfg->addr);
                }
            }
        }
        else if (type == USBD_EP_TYPE_BULK)
        {
            used += cfg->mps;
            used_double += cfg->mps * 2;
            if (cfg->addr & 0x80)
            {
                if (ep_out[ep])
                {
                    TUSB_LOGD("Other out direction used, disable double buffer mode\n");
                    if (used_double < PMA_SIZE)
                    {
                        used_double = PMA_SIZE + 1;
                    }
                }
            }
            else
            {
                if (ep_in[ep])
                {
                    TUSB_LOGD("Other in direction used, disable double buffer mode\n");
                    if (used_double < PMA_SIZE)
                    {
                        used_double = PMA_SIZE + 1;
                    }
                }
            }
        }
        else
        {
            used += cfg->mps;
            used_double += cfg->mps;
        }
    }
    // reserved space for endpoint config
    uint32_t bt_start = (max_ep_id + 1) * 8;
    uint8_t real_double = 0;
    if (bt_start + used_double <= PMA_SIZE)
    {
        // used double buffer mode
        if (used != used_double)
        {
            // real has double buffer endpoint
            TUSB_LOGD("Double buffer for bulk endpoint enabled\n");
            real_double = 1;
            enable_double_buffer_interrupt();
        }
    }
    else if (bt_start + used <= PMA_SIZE)
    {
        // use single buffer mode
    }
    else
    {
        TUSB_LOGE("Endpoint PMA buffer overflow, some ep will not init\n");
    }
    uint8_t ep_in_used[TUSB_MAX_EP_PAIR_COUNT] = {0};
    uint8_t ep_out_used[TUSB_MAX_EP_PAIR_COUNT] = {0};
    for (int i = 0; i < count; i++)
    {
        const tusb_ep_config *cfg = ep_config + i;
        uint8_t ep = cfg->addr & 0x7f;
        uint8_t type = cfg->attribute & 3;
        uint16_t s_type = 0;
        switch(type){
            case USBD_EP_TYPE_CTRL: s_type = USB_EP_CONTROL; break;
            case USBD_EP_TYPE_BULK: s_type = USB_EP_BULK; break;
            case USBD_EP_TYPE_ISOC: s_type = USB_EP_ISOCHRONOUS; break;
            case USBD_EP_TYPE_INTR: s_type = USB_EP_INTERRUPT; break;
        }
        if (ep >= STM32_MAX_EP_COUNT)
        {
            TUSB_LOGE("Endpoint 0x%02x large than STM32 max ep\n", cfg->addr);
            continue;
        }
        if (((type == USBD_EP_TYPE_BULK) && real_double) || (type == USBD_EP_TYPE_ISOC))
        {
            if (cfg->addr & 0x80)
            {
                if (!ep_in_used[ep])
                {
                    ep_in_used[ep] = 1;
                    INIT_EP_TxDouble(drv, ep, s_type);
                    SET_DBL_TX_CNT(drv, ep, 0);
                    SET_DOUBLE_ADDR(drv, ep, bt_start, bt_start + cfg->mps);
                    TUSB_LOGD("EP: 0x%02x, addr0: 0x%x addr1:0x%x, size: %d, Double Tx, Type 0x%x\n",
                              cfg->addr, (int)bt_start, (int)(bt_start + cfg->mps), cfg->mps, s_type);
                    bt_start += cfg->mps * 2;
                }
                else
                {
                    TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                }
            }
            else
            {
                if (!ep_out_used[ep])
                {
                    ep_out_used[ep] = 1;
                    INIT_EP_RxDouble(drv, ep, s_type);
                    SET_DBL_RX_CNT(drv, ep, cfg->mps);
                    SET_DOUBLE_ADDR(drv, ep, bt_start, bt_start + cfg->mps);
                    TUSB_LOGD("EP: 0x%02x, addr0: 0x%x addr1:0x%x, size: %d, Double Rx, Type 0x%x\n",
                              cfg->addr, (int)bt_start, (int)(bt_start + cfg->mps), cfg->mps, s_type);
                    bt_start += cfg->mps * 2;
                }
                else
                {
                    TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                }
            }
        }
        else
        {
            if (ep_in[ep] && ep_out[ep])
            {
                if (cfg->addr & 0x80)
                {
                    if (!ep_in_used[ep])
                    {
                        ep_in_used[ep] = 1;
                        
                        if( ep != 0 || is_reset ){
                            if(!ep_out_used[ep]){
                                TUSB_LOGD("INIT Bi-Direction Ep %d, type 0x%x\n", ep ,s_type);
                                INIT_EP_BiDirection(drv, ep, s_type);
                            }
                            SET_TX_ADDR(drv, ep, bt_start);
                        }
                        TUSB_LOGD("EP: 0x%02x, addr: 0x%x, size: %d, Bi-Direction Tx, Type 0x%x\n", cfg->addr, (int)bt_start, cfg->mps, s_type);
                        bt_start += cfg->mps;
                    }
                    else
                    {
                        TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                    }
                }
                else
                {
                    if (!ep_out_used[ep])
                    {
                        ep_out_used[ep] = 1;
                        if( ep != 0 || is_reset ){
                            if(!ep_in_used[ep]){
                                TUSB_LOGD("INIT Bi-Direction Ep %d, type 0x%x\n", ep ,s_type);
                                INIT_EP_BiDirection(drv, ep, s_type);
                            }
                            SET_RX_ADDR(drv, ep, bt_start);
                            SET_RX_CNT(drv, ep, cfg->mps);
                        }
                        TUSB_LOGD("EP: 0x%02x, addr: 0x%x, size: %d, Bi-Direction Rx, Type 0x%x\n", cfg->addr, (int)bt_start, cfg->mps, s_type);
                        bt_start += cfg->mps;
                    }
                    else
                    {
                        TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                    }
                }
            }
            else
            {
                if (cfg->addr & 0x80)
                {
                    if (!ep_in_used[ep])
                    {
                        ep_in_used[ep] = 1;
                        INIT_EP_TxOnly(drv, ep, s_type);
                        SET_TX_ADDR(drv, ep, bt_start);
                        TUSB_LOGD("EP: 0x%02x, addr: 0x%x, size: %d, Tx Only, Type 0x%x\n", cfg->addr, (int)bt_start, cfg->mps, s_type);
                        bt_start += cfg->mps;
                    }
                    else
                    {
                        TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                    }
                }
                else
                {
                    if (!ep_out_used[ep])
                    {
                        ep_out_used[ep] = 1;
                        INIT_EP_RxOnly(drv, ep, s_type);
                        SET_RX_ADDR(drv, ep, bt_start);
                        SET_RX_CNT(drv, ep, cfg->mps);
                        TUSB_LOGD("EP: 0x%02x, addr: 0x%x, size: %d, Rx Only, Type 0x%x\n", cfg->addr, (int)bt_start, cfg->mps, s_type);
                        bt_start += cfg->mps;
                    }
                    else
                    {
                        TUSB_LOGW("Ignore duplicate EP: 0x%02x\n", cfg->addr);
                    }
                }
            }
        }
    }
    return 0;
}

int tusb_dev_drv_get_speed(tusb_device_driver_t *drv)
{
    return PORT_SPEED_FULL;
}

int tusb_dev_drv_set_address(tusb_device_driver_t *drv, uint8_t addr, uint8_t after_status_out)
{
    if (after_status_out)
    {
        // FS device must set address after status out
        if (addr)
        {
            GetUSB(drv)->DADDR = (addr | USB_DADDR_EF);
        }
    }
    return 0;
}

// Copy data to PMA buffer, if memory is aligned, copy two by two
static void tusb_pma_tx(tusb_device_driver_t *drv, pma_record *pma, const void *data, uint32_t len)
{
    uint32_t count = (len + 1) / 2;
    pma_data *dest = GetPMAAddr(drv, pma->addr);
    pma->cnt = len;
#ifdef ALIGNED
    const uint16_t *src = (const uint16_t *)data;
    for (uint8_t i = 0; i < count; i++)
    {
        dest->data = *src;
        dest++;
        src++;
    }
#else
    const uint8_t *src = (const uint8_t *)data;
    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t v = src[0] | (src[1] << 8);
        src += 2;
        dest->data = v;
        dest++;
    }
#endif
}

// Copy data from PMA buffer, if memory is aligned, copy two by two
static uint32_t tusb_pma_rx(tusb_device_driver_t *drv, pma_record *pma, void *data)
{
    uint32_t len = pma->cnt;
    pma_data *src = GetPMAAddr(drv, pma->addr);
    uint32_t count = (len + 1) / 2;
#ifdef ALIGNED
    uint16_t *dest = (uint16_t *)data;
    for (uint32_t i = 0; i < count; i++)
    {
        *dest = src->data;
        dest++;
        src++;
    }
#else
    uint8_t *dest = (uint8_t *)data;
    for (uint32_t i = 0; i < count; i++)
    {
        uint16_t v = src->data;
        *dest = (uint8_t)v;
        dest++;
        v >>= 8;
        *dest = (uint8_t)v;
        dest++;
        src++;
    }
#endif
    return len;
}

// helper function to copy data to end point
static void copy_tx(tusb_device_driver_t *drv, tusb_ep_data *ep, pma_record *pma, const void *data, int len, uint16_t tx_max_size)
{
    ep->tx_remain_size = len;
    ep->tx_buf = (const uint8_t *)data;
    if (len > tx_max_size)
    {
        len = tx_max_size;
    }
    tusb_pma_tx(drv, pma, data, len);
    ep->tx_pushed++;
    ep->tx_buf += len;
    ep->tx_remain_size -= len;
    ep->tx_last_size = len;
}

#define DOUBLE_BUFF 1
#define ISO_EP 1

int tusb_dev_drv_send_data(tusb_device_driver_t *drv, uint8_t EPn, const void *data, int len, uint8_t option)
{
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    tusb_ep_data *ep = &drv->Ep[EPn];
    ep->tx_total_size = len;
    ep->tx_need_zlp = (option & TUSB_TXF_ZLP) != 0;
    if (ep->tx_remain_size)
        return -1;
    if (DOUBLE_BUFF && (EP & (USB_EP_TYPE_MASK | USB_EP_KIND)) == (USB_EP_BULK | USB_EP_KIND))
    {
        // double buffer ep send data
        pma_record *pma0;
        pma_record *pma1;
        uint16_t TOG = (EP & (USB_EP_DTOG_RX | USB_EP_DTOG_TX));
        if (TOG != 0 && TOG != (USB_EP_DTOG_RX | USB_EP_DTOG_TX))
        {
            return -1;
        }
        ep->tx_pushed = 0;
        if (EP & USB_EP_DTOG_RX)
        {
            pma0 = PMA_TX1(drv, EPn);
            pma1 = PMA_TX0(drv, EPn);
        }
        else
        {
            pma0 = PMA_TX0(drv, EPn);
            pma1 = PMA_TX1(drv, EPn);
        }
        copy_tx(drv, ep, pma0, data, len, GetInMaxPacket(drv, EPn));
        if (ep->tx_remain_size)
        {
            copy_tx(drv, ep, pma1, ep->tx_buf, ep->tx_remain_size, GetInMaxPacket(drv, EPn));
        }
        // toggle it
        TUSB_RX_DTOG(GetUSB(drv), EPn, EP);
    }
    else if (ISO_EP && ((EP & USB_EP_TYPE_MASK) == USB_EP_ISOCHRONOUS))
    {
        pma_record *pma = EP & USB_EP_DTOG_TX ? PMA_TX0(drv, EPn) : PMA_TX1(drv, EPn);
        ep->tx_pushed = 0;
        copy_tx(drv, ep, pma, data, len, GetInMaxPacket(drv, EPn));
        // ISO endpoint don't neet set valid, only need set the tx size
        // TUSB_SET_TX_STATUS(GetUSB(drv), EPn, EP, USB_EP_TX_VALID);
    }
    else
    {
        // normal ep send data
        if ((EP & USB_EPTX_STAT) != USB_EP_TX_NAK)
        {
            return -1;
        }
        ep->tx_pushed = 0;
        copy_tx(drv, ep, PMA_TX(drv, EPn), data, len, GetInMaxPacket(drv, EPn));
        TUSB_SET_TX_STATUS(GetUSB(drv), EPn, EP, USB_EP_TX_VALID);
    }
    return 0;
}

// called by the ep data interrupt handler when last packet tranfer done
static void tusb_send_data_done(tusb_device_driver_t *drv, uint8_t EPn)
{
    tusb_ep_data *ep = &drv->Ep[EPn];
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    pma_record *pma;

#define IS_DOUBLE() (DOUBLE_BUFF && (EP & (USB_EP_TYPE_MASK | USB_EP_KIND)) == (USB_EP_BULK | USB_EP_KIND))
#define IS_ISO() (ISO_EP && ((EP & USB_EP_TYPE_MASK) == USB_EP_ISOCHRONOUS))

    if (IS_DOUBLE())
    {
        // double buffer bulk end point, toggle first then copy data
        if (ep->tx_pushed)
        {
            ep->tx_pushed--;
        }
        if (ep->tx_pushed)
        {
            // toggle it first, then do some copy
            TUSB_RX_DTOG(GetUSB(drv), EPn, EP);
        }
        pma = (EP & USB_EP_DTOG_RX) ? PMA_TX0(drv, EPn) : PMA_TX1(drv, EPn);
    }
    else if (IS_ISO())
    {
        ep->tx_pushed = 0;
        pma = (EP & USB_EP_DTOG_TX) ? PMA_TX0(drv, EPn) : PMA_TX1(drv, EPn);
        pma->cnt = 0;
    }
    else
    {
        ep->tx_pushed = 0;
        pma = PMA_TX(drv, EPn);
    }
    if (ep->tx_remain_size || ((ep->tx_need_zlp) && ep->tx_last_size == GetInMaxPacket(drv, EPn)))
    {
        copy_tx(drv, ep, pma, ep->tx_buf, ep->tx_remain_size, GetInMaxPacket(drv, EPn));
        PCD_SET_EP_TX_STATUS(GetUSB(drv), EPn, USB_EP_TX_VALID);
        return;
    }
    if (ep->tx_pushed == 0)
    {
        if (IS_ISO())
        {
            if (ep->tx_buf)
            {
                int len = ep->tx_total_size - ep->tx_remain_size;
                tusb_device_ep_xfer_done(drv, EPn | 0x80, ep->tx_buf - len, len, 0);
                ep->tx_buf = 0;
            }
        }
        else
        {
            int len = ep->tx_total_size - ep->tx_remain_size;
            tusb_device_ep_xfer_done(drv, EPn | 0x80, ep->tx_buf - len, len, 0);
        }
    }
}

int tusb_dev_drv_cancel_send(tusb_device_driver_t *drv, uint8_t EPn)
{
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    tusb_ep_data *ep = &drv->Ep[EPn];
    int res = ep->tx_remain_size;
    ep->tx_remain_size = 0;
    if (DOUBLE_BUFF && (EP & (USB_EP_TYPE_MASK | USB_EP_KIND)) == (USB_EP_BULK | USB_EP_KIND))
    {
        uint16_t TOG = (EP & (USB_EP_DTOG_RX | USB_EP_DTOG_TX));
        if (TOG != 0 && TOG != (USB_EP_DTOG_RX | USB_EP_DTOG_TX))
        {
            // make sure toogle bit is the same, so the ep will enter nak mode
            TUSB_RX_DTOG(GetUSB(drv), EPn, EP);
        }
    }
    else if (ISO_EP && ((EP & USB_EP_TYPE_MASK) == USB_EP_ISOCHRONOUS))
    {
        // just the count to zero
        PMA_TX0(drv, EPn)->cnt = 0;
        PMA_TX1(drv, EPn)->cnt = 0;
    }
    else
    {
        // normal ep send data
        if ((EP & USB_EPTX_STAT) != USB_EP_TX_NAK)
        {
            TUSB_SET_TX_STATUS(GetUSB(drv), EPn, EP, USB_EP_TX_NAK);
        }
    }
    return res;
}

void tusb_dev_drv_set_stall(tusb_device_driver_t *drv, uint8_t EPn)
{
    uint8_t ep = EPn & 0x7f;
    if (ep == 0)
    {
        //PCD_SET_EP_TXRX_STATUS(GetUSB(dev), 0, USB_EP_RX_STALL, USB_EP_TX_STALL);
        PCD_SET_EP_TX_STATUS(GetUSB(drv), 0, USB_EP_TX_STALL);
        // We should prepare endpoint 0 to receive setup packet
        // But the setup packet always success receive, so do nothing here
    }
    else
    {
        if (EPn & 0x80)
        {
            PCD_SET_EP_TX_STATUS(GetUSB(drv), ep, USB_EP_TX_STALL);
        }
        else
        {
            PCD_SET_EP_RX_STATUS(GetUSB(drv), ep, USB_EP_RX_STALL);
        }
    }
}

void tusb_dev_drv_clear_stall(tusb_device_driver_t *drv, uint8_t EPn)
{
    uint8_t ep = EPn & 0x7f;
    if (ep == 0)
    {
        PCD_CLEAR_TX_DTOG(GetUSB(drv), 0);
        //PCD_SET_EP_TX_STATUS(GetUSB(drv), 0, USB_EP_TX_VALID);
        PCD_CLEAR_RX_DTOG(GetUSB(drv), 0);
        PCD_SET_EP_RX_STATUS(GetUSB(drv), 0, USB_EP_RX_VALID);
    }
    else
    {
        if (EPn & 0x80)
        {
            PCD_CLEAR_TX_DTOG(GetUSB(drv), ep);
            PCD_SET_EP_TX_STATUS(GetUSB(drv), ep, USB_EP_TX_NAK);
        }
        else
        {
            PCD_CLEAR_RX_DTOG(GetUSB(drv), ep);
            PCD_SET_EP_RX_STATUS(GetUSB(drv), ep, USB_EP_RX_VALID);
        }
    }
}

// helper function to read ep0
uint32_t tusb_read_ep0(tusb_device_driver_t *drv, void *buf)
{
    return tusb_pma_rx(drv, PMA_RX(drv, 0), buf);
}

// called by the ep data interrupt handler when got data
void tusb_recv_data(tusb_device_driver_t *drv, uint8_t EPn)
{
    tusb_ep_data *ep = &drv->Ep[EPn];
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    pma_record *pma = 0;
    if (DOUBLE_BUFF && (EP & (USB_EP_TYPE_MASK | USB_EP_KIND)) == (USB_EP_BULK | USB_EP_KIND))
    {
        // double buffer bulk end point
        // If rx count is not valid, freeze the DTOG, this will cause ep NAK
        if (ep->rx_count < ep->rx_size)
        {
            TUSB_TX_DTOG(GetUSB(drv), EPn, EP);
            // If App last used buffer is 1, drv will fill data in 0
            // If App last used buffer is 0, drv will fill data in 1
            // We read the data filled by device
            pma = (EP & USB_EP_DTOG_TX) ? PMA_RX0(drv, EPn) : PMA_RX1(drv, EPn);
        }
    }
    else if (ISO_EP && ((EP & USB_EP_TYPE_MASK) == USB_EP_ISOCHRONOUS))
    {
        // ISO out endpoint will always receive the new packet
        // if rx count is not valid, current packet in PMA buffer will be dropped
        if (ep->rx_count < ep->rx_size)
        {
            pma = (EP & USB_EP_DTOG_RX) ? PMA_TX0(drv, EPn) : PMA_TX1(drv, EPn);
        }
    }
    else
    {
        pma = PMA_RX(drv, EPn);
    }
    if (ep->rx_buf && pma)
    {
        uint32_t len = tusb_pma_rx(drv, pma, ep->rx_buf + ep->rx_count);
        pma->cnt = 0;
        ep->rx_count += len;
        if (len != GetOutMaxPacket(drv, EPn) || ep->rx_count >= ep->rx_size)
        {
            if (tusb_device_ep_xfer_done(drv, EPn, ep->rx_buf, ep->rx_count, 0) == 0)
            {
                ep->rx_count = 0;
            }
            else
            {
                // of rx done not return success, change rx_count to rx_size, this will block
                // the data recieve
                ep->rx_count = ep->rx_size;
            }
        }
    }
    if (ep->rx_count < ep->rx_size)
    {
        TUSB_SET_RX_STATUS(GetUSB(drv), EPn, EP, USB_EP_RX_VALID);
    }
}

// initial the ep recv buffer
int tusb_dev_drv_set_recv_buffer(tusb_device_driver_t *drv, uint8_t EPn, void *data, int len)
{
    tusb_ep_data *ep = &drv->Ep[EPn];
    ep->rx_buf = (uint8_t *)data;
    ep->rx_size = len;
    ep->rx_count = 0;
    return 0;
}

void tusb_dev_drv_set_rx_valid(tusb_device_driver_t *drv, uint8_t EPn)
{
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    tusb_ep_data *ep = &drv->Ep[EPn];
    ep->rx_count = 0;
    if (DOUBLE_BUFF && (EP & (USB_EP_TYPE_MASK | USB_EP_KIND)) == (USB_EP_BULK | USB_EP_KIND))
    {
        // Rx inital for double buffer ep
        uint16_t TOG = (EP & (USB_EP_DTOG_RX | USB_EP_DTOG_TX));
        pma_record *pma = 0;
        pma = (EP & USB_EP_DTOG_TX) ? PMA_RX0(drv, EPn) : PMA_RX1(drv, EPn);
        // got the pma buffer last used by the USB core
        if (pma->cnt)
        {
            // already has data in the pma buffer, copy it to the rx buffer
            uint32_t len = tusb_pma_rx(drv, pma, ep->rx_buf + ep->rx_count);
            pma->cnt = 0;
            ep->rx_count += len;
            if (len != GetOutMaxPacket(drv, EPn) || ep->rx_count >= ep->rx_size)
            {
                if (tusb_device_ep_xfer_done(drv, EPn, ep->rx_buf, ep->rx_count, 0) == 0)
                {
                    ep->rx_count = 0;
                }
                else
                {
                    ep->rx_count = ep->rx_size;
                }
            }
        }
        if (TOG == 0 || TOG == (USB_EP_DTOG_RX | USB_EP_DTOG_TX))
        {
            // always enter here
            TUSB_TX_DTOG(GetUSB(drv), EPn, EP);
        }
    }
    else
    {
        // Rx initial for normal ep
        TUSB_SET_RX_STATUS(GetUSB(drv), EPn, EP, USB_EP_RX_VALID);
    }
}

// end point handler for USB_FS core
void tusb_ep_handler(tusb_device_driver_t *drv, uint8_t EPn)
{
    uint16_t EP = PCD_GET_ENDPOINT(GetUSB(drv), EPn);
    if (EP & USB_EP_CTR_RX)
    {
        if (EPn == 0)
        {
            if (EP & USB_EP_SETUP)
            {
                // Handle setup packet
                uint8_t temp[8];
                tusb_read_ep0(drv, temp);
                tusb_device_ep_xfer_done(drv, EPn, temp, 8, 1);
            }
            else
            {
                // Handle ep 0 data packet
                tusb_recv_data(drv, EPn);
            }
            PCD_CLEAR_RX_EP_CTR(GetUSB(drv), PCD_ENDP0);
            PCD_SET_EP_RX_STATUS(GetUSB(drv), PCD_ENDP0, USB_EP_RX_VALID);
        }
        else
        {
            TUSB_CLEAR_RX_CTR(GetUSB(drv), EPn, EP);
            tusb_recv_data(drv, EPn);
        }
    }
    if ((EP & USB_EP_CTR_TX))
    { // something transmitted
        TUSB_CLEAR_TX_CTR(GetUSB(drv), EPn, EP);
        tusb_send_data_done(drv, EPn);
    }
}

#endif // #ifdef USB
