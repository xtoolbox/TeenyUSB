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
#include "tusb_dev_drv_stm32_otg.h"
#include "string.h"

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#ifndef NO_DEVICE

struct _tusb_device_driver
{
    void* context;
    uint16_t  fifo_size;
    uint16_t  max_ep_num;
    uint32_t  ep0_in_remain_size;
    uint32_t  ep0_out_remain_size;
    uint32_t  in_ep_len[TUSB_MAX_EP_PAIR_COUNT];
    uint32_t  in_ep_option[TUSB_MAX_EP_PAIR_COUNT];
    void*     out_ep_addr[TUSB_MAX_EP_PAIR_COUNT];
    uint32_t  out_ep_len[TUSB_MAX_EP_PAIR_COUNT];
    PCD_HandleTypeDef pcd;
};

#ifdef USB_OTG_FS
static tusb_device_driver_t stm32_fs;
#if USB_OTG_FS_MAX_EP_NUM >= TUSB_MAX_EP_PAIR_COUNT
#error  STM32 OTG FS core endpoint number large than TUSB_MAX_EP_PAIR_COUNT
#endif
#endif

#ifdef USB_OTG_HS
static tusb_device_driver_t stm32_hs;
#if USB_OTG_HS_MAX_EP_NUM >= TUSB_MAX_EP_PAIR_COUNT
#error  STM32 OTG HS core endpoint number large than TUSB_MAX_EP_PAIR_COUNT
#endif
#endif

void tusb_stm32_init_otg_io(const tusb_stm32_otg_io_cfg_t* cfg);

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    if(hpcd->Init.speed == USB_OTG_SPEED_HIGH){
#ifdef USB_OTG_HS
        NVIC_SetPriority(OTG_HS_IRQn, 0);
        NVIC_EnableIRQ(OTG_HS_IRQn);
#if defined(USB_HS_PHYC)
        if(hpcd->Init.phy_itface == USB_OTG_HS_EMBEDDED_PHY){
            __HAL_RCC_OTGPHYC_CLK_ENABLE();
        }
#endif
        __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
#endif
    }else{
#ifdef USB_OTG_FS
        NVIC_SetPriority(OTG_FS_IRQn, 0);
        NVIC_EnableIRQ(OTG_FS_IRQn);
        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
#endif
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    if(hpcd->Init.speed == USB_OTG_SPEED_HIGH){
#ifdef USB_OTG_HS
        NVIC_DisableIRQ(OTG_HS_IRQn);
        if(hpcd->Init.phy_itface == USB_OTG_EMBEDDED_PHY){
#if defined(USB_HS_PHYC)
            __HAL_RCC_OTGPHYC_CLK_DISABLE();
#endif
        }
        __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_DISABLE();
#endif
    }else{
#ifdef USB_OTG_FS
        NVIC_DisableIRQ(OTG_FS_IRQn);
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
#endif
    }
}

