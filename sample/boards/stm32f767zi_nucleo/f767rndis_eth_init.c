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

// STM32F767ZI Nucleo eth init

#include "stm32f7xx_hal.h"
#include "teeny_usb.h"
#include <string.h>

#define LAN8742A_PHY_ADDRESS  0
#define PHY_ADDR  LAN8742A_PHY_ADDRESS

#define PHY_ISFR                        ((uint16_t)0x000BU)    /*!< PHY Interrupt Source Flag register Offset   */
#define PHY_ISFR_INT4                   ((uint16_t)0x000BU)  /*!< PHY Link down inturrupt       */  

ETH_HandleTypeDef heth;

__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB

#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG

#define MODE(x, pin)  ((x) << ((pin)*2))
#define TYPE(x, pin)  ((x) << ((pin)))
#define AF(x,  pin)    (((pin)<8) ? ((uint32_t)(x) << ((pin)*4)) : ((uint32_t)(x) << ( ((pin)-8)*4)) )

void eth_init(uint8_t* mac)
{   
    __HAL_RCC_ETH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
        /**ETH GPIO Configuration    
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB13     ------> ETH_TXD1
    PG11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0 
    */
    GPIOA->MODER &= ~ (MODE(3,1) | MODE(3,2) | MODE(3,7));
    GPIOA->MODER |=   (MODE(2,1) | MODE(2,2) | MODE(2,7));
    GPIOA->OTYPER &= ~(TYPE(1,1) | TYPE(1,2) | TYPE(1,7));
    GPIOA->OSPEEDR |= (MODE(3,1) | MODE(3,2) | MODE(3,7));
    GPIOA->AFR[0] &= ~(AF(0xf,1) | AF(0xf,2) | AF(0xf,7));
    GPIOA->AFR[0] |=  (AF(GPIO_AF11_ETH,1) | AF(GPIO_AF11_ETH,2) | AF(GPIO_AF11_ETH,7));
    
    GPIOB->MODER &= ~ (MODE(3,13));
    GPIOB->MODER |=   (MODE(2,13));
    GPIOB->OTYPER &= ~(TYPE(1,13));
    GPIOB->OSPEEDR |= (MODE(3,13));
    GPIOB->AFR[1] &= ~(AF(0xf,13));
    GPIOB->AFR[1] |=  (AF(GPIO_AF11_ETH,13));
    
    GPIOC->MODER &= ~ (MODE(3,1) | MODE(3,4) | MODE(3,5));
    GPIOC->MODER |=   (MODE(2,1) | MODE(2,4) | MODE(2,5));
    GPIOC->OTYPER &= ~(TYPE(1,1) | TYPE(1,4) | TYPE(1,5));
    GPIOC->OSPEEDR |= (MODE(3,1) | MODE(3,4) | MODE(3,5));
    GPIOC->AFR[0] &= ~(AF(0xf,1) | AF(0xf,4) | AF(0xf,5));
    GPIOC->AFR[0] |=  (AF(GPIO_AF11_ETH,1) | AF(GPIO_AF11_ETH,4) | AF(GPIO_AF11_ETH,5));
    
    GPIOG->MODER &= ~ (MODE(3,11) | MODE(3,13));
    GPIOG->MODER |=   (MODE(2,11) | MODE(2,13));
    GPIOG->OTYPER &= ~(TYPE(1,11) | TYPE(1,13));
    GPIOG->OSPEEDR |= (MODE(3,11) | MODE(3,13));
    GPIOG->AFR[1] &= ~(AF(0xf,11) | AF(0xf,13));
    GPIOG->AFR[1] |=  (AF(GPIO_AF11_ETH,11) | AF(GPIO_AF11_ETH,13));

    heth.Instance = ETH;
    heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
    heth.Init.MACAddr = mac;
    heth.Init.RxMode = ETH_RXPOLLING_MODE;
    // Disable hardware checksum, or it will set checksum to 0
    //heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    heth.Init.ChecksumMode = ETH_CHECKSUM_BY_SOFTWARE;
    heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
    HAL_ETH_Init(&heth);
    
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
     
    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
    
    
    HAL_ETH_Start(&heth);
    
    uint32_t regvalue = 0;
    HAL_ETH_ReadPHYRegister(&heth, PHY_ISFR, &regvalue);
    regvalue |= (PHY_ISFR_INT4);

    /* Enable Interrupt on change of link status */ 
    HAL_ETH_WritePHYRegister(&heth, PHY_ISFR , regvalue );
  
    /* Read Register Configuration */
    HAL_ETH_ReadPHYRegister(&heth, PHY_ISFR , &regvalue);
}

