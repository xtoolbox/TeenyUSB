/*
 * Name   :  teeny_usb_init.h
 * Author :  admin@xtoolbox.org
 * Date   :  2019-12-10 17:48:38

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  drd_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return {

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB CDC DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0012,
    prefix = "CDC",
    Config {
        CDC_ACM{
            EndPoint(IN(2),  Interrupt, 16),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB MSC DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0013,
    prefix = "MSC",
    Config {
        Interface{
            bInterfaceClass = 0x08,        -- MSC
            bInterfaceSubClass = 0x06,     -- SCSI
            bInterfaceProtocol = 0x50,     -- BOT
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
        },
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB Boot Keyboard DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0014,
    prefix = "KB",
    Config {
        USB_HID{
            isBoot = true, -- same as bInterfaceSubClass = 1
            isKey = true,  -- same as bInterfaceProtocol = 1
            report = HID_BootKeyBoard(),
            EndPoint(IN(1),  Interrupt, 8),
       }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB Boot Mouse DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0015,
    prefix = "MOUSE",
    Config {
        USB_HID{
            isBoot = true,   -- same as bInterfaceSubClass = 1
            isMouse = true,  -- same as bInterfaceProtocol = 2
            extDesc = HID_BootMouse(), -- same as report = xxx
            EndPoint(IN(1),  Interrupt, 8),
       }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0016,
    prefix = "WINUSB",
    Config {
        Interface{
            extDesc = WinUSB("{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}"),
            strInterface = "TeenyUSB WinUSB RTT",
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(1), BulkDouble, 64),
        },
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB RNDIS DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0017,
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

}

  ------------- lua script end   ------------
 */
/////////////////////////////////////////
//// Device 0
/////////////////////////////////////////
#ifndef __CDC_TEENY_USB_INIT_H__
#define __CDC_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define CDC_VID                                            0x0483
#define CDC_PID                                            0x0012
#define CDC_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define CDC_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define CDC_MAX_EP                                         (2)
#define CDC_EP_NUM                                         (CDC_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef CDC_BTABLE_ADDRESS
#undef CDC_BTABLE_ADDRESS
#endif
#define CDC_BTABLE_ADDRESS                                 (0)
#define CDC_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define CDC_USB_BUF_START                                  (CDC_EP_BUF_DESC_TABLE_SIZE * CDC_EP_NUM)

// EndPoints 0 defines
#define CDC_EP0_RX_SIZE                                    (64)
#define CDC_EP0_RX_ADDR                                    (CDC_USB_BUF_START + (0))
#define CDC_EP0_TX_SIZE                                    (64)
#define CDC_EP0_TX_ADDR                                    (CDC_USB_BUF_START + (64))
#define CDC_EP0_RX_TYPE                                    USB_EP_CONTROL
#define CDC_EP0_TX_TYPE                                    USB_EP_CONTROL

#define CDC_EP0_TYPE                                       USB_EP_CONTROL
#define CDC_EP0_TX0_ADDR                                   (CDC_USB_BUF_START + (0))
#define CDC_EP0_TX1_ADDR                                   (CDC_USB_BUF_START + (64))
#define CDC_EP0_RX0_ADDR                                   (CDC_USB_BUF_START + (0))
#define CDC_EP0_RX1_ADDR                                   (CDC_USB_BUF_START + (64))

// EndPoints 1 defines
#define CDC_EP1_RX_SIZE                                    (64)
#define CDC_EP1_RX_ADDR                                    (CDC_USB_BUF_START + (128))
#define CDC_EP1_TX_SIZE                                    (64)
#define CDC_EP1_TX_ADDR                                    (CDC_USB_BUF_START + (192))
#define CDC_EP1_RX_TYPE                                    USB_EP_BULK
#define CDC_EP1_TX_TYPE                                    USB_EP_BULK

#define CDC_EP1_TYPE                                       USB_EP_BULK
#define CDC_EP1_TX0_ADDR                                   (CDC_USB_BUF_START + (128))
#define CDC_EP1_TX1_ADDR                                   (CDC_USB_BUF_START + (192))
#define CDC_EP1_RX0_ADDR                                   (CDC_USB_BUF_START + (128))
#define CDC_EP1_RX1_ADDR                                   (CDC_USB_BUF_START + (192))

// EndPoints 2 defines
#define CDC_EP2_RX_SIZE                                    (0)
#define CDC_EP2_RX_ADDR                                    (CDC_USB_BUF_START + (256))
#define CDC_EP2_TX_SIZE                                    (16)
#define CDC_EP2_TX_ADDR                                    (CDC_USB_BUF_START + (256))
#define CDC_EP2_RX_TYPE                                    USB_EP_Invalid
#define CDC_EP2_TX_TYPE                                    USB_EP_INTERRUPT

