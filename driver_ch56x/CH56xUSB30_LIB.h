/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_usb30_lib.h
* Author             : WCH
* Version            : V1.1
* Date               : 2020/12/23
* Description        :
*******************************************************************************/
#ifndef USB30_CH56X_USB30_LIB_H_
#define USB30_CH56X_USB30_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CH56x_common.h"

// link CFG
#define TERM_EN                 (1<<1)
#define PIPE_RESET              (1<<3)
#define LFPS_RX_PD              (1<<5)
#define CFG_EQ_EN               (1<<6)
#define DEEMPH_CFG              (1<<8)

#define POWER_MODE_0            ((UINT32)0x00000000)
#define POWER_MODE_1            ((UINT32)0x00000001)
#define POWER_MODE_2            ((UINT32)0x00000002)
#define POWER_MODE_3            ((UINT32)0x00000003)

#define LINK_PRESENT            (1<<0)
#define RX_WARM_RESET           ((UINT32)1<<1)

#define LINK_TXEQ               (1<<6)
#define GO_DISABLED             (1<<4)
#define POLLING_EN              (1<<12)

#define TX_HOT_RESET            ((UINT32)1<<16)
#define RX_HOT_RESET            ((UINT32)1<<24)

#define TX_WARM_RESET           ((UINT32)1<<8)
#define TX_Ux_EXIT              ((UINT32)1<<9)
// link int flag
#define LINK_RDY_FLAG           (1<<0)
#define LINK_RECOV_FLAG         (1<<1)
#define LINK_INACT_FLAG         (1<<2)
#define LINK_DISABLE_FLAG       (1<<3)
#define LINK_GO_U3_FLAG         (1<<4)
#define LINK_GO_U2_FLAG         (1<<5)
#define LINK_GO_U1_FLAG         (1<<6)
#define LINK_GO_U0_FLAG         (1<<7)
#define LINK_U3_WAKE_FLAG       (1<<8)
#define LINK_Ux_REJECT_FLAG     (1<<9)
#define TERM_PRESENT_FLAG       (1<<10)
#define LINK_TXEQ_FLAG          (1<<11)
#define LINK_Ux_EXIT_FLAG       (1<<12)
#define WARM_RESET_FLAG         (1<<13)
#define U3_WAKEUP_FLAG          (1<<14)
#define HOT_RESET_FLAG          (1<<15)
#define LINK_RX_DET_FLAG        (1<<20)

#define EP0_R_EN                (1<<0)
#define EP1_R_EN                (1<<1)
#define EP2_R_EN                (1<<2)
#define EP3_R_EN                (1<<3)
#define EP4_R_EN                (1<<4)
#define EP5_R_EN                (1<<5)
#define EP6_R_EN                (1<<6)
#define EP7_R_EN                (1<<7)

#define EP0_T_EN                (1<<8)
#define EP1_T_EN                (1<<9)
#define EP2_T_EN                (1<<10)
#define EP3_T_EN                (1<<11)
#define EP4_T_EN                (1<<12)
#define EP5_T_EN                (1<<13)
#define EP6_T_EN                (1<<14)
#define EP7_T_EN                (1<<15)
// LMP
#define LMP_HP                  0
#define LMP_SUBTYPE_MASK        (0xf<<5)
#define SET_LINK_FUNC           (0x1<<5)
#define U2_INACT_TOUT           (0x2<<5)
#define VENDOR_TEST             (0x3<<5)
#define PORT_CAP                (0x4<<5)
#define PORT_CFG                (0x5<<5)
#define PORT_CFG_RES            (0x6<<5)

#define LINK_SPEED              (1<<9)

#define NUM_HP_BUF              (4<<0)
#define DOWN_STREAM             (1<<16)
#define UP_STREAM               (2<<16)
#define TIE_BRK                 (1<<20)

// status response
#define NRDY                0
#define ACK                 0x01
#define STALL               0x02
#define INVALID             0x03

#define USB_ALL_CLR         (1<<1)
#define USB_FORCE_RST       (1<<2)


#define SS_RX_CONTRL(ep)  (&USBSS->UEP0_RX_CTRL)[ep*4]
#define SS_TX_CONTRL(ep)  (&USBSS->UEP0_TX_CTRL)[ep*4]