int tusb_dev_drv_open(tusb_device_driver_t **pdrv, const tusb_hardware_param_t *param, void *context)
{
    if (!pdrv){
        return -1;
    }
    tusb_device_driver_t* drv;
    tusb_stm32_init_otg_io(param->io_cfgs);
    if(param->is_hs_core){
#ifdef USB_OTG_HS
        drv = &stm32_hs;
        stm32_hs.pcd.Instance = USB_OTG_HS;
        drv->fifo_size = USB_OTG_HS_FIFO_SIZE;
        drv->max_ep_num = USB_OTG_HS_MAX_EP_NUM;
#else
        TUSB_LOGE("STM32 OTG HS core not exist\n");
        return -1;
#endif
    }else{
#ifdef USB_OTG_FS
        drv = &stm32_fs;
        stm32_fs.pcd.Instance = USB_OTG_FS;
        drv->fifo_size = USB_OTG_FS_FIFO_SIZE;
        drv->max_ep_num = USB_OTG_FS_MAX_EP_NUM;
#else
        TUSB_LOGE("STM32 OTG FS core not exist\n");
        return -1;
#endif
    }
    *pdrv = drv;
    drv->pcd.pData = drv;
    drv->context = context;
    
    if(param->is_hs_core){
        if(param->is_high_speed){
            drv->pcd.Init.speed = USB_OTG_SPEED_HIGH;
        }else{
            drv->pcd.Init.speed = USB_OTG_SPEED_HIGH_IN_FULL;
        }
        if(param->is_internal_phy){
#if defined(USB_HS_PHYC)
            drv->pcd.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
#else
            TUSB_LOGE("STM32 OTG HS not have embedded phy\n");
            return -1;
#endif
        }else{
            drv->pcd.Init.phy_itface = USB_OTG_ULPI_PHY;
        }
    }else{
        if(param->dma_enable){
            TUSB_LOGW("STM32 OTG FS core not support DMA\n");
        }
        drv->pcd.Init.speed = USB_OTG_SPEED_FULL;
        drv->pcd.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
    }
    
    if(param->low_power_enable){
        TUSB_LOGW("Low power mode is not handle by Teeny USB core\n");
    }
    drv->pcd.Init.dev_endpoints = drv->max_ep_num;
    drv->pcd.Init.dma_enable = param->dma_enable;
    drv->pcd.Init.Sof_enable = param->sof_enable;
    drv->pcd.Init.low_power_enable = param->low_power_enable;
    drv->pcd.Init.lpm_enable = param->lpm_enable;
    drv->pcd.Init.battery_charging_enable = param->battery_charging_enable;
    drv->pcd.Init.vbus_sensing_enable = param->vbus_sensing_enable;
    drv->pcd.Init.use_dedicated_ep1 = param->use_dedicated_ep1;
    drv->pcd.Init.use_external_vbus = param->use_external_vbus;
    
    HAL_PCD_Init(&drv->pcd);
    
    HAL_PCD_Start(&drv->pcd);
    return 0;
}

void *tusb_dev_drv_get_context(tusb_device_driver_t *drv)
{
    return drv->context;
}

int tusb_dev_drv_close(tusb_device_driver_t *drv)
{
    HAL_PCD_DeInit(&drv->pcd);
    return 0;
}

