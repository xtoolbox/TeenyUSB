/*
 * Name   :  teeny_usb_init.h
 * Author :  admin@xtoolbox.org
 * Date   :  2019-10-03 22:14:42
 * Desc   :  This file is auto generate by the teeny_usb script tool
 *           Visit https://github.com/xtoolbox/TeenyUSB for more info
 */

/*
  Input source name:  custom_bulk_desc.lua  
  Content type is lua script:
  ------------- lua script begin ------------
return Device {
    strManufacture = "TeenyUSB",
    strProduct = "TeenyUSB Custom Bulk",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0001,
    prefix = "BULK",
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
        },
   }
}  

  ------------- lua script end   ------------
 */
#ifndef _TEENY_USB_INIT_H_
#define _TEENY_USB_INIT_H_
// include this file in "usbd_conf.h"

// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;
#define BULK_VID                                            0x0483
#define BULK_PID                                            0x0001
#define BULK_STRING_COUNT                                   (4)
// Endpoint usage:
#ifdef BULK_BTABLE_ADDRESS
#undef BULK_BTABLE_ADDRESS
#endif
#define BULK_BTABLE_ADDRESS                                 (0)
#define BULK_MAX_EP                                         (2)
#define BULK_EP_NUM                                         (BULK_MAX_EP + 1)
#define BULK_EP_BUF_DESC_TABLE_SIZE                         (8)

// PMA buffer reserved for buffer description table  
#define BULK_USB_BUF_START                                  (BULK_EP_BUF_DESC_TABLE_SIZE * BULK_EP_NUM)

// EndPoints 0 defines
#define BULK_EP0_RX_SIZE                                    (64)
#define BULK_EP0_RX_ADDR                                    (BULK_USB_BUF_START + 0)
#define BULK_EP0_TX_SIZE                                    (64)
#define BULK_EP0_TX_ADDR                                    (BULK_USB_BUF_START + 64)
#define BULK_EP0_TYPE                                       USB_EP_CONTROL

// EndPoints 1 defines
#define BULK_EP1_TX_SIZE                                    (64)
#define BULK_EP1_TX0_ADDR                                   (BULK_USB_BUF_START + 128)
#define BULK_EP1_TX1_ADDR                                   (BULK_USB_BUF_START + 192)
#define BULK_EP1_TYPE                                       USB_EP_BULK

// EndPoints 2 defines
#define BULK_EP2_RX_SIZE                                    (64)
#define BULK_EP2_RX0_ADDR                                   (BULK_USB_BUF_START + 256)
#define BULK_EP2_RX1_ADDR                                   (BULK_USB_BUF_START + 320)
#define BULK_EP2_TYPE                                       USB_EP_BULK


// Endpoint define for OTG core
#define BULK_OTG_MAX_OUT_SIZE                               (64)
#define BULK_OTG_CONTROL_EP_NUM                             (1)
#define BULK_OTG_OUT_EP_NUM                                 (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33
#define BULK_OTG_RX_FIFO_SIZE_FS                            (256)
#define BULK_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 
#define BULK_EP0_TX_FIFO_ADDR_FS                            (256)
#define BULK_EP0_TX_FIFO_SIZE_FS                            (BULK_EP0_TX_SIZE * 7)
#define BULK_EP1_TX_FIFO_ADDR_FS                            (704)
#define BULK_EP1_TX_FIFO_SIZE_FS                            (BULK_EP1_TX_SIZE * 7)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33
#define BULK_OTG_RX_FIFO_SIZE_HS                            (512)
#define BULK_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 
#define BULK_EP0_TX_FIFO_ADDR_HS                            (512)
#define BULK_EP0_TX_FIFO_SIZE_HS                            (BULK_EP0_TX_SIZE * 7)
#define BULK_EP1_TX_FIFO_ADDR_HS                            (960)
#define BULK_EP1_TX_FIFO_SIZE_HS                            (BULK_EP1_TX_SIZE * 7)

// EndPoint max packed sizes
extern const uint8_t BULK_txEpMaxSize[];
#define BULK_TXEP_MAX_SIZE                                  \
const uint8_t BULK_txEpMaxSize[] = \
{BULK_EP0_TX_SIZE, BULK_EP1_TX_SIZE, 0, };
extern const uint8_t BULK_rxEpMaxSize[];
#define BULK_RXEP_MAX_SIZE                                  \
const uint8_t BULK_rxEpMaxSize[] = \
{BULK_EP0_RX_SIZE, 0, BULK_EP2_RX_SIZE, };