int eth_send(const void* data, int len)
{
  int errval;
  const uint8_t* payload = (const uint8_t*)data;
  uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);
  __IO ETH_DMADescTypeDef *DmaTxDesc;
  uint32_t framelength = 0;
  uint32_t bufferoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t payloadoffset = 0;
  DmaTxDesc = heth.TxDesc;
  bufferoffset = 0;
  
  /* copy frame from pbufs to driver buffers */
  //for(q = p; q != NULL; q = q->next)
    {
      /* Is this buffer available? If not, goto error */
      if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
      {
        errval = -1;
        goto error;
      }
    
      /* Get bytes in current lwIP buffer */
      byteslefttocopy = len;
      payloadoffset = 0;
    
      /* Check if the length of data to copy is bigger than Tx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE )
      {
        /* Copy data to Tx buffer*/
        memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset) );
      
        /* Point to next descriptor */
        DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
      
        /* Check if the buffer is available */
        if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
        {
          errval = -1;
          goto error;
        }
      
        buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);
      
        byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
        framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }
    
      /* Copy the remaining bytes */
      memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)payload + payloadoffset), byteslefttocopy );
      bufferoffset = bufferoffset + byteslefttocopy;
      framelength = framelength + byteslefttocopy;
    }
  
  /* Prepare transmit descriptors to give to DMA */ 
  HAL_ETH_TransmitFrame(&heth, framelength);
  
  errval = framelength;
  
error:
  
  /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
  if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
  {
    /* Clear TUS ETHERNET DMA flag */
    heth.Instance->DMASR = ETH_DMASR_TUS;

    /* Resume DMA transmission*/
    heth.Instance->DMATPDR = 0;
  }
  return errval;
}

int eth_recv(void* data, int size)
{
    int res = 0;
    uint16_t len = 0;
    uint8_t *buffer;
    uint8_t* payload = (uint8_t*)data;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t i=0;
    if (HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
        return 0;
    len = heth.RxFrameInfos.length;
    buffer = (uint8_t *)heth.RxFrameInfos.buffer;
    
  if (data && len<=size)
  {
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    bufferoffset = 0;
    //for(q = p; q != NULL; q = q->next)
    {
      byteslefttocopy = len;
      payloadoffset = 0;
      
      /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
      {
        /* Copy data to pbuf */
        memcpy( (uint8_t*)((uint8_t*)payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));
        
        /* Point to next descriptor */
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
        buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);
        
        byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }
      /* Copy remaining data in pbuf */
      memcpy( (uint8_t*)((uint8_t*)payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
    res = len;
  }else{
      res = -1;
  } 
    
    
    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i=0; i< heth.RxFrameInfos.SegCount; i++)
    {  
      dmarxdesc->Status |= ETH_DMARXDESC_OWN;
      dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    
    /* Clear Segment_Count */
    heth.RxFrameInfos.SegCount =0;  
  
    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)  
    {
        /* Clear RBUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        heth.Instance->DMARPDR = 0;
    }
    return res;
}

void eth_linkchange(int is_up)
{
  __IO uint32_t tickstart = 0;
  uint32_t regvalue = 0;
  
  if(is_up)
  { 
    /* Restart the auto-negotiation */
    if(heth.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
    {
      /* Enable Auto-Negotiation */
      HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_AUTONEGOTIATION);
      
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Wait until the auto-negotiation will be completed */
      do
      {
        HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
        
        /* Check for the Timeout ( 1s ) */
        if((HAL_GetTick() - tickstart ) > 1000)
        {     
          /* In case of timeout */ 
          goto error;
        }   
      } while (((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
      
      /* Read the result of the auto-negotiation */
      HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);
      
      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        heth.Init.DuplexMode = ETH_MODE_HALFDUPLEX;           
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(regvalue & PHY_SPEED_STATUS)
      {  
        /* Set Ethernet speed to 10M following the auto-negotiation */
        heth.Init.Speed = ETH_SPEED_10M; 
      }
      else
      {   
        /* Set Ethernet speed to 100M following the auto-negotiation */ 
        heth.Init.Speed = ETH_SPEED_100M;
      }
    }
    else /* AutoNegotiation Disable */
    {
    error :
      /* Check parameters */
      assert_param(IS_ETH_SPEED(heth.Init.Speed));
      assert_param(IS_ETH_DUPLEX_MODE(heth.Init.DuplexMode));
      
      /* Set MAC Speed and Duplex Mode to PHY */
      HAL_ETH_WritePHYRegister(&heth, PHY_BCR, ((uint16_t)(heth.Init.DuplexMode >> 3) |
                                                     (uint16_t)(heth.Init.Speed >> 1))); 
    }

    /* ETHERNET MAC Re-Configuration */
    HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *) NULL);

    /* Restart MAC interface */
    HAL_ETH_Start(&heth);   
  }
  else
  {
    /* Stop MAC interface */
    HAL_ETH_Stop(&heth);
  }
}
