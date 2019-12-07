/*
 * Name   :  teeny_usb_init.h
 * Author :  admin@xtoolbox.org
 * Date   :  2019-12-03 22:22:50

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  rndis_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB RNDIS DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0031,
    prefix = "RNDIS",
    Config {
        CDC_ACM{
            bInterfaceProtocol = 0xff,
            extDesc = RNDIS("5162001"),
            EndPoint(IN(3),  Interrupt, 8),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        },
   }
}

  ------------- lua script end   ------------
 */
#ifndef __RNDIS_TEENY_USB_INIT_H__
#define __RNDIS_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define RNDIS_VID                                            0x0483
#define RNDIS_PID                                            0x0031
#define RNDIS_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define RNDIS_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define RNDIS_MAX_EP                                         (3)
#define RNDIS_EP_NUM                                         (RNDIS_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef RNDIS_BTABLE_ADDRESS
#undef RNDIS_BTABLE_ADDRESS
#endif
#define RNDIS_BTABLE_ADDRESS                                 (0)
#define RNDIS_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define RNDIS_USB_BUF_START                                  (RNDIS_EP_BUF_DESC_TABLE_SIZE * RNDIS_EP_NUM)

// EndPoints 0 defines
#define RNDIS_EP0_RX_SIZE                                    (64)
#define RNDIS_EP0_RX_ADDR                                    (RNDIS_USB_BUF_START + (0))
#define RNDIS_EP0_TX_SIZE                                    (64)
#define RNDIS_EP0_TX_ADDR                                    (RNDIS_USB_BUF_START + (64))
#define RNDIS_EP0_RX_TYPE                                    USB_EP_CONTROL
#define RNDIS_EP0_TX_TYPE                                    USB_EP_CONTROL

#define RNDIS_EP0_TYPE                                       USB_EP_CONTROL
#define RNDIS_EP0_TX0_ADDR                                   (RNDIS_USB_BUF_START + (0))
#define RNDIS_EP0_TX1_ADDR                                   (RNDIS_USB_BUF_START + (64))
#define RNDIS_EP0_RX0_ADDR                                   (RNDIS_USB_BUF_START + (0))
#define RNDIS_EP0_RX1_ADDR                                   (RNDIS_USB_BUF_START + (64))

// EndPoints 1 defines
#define RNDIS_EP1_RX_SIZE                                    (64)
#define RNDIS_EP1_RX_ADDR                                    (RNDIS_USB_BUF_START + (128))
#define RNDIS_EP1_TX_SIZE                                    (64)
#define RNDIS_EP1_TX_ADDR                                    (RNDIS_USB_BUF_START + (192))
#define RNDIS_EP1_RX_TYPE                                    USB_EP_BULK
#define RNDIS_EP1_TX_TYPE                                    USB_EP_BULK

#define RNDIS_EP1_TYPE                                       USB_EP_BULK
#define RNDIS_EP1_TX0_ADDR                                   (RNDIS_USB_BUF_START + (128))
#define RNDIS_EP1_TX1_ADDR                                   (RNDIS_USB_BUF_START + (192))
#define RNDIS_EP1_RX0_ADDR                                   (RNDIS_USB_BUF_START + (128))
#define RNDIS_EP1_RX1_ADDR                                   (RNDIS_USB_BUF_START + (192))

// EndPoints 2 defines
#define RNDIS_EP2_RX_SIZE                                    (0)
#define RNDIS_EP2_RX_ADDR                                    (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP2_TX_SIZE                                    (0)
#define RNDIS_EP2_TX_ADDR                                    (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP2_RX_TYPE                                    USB_EP_Invalid
#define RNDIS_EP2_TX_TYPE                                    USB_EP_Invalid

#define RNDIS_EP2_TYPE                                       USB_EP_Invalid
#define RNDIS_EP2_TX0_ADDR                                   (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP2_TX1_ADDR                                   (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP2_RX0_ADDR                                   (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP2_RX1_ADDR                                   (RNDIS_USB_BUF_START + (256))

// EndPoints 3 defines
#define RNDIS_EP3_RX_SIZE                                    (0)
#define RNDIS_EP3_RX_ADDR                                    (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP3_TX_SIZE                                    (8)
#define RNDIS_EP3_TX_ADDR                                    (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP3_RX_TYPE                                    USB_EP_Invalid
#define RNDIS_EP3_TX_TYPE                                    USB_EP_INTERRUPT

#define RNDIS_EP3_TYPE                                       USB_EP_INTERRUPT
#define RNDIS_EP3_TX0_ADDR                                   (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP3_TX1_ADDR                                   (RNDIS_USB_BUF_START + (264))
#define RNDIS_EP3_RX0_ADDR                                   (RNDIS_USB_BUF_START + (256))
#define RNDIS_EP3_RX1_ADDR                                   (RNDIS_USB_BUF_START + (264))


// EndPoint max packed sizes
extern const uint8_t RNDIS_txEpMaxSize[];
#define RNDIS_TXEP_MAX_SIZE                                  \
const uint8_t RNDIS_txEpMaxSize[] = \
{ RNDIS_EP0_TX_SIZE, RNDIS_EP1_TX_SIZE, 0, RNDIS_EP3_TX_SIZE,  };
extern const uint8_t RNDIS_rxEpMaxSize[];
#define RNDIS_RXEP_MAX_SIZE                                  \
const uint8_t RNDIS_rxEpMaxSize[] = \
{ RNDIS_EP0_RX_SIZE, RNDIS_EP1_RX_SIZE, 0, 0,  };