// EndPoints init function for USB FS core
#define BULK_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, BULK_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, BULK_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, BULK_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, BULK_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_TxDouble(dev, PCD_ENDP1, BULK_EP1_TYPE);     \
    SET_DOUBLE_ADDR(dev, PCD_ENDP1, BULK_EP1_TX0_ADDR, BULK_EP1_TX1_ADDR);  \
    SET_DBL_TX_CNT(dev, PCD_ENDP1, 0);     \
    /* Init ep2 */ \
    INIT_EP_RxDouble(dev, PCD_ENDP2, BULK_EP2_TYPE);     \
    SET_DOUBLE_ADDR(dev, PCD_ENDP2, BULK_EP2_RX0_ADDR, BULK_EP2_RX1_ADDR);     \
    SET_DBL_RX_CNT(dev, PCD_ENDP2, BULK_EP2_RX_SIZE);     \
  }while(0)


// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define BULK_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_FS) { \
    SET_RX_FIFO(dev, BULK_OTG_RX_FIFO_ADDR_FS, BULK_OTG_RX_FIFO_SIZE_FS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, BULK_EP0_TYPE, BULK_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, BULK_EP0_TX_FIFO_ADDR_FS, BULK_EP0_TX_FIFO_SIZE_FS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, BULK_EP0_TYPE, BULK_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Tx(dev, PCD_ENDP1, BULK_EP1_TYPE, BULK_EP1_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP1, BULK_EP1_TX_FIFO_ADDR_FS, BULK_EP1_TX_FIFO_SIZE_FS);  \
    /* Init ep2 */ \
    INIT_EP_Rx(dev, PCD_ENDP2, BULK_EP2_TYPE, BULK_EP2_RX_SIZE);  \
  } \
  }while(0)

#else  // USB_OTG_FS
#define BULK_TUSB_INIT_EP_OTG_FS(dev) 

#endif  // USB_OTG_FS

#if defined(USB_OTG_HS)
#define BULK_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_HS) { \
    SET_RX_FIFO(dev, BULK_OTG_RX_FIFO_ADDR_HS, BULK_OTG_RX_FIFO_SIZE_HS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, BULK_EP0_TYPE, BULK_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, BULK_EP0_TX_FIFO_ADDR_HS, BULK_EP0_TX_FIFO_SIZE_HS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, BULK_EP0_TYPE, BULK_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Tx(dev, PCD_ENDP1, BULK_EP1_TYPE, BULK_EP1_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP1, BULK_EP1_TX_FIFO_ADDR_HS, BULK_EP1_TX_FIFO_SIZE_HS);  \
    /* Init ep2 */ \
    INIT_EP_Rx(dev, PCD_ENDP2, BULK_EP2_TYPE, BULK_EP2_RX_SIZE);  \
  } \
  }while(0)

#else  // USB_OTG_HS
#define BULK_TUSB_INIT_EP_OTG_HS(dev) 

#endif // USB_OTG_HS

#define BULK_TUSB_INIT_EP_OTG(dev) \
  do{\
    BULK_TUSB_INIT_EP_OTG_FS(dev); \
    BULK_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define BULK_TUSB_INIT_EP(dev) BULK_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define BULK_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = BULK_DEV_STATUS;         \
    dev->rx_max_size = BULK_rxEpMaxSize;         \
    dev->tx_max_size = BULK_txEpMaxSize;         \
    dev->descriptors = &BULK_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define BULK_TUSB_INIT_EP(dev) BULK_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define BULK_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = BULK_DEV_STATUS;         \
    dev->descriptors = &BULK_descriptors;         \
  }while(0)

#endif

#define BULK_TUSB_INIT(dev) \
  do{\
    BULK_TUSB_INIT_EP(dev);   \
    BULK_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if BULK_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  BULK_EP_NUM
#endif


// Enable double buffer related code
#define  HAS_DOUBLE_BUFFER


// Enable WCID related code
#define  HAS_WCID


#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif

// Descriptor declare
#define  BULK_DEVICE_DESCRIPTOR_SIZE  (18)
extern const uint8_t BULK_DeviceDescriptor [18];
#define  BULK_CONFIG_DESCRIPTOR_SIZE  (32)
extern const uint8_t BULK_ConfigDescriptor [32];
#define BULK_DEV_STATUS0      (0)
#define BULK_DEV_STATUS1      (0)
#define BULK_DEV_STATUS    ((BULK_DEV_STATUS0) |(BULK_DEV_STATUS1) )

extern const uint8_t* const BULK_StringDescriptors[BULK_STRING_COUNT];
extern const tusb_descriptors BULK_descriptors;



#endif