int tusb_dev_drv_setup_endpoint(tusb_device_driver_t *drv, const tusb_ep_config *ep_config, int count, int is_reset)
{
    uint32_t max_out_mps = 0;
    uint32_t out_ep_count = 0;
    uint32_t control_ep_count = 0;
    uint32_t in_total_size = 0;
    uint32_t used_in_ep = 0;
    uint32_t max_in_ep = 0;
    uint16_t in_mps[TUSB_MAX_EP_PAIR_COUNT] = {0};
    for(int i =0; i<count; i++){
        const tusb_ep_config *cfg = ep_config + i;
        uint8_t ep = cfg->addr & 0x7f;
        uint8_t type = cfg->attribute & 3;
        if (ep > drv->max_ep_num)
        {
            TUSB_LOGE("Endpoint 0x%02x large than STM32 max ep %d\n", cfg->addr, drv->max_ep_num);
            continue;
        }
        if(ep >= TUSB_MAX_EP_PAIR_COUNT){
            TUSB_LOGE("Endpoint 0x%02x large than TUSB_MAX_EP_PAIR_COUNT (%d)\n", cfg->addr, TUSB_MAX_EP_PAIR_COUNT);
            continue;
        }
        if(ep != 0){
            HAL_PCD_EP_Close(&drv->pcd, cfg->addr);
            TUSB_LOGD("Close EP 0x%02x\n", cfg->addr);
        }
        if(!(cfg->addr & 0x80)){
            if(max_out_mps < cfg->mps){
                max_out_mps = cfg->mps;
            }
            out_ep_count++;
            if(type == USBD_EP_TYPE_CTRL){
                control_ep_count++;
            }
        }else{
            if(in_mps[ep]){
                TUSB_LOGW("In ep %d already used\n", ep);
                continue;
            }
            used_in_ep |= (1<<ep);
            if(max_in_ep < ep){
                max_in_ep = ep;
            }
            in_mps[ep] = cfg->mps;
            if(cfg->mps < 64){
                in_total_size += 64;
                in_mps[ep] = 64;
            }else{
                in_total_size += cfg->mps;
            }
        }
    }
    for(uint32_t i=0;i<=max_in_ep;i++){
        if(! (used_in_ep & (1<<i))){
            TUSB_LOGD("In ep %d is not used, allocate it use min size\n", (int)i);
            in_total_size += 64;
            in_mps[i] = 64;
        }
    }
    TUSB_LOGD("Max Out MPS is %d, Out ep count: %d, control ep count: %d\n", 
        (int)max_out_mps, (int)out_ep_count, (int)control_ep_count);
    TUSB_LOGD("In total MPS: %d\n", (int)in_total_size);
    
#if defined(STM32F7) || defined(STM32H7)
    // config in 7xx mode
    // In ST's RM0431
    // Device RxFIFO =
    // (5 * number of control endpoints + 8) + ((largest USB packet used / 4) + 1 for status
    // information) + (2 * number of OUT endpoints) + 1 for Global NAK
    
    uint32_t rx_value = (5 * control_ep_count + 8) + (max_out_mps/4 + 1) + 2 * out_ep_count + 1;
    if(rx_value < 16){
        rx_value = 16;
    }
    if(rx_value > 1024){
        TUSB_LOGE("Rx fifo value will large than 256\n");
        return -1;
    }
    if(drv == &stm32_hs){
        if(rx_value < 512){
            rx_value = 512;
        }
    }else{
        if(rx_value < 128){
            rx_value = 128;
        }
    }
    
    uint32_t rx_size = rx_value * 4;
#else
    // config in other mode
    // In ST's RM0090
    // Typically, two (Largest Packet Size / 4) + 1
    // spaces are recommended
    uint32_t rx_value = (max_out_mps/4 + 1)*2;
    if(rx_value * 4 + in_total_size > drv->fifo_size){
        rx_value = (max_out_mps/4 + 1);
        TUSB_LOGW("Not enough fifo, so use signle packet buffer for rx\n");
    }
    if(rx_value < 16){
        rx_value = 16;
    }
#ifdef USB_OTG_FS
    if(drv == &stm32_fs){
        if(rx_value > 256){
            TUSB_LOGE("Rx fifo value will large than 256\n");
            return -1;
        }
        if(rx_value < 128){
            rx_value = 128;
        }
    }
#endif
#ifdef USB_OTG_HS
    if(drv == &stm32_hs){
        if(rx_value > 1024){
            TUSB_LOGE("Rx fifo value will large than 1024\n");
            return -1;
        }
        TUSB_LOGD("Fixme: We can not use the value calculate by RM0090, it will cause EP 0x00 always NAK\n");
        if(rx_value < 512){
            rx_value = 512;
        }
    }
#endif
    uint32_t rx_size = rx_value * 4;
#endif
    uint32_t remain = drv->fifo_size - rx_size;
    TUSB_LOGD("Rx Fifo size is %d, remain %d\n", (int)rx_size, (int)remain);
    if(in_total_size > remain){
        TUSB_LOGE("Remain fifo size(%d) is less than in ep total mps (%d)\n", 
            (int)remain, (int)in_total_size);
        return -1;
    }
    uint32_t mul = remain / in_total_size;
#if defined(STM32F7) || defined(STM32H7)
#else
    if(mul > 7){
        mul = 7;
    }
#endif
    
    HAL_PCDEx_SetRxFiFo(&drv->pcd, rx_size/4);
    
    for(int i =0; i<=max_in_ep; i++){
        uint32_t tx_size = in_mps[i]*mul;
        HAL_PCDEx_SetTxFiFo(&drv->pcd, i, tx_size / 4);
        remain -= tx_size;
        TUSB_LOGD("Set Tx Fifo %d to size %d\n", i, (int)tx_size);
    }
    TUSB_LOGD("Fifo Remain %d\n", (int)remain);
    
    
    for(int i =0; i<count; i++){
        const tusb_ep_config *cfg = ep_config + i;
        uint8_t type = cfg->attribute & 3;
        HAL_PCD_EP_Open(&drv->pcd, cfg->addr, cfg->mps, type);
        TUSB_LOGD("Open ep 0x%02x, mps: %d, type: %d\n", cfg->addr, cfg->mps, type);
    }
    return 0;
}