// EndPoints init function for USB FS core
#define RNDIS_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, RNDIS_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, RNDIS_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, RNDIS_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, RNDIS_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP1, RNDIS_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, RNDIS_EP1_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP1, RNDIS_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, RNDIS_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    /* Init ep3 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP3, RNDIS_EP3_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP3, RNDIS_EP3_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define RNDIS_OTG_MAX_OUT_SIZE                               (64)
#define RNDIS_OTG_CONTROL_EP_NUM                             (1)
#define RNDIS_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define RNDIS_OTG_RX_FIFO_SIZE_FS                            (256)
#define RNDIS_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 136
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define RNDIS_EP0_TX_FIFO_ADDR_FS                            (256)
#define RNDIS_EP0_TX_FIFO_SIZE_FS                            (RNDIS_EP0_TX_SIZE * 7)
#define RNDIS_EP1_TX_FIFO_ADDR_FS                            (704)
#define RNDIS_EP1_TX_FIFO_SIZE_FS                            (RNDIS_EP1_TX_SIZE * 7)
#define RNDIS_EP3_TX_FIFO_ADDR_FS                            (1152)
#define RNDIS_EP3_TX_FIFO_SIZE_FS                            (RNDIS_EP3_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define RNDIS_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, RNDIS_OTG_RX_FIFO_ADDR_FS, RNDIS_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, RNDIS_EP0_TX_TYPE, RNDIS_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, RNDIS_EP0_TX_FIFO_ADDR_FS, RNDIS_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, RNDIS_EP0_RX_TYPE, RNDIS_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, RNDIS_EP1_TX_TYPE, RNDIS_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, RNDIS_EP1_TX_FIFO_ADDR_FS, RNDIS_EP1_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, RNDIS_EP1_RX_TYPE, RNDIS_EP1_RX_SIZE); \
      /* Init Ep3  */\
      INIT_EP_Tx(dev, PCD_ENDP3, RNDIS_EP3_TX_TYPE, RNDIS_EP3_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP3, RNDIS_EP3_TX_FIFO_ADDR_FS, RNDIS_EP3_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define RNDIS_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define RNDIS_OTG_RX_FIFO_SIZE_HS                            (512)
#define RNDIS_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 136
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define RNDIS_EP0_TX_FIFO_ADDR_HS                            (512)
#define RNDIS_EP0_TX_FIFO_SIZE_HS                            (RNDIS_EP0_TX_SIZE * 7)
#define RNDIS_EP1_TX_FIFO_ADDR_HS                            (960)
#define RNDIS_EP1_TX_FIFO_SIZE_HS                            (RNDIS_EP1_TX_SIZE * 7)
#define RNDIS_EP3_TX_FIFO_ADDR_HS                            (1408)
#define RNDIS_EP3_TX_FIFO_SIZE_HS                            (RNDIS_EP3_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define RNDIS_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, RNDIS_OTG_RX_FIFO_ADDR_HS, RNDIS_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, RNDIS_EP0_TX_TYPE, RNDIS_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, RNDIS_EP0_TX_FIFO_ADDR_HS, RNDIS_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, RNDIS_EP0_RX_TYPE, RNDIS_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, RNDIS_EP1_TX_TYPE, RNDIS_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, RNDIS_EP1_TX_FIFO_ADDR_HS, RNDIS_EP1_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, RNDIS_EP1_RX_TYPE, RNDIS_EP1_RX_SIZE); \
      /* Init Ep3  */\
      INIT_EP_Tx(dev, PCD_ENDP3, RNDIS_EP3_TX_TYPE, RNDIS_EP3_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP3, RNDIS_EP3_TX_FIFO_ADDR_HS, RNDIS_EP3_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define RNDIS_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define RNDIS_TUSB_INIT_EP_OTG(dev) \
  do{\
    RNDIS_TUSB_INIT_EP_OTG_FS(dev); \
    RNDIS_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define RNDIS_TUSB_INIT_EP(dev) RNDIS_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define RNDIS_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = RNDIS_DEV_STATUS;         \
    dev->rx_max_size = RNDIS_rxEpMaxSize;         \
    dev->tx_max_size = RNDIS_txEpMaxSize;         \
    dev->descriptors = &RNDIS_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define RNDIS_TUSB_INIT_EP(dev) RNDIS_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define RNDIS_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = RNDIS_DEV_STATUS;         \
    dev->descriptors = &RNDIS_descriptors;         \
  }while(0)

#endif

#define RNDIS_TUSB_INIT(dev) \
  do{\
    RNDIS_TUSB_INIT_EP(dev);   \
    RNDIS_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if RNDIS_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  RNDIS_EP_NUM
#endif

extern const uint8_t* const RNDIS_StringDescriptors[RNDIS_STRING_COUNT];
extern const tusb_descriptors RNDIS_descriptors;

// Enable WCID related code
#define  HAS_WCID

#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif


#endif   // #ifndef __RNDIS_TEENY_USB_INIT_H__
