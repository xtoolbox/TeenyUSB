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

#include "teeny_usb_host.h"
#include "teeny_usb_host_driver.h"
#include "teeny_usb_util.h"
#include "teeny_usb_def.h"
#include "tusb_platform_stm32.h"
#include "tusb_host_drv_stm32_otg.h"
#include "string.h"
#include "teeny_usb_osal.h"

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#ifndef NO_HOST

struct _tusb_host_driver
{
    void* context;
    HCD_HandleTypeDef hcd;
    uint32_t   fifo_size;
    uint32_t   channel_count;
    tusbh_transfer_t* transfer_map[MAX(USB_OTG_HS_MAX_CH_NUM,USB_OTG_FS_MAX_CH_NUM)];
};


struct _tusbh_drv_xfer_data
{
    int ch_num;
};

#ifdef USB_OTG_FS
static tusb_host_driver_t stm32_fs;
#endif

#ifdef USB_OTG_HS
static tusb_host_driver_t stm32_hs;
#endif

#define TUSB_DRV_LOGD(fmt, ...)  //TUSB_LOGD("[DRIV] " fmt, ##__VA_ARGS__)
#define TUSB_DRV_LOGW(fmt, ...)  TUSB_LOGW("[DRIV] " fmt, ##__VA_ARGS__)
#define TUSB_DRV_LOGE(fmt, ...)  TUSB_LOGE("[DRIV] " fmt, ##__VA_ARGS__)

void tusb_stm32_init_otg_io(const tusb_stm32_otg_io_cfg_t* cfg);