int tusb_dev_drv_get_speed(tusb_device_driver_t *drv)
{
    if(drv->pcd.Init.speed == USB_OTG_SPEED_HIGH){
        return PORT_SPEED_HIGH;
    }
    return PORT_SPEED_FULL;
}

int tusb_dev_drv_set_address(tusb_device_driver_t *drv, uint8_t addr, uint8_t after_status_out)
{
    // OTG core can set address directly
    if(!after_status_out){
        HAL_PCD_SetAddress(&drv->pcd, addr);
    }
    return 0;
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    tusb_device_reset((tusb_device_driver_t*)hpcd->pData);
}

void stm32_otg_device_handler(int is_hs)
{
    if(is_hs){
#ifdef USB_OTG_HS
        HAL_PCD_IRQHandler(&stm32_hs.pcd);
#endif
    }else{
#ifdef USB_OTG_FS
        HAL_PCD_IRQHandler(&stm32_fs.pcd);
#endif
    }
}

void tusb_dev_drv_set_stall(tusb_device_driver_t *drv, uint8_t EPn)
{
    HAL_PCD_EP_SetStall(&drv->pcd, EPn);
}

void tusb_dev_drv_clear_stall(tusb_device_driver_t *drv, uint8_t EPn)
{
    HAL_PCD_EP_ClrStall(&drv->pcd, EPn);
}

int tusb_dev_drv_send_data(tusb_device_driver_t *drv, uint8_t EPn, const void *data, int len, uint8_t option)
{
    drv->in_ep_len[EPn&0x7f] = len;
    drv->in_ep_option[EPn&0x7f] = option;
    if( (EPn&0x7f) == 0x00){
        drv->ep0_in_remain_size = len;
    }
    HAL_PCD_EP_Transmit(&drv->pcd, EPn, (uint8_t*)data, len);
    return 0;
}

int tusb_dev_drv_cancel_send(tusb_device_driver_t *drv, uint8_t EPn)
{
    TUSB_LOGW("OTG Core not support cancel send\n");
    return -1;
}

int tusb_dev_drv_set_recv_buffer(tusb_device_driver_t *drv, uint8_t EPn, void *data, int len)
{
    drv->out_ep_addr[EPn&0x7f] = data;
    drv->out_ep_len[EPn&0x7f] = len;
    if( (EPn & 0x7f) == 0){
        drv->ep0_out_remain_size = len;
    }
    return 0;
}

