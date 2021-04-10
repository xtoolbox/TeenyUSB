/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_usb20.h
* Author             : WCH
* Version            : V1.1
* Date               : 2020/12/23
* Description        :
*******************************************************************************/
#ifndef USB20_CH56X_USB20_H_
#define USB20_CH56X_USB20_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Global define */
// USB CONTROL
#define DMA_EN				(1<<0)
#define USB_ALL_CLR			(1<<1)
#define USB_FORCE_RST		(1<<2)
#define INT_BUSY_EN			(1<<3)
#define DEV_PU_EN			(1<<4)	

#define HOST_MODE			(1<<7)

#define FULL_SPEED			(0<<5)
#define HIGH_SPEED			(1<<5)
#define LOW_SPEED			(2<<5)

// UHOST
#define HOST_TX_EN			(1<<14)
#define HOST_RX_EN			(1<<11)

#define SEND_BUS_RESET		(UINT32)(1<<8)
#define SEND_BUS_SUSPEND	(UINT32)(1<<9)
#define SEND_BUS_RESUME		(UINT32)(1<<10)
#define SEND_SOF_EN			(UINT32)(1<<15)
// INT_EN
#define USB2_DETECT_EN		(1<<16)
#define USB2_ACT_EN			(1<<17)
#define USB2_SUSP_EN		(1<<18)
#define USB2_SOF_EN			(1<<19)
#define USB2_OVER_EN		(1<<20)
#define USB2_SETUP_EN		(1<<21)
#define USB2_ISO_EN			(1<<22)

#define USB2_ATTACH			(1<<9)
// INT_FLAG
#define USB2_DETECT_FLAG	(1<<16)
#define USB2_ACT_FLAG		(1<<17)
#define USB2_SUSP_FLAG		(1<<18)
#define USB2_SOF_FLAG		(1<<19)
#define USB2_OVER_FLAG		(1<<20)
#define USB2_SETUP_FLAG		(1<<21)
#define USB2_ISO_FLAG		(1<<22)

#define EP_T_RES_MASK       (3<<(16+0))
#define EP_T_RES_ACK        (0<<(16+0))
#define EP_T_RES_NYET       (1<<(16+0))
#define EP_T_RES_NAK        (2<<(16+0))
#define EP_T_RES_STALL      (3<<(16+0))

#define EP_T_TRES_NO        (1<<(16+2))


#define EP_T_TOG_MASK       (3<<(16+3))
#define EP_T_TOG_0          (0<<(16+3))
#define EP_T_TOG_1          (1<<(16+3))
#define EP_T_TOG_2          (2<<(16+3))
#define EP_T_TOG_M          (3<<(16+3))

#define EP_T_AUTO_TOG       (1<<(16+5))

#define EP_R_RES_MASK       (3<<(24+0))
#define EP_R_RES_ACK        (0<<(24+0))
#define EP_R_RES_NYET       (1<<(24+0))
#define EP_R_RES_NAK        (2<<(24+0))
#define EP_R_RES_STALL      (3<<(24+0))

#define EP_R_TRES_NO        (1<<(24+2))

#define EP_R_TOG_MASK       (3<<(24+3))
#define EP_R_TOG_0          (0<<(24+3))
#define EP_R_TOG_1          (1<<(24+3))
#define EP_R_TOG_2          (2<<(24+3))
#define EP_R_TOG_M          (3<<(24+3))

#define EP_R_AUTO_TOG       (1<<(24+5))

#define TOG_MATCH			(UINT32)(1<<30)

#define EP_DB_BUFFER(ep)    (1 << ( ((ep) == 4 ? 0 : (ep))*4))
#define EP_RX_EN(ep)        (8 << ( ((ep) == 4 ? 0 : (ep))*4))
#define EP_TX_EN(ep)        (4 << ( ((ep) == 4 ? 0 : (ep))*4))

// 00: OUT, 01:SOF, 10:IN, 11:SETUP
#define PID_OUT		0
#define PID_SOF		1
#define PID_IN		2
//#define PID_SETUP	3
#define USBHS_BASE             (0x40000000 + 0x9000)
typedef struct
{
__IO uint32_t USB_CONTROL;
__IO uint32_t USB_FRAME_NO;
__IO uint32_t USB_STATUS;
__IO uint32_t USB_RX_LEN;
__IO uint32_t USB_BUF_MODE;
__IO uint32_t UEP0_DMA;
__IO uint32_t UEP1_RX_DMA;
__IO uint32_t UEP2_RX_DMA;
__IO uint32_t UEP3_RX_DMA;
__IO uint32_t UEP4_RX_DMA;
__IO uint32_t UEP5_RX_DMA;
__IO uint32_t UEP6_RX_DMA;
__IO uint32_t UEP7_RX_DMA;
__IO uint32_t UEP1_TX_DMA;
__IO uint32_t UEP2_TX_DMA;
__IO uint32_t UEP3_TX_DMA;
__IO uint32_t UEP4_TX_DMA;
__IO uint32_t UEP5_TX_DMA;
__IO uint32_t UEP6_TX_DMA;
__IO uint32_t UEP7_TX_DMA;
__IO uint32_t UEP0_MAX_LEN;
__IO uint32_t UEP1_MAX_LEN;
__IO uint32_t UEP2_MAX_LEN;
__IO uint32_t UEP3_MAX_LEN;
__IO uint32_t UEP4_MAX_LEN;
__IO uint32_t UEP5_MAX_LEN;
__IO uint32_t UEP6_MAX_LEN;
__IO uint32_t UEP7_MAX_LEN;
__IO uint32_t UEP0_CTRL;
__IO uint32_t UEP1_CTRL;
__IO uint32_t UEP2_CTRL;
__IO uint32_t UEP3_CTRL;
__IO uint32_t UEP4_CTRL;
__IO uint32_t UEP5_CTRL;
__IO uint32_t UEP6_CTRL;
__IO uint32_t UEP7_CTRL;
}USBHS_TypeDef;

#define USBHS   ((USBHS_TypeDef *) USBHS_BASE)

void USB20_Device_Init ( FunctionalState sta );  // USBSS device initial

#ifdef __cplusplus
}
#endif

#endif