void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    if(hhcd->Init.speed == USB_OTG_SPEED_HIGH){
#ifdef USB_OTG_HS
        NVIC_SetPriority(OTG_HS_IRQn, 0);
        NVIC_EnableIRQ(OTG_HS_IRQn);
#if defined(USB_HS_PHYC)
        if(hhcd->Init.phy_itface == USB_OTG_HS_EMBEDDED_PHY){
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

void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
    if(hhcd->Init.speed == USB_OTG_SPEED_HIGH){
#ifdef USB_OTG_HS
        NVIC_DisableIRQ(OTG_HS_IRQn);
        if(hhcd->Init.phy_itface == USB_OTG_EMBEDDED_PHY){
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



int tusb_host_drv_open(tusb_host_driver_t **pdrv, const tusb_hardware_param_t *param, void *context)
{
    if (!pdrv){
        return -1;
    }
    tusb_host_driver_t* drv;
    tusb_stm32_init_otg_io(param->io_cfgs);
    if(param->is_hs_core){
#ifdef USB_OTG_HS
        drv = &stm32_hs;
        stm32_hs.hcd.Instance = USB_OTG_HS;
        drv->fifo_size = USB_OTG_HS_FIFO_SIZE;
        drv->channel_count = USB_OTG_HS_MAX_CH_NUM;
#else
        TUSB_DRV_LOGE("STM32 OTG HS core not exist\n");
        return -1;
#endif
    }else{
#ifdef USB_OTG_FS
        drv = &stm32_fs;
        stm32_fs.hcd.Instance = USB_OTG_FS;
        drv->fifo_size = USB_OTG_FS_FIFO_SIZE;
        drv->channel_count = USB_OTG_FS_MAX_CH_NUM;
#else
        TUSB_DRV_LOGE("STM32 OTG FS core not exist\n");
        return -1;
#endif
    }
    *pdrv = drv;
    drv->hcd.pData = drv;
    drv->context = context;
    memset(drv->transfer_map, 0, sizeof(drv->transfer_map));
    
    if(param->is_hs_core){
        if(param->is_high_speed){
            drv->hcd.Init.speed = USB_OTG_SPEED_HIGH;
        }else{
            drv->hcd.Init.speed = USB_OTG_SPEED_HIGH_IN_FULL;
        }
        if(param->is_internal_phy){
#if defined(USB_HS_PHYC)
            drv->hcd.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
#else
            TUSB_DRV_LOGE("STM32 OTG HS not have embedded phy\n");
            return -1;
#endif
        }else{
            drv->hcd.Init.phy_itface = USB_OTG_ULPI_PHY;
        }
    }else{
        if(param->dma_enable){
            TUSB_DRV_LOGW("STM32 OTG FS core not support DMA\n");
        }
        drv->hcd.Init.speed = USB_OTG_SPEED_FULL;
        drv->hcd.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
    }
    
    if(param->low_power_enable){
        TUSB_DRV_LOGW("Low power mode is not handle by Teeny USB core\n");
    }
    drv->hcd.Init.Host_channels = drv->channel_count;
    drv->hcd.Init.dma_enable = param->dma_enable;
    drv->hcd.Init.Sof_enable = param->sof_enable;
    drv->hcd.Init.low_power_enable = param->low_power_enable;
    drv->hcd.Init.lpm_enable = param->lpm_enable;
    drv->hcd.Init.battery_charging_enable = param->battery_charging_enable;
    drv->hcd.Init.vbus_sensing_enable = param->vbus_sensing_enable;
    drv->hcd.Init.use_dedicated_ep1 = param->use_dedicated_ep1;
    drv->hcd.Init.use_external_vbus = param->use_external_vbus;
    
    HAL_HCD_Init(&drv->hcd);
    
    HAL_HCD_Start(&drv->hcd);
    return 0;
}

void* tusb_host_drv_get_context(tusb_host_driver_t *drv)
{
    return drv->context;
}


int tusb_host_drv_reset(tusb_host_driver_t *drv, int port)
{
    HAL_StatusTypeDef res = HAL_HCD_ResetPort(&drv->hcd);
    if(res != HAL_OK){
        return -res;
    }
    return 0;
}

static int allocate_channel(tusb_host_driver_t *drv, tusbh_transfer_t* transfer)
{
    int result = -1;
    for(int i=0;i<drv->channel_count;i++){
        if(drv->transfer_map[i]){
            continue;
        }
        drv->transfer_map[i] = transfer;
        result = i;
        break;
    }
    if(result < 0){
        TUSB_DRV_LOGE("No free channel found\n");
    }
    return result;
}

static tusbh_transfer_t* get_channel_transfer(tusb_host_driver_t *drv, int ch_num)
{
    if(ch_num < drv->channel_count){
        return drv->transfer_map[ch_num];
    }
    TUSB_DRV_LOGE("Channel number(%d) large than channel count(%ld), when get transfer\n", ch_num, drv->channel_count);
    return 0;
}

static int free_channel(tusb_host_driver_t *drv, int ch_num)
{
    if(ch_num < drv->channel_count){
        drv->transfer_map[ch_num] = 0;
    }else{
        TUSB_DRV_LOGE("Channel number(%d) large than channel count(%ld), when free channel\n", ch_num, drv->channel_count);
    }
    return 0;
}

static uint8_t tusb_speed_to_hal_speed(uint8_t speed)
{
    switch(speed){
        case PORT_SPEED_HIGH:
            return HCD_SPEED_HIGH;
        case PORT_SPEED_FULL:
            return HCD_SPEED_FULL;
    }
    return HCD_SPEED_LOW;
}

int tusb_host_drv_transfer_add(tusb_host_driver_t *drv, tusbh_transfer_t* transfer)
{
    int ch_num = -1;
    if(transfer->drv_data){
        ch_num = transfer->drv_data->ch_num;
        if(ch_num < drv->channel_count){
            if(drv->transfer_map[ch_num] == transfer){
                TUSB_DRV_LOGD("Use pre-allocate channel number(%d)\n", ch_num);
            }else{
                TUSB_DRV_LOGW("Pre-allocate channel number(%d) error, re-allocate it\n", ch_num);
                ch_num = -1;
            }
        }
    }
    
    if(ch_num < 0){
        ch_num = allocate_channel(drv, transfer);
        if(ch_num < 0){
            return -1;
        }
        TUSB_DRV_LOGD("Channel number(%d) allocate success\n", ch_num);
        tusbh_drv_xfer_data_t* drv_data = (tusbh_drv_xfer_data_t*)tusb_malloc(sizeof(tusbh_drv_xfer_data_t));
        if(!drv_data){
            TUSB_DRV_LOGE("Fail to allocate memory for transfer driver data\n");
            free_channel(drv, ch_num);
            return -1;
        }
        drv_data->ch_num = ch_num;
        transfer->drv_data = drv_data;
    }
    
    HAL_StatusTypeDef ret = HAL_HCD_HC_Init(&drv->hcd, ch_num, transfer->ep_num, transfer->dev_addr, 
    tusb_speed_to_hal_speed(transfer->dev_speed), transfer->ep_type, transfer->ep_mps);
    if(ret != HAL_OK){
        free_channel(drv, ch_num);
        tusb_free(transfer->drv_data);
        transfer->drv_data = 0;
        return -ret;
    }
    
    HAL_HCD_HC_SubmitRequest(
            &drv->hcd, 
            ch_num, 
            (transfer->ep_num & 0x80)?1:0,
            transfer->ep_type & EP_TYPE_MSK,
            transfer->is_setup ? 0 : 1,
            transfer->data,
            transfer->total_length,
            transfer->is_setup ? 0 : 1
         );
    return 0;
}

int tusb_host_drv_transfer_remove(tusb_host_driver_t *drv, tusbh_transfer_t* transfer)
{
    if(!transfer) return -1;
    if(!transfer->drv_data) return -1;
    int ch_num = transfer->drv_data->ch_num;
    TUSB_DRV_LOGD("Channel number(%d) free success\n", ch_num);
    tusb_free(transfer->drv_data);
    transfer->drv_data = 0;
    HAL_HCD_HC_Halt(&drv->hcd, ch_num);
    free_channel(drv, ch_num);
    return 0;
}

uint8_t tusb_host_drv_get_speed(tusb_host_driver_t* drv, uint8_t port)
{
    uint32_t speed = HAL_HCD_GetCurrentSpeed(&drv->hcd);
    switch(speed){
        case USB_OTG_SPEED_HIGH:         return PORT_SPEED_HIGH;
        case USB_OTG_SPEED_HIGH_IN_FULL: return PORT_SPEED_FULL;
        case USB_OTG_SPEED_FULL:         return PORT_SPEED_FULL;
    }
    return PORT_SPEED_LOW;
}

uint32_t tusb_host_drv_get_frame_number(tusb_host_driver_t* drv)
{
    return HAL_HCD_GetCurrentFrame(&drv->hcd);
}

void stm32_otg_host_handler(int is_hs)
{
    if(is_hs){
#ifdef USB_OTG_HS
        HAL_HCD_IRQHandler(&stm32_hs.hcd);
#endif
    }else{
#ifdef USB_OTG_FS
        HAL_HCD_IRQHandler(&stm32_fs.hcd);
#endif
    }
}


void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusb_host_sof_event(drv);
}

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusb_host_port_changed(drv, 0, TUSB_HOST_PORT_CONNECTED);
}

void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusb_host_port_changed(drv, 0, TUSB_HOST_PORT_DISCONNECTED);
}

void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusb_host_port_changed(drv, 0, TUSB_HOST_PORT_ENABLED);
} 