void tusb_dev_drv_set_rx_valid(tusb_device_driver_t *drv, uint8_t EPn)
{
    HAL_PCD_EP_Receive(&drv->pcd, EPn, (uint8_t*)drv->out_ep_addr[EPn&0x7f], drv->out_ep_len[EPn&0x7f]);
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    tusb_device_ep_xfer_done(
        (tusb_device_driver_t*)hpcd->pData,
            0x00,
            &hpcd->Setup, 8, 1);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    tusb_device_driver_t* drv = (tusb_device_driver_t*)hpcd->pData;
    PCD_EPTypeDef* ep = &hpcd->OUT_ep[epnum];
    if(epnum == 0){
        // EP0 only recv MPS each time in HAL
        if(ep->xfer_count != ep->maxpacket){
            tusb_device_ep_xfer_done(drv,
            0x00,
            ep->xfer_buff - drv->out_ep_len[0], drv->out_ep_len[0], 0);
        }else if(drv->ep0_out_remain_size >= ep->maxpacket){
            drv->ep0_out_remain_size -= ep->maxpacket;
            HAL_PCD_EP_Receive(&drv->pcd, 0, ep->xfer_buff, drv->ep0_out_remain_size);
        }else{
            TUSB_LOGE("STM32 OTG Out Ep error\n");
        }
    }else{
        
        // HAL got wrong count, fix it here
        PCD_TypeDef* USBx = hpcd->Instance;
        uint32_t len = ep->xfer_len;
        if(ep->xfer_len < ep->maxpacket){
        }else if(ep->xfer_len % ep->maxpacket){
            len = ep->xfer_count;
        }else{
            len = ep->xfer_len - (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
        }
        int r = tusb_device_ep_xfer_done(drv,
            epnum & 0x7f,
            drv->out_ep_addr[epnum], len, 0);
        if(r == 0){
            tusb_dev_drv_set_rx_valid(drv, epnum);
        }
    }
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    tusb_device_driver_t* drv = (tusb_device_driver_t*)hpcd->pData;
    PCD_EPTypeDef* ep = &hpcd->IN_ep[epnum];
    uint8_t* data_buff;
    if (hpcd->Init.dma_enable == 1U){
        data_buff = ep->xfer_buff - ep->maxpacket;
    }else{
        data_buff = ep->xfer_buff - drv->in_ep_len[epnum];
    }
    if(epnum == 0){
        // EP0 only send MPS each time in HAL
        if(drv->ep0_in_remain_size > ep->maxpacket){
            drv->ep0_in_remain_size -= ep->maxpacket;
            HAL_PCD_EP_Transmit(hpcd, 0, ep->xfer_buff, drv->ep0_in_remain_size);
        }else if(drv->ep0_in_remain_size == ep->maxpacket){
            drv->ep0_in_remain_size = 0;
            if(drv->in_ep_option[0] & TUSB_TXF_ZLP){
                HAL_PCD_EP_Transmit(hpcd, 0, ep->xfer_buff, 0);
            }else{
                tusb_device_ep_xfer_done(drv,
                    0x80,
                    data_buff, drv->in_ep_len[0], 0);
            }
        }else{
            drv->ep0_in_remain_size = 0;
            tusb_device_ep_xfer_done(drv,
            0x80,
            data_buff, drv->in_ep_len[0], 0);
        }
    }else{
        if( (ep->xfer_count != 0) 
            && ((ep->xfer_count % ep->maxpacket) == 0) 
            && (drv->in_ep_option[epnum] & TUSB_TXF_ZLP) ){
            // send ZLP
            HAL_PCD_EP_Transmit(hpcd, epnum | 0x80, ep->xfer_buff, 0);
        }else{
            tusb_device_ep_xfer_done(drv,
                epnum | 0x80,
                data_buff, drv->in_ep_len[epnum], 0);
        }
    }
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    tusb_device_driver_t* drv = (tusb_device_driver_t*)hpcd->pData;
    PCD_EPTypeDef* ep = &hpcd->OUT_ep[epnum];
    tusb_device_ep_xfer_done(drv,
        epnum & 0x7f,
        ep->xfer_buff - ep->xfer_count, ep->xfer_count, 0);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    tusb_device_driver_t* drv = (tusb_device_driver_t*)hpcd->pData;
    PCD_EPTypeDef* ep = &hpcd->IN_ep[epnum];
    tusb_device_ep_xfer_done(drv,
        epnum | 0x80,
        ep->xfer_buff - drv->in_ep_len[epnum], drv->in_ep_len[epnum], 0);
}

void tusb_fix_control_xfer_corrupt_issue(tusb_device_t* dev)
{
#if defined(STM32F7) && defined(USB_OTG_FS)
    if(dev->dev_drv->pcd.Instance == USB_OTG_FS){
        TUSB_LOGD("TODO: STM32F7 OTG FS need flush ep to fix the response data corrupt issue\n");
        USB_FlushTxFifo(dev->dev_drv->pcd.Instance, 0);
    }
#endif
}


#endif // #ifndef NO_DEVICE
#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)