static int USB30_Device_Open(void)
{
    USBSS->LINK_CFG = 0x140;
    USBSS->LINK_CTRL = 0x12;
    uint32_t t = 0x4c4b41;
    while(USBSS->LINK_STATUS&4){
        t--;
        if(t == 0)return -1;
    }
    for(int i =0;i<8;i++){
        SS_TX_CONTRL(i) = 0;
        SS_RX_CONTRL(i) = 0;
    }
    USBSS->USB_STATUS = 0x13;

    USBSS->USB_CONTROL = 0x30021;
    USBSS->UEP_CFG = 0;

    USBSS->LINK_CFG |= 2;

    USBSS->LINK_INT_CTRL = 0x10bc7d;

    USBSS->LINK_CTRL = 2;
    return 0;
}

static inline void USB30_Switch_Powermode( UINT8 pwr_mode )
{
    USBSS->LINK_CTRL &= 0xfffffffc;
    USBSS->LINK_CTRL |= pwr_mode;
    while(USBSS->LINK_STATUS & 4);
}

static int USB30_Device_Close(void)
{
    USB30_Switch_Powermode(POWER_MODE_3);
    USBSS->LINK_CFG = PIPE_RESET | LFPS_RX_PD;
    USBSS->LINK_CTRL = GO_DISABLED | POWER_MODE_3;
    USBSS->LINK_INT_CTRL = 0;
    USBSS->USB_CONTROL = USB_FORCE_RST | USB_ALL_CLR;
    return 0;
}

static inline void USB30_BUS_RESET( ){
    USB30_Device_Close();
    mDelaymS(30);
    USB30_Device_Open();
}

static inline void USB30_OUT_Set(UINT8 endp,UINT8 status,UINT8 nump)
{
    UINT32V* p = &USBSS->UEP0_RX_CTRL;
    p+= endp*4;
    *p = *p | ((nump)<<16) | (status << 26);
}

static inline void USB30_Device_Setaddress( UINT32 address )
{
    USBSS->USB_CONTROL &= 0x00ffffff;
    USBSS->USB_CONTROL |= address<<24;
}

static inline uint8_t USB30_IN_Nump(UINT8 endp)
{
    UINT32V* p = &USBSS->UEP0_TX_CTRL;
    p+= endp*4;
    return ((*p)>>16) & 0x1f;
}

static inline void USB30_EP0_IN_set(UINT8 status,UINT8 nump,UINT16 TxLen, uint8_t toggle)
{
    USBSS->UEP0_TX_CTRL |= ((nump<<16) | (status<<26) | (TxLen & 0x3ff) | (toggle << 31));
}

static inline void USB30_IN_Set(UINT8 endp,FunctionalState lpf,UINT8 status,UINT8 nump,UINT16 TxLen)
{
    UINT32V* p = &USBSS->UEP0_TX_CTRL;
    p+= endp*4;
    *p = *p | (nump<<16) | (status<<26) | (TxLen & 0x3ff) | (lpf << 28);
}

static inline void USB30_Send_ERDY(UINT8 endp,UINT8 nump)
{
    UINT32 t = endp & 0xf;
    t = (t << 2) | ((UINT32)nump << 6);
    if( (endp&0x80) == 0){
        USBSS->USB_FC_CTRL = t | (UINT32)1;
        return;
    }else{
        USBSS->USB_FC_CTRL = t | (UINT32)3;
        return;
    }
}

static inline void USB30_OUT_ClearIT(UINT8 endp)
{
    UINT32V* p = &USBSS->UEP0_RX_CTRL;
    p+= endp*4;
    *p &= 0x03e00000;
    if(endp == 5){
        uint32_t t = *p;
        *p = t;
    }
}

static inline void USB30_IN_ClearIT(UINT8 endp)
{
    UINT32V* p = &USBSS->UEP0_TX_CTRL;
    p+= endp*4;
    *p &= 0x03e00000;
}

static inline void USB30_OUT_Status(UINT8 endp,UINT8 *nump,UINT16 *len,UINT8 *status)
{
    UINT32V* p = &USBSS->UEP0_RX_CTRL;
    p+= endp*4;
    uint32_t t = *p;
    *len = t;
    *nump = (t >>16) & 31;
    *status = (t>>28) & 7;
}




#ifdef __cplusplus
}
#endif

#endif /* USB30_CH56X_USB30_LIB_H_ */