void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusb_host_port_changed(drv, 0, TUSB_HOST_PORT_DISABLED);
} 

void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
    tusb_host_driver_t *drv = (tusb_host_driver_t*)hhcd->pData;
    tusbh_transfer_t* transfer = get_channel_transfer(drv, chnum);
    
    if(!transfer){
        //TUSB_DRV_LOGE("Channel number(%d) transfer not exist\n", chnum);
        return;
    }
    
/************************************************************************/
/*******  Fix the HAL interrupt transfer state issue when Nak         ***/
/***/if(urb_state == URB_IDLE && transfer->ep_type == EP_TYPE_INTR){  /**/
/***/    urb_state = URB_DONE;                                        /**/
/***/    drv->hcd.hc[chnum].state = HC_NAK;                           /**/
/***/}                                                                /**/
/************************************************************************/
    
    if(urb_state == URB_DONE 
        || urb_state == URB_ERROR
        || urb_state == URB_STALL){
        
        TUSB_DRV_LOGD("URB state changed %d\n", urb_state);
        if(transfer){
            switch( HAL_HCD_HC_GetState(&drv->hcd, chnum) ){
                case HC_IDLE:   transfer->state = TS_IDLE;   break;
                case HC_XFRC:   
                    transfer->state = TS_ACK;
                    transfer->actual_length = HAL_HCD_HC_GetXferCount(&drv->hcd, chnum);
                break;
                case HC_NAK:    transfer->state = TS_NAK;    break;
                case HC_NYET:   transfer->state = TS_NYET;   break;
                case HC_STALL:  transfer->state = TS_STALL;  break;
                default:        transfer->state = TS_ERROR;  break;
            }
            
            if(transfer->cb_complete){
                transfer->cb_complete(transfer);
            }
        }
    }
    //tusb_host_channel_event(drv, chnum, ((int)urb_state) + 10);
}


#endif // #ifndef NO_HOST
#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)