#define CDC_EP2_TYPE                                       USB_EP_INTERRUPT
#define CDC_EP2_TX0_ADDR                                   (CDC_USB_BUF_START + (256))
#define CDC_EP2_TX1_ADDR                                   (CDC_USB_BUF_START + (272))
#define CDC_EP2_RX0_ADDR                                   (CDC_USB_BUF_START + (256))
#define CDC_EP2_RX1_ADDR                                   (CDC_USB_BUF_START + (272))


// EndPoint max packed sizes
extern const uint8_t CDC_txEpMaxSize[];
#define CDC_TXEP_MAX_SIZE                                  \
const uint8_t CDC_txEpMaxSize[] = \
{ CDC_EP0_TX_SIZE, CDC_EP1_TX_SIZE, CDC_EP2_TX_SIZE,  };
extern const uint8_t CDC_rxEpMaxSize[];
#define CDC_RXEP_MAX_SIZE                                  \
const uint8_t CDC_rxEpMaxSize[] = \
{ CDC_EP0_RX_SIZE, CDC_EP1_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define CDC_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, CDC_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, CDC_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, CDC_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, CDC_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP1, CDC_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, CDC_EP1_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP1, CDC_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, CDC_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP2, CDC_EP2_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP2, CDC_EP2_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define CDC_OTG_MAX_OUT_SIZE                               (64)
#define CDC_OTG_CONTROL_EP_NUM                             (1)
#define CDC_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define CDC_OTG_RX_FIFO_SIZE_FS                            (256)
#define CDC_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 144
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define CDC_EP0_TX_FIFO_ADDR_FS                            (256)
#define CDC_EP0_TX_FIFO_SIZE_FS                            (CDC_EP0_TX_SIZE * 7)
#define CDC_EP1_TX_FIFO_ADDR_FS                            (704)
#define CDC_EP1_TX_FIFO_SIZE_FS                            (CDC_EP1_TX_SIZE * 7)
#define CDC_EP2_TX_FIFO_ADDR_FS                            (1152)
#define CDC_EP2_TX_FIFO_SIZE_FS                            (CDC_EP2_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define CDC_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, CDC_OTG_RX_FIFO_ADDR_FS, CDC_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, CDC_EP0_TX_TYPE, CDC_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, CDC_EP0_TX_FIFO_ADDR_FS, CDC_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, CDC_EP0_RX_TYPE, CDC_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, CDC_EP1_TX_TYPE, CDC_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, CDC_EP1_TX_FIFO_ADDR_FS, CDC_EP1_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, CDC_EP1_RX_TYPE, CDC_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, CDC_EP2_TX_TYPE, CDC_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, CDC_EP2_TX_FIFO_ADDR_FS, CDC_EP2_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define CDC_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define CDC_OTG_RX_FIFO_SIZE_HS                            (512)
#define CDC_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 144
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define CDC_EP0_TX_FIFO_ADDR_HS                            (512)
#define CDC_EP0_TX_FIFO_SIZE_HS                            (CDC_EP0_TX_SIZE * 7)
#define CDC_EP1_TX_FIFO_ADDR_HS                            (960)
#define CDC_EP1_TX_FIFO_SIZE_HS                            (CDC_EP1_TX_SIZE * 7)
#define CDC_EP2_TX_FIFO_ADDR_HS                            (1408)
#define CDC_EP2_TX_FIFO_SIZE_HS                            (CDC_EP2_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define CDC_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, CDC_OTG_RX_FIFO_ADDR_HS, CDC_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, CDC_EP0_TX_TYPE, CDC_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, CDC_EP0_TX_FIFO_ADDR_HS, CDC_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, CDC_EP0_RX_TYPE, CDC_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, CDC_EP1_TX_TYPE, CDC_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, CDC_EP1_TX_FIFO_ADDR_HS, CDC_EP1_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, CDC_EP1_RX_TYPE, CDC_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, CDC_EP2_TX_TYPE, CDC_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, CDC_EP2_TX_FIFO_ADDR_HS, CDC_EP2_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define CDC_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define CDC_TUSB_INIT_EP_OTG(dev) \
  do{\
    CDC_TUSB_INIT_EP_OTG_FS(dev); \
    CDC_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define CDC_TUSB_INIT_EP(dev) CDC_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define CDC_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = CDC_DEV_STATUS;         \
    dev->rx_max_size = CDC_rxEpMaxSize;         \
    dev->tx_max_size = CDC_txEpMaxSize;         \
    dev->descriptors = &CDC_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define CDC_TUSB_INIT_EP(dev) CDC_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define CDC_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = CDC_DEV_STATUS;         \
    dev->descriptors = &CDC_descriptors;         \
  }while(0)

#endif

#define CDC_TUSB_INIT(dev) \
  do{\
    CDC_TUSB_INIT_EP(dev);   \
    CDC_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if CDC_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  CDC_EP_NUM
#endif

extern const uint8_t* const CDC_StringDescriptors[CDC_STRING_COUNT];
extern const tusb_descriptors CDC_descriptors;


#endif   // #ifndef __CDC_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 1
/////////////////////////////////////////
#ifndef __MSC_TEENY_USB_INIT_H__
#define __MSC_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define MSC_VID                                            0x0483
#define MSC_PID                                            0x0013
#define MSC_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define MSC_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define MSC_MAX_EP                                         (2)
#define MSC_EP_NUM                                         (MSC_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef MSC_BTABLE_ADDRESS
#undef MSC_BTABLE_ADDRESS
#endif
#define MSC_BTABLE_ADDRESS                                 (0)
#define MSC_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define MSC_USB_BUF_START                                  (MSC_EP_BUF_DESC_TABLE_SIZE * MSC_EP_NUM)

// EndPoints 0 defines
#define MSC_EP0_RX_SIZE                                    (64)
#define MSC_EP0_RX_ADDR                                    (MSC_USB_BUF_START + (0))
#define MSC_EP0_TX_SIZE                                    (64)
#define MSC_EP0_TX_ADDR                                    (MSC_USB_BUF_START + (64))
#define MSC_EP0_RX_TYPE                                    USB_EP_CONTROL
#define MSC_EP0_TX_TYPE                                    USB_EP_CONTROL

#define MSC_EP0_TYPE                                       USB_EP_CONTROL
#define MSC_EP0_TX0_ADDR                                   (MSC_USB_BUF_START + (0))
#define MSC_EP0_TX1_ADDR                                   (MSC_USB_BUF_START + (64))
#define MSC_EP0_RX0_ADDR                                   (MSC_USB_BUF_START + (0))
#define MSC_EP0_RX1_ADDR                                   (MSC_USB_BUF_START + (64))

// EndPoints 1 defines
#define MSC_EP1_RX_SIZE                                    (0)
#define MSC_EP1_RX_ADDR                                    (MSC_USB_BUF_START + (128))
#define MSC_EP1_TX_SIZE                                    (64)
#define MSC_EP1_TX_ADDR                                    (MSC_USB_BUF_START + (128))
#define MSC_EP1_RX_TYPE                                    USB_EP_Invalid
#define MSC_EP1_TX_TYPE                                    USB_EP_BULK

#define MSC_EP1_TYPE                                       USB_EP_BULK
#define MSC_EP1_TX0_ADDR                                   (MSC_USB_BUF_START + (128))
#define MSC_EP1_TX1_ADDR                                   (MSC_USB_BUF_START + (192))
#define MSC_EP1_RX0_ADDR                                   (MSC_USB_BUF_START + (128))
#define MSC_EP1_RX1_ADDR                                   (MSC_USB_BUF_START + (192))

// EndPoints 2 defines
#define MSC_EP2_RX_SIZE                                    (64)
#define MSC_EP2_RX_ADDR                                    (MSC_USB_BUF_START + (256))
#define MSC_EP2_TX_SIZE                                    (0)
#define MSC_EP2_TX_ADDR                                    (MSC_USB_BUF_START + (320))
#define MSC_EP2_RX_TYPE                                    USB_EP_BULK
#define MSC_EP2_TX_TYPE                                    USB_EP_Invalid

#define MSC_EP2_TYPE                                       USB_EP_BULK
#define MSC_EP2_TX0_ADDR                                   (MSC_USB_BUF_START + (256))
#define MSC_EP2_TX1_ADDR                                   (MSC_USB_BUF_START + (320))
#define MSC_EP2_RX0_ADDR                                   (MSC_USB_BUF_START + (256))
#define MSC_EP2_RX1_ADDR                                   (MSC_USB_BUF_START + (320))


// EndPoint max packed sizes
extern const uint8_t MSC_txEpMaxSize[];
#define MSC_TXEP_MAX_SIZE                                  \
const uint8_t MSC_txEpMaxSize[] = \
{ MSC_EP0_TX_SIZE, MSC_EP1_TX_SIZE, 0,  };
extern const uint8_t MSC_rxEpMaxSize[];
#define MSC_RXEP_MAX_SIZE                                  \
const uint8_t MSC_rxEpMaxSize[] = \
{ MSC_EP0_RX_SIZE, 0, MSC_EP2_RX_SIZE,  };

// EndPoints init function for USB FS core
#define MSC_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, MSC_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, MSC_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, MSC_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, MSC_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_TxDouble(dev, PCD_ENDP1, MSC_EP1_TYPE);     \
    SET_DOUBLE_ADDR(dev, PCD_ENDP1, MSC_EP1_TX0_ADDR, MSC_EP1_TX1_ADDR);  \
    SET_DBL_TX_CNT(dev, PCD_ENDP1, 0);     \
    /* Init ep2 */ \
    INIT_EP_RxDouble(dev, PCD_ENDP2, MSC_EP2_TYPE);     \
    SET_DOUBLE_ADDR(dev, PCD_ENDP2, MSC_EP2_RX0_ADDR, MSC_EP2_RX1_ADDR);  \
    SET_DBL_RX_CNT(dev, PCD_ENDP2, MSC_EP2_RX_SIZE);     \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define MSC_OTG_MAX_OUT_SIZE                               (64)
#define MSC_OTG_CONTROL_EP_NUM                             (1)
#define MSC_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define MSC_OTG_RX_FIFO_SIZE_FS                            (256)
#define MSC_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define MSC_EP0_TX_FIFO_ADDR_FS                            (256)
#define MSC_EP0_TX_FIFO_SIZE_FS                            (MSC_EP0_TX_SIZE * 7)
#define MSC_EP1_TX_FIFO_ADDR_FS                            (704)
#define MSC_EP1_TX_FIFO_SIZE_FS                            (MSC_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define MSC_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, MSC_OTG_RX_FIFO_ADDR_FS, MSC_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, MSC_EP0_TX_TYPE, MSC_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, MSC_EP0_TX_FIFO_ADDR_FS, MSC_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, MSC_EP0_RX_TYPE, MSC_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, MSC_EP1_TX_TYPE, MSC_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, MSC_EP1_TX_FIFO_ADDR_FS, MSC_EP1_TX_FIFO_SIZE_FS);  \
      /* Init Ep2  */\
      INIT_EP_Rx(dev, PCD_ENDP2, MSC_EP2_RX_TYPE, MSC_EP2_RX_SIZE); \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define MSC_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define MSC_OTG_RX_FIFO_SIZE_HS                            (512)
#define MSC_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define MSC_EP0_TX_FIFO_ADDR_HS                            (512)
#define MSC_EP0_TX_FIFO_SIZE_HS                            (MSC_EP0_TX_SIZE * 7)
#define MSC_EP1_TX_FIFO_ADDR_HS                            (960)
#define MSC_EP1_TX_FIFO_SIZE_HS                            (MSC_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define MSC_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, MSC_OTG_RX_FIFO_ADDR_HS, MSC_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, MSC_EP0_TX_TYPE, MSC_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, MSC_EP0_TX_FIFO_ADDR_HS, MSC_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, MSC_EP0_RX_TYPE, MSC_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, MSC_EP1_TX_TYPE, MSC_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, MSC_EP1_TX_FIFO_ADDR_HS, MSC_EP1_TX_FIFO_SIZE_HS);  \
      /* Init Ep2  */\
      INIT_EP_Rx(dev, PCD_ENDP2, MSC_EP2_RX_TYPE, MSC_EP2_RX_SIZE); \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define MSC_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define MSC_TUSB_INIT_EP_OTG(dev) \
  do{\
    MSC_TUSB_INIT_EP_OTG_FS(dev); \
    MSC_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define MSC_TUSB_INIT_EP(dev) MSC_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define MSC_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = MSC_DEV_STATUS;         \
    dev->rx_max_size = MSC_rxEpMaxSize;         \
    dev->tx_max_size = MSC_txEpMaxSize;         \
    dev->descriptors = &MSC_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define MSC_TUSB_INIT_EP(dev) MSC_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define MSC_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = MSC_DEV_STATUS;         \
    dev->descriptors = &MSC_descriptors;         \
  }while(0)

#endif

#define MSC_TUSB_INIT(dev) \
  do{\
    MSC_TUSB_INIT_EP(dev);   \
    MSC_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if MSC_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  MSC_EP_NUM
#endif

// Enable double buffer related code
#define  HAS_DOUBLE_BUFFER

extern const uint8_t* const MSC_StringDescriptors[MSC_STRING_COUNT];
extern const tusb_descriptors MSC_descriptors;


#endif   // #ifndef __MSC_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 2
/////////////////////////////////////////
#ifndef __KB_TEENY_USB_INIT_H__
#define __KB_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define KB_VID                                            0x0483
#define KB_PID                                            0x0014
#define KB_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define KB_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define KB_MAX_EP                                         (1)
#define KB_EP_NUM                                         (KB_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef KB_BTABLE_ADDRESS
#undef KB_BTABLE_ADDRESS
#endif
#define KB_BTABLE_ADDRESS                                 (0)
#define KB_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define KB_USB_BUF_START                                  (KB_EP_BUF_DESC_TABLE_SIZE * KB_EP_NUM)

// EndPoints 0 defines
#define KB_EP0_RX_SIZE                                    (64)
#define KB_EP0_RX_ADDR                                    (KB_USB_BUF_START + (0))
#define KB_EP0_TX_SIZE                                    (64)
#define KB_EP0_TX_ADDR                                    (KB_USB_BUF_START + (64))
#define KB_EP0_RX_TYPE                                    USB_EP_CONTROL
#define KB_EP0_TX_TYPE                                    USB_EP_CONTROL

#define KB_EP0_TYPE                                       USB_EP_CONTROL
#define KB_EP0_TX0_ADDR                                   (KB_USB_BUF_START + (0))
#define KB_EP0_TX1_ADDR                                   (KB_USB_BUF_START + (64))
#define KB_EP0_RX0_ADDR                                   (KB_USB_BUF_START + (0))
#define KB_EP0_RX1_ADDR                                   (KB_USB_BUF_START + (64))

// EndPoints 1 defines
#define KB_EP1_RX_SIZE                                    (0)
#define KB_EP1_RX_ADDR                                    (KB_USB_BUF_START + (128))
#define KB_EP1_TX_SIZE                                    (8)
#define KB_EP1_TX_ADDR                                    (KB_USB_BUF_START + (128))
#define KB_EP1_RX_TYPE                                    USB_EP_Invalid
#define KB_EP1_TX_TYPE                                    USB_EP_INTERRUPT

#define KB_EP1_TYPE                                       USB_EP_INTERRUPT
#define KB_EP1_TX0_ADDR                                   (KB_USB_BUF_START + (128))
#define KB_EP1_TX1_ADDR                                   (KB_USB_BUF_START + (136))
#define KB_EP1_RX0_ADDR                                   (KB_USB_BUF_START + (128))
#define KB_EP1_RX1_ADDR                                   (KB_USB_BUF_START + (136))


// EndPoint max packed sizes
extern const uint8_t KB_txEpMaxSize[];
#define KB_TXEP_MAX_SIZE                                  \
const uint8_t KB_txEpMaxSize[] = \
{ KB_EP0_TX_SIZE, KB_EP1_TX_SIZE,  };
extern const uint8_t KB_rxEpMaxSize[];
#define KB_RXEP_MAX_SIZE                                  \
const uint8_t KB_rxEpMaxSize[] = \
{ KB_EP0_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define KB_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, KB_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, KB_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, KB_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, KB_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP1, KB_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, KB_EP1_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define KB_OTG_MAX_OUT_SIZE                               (0)
#define KB_OTG_CONTROL_EP_NUM                             (1)
#define KB_OTG_OUT_EP_NUM                                 (0)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 15

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define KB_OTG_RX_FIFO_SIZE_FS                            (256)
#define KB_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 72
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define KB_EP0_TX_FIFO_ADDR_FS                            (256)
#define KB_EP0_TX_FIFO_SIZE_FS                            (KB_EP0_TX_SIZE * 7)
#define KB_EP1_TX_FIFO_ADDR_FS                            (704)
#define KB_EP1_TX_FIFO_SIZE_FS                            (KB_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define KB_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, KB_OTG_RX_FIFO_ADDR_FS, KB_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, KB_EP0_TX_TYPE, KB_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, KB_EP0_TX_FIFO_ADDR_FS, KB_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, KB_EP0_RX_TYPE, KB_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, KB_EP1_TX_TYPE, KB_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, KB_EP1_TX_FIFO_ADDR_FS, KB_EP1_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define KB_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define KB_OTG_RX_FIFO_SIZE_HS                            (512)
#define KB_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 72
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define KB_EP0_TX_FIFO_ADDR_HS                            (512)
#define KB_EP0_TX_FIFO_SIZE_HS                            (KB_EP0_TX_SIZE * 7)
#define KB_EP1_TX_FIFO_ADDR_HS                            (960)
#define KB_EP1_TX_FIFO_SIZE_HS                            (KB_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define KB_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, KB_OTG_RX_FIFO_ADDR_HS, KB_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, KB_EP0_TX_TYPE, KB_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, KB_EP0_TX_FIFO_ADDR_HS, KB_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, KB_EP0_RX_TYPE, KB_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, KB_EP1_TX_TYPE, KB_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, KB_EP1_TX_FIFO_ADDR_HS, KB_EP1_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define KB_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define KB_TUSB_INIT_EP_OTG(dev) \
  do{\
    KB_TUSB_INIT_EP_OTG_FS(dev); \
    KB_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define KB_TUSB_INIT_EP(dev) KB_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define KB_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = KB_DEV_STATUS;         \
    dev->rx_max_size = KB_rxEpMaxSize;         \
    dev->tx_max_size = KB_txEpMaxSize;         \
    dev->descriptors = &KB_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define KB_TUSB_INIT_EP(dev) KB_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define KB_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = KB_DEV_STATUS;         \
    dev->descriptors = &KB_descriptors;         \
  }while(0)

#endif

#define KB_TUSB_INIT(dev) \
  do{\
    KB_TUSB_INIT_EP(dev);   \
    KB_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if KB_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  KB_EP_NUM
#endif

extern const uint8_t* const KB_StringDescriptors[KB_STRING_COUNT];
extern const tusb_descriptors KB_descriptors;

#define KB_REPORT_DESCRIPTOR_SIZE_IF0  (63)
extern const uint8_t KB_ReportDescriptor_if0[KB_REPORT_DESCRIPTOR_SIZE_IF0];


#endif   // #ifndef __KB_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 3
/////////////////////////////////////////
#ifndef __MOUSE_TEENY_USB_INIT_H__
#define __MOUSE_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define MOUSE_VID                                            0x0483
#define MOUSE_PID                                            0x0015
#define MOUSE_STRING_COUNT                                   (4)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define MOUSE_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define MOUSE_MAX_EP                                         (1)
#define MOUSE_EP_NUM                                         (MOUSE_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef MOUSE_BTABLE_ADDRESS
#undef MOUSE_BTABLE_ADDRESS
#endif
#define MOUSE_BTABLE_ADDRESS                                 (0)
#define MOUSE_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define MOUSE_USB_BUF_START                                  (MOUSE_EP_BUF_DESC_TABLE_SIZE * MOUSE_EP_NUM)

// EndPoints 0 defines
#define MOUSE_EP0_RX_SIZE                                    (64)
#define MOUSE_EP0_RX_ADDR                                    (MOUSE_USB_BUF_START + (0))
#define MOUSE_EP0_TX_SIZE                                    (64)
#define MOUSE_EP0_TX_ADDR                                    (MOUSE_USB_BUF_START + (64))
#define MOUSE_EP0_RX_TYPE                                    USB_EP_CONTROL
#define MOUSE_EP0_TX_TYPE                                    USB_EP_CONTROL

#define MOUSE_EP0_TYPE                                       USB_EP_CONTROL
#define MOUSE_EP0_TX0_ADDR                                   (MOUSE_USB_BUF_START + (0))
#define MOUSE_EP0_TX1_ADDR                                   (MOUSE_USB_BUF_START + (64))
#define MOUSE_EP0_RX0_ADDR                                   (MOUSE_USB_BUF_START + (0))
#define MOUSE_EP0_RX1_ADDR                                   (MOUSE_USB_BUF_START + (64))

// EndPoints 1 defines
#define MOUSE_EP1_RX_SIZE                                    (0)
#define MOUSE_EP1_RX_ADDR                                    (MOUSE_USB_BUF_START + (128))
#define MOUSE_EP1_TX_SIZE                                    (8)
#define MOUSE_EP1_TX_ADDR                                    (MOUSE_USB_BUF_START + (128))
#define MOUSE_EP1_RX_TYPE                                    USB_EP_Invalid
#define MOUSE_EP1_TX_TYPE                                    USB_EP_INTERRUPT

#define MOUSE_EP1_TYPE                                       USB_EP_INTERRUPT
#define MOUSE_EP1_TX0_ADDR                                   (MOUSE_USB_BUF_START + (128))
#define MOUSE_EP1_TX1_ADDR                                   (MOUSE_USB_BUF_START + (136))
#define MOUSE_EP1_RX0_ADDR                                   (MOUSE_USB_BUF_START + (128))
#define MOUSE_EP1_RX1_ADDR                                   (MOUSE_USB_BUF_START + (136))


// EndPoint max packed sizes
extern const uint8_t MOUSE_txEpMaxSize[];
#define MOUSE_TXEP_MAX_SIZE                                  \
const uint8_t MOUSE_txEpMaxSize[] = \
{ MOUSE_EP0_TX_SIZE, MOUSE_EP1_TX_SIZE,  };
extern const uint8_t MOUSE_rxEpMaxSize[];
#define MOUSE_RXEP_MAX_SIZE                                  \
const uint8_t MOUSE_rxEpMaxSize[] = \
{ MOUSE_EP0_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define MOUSE_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, MOUSE_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, MOUSE_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, MOUSE_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, MOUSE_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP1, MOUSE_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, MOUSE_EP1_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define MOUSE_OTG_MAX_OUT_SIZE                               (0)
#define MOUSE_OTG_CONTROL_EP_NUM                             (1)
#define MOUSE_OTG_OUT_EP_NUM                                 (0)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 15

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define MOUSE_OTG_RX_FIFO_SIZE_FS                            (256)
#define MOUSE_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 72
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define MOUSE_EP0_TX_FIFO_ADDR_FS                            (256)
#define MOUSE_EP0_TX_FIFO_SIZE_FS                            (MOUSE_EP0_TX_SIZE * 7)
#define MOUSE_EP1_TX_FIFO_ADDR_FS                            (704)
#define MOUSE_EP1_TX_FIFO_SIZE_FS                            (MOUSE_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define MOUSE_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, MOUSE_OTG_RX_FIFO_ADDR_FS, MOUSE_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, MOUSE_EP0_TX_TYPE, MOUSE_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, MOUSE_EP0_TX_FIFO_ADDR_FS, MOUSE_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, MOUSE_EP0_RX_TYPE, MOUSE_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, MOUSE_EP1_TX_TYPE, MOUSE_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, MOUSE_EP1_TX_FIFO_ADDR_FS, MOUSE_EP1_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define MOUSE_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define MOUSE_OTG_RX_FIFO_SIZE_HS                            (512)
#define MOUSE_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 72
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define MOUSE_EP0_TX_FIFO_ADDR_HS                            (512)
#define MOUSE_EP0_TX_FIFO_SIZE_HS                            (MOUSE_EP0_TX_SIZE * 7)
#define MOUSE_EP1_TX_FIFO_ADDR_HS                            (960)
#define MOUSE_EP1_TX_FIFO_SIZE_HS                            (MOUSE_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define MOUSE_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, MOUSE_OTG_RX_FIFO_ADDR_HS, MOUSE_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, MOUSE_EP0_TX_TYPE, MOUSE_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, MOUSE_EP0_TX_FIFO_ADDR_HS, MOUSE_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, MOUSE_EP0_RX_TYPE, MOUSE_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, MOUSE_EP1_TX_TYPE, MOUSE_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, MOUSE_EP1_TX_FIFO_ADDR_HS, MOUSE_EP1_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define MOUSE_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define MOUSE_TUSB_INIT_EP_OTG(dev) \
  do{\
    MOUSE_TUSB_INIT_EP_OTG_FS(dev); \
    MOUSE_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define MOUSE_TUSB_INIT_EP(dev) MOUSE_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define MOUSE_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = MOUSE_DEV_STATUS;         \
    dev->rx_max_size = MOUSE_rxEpMaxSize;         \
    dev->tx_max_size = MOUSE_txEpMaxSize;         \
    dev->descriptors = &MOUSE_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define MOUSE_TUSB_INIT_EP(dev) MOUSE_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define MOUSE_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = MOUSE_DEV_STATUS;         \
    dev->descriptors = &MOUSE_descriptors;         \
  }while(0)

#endif

#define MOUSE_TUSB_INIT(dev) \
  do{\
    MOUSE_TUSB_INIT_EP(dev);   \
    MOUSE_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if MOUSE_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  MOUSE_EP_NUM
#endif

extern const uint8_t* const MOUSE_StringDescriptors[MOUSE_STRING_COUNT];
extern const tusb_descriptors MOUSE_descriptors;

#define MOUSE_REPORT_DESCRIPTOR_SIZE_IF0  (50)
extern const uint8_t MOUSE_ReportDescriptor_if0[MOUSE_REPORT_DESCRIPTOR_SIZE_IF0];


#endif   // #ifndef __MOUSE_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 4
/////////////////////////////////////////
#ifndef __WINUSB_TEENY_USB_INIT_H__
#define __WINUSB_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define WINUSB_VID                                            0x0483
#define WINUSB_PID                                            0x0016
#define WINUSB_STRING_COUNT                                   (5)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define WINUSB_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define WINUSB_MAX_EP                                         (1)
#define WINUSB_EP_NUM                                         (WINUSB_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef WINUSB_BTABLE_ADDRESS
#undef WINUSB_BTABLE_ADDRESS
#endif
#define WINUSB_BTABLE_ADDRESS                                 (0)
#define WINUSB_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define WINUSB_USB_BUF_START                                  (WINUSB_EP_BUF_DESC_TABLE_SIZE * WINUSB_EP_NUM)

// EndPoints 0 defines
#define WINUSB_EP0_RX_SIZE                                    (64)
#define WINUSB_EP0_RX_ADDR                                    (WINUSB_USB_BUF_START + (0))
#define WINUSB_EP0_TX_SIZE                                    (64)
#define WINUSB_EP0_TX_ADDR                                    (WINUSB_USB_BUF_START + (64))
#define WINUSB_EP0_RX_TYPE                                    USB_EP_CONTROL
#define WINUSB_EP0_TX_TYPE                                    USB_EP_CONTROL

#define WINUSB_EP0_TYPE                                       USB_EP_CONTROL
#define WINUSB_EP0_TX0_ADDR                                   (WINUSB_USB_BUF_START + (0))
#define WINUSB_EP0_TX1_ADDR                                   (WINUSB_USB_BUF_START + (64))
#define WINUSB_EP0_RX0_ADDR                                   (WINUSB_USB_BUF_START + (0))
#define WINUSB_EP0_RX1_ADDR                                   (WINUSB_USB_BUF_START + (64))

// EndPoints 1 defines
#define WINUSB_EP1_RX_SIZE                                    (64)
#define WINUSB_EP1_RX_ADDR                                    (WINUSB_USB_BUF_START + (128))
#define WINUSB_EP1_TX_SIZE                                    (64)
#define WINUSB_EP1_TX_ADDR                                    (WINUSB_USB_BUF_START + (192))
#define WINUSB_EP1_RX_TYPE                                    USB_EP_BULK
#define WINUSB_EP1_TX_TYPE                                    USB_EP_BULK

#define WINUSB_EP1_TYPE                                       USB_EP_BULK
#define WINUSB_EP1_TX0_ADDR                                   (WINUSB_USB_BUF_START + (128))
#define WINUSB_EP1_TX1_ADDR                                   (WINUSB_USB_BUF_START + (192))
#define WINUSB_EP1_RX0_ADDR                                   (WINUSB_USB_BUF_START + (128))
#define WINUSB_EP1_RX1_ADDR                                   (WINUSB_USB_BUF_START + (192))


// EndPoint max packed sizes
extern const uint8_t WINUSB_txEpMaxSize[];
#define WINUSB_TXEP_MAX_SIZE                                  \
const uint8_t WINUSB_txEpMaxSize[] = \
{ WINUSB_EP0_TX_SIZE, WINUSB_EP1_TX_SIZE,  };
extern const uint8_t WINUSB_rxEpMaxSize[];
#define WINUSB_RXEP_MAX_SIZE                                  \
const uint8_t WINUSB_rxEpMaxSize[] = \
{ WINUSB_EP0_RX_SIZE, WINUSB_EP1_RX_SIZE,  };

// EndPoints init function for USB FS core
#define WINUSB_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, WINUSB_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, WINUSB_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, WINUSB_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, WINUSB_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP1, WINUSB_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, WINUSB_EP1_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP1, WINUSB_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, WINUSB_EP1_RX_SIZE);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define WINUSB_OTG_MAX_OUT_SIZE                               (64)
#define WINUSB_OTG_CONTROL_EP_NUM                             (1)
#define WINUSB_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define WINUSB_OTG_RX_FIFO_SIZE_FS                            (256)
#define WINUSB_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define WINUSB_EP0_TX_FIFO_ADDR_FS                            (256)
#define WINUSB_EP0_TX_FIFO_SIZE_FS                            (WINUSB_EP0_TX_SIZE * 7)
#define WINUSB_EP1_TX_FIFO_ADDR_FS                            (704)
#define WINUSB_EP1_TX_FIFO_SIZE_FS                            (WINUSB_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define WINUSB_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, WINUSB_OTG_RX_FIFO_ADDR_FS, WINUSB_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, WINUSB_EP0_TX_TYPE, WINUSB_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, WINUSB_EP0_TX_FIFO_ADDR_FS, WINUSB_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, WINUSB_EP0_RX_TYPE, WINUSB_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, WINUSB_EP1_TX_TYPE, WINUSB_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, WINUSB_EP1_TX_FIFO_ADDR_FS, WINUSB_EP1_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, WINUSB_EP1_RX_TYPE, WINUSB_EP1_RX_SIZE); \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define WINUSB_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define WINUSB_OTG_RX_FIFO_SIZE_HS                            (512)
#define WINUSB_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define WINUSB_EP0_TX_FIFO_ADDR_HS                            (512)
#define WINUSB_EP0_TX_FIFO_SIZE_HS                            (WINUSB_EP0_TX_SIZE * 7)
#define WINUSB_EP1_TX_FIFO_ADDR_HS                            (960)
#define WINUSB_EP1_TX_FIFO_SIZE_HS                            (WINUSB_EP1_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define WINUSB_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, WINUSB_OTG_RX_FIFO_ADDR_HS, WINUSB_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, WINUSB_EP0_TX_TYPE, WINUSB_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, WINUSB_EP0_TX_FIFO_ADDR_HS, WINUSB_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, WINUSB_EP0_RX_TYPE, WINUSB_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, WINUSB_EP1_TX_TYPE, WINUSB_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, WINUSB_EP1_TX_FIFO_ADDR_HS, WINUSB_EP1_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, WINUSB_EP1_RX_TYPE, WINUSB_EP1_RX_SIZE); \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define WINUSB_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define WINUSB_TUSB_INIT_EP_OTG(dev) \
  do{\
    WINUSB_TUSB_INIT_EP_OTG_FS(dev); \
    WINUSB_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define WINUSB_TUSB_INIT_EP(dev) WINUSB_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define WINUSB_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = WINUSB_DEV_STATUS;         \
    dev->rx_max_size = WINUSB_rxEpMaxSize;         \
    dev->tx_max_size = WINUSB_txEpMaxSize;         \
    dev->descriptors = &WINUSB_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define WINUSB_TUSB_INIT_EP(dev) WINUSB_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define WINUSB_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = WINUSB_DEV_STATUS;         \
    dev->descriptors = &WINUSB_descriptors;         \
  }while(0)

#endif

#define WINUSB_TUSB_INIT(dev) \
  do{\
    WINUSB_TUSB_INIT_EP(dev);   \
    WINUSB_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if WINUSB_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  WINUSB_EP_NUM
#endif

extern const uint8_t* const WINUSB_StringDescriptors[WINUSB_STRING_COUNT];
extern const tusb_descriptors WINUSB_descriptors;

// Enable WCID related code
#define  HAS_WCID

#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif


#endif   // #ifndef __WINUSB_TEENY_USB_INIT_H__
/////////////////////////////////////////
//// Device 5
/////////////////////////////////////////
#ifndef __RNDIS_TEENY_USB_INIT_H__
#define __RNDIS_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define RNDIS_VID                                            0x0483
#define RNDIS_PID                                            0x0017
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
