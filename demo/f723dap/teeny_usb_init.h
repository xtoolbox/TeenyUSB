/*
 * Name   :  teeny_usb_init.h
 * Author :  admin@xtoolbox.org
 * Date   :  2019-10-29 22:42:06

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit https://github.com/xtoolbox/TeenyDT for more info
 */

/*
  Input source name:  cmsis_dap_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return {
  Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB HS CMSIS-DAP",
    strSerial = "TUSB123456HS",
    idVendor = 0x0d28,
    idProduct = 0x0204,
    bcdUSB = 0x200,
    prefix = "TUSB_HS_DAP",
    Config {
        Interface{
            strInterface = "CMSIS-DAP v2",
            WCID=WinUSB,
            GUID="{CDB3B5AD-293B-4663-AA36-1AAE46463776}",
            bInterfaceClass = 0xff,
            bInterfaceSubClass = 0x00,
            EndPoint(OUT(1),  Bulk, 512),
            EndPoint(IN(2), Bulk, 512),
        },
   }
 },
 Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB FS CMSIS-DAP",
    strSerial = "TUSB123456FS",
    idVendor = 0x0d28,
    idProduct = 0x0204,
    bcdUSB = 0x200,
    prefix = "TUSB_FS_DAP",
    Config {
        USB_HID{
            strInterface = "CMSIS-DAP v1",
            ReadEp = EndPoint(IN(2),  Interrupt, 64),
            WriteEp = EndPoint(OUT(1), Interrupt, 64),
            report = HID_BuildReport([[
// report descriptor for general input/output
  0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        // Usage (0x01)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (0x01)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x40,        //   Report Count 64
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x40,        //   Report Count 64
  0x09, 0x01,        //   Usage (0x01)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x01,        //   Report Count 1
  0x09, 0x01,        //   Usage (0x01)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0               // End Collection
]]),
        }
   }
 }
}

  ------------- lua script end   ------------
 */
/////////////////////////////////////////
//// Device 0
/////////////////////////////////////////
#ifndef __TUSB_HS_DAP_TEENY_USB_INIT_H__
#define __TUSB_HS_DAP_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define TUSB_HS_DAP_VID                                            0x0d28
#define TUSB_HS_DAP_PID                                            0x0204
#define TUSB_HS_DAP_STRING_COUNT                                   (5)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define TUSB_HS_DAP_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define TUSB_HS_DAP_MAX_EP                                         (2)
#define TUSB_HS_DAP_EP_NUM                                         (TUSB_HS_DAP_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef TUSB_HS_DAP_BTABLE_ADDRESS
#undef TUSB_HS_DAP_BTABLE_ADDRESS
#endif
#define TUSB_HS_DAP_BTABLE_ADDRESS                                 (0)
#define TUSB_HS_DAP_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define TUSB_HS_DAP_USB_BUF_START                                  (TUSB_HS_DAP_EP_BUF_DESC_TABLE_SIZE * TUSB_HS_DAP_EP_NUM)

// EndPoints 0 defines
#define TUSB_HS_DAP_EP0_RX_SIZE                                    (64)
#define TUSB_HS_DAP_EP0_RX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (0))
#define TUSB_HS_DAP_EP0_TX_SIZE                                    (64)
#define TUSB_HS_DAP_EP0_TX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (64))
#define TUSB_HS_DAP_EP0_RX_TYPE                                    USB_EP_CONTROL
#define TUSB_HS_DAP_EP0_TX_TYPE                                    USB_EP_CONTROL

#define TUSB_HS_DAP_EP0_TYPE                                       USB_EP_CONTROL
#define TUSB_HS_DAP_EP0_TX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (0))
#define TUSB_HS_DAP_EP0_TX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (64))
#define TUSB_HS_DAP_EP0_RX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (0))
#define TUSB_HS_DAP_EP0_RX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (64))

// EndPoints 1 defines
#define TUSB_HS_DAP_EP1_RX_SIZE                                    (512)
#define TUSB_HS_DAP_EP1_RX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (128))
#define TUSB_HS_DAP_EP1_TX_SIZE                                    (0)
#define TUSB_HS_DAP_EP1_TX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP1_RX_TYPE                                    USB_EP_BULK
#define TUSB_HS_DAP_EP1_TX_TYPE                                    USB_EP_Invalid

#define TUSB_HS_DAP_EP1_TYPE                                       USB_EP_BULK
#define TUSB_HS_DAP_EP1_TX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (128))
#define TUSB_HS_DAP_EP1_TX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP1_RX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (128))
#define TUSB_HS_DAP_EP1_RX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (640))

// EndPoints 2 defines
#define TUSB_HS_DAP_EP2_RX_SIZE                                    (0)
#define TUSB_HS_DAP_EP2_RX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP2_TX_SIZE                                    (512)
#define TUSB_HS_DAP_EP2_TX_ADDR                                    (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP2_RX_TYPE                                    USB_EP_Invalid
#define TUSB_HS_DAP_EP2_TX_TYPE                                    USB_EP_BULK

#define TUSB_HS_DAP_EP2_TYPE                                       USB_EP_BULK
#define TUSB_HS_DAP_EP2_TX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP2_TX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (1152))
#define TUSB_HS_DAP_EP2_RX0_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (640))
#define TUSB_HS_DAP_EP2_RX1_ADDR                                   (TUSB_HS_DAP_USB_BUF_START + (1152))


// EndPoint max packed sizes
extern const uint16_t TUSB_HS_DAP_txEpMaxSize[];
#define TUSB_HS_DAP_TXEP_MAX_SIZE                                  \
const uint16_t TUSB_HS_DAP_txEpMaxSize[] = \
{ TUSB_HS_DAP_EP0_TX_SIZE, 0, TUSB_HS_DAP_EP2_TX_SIZE,  };
extern const uint16_t TUSB_HS_DAP_rxEpMaxSize[];
#define TUSB_HS_DAP_RXEP_MAX_SIZE                                  \
const uint16_t TUSB_HS_DAP_rxEpMaxSize[] = \
{ TUSB_HS_DAP_EP0_RX_SIZE, TUSB_HS_DAP_EP1_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define TUSB_HS_DAP_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_RxOnly(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_TYPE);  \
    SET_RX_ADDR(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define TUSB_HS_DAP_OTG_MAX_OUT_SIZE                               (512)
#define TUSB_HS_DAP_OTG_CONTROL_EP_NUM                             (1)
#define TUSB_HS_DAP_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 145

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define TUSB_HS_DAP_OTG_RX_FIFO_SIZE_FS                            (580)
#define TUSB_HS_DAP_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 576
// Remain Fifo size is 700 in bytes, Rx Used 580 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define TUSB_HS_DAP_EP0_TX_FIFO_ADDR_FS                            (580)
#define TUSB_HS_DAP_EP0_TX_FIFO_SIZE_FS                            (TUSB_HS_DAP_EP0_TX_SIZE * 1)
#define TUSB_HS_DAP_EP2_TX_FIFO_ADDR_FS                            (644)
#define TUSB_HS_DAP_EP2_TX_FIFO_SIZE_FS                            (TUSB_HS_DAP_EP2_TX_SIZE * 1)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, TUSB_HS_DAP_OTG_RX_FIFO_ADDR_FS, TUSB_HS_DAP_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_TYPE, TUSB_HS_DAP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_FIFO_ADDR_FS, TUSB_HS_DAP_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_TYPE, TUSB_HS_DAP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Rx(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_TYPE, TUSB_HS_DAP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_TYPE, TUSB_HS_DAP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_FIFO_ADDR_FS, TUSB_HS_DAP_EP2_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define TUSB_HS_DAP_OTG_RX_FIFO_SIZE_HS                            (580)
#define TUSB_HS_DAP_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 576
// Remain Fifo size is 3516 in bytes, Rx Used 580 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define TUSB_HS_DAP_EP0_TX_FIFO_ADDR_HS                            (580)
#define TUSB_HS_DAP_EP0_TX_FIFO_SIZE_HS                            (TUSB_HS_DAP_EP0_TX_SIZE * 6)
#define TUSB_HS_DAP_EP2_TX_FIFO_ADDR_HS                            (964)
#define TUSB_HS_DAP_EP2_TX_FIFO_SIZE_HS                            (TUSB_HS_DAP_EP2_TX_SIZE * 6)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, TUSB_HS_DAP_OTG_RX_FIFO_ADDR_HS, TUSB_HS_DAP_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_TYPE, TUSB_HS_DAP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_FIFO_ADDR_HS, TUSB_HS_DAP_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_TYPE, TUSB_HS_DAP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Rx(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_TYPE, TUSB_HS_DAP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_TYPE, TUSB_HS_DAP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_FIFO_ADDR_HS, TUSB_HS_DAP_EP2_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG(dev) \
  do{\
    TUSB_HS_DAP_TUSB_INIT_EP_OTG_FS(dev); \
    TUSB_HS_DAP_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define TUSB_HS_DAP_TUSB_INIT_EP(dev) TUSB_HS_DAP_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define TUSB_HS_DAP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = TUSB_HS_DAP_DEV_STATUS;         \
    dev->rx_max_size = TUSB_HS_DAP_rxEpMaxSize;         \
    dev->tx_max_size = TUSB_HS_DAP_txEpMaxSize;         \
    dev->descriptors = &TUSB_HS_DAP_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define TUSB_HS_DAP_TUSB_INIT_EP(dev) TUSB_HS_DAP_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define TUSB_HS_DAP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = TUSB_HS_DAP_DEV_STATUS;         \
    dev->descriptors = &TUSB_HS_DAP_descriptors;         \
  }while(0)

#endif

#define TUSB_HS_DAP_TUSB_INIT(dev) \
  do{\
    TUSB_HS_DAP_TUSB_INIT_EP(dev);   \
    TUSB_HS_DAP_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if TUSB_HS_DAP_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  TUSB_HS_DAP_EP_NUM
#endif

extern const uint8_t* const TUSB_HS_DAP_StringDescriptors[TUSB_HS_DAP_STRING_COUNT];
extern const tusb_descriptors TUSB_HS_DAP_descriptors;

// Enable WCID related code
#define  HAS_WCID

#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif


#endif   // #ifndef __TUSB_HS_DAP_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 1
/////////////////////////////////////////
#ifndef __TUSB_FS_DAP_TEENY_USB_INIT_H__
#define __TUSB_FS_DAP_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define TUSB_FS_DAP_VID                                            0x0d28
#define TUSB_FS_DAP_PID                                            0x0204
#define TUSB_FS_DAP_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define TUSB_FS_DAP_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define TUSB_FS_DAP_MAX_EP                                         (2)
#define TUSB_FS_DAP_EP_NUM                                         (TUSB_FS_DAP_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef TUSB_FS_DAP_BTABLE_ADDRESS
#undef TUSB_FS_DAP_BTABLE_ADDRESS
#endif
#define TUSB_FS_DAP_BTABLE_ADDRESS                                 (0)
#define TUSB_FS_DAP_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define TUSB_FS_DAP_USB_BUF_START                                  (TUSB_FS_DAP_EP_BUF_DESC_TABLE_SIZE * TUSB_FS_DAP_EP_NUM)

// EndPoints 0 defines
#define TUSB_FS_DAP_EP0_RX_SIZE                                    (64)
#define TUSB_FS_DAP_EP0_RX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (0))
#define TUSB_FS_DAP_EP0_TX_SIZE                                    (64)
#define TUSB_FS_DAP_EP0_TX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (64))
#define TUSB_FS_DAP_EP0_RX_TYPE                                    USB_EP_CONTROL
#define TUSB_FS_DAP_EP0_TX_TYPE                                    USB_EP_CONTROL

#define TUSB_FS_DAP_EP0_TYPE                                       USB_EP_CONTROL
#define TUSB_FS_DAP_EP0_TX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (0))
#define TUSB_FS_DAP_EP0_TX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (64))
#define TUSB_FS_DAP_EP0_RX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (0))
#define TUSB_FS_DAP_EP0_RX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (64))

// EndPoints 1 defines
#define TUSB_FS_DAP_EP1_RX_SIZE                                    (64)
#define TUSB_FS_DAP_EP1_RX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (128))
#define TUSB_FS_DAP_EP1_TX_SIZE                                    (0)
#define TUSB_FS_DAP_EP1_TX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP1_RX_TYPE                                    USB_EP_INTERRUPT
#define TUSB_FS_DAP_EP1_TX_TYPE                                    USB_EP_Invalid

#define TUSB_FS_DAP_EP1_TYPE                                       USB_EP_INTERRUPT
#define TUSB_FS_DAP_EP1_TX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (128))
#define TUSB_FS_DAP_EP1_TX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP1_RX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (128))
#define TUSB_FS_DAP_EP1_RX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (192))

// EndPoints 2 defines
#define TUSB_FS_DAP_EP2_RX_SIZE                                    (0)
#define TUSB_FS_DAP_EP2_RX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP2_TX_SIZE                                    (64)
#define TUSB_FS_DAP_EP2_TX_ADDR                                    (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP2_RX_TYPE                                    USB_EP_Invalid
#define TUSB_FS_DAP_EP2_TX_TYPE                                    USB_EP_INTERRUPT

#define TUSB_FS_DAP_EP2_TYPE                                       USB_EP_INTERRUPT
#define TUSB_FS_DAP_EP2_TX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP2_TX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (256))
#define TUSB_FS_DAP_EP2_RX0_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (192))
#define TUSB_FS_DAP_EP2_RX1_ADDR                                   (TUSB_FS_DAP_USB_BUF_START + (256))


// EndPoint max packed sizes
extern const uint8_t TUSB_FS_DAP_txEpMaxSize[];
#define TUSB_FS_DAP_TXEP_MAX_SIZE                                  \
const uint8_t TUSB_FS_DAP_txEpMaxSize[] = \
{ TUSB_FS_DAP_EP0_TX_SIZE, 0, TUSB_FS_DAP_EP2_TX_SIZE,  };
extern const uint8_t TUSB_FS_DAP_rxEpMaxSize[];
#define TUSB_FS_DAP_RXEP_MAX_SIZE                                  \
const uint8_t TUSB_FS_DAP_rxEpMaxSize[] = \
{ TUSB_FS_DAP_EP0_RX_SIZE, TUSB_FS_DAP_EP1_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define TUSB_FS_DAP_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_RxOnly(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_TYPE);  \
    SET_RX_ADDR(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define TUSB_FS_DAP_OTG_MAX_OUT_SIZE                               (64)
#define TUSB_FS_DAP_OTG_CONTROL_EP_NUM                             (1)
#define TUSB_FS_DAP_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define TUSB_FS_DAP_OTG_RX_FIFO_SIZE_FS                            (256)
#define TUSB_FS_DAP_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define TUSB_FS_DAP_EP0_TX_FIFO_ADDR_FS                            (256)
#define TUSB_FS_DAP_EP0_TX_FIFO_SIZE_FS                            (TUSB_FS_DAP_EP0_TX_SIZE * 7)
#define TUSB_FS_DAP_EP2_TX_FIFO_ADDR_FS                            (704)
#define TUSB_FS_DAP_EP2_TX_FIFO_SIZE_FS                            (TUSB_FS_DAP_EP2_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, TUSB_FS_DAP_OTG_RX_FIFO_ADDR_FS, TUSB_FS_DAP_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_TYPE, TUSB_FS_DAP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_FIFO_ADDR_FS, TUSB_FS_DAP_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_TYPE, TUSB_FS_DAP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Rx(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_TYPE, TUSB_FS_DAP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_TYPE, TUSB_FS_DAP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_FIFO_ADDR_FS, TUSB_FS_DAP_EP2_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define TUSB_FS_DAP_OTG_RX_FIFO_SIZE_HS                            (512)
#define TUSB_FS_DAP_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define TUSB_FS_DAP_EP0_TX_FIFO_ADDR_HS                            (512)
#define TUSB_FS_DAP_EP0_TX_FIFO_SIZE_HS                            (TUSB_FS_DAP_EP0_TX_SIZE * 7)
#define TUSB_FS_DAP_EP2_TX_FIFO_ADDR_HS                            (960)
#define TUSB_FS_DAP_EP2_TX_FIFO_SIZE_HS                            (TUSB_FS_DAP_EP2_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, TUSB_FS_DAP_OTG_RX_FIFO_ADDR_HS, TUSB_FS_DAP_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_TYPE, TUSB_FS_DAP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_FIFO_ADDR_HS, TUSB_FS_DAP_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_TYPE, TUSB_FS_DAP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Rx(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_TYPE, TUSB_FS_DAP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_TYPE, TUSB_FS_DAP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_FIFO_ADDR_HS, TUSB_FS_DAP_EP2_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG(dev) \
  do{\
    TUSB_FS_DAP_TUSB_INIT_EP_OTG_FS(dev); \
    TUSB_FS_DAP_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define TUSB_FS_DAP_TUSB_INIT_EP(dev) TUSB_FS_DAP_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define TUSB_FS_DAP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = TUSB_FS_DAP_DEV_STATUS;         \
    dev->rx_max_size = TUSB_FS_DAP_rxEpMaxSize;         \
    dev->tx_max_size = TUSB_FS_DAP_txEpMaxSize;         \
    dev->descriptors = &TUSB_FS_DAP_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define TUSB_FS_DAP_TUSB_INIT_EP(dev) TUSB_FS_DAP_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define TUSB_FS_DAP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = TUSB_FS_DAP_DEV_STATUS;         \
    dev->descriptors = &TUSB_FS_DAP_descriptors;         \
  }while(0)

#endif

#define TUSB_FS_DAP_TUSB_INIT(dev) \
  do{\
    TUSB_FS_DAP_TUSB_INIT_EP(dev);   \
    TUSB_FS_DAP_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if TUSB_FS_DAP_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  TUSB_FS_DAP_EP_NUM
#endif

extern const uint8_t* const TUSB_FS_DAP_StringDescriptors[TUSB_FS_DAP_STRING_COUNT];
extern const tusb_descriptors TUSB_FS_DAP_descriptors;

#define TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0  (33)
extern const uint8_t TUSB_FS_DAP_ReportDescriptor_if0[TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0];


#endif   // #ifndef __TUSB_FS_DAP_TEENY_USB_INIT_H__
