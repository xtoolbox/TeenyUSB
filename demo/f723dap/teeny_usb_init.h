/*
 * Name   :  teeny_usb_init.h
 * Author :  admin@xtoolbox.org
 * Date   :  2019-09-17 11:21:35
 * Desc   :  This file is auto generate by the teeny_usb script tool
 *           Visit https://github.com/xtoolbox/TeenyUSB for more info
 */

/*
  Input source name:  cmsis_dap_desc.lua  
  Content type is lua script:
  ------------- lua script begin ------------
return {
  Device {
    strManufacture = "TeenyUSB",
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
    strManufacture = "TeenyUSB",
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
#ifndef _TEENY_USB_INIT_H_
#define _TEENY_USB_INIT_H_
// include this file in "usbd_conf.h"

// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;
/////////////////////////////////////////////////////////////////////
//// EndPoint for device1 define begin
/////////////////////////////////////////////////////////////////////
#define TUSB_HS_DAP_VID                                     0x0d28
#define TUSB_HS_DAP_PID                                     0x0204
#define TUSB_HS_DAP_STRING_COUNT                            (5)
// Endpoint usage:
#ifdef TUSB_HS_DAP_BTABLE_ADDRESS
#undef TUSB_HS_DAP_BTABLE_ADDRESS
#endif
#define TUSB_HS_DAP_BTABLE_ADDRESS                          (0)
#define TUSB_HS_DAP_MAX_EP                                  (2)
#define TUSB_HS_DAP_EP_NUM                                  (TUSB_HS_DAP_MAX_EP + 1)
#define TUSB_HS_DAP_EP_BUF_DESC_TABLE_SIZE                  (8)

// PMA buffer reserved for buffer description table  
#define TUSB_HS_DAP_USB_BUF_START                           (TUSB_HS_DAP_EP_BUF_DESC_TABLE_SIZE * TUSB_HS_DAP_EP_NUM)

// EndPoints 0 defines
#define TUSB_HS_DAP_EP0_RX_SIZE                             (64)
#define TUSB_HS_DAP_EP0_RX_ADDR                             (TUSB_HS_DAP_USB_BUF_START + 0)
#define TUSB_HS_DAP_EP0_TX_SIZE                             (64)
#define TUSB_HS_DAP_EP0_TX_ADDR                             (TUSB_HS_DAP_USB_BUF_START + 64)
#define TUSB_HS_DAP_EP0_TYPE                                USB_EP_CONTROL

// EndPoints 1 defines
#define TUSB_HS_DAP_EP1_RX_SIZE                             (512)
#define TUSB_HS_DAP_EP1_RX_ADDR                             (TUSB_HS_DAP_USB_BUF_START + 128)
#define TUSB_HS_DAP_EP1_TYPE                                USB_EP_BULK

// EndPoints 2 defines
#define TUSB_HS_DAP_EP2_TX_SIZE                             (512)
#define TUSB_HS_DAP_EP2_TX_ADDR                             (TUSB_HS_DAP_USB_BUF_START + 640)
#define TUSB_HS_DAP_EP2_TYPE                                USB_EP_BULK


// Endpoint define for OTG core
#define TUSB_HS_DAP_OTG_MAX_OUT_SIZE                        (512)
#define TUSB_HS_DAP_OTG_CONTROL_EP_NUM                      (1)
#define TUSB_HS_DAP_OTG_OUT_EP_NUM                          (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 145
#define TUSB_HS_DAP_OTG_RX_FIFO_SIZE_FS                     (580)
#define TUSB_HS_DAP_OTG_RX_FIFO_ADDR_FS                     (0)
// Sum of IN ep max packet size is 576
// Remain Fifo size is 700 in bytes, Rx Used 580 bytes 
#define TUSB_HS_DAP_EP0_TX_FIFO_ADDR_FS                     (580)
#define TUSB_HS_DAP_EP0_TX_FIFO_SIZE_FS                     (TUSB_HS_DAP_EP0_TX_SIZE * 1)
#define TUSB_HS_DAP_EP2_TX_FIFO_ADDR_FS                     (644)
#define TUSB_HS_DAP_EP2_TX_FIFO_SIZE_FS                     (TUSB_HS_DAP_EP2_TX_SIZE * 1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 145
#define TUSB_HS_DAP_OTG_RX_FIFO_SIZE_HS                     (580)
#define TUSB_HS_DAP_OTG_RX_FIFO_ADDR_HS                     (0)
// Sum of IN ep max packet size is 576
// Remain Fifo size is 3516 in bytes, Rx Used 580 bytes 
#define TUSB_HS_DAP_EP0_TX_FIFO_ADDR_HS                     (580)
#define TUSB_HS_DAP_EP0_TX_FIFO_SIZE_HS                     (TUSB_HS_DAP_EP0_TX_SIZE * 6)
#define TUSB_HS_DAP_EP2_TX_FIFO_ADDR_HS                     (964)
#define TUSB_HS_DAP_EP2_TX_FIFO_SIZE_HS                     (TUSB_HS_DAP_EP2_TX_SIZE * 6)

// EndPoint max packed sizes
extern const uint16_t TUSB_HS_DAP_txEpMaxSize[];
#define TUSB_HS_DAP_TXEP_MAX_SIZE                           \
const uint16_t TUSB_HS_DAP_txEpMaxSize[] = \
{TUSB_HS_DAP_EP0_TX_SIZE, 0, TUSB_HS_DAP_EP2_TX_SIZE, };
extern const uint16_t TUSB_HS_DAP_rxEpMaxSize[];
#define TUSB_HS_DAP_RXEP_MAX_SIZE                           \
const uint16_t TUSB_HS_DAP_rxEpMaxSize[] = \
{TUSB_HS_DAP_EP0_RX_SIZE, TUSB_HS_DAP_EP1_RX_SIZE, 0, };

// EndPoints init function for USB FS core
#define TUSB_HS_DAP_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_RxOnly(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_TYPE);       \
    SET_RX_ADDR(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TYPE);       \
    SET_TX_ADDR(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_ADDR);  \
  }while(0)


// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_FS) { \
    SET_RX_FIFO(dev, TUSB_HS_DAP_OTG_RX_FIFO_ADDR_FS, TUSB_HS_DAP_OTG_RX_FIFO_SIZE_FS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE, TUSB_HS_DAP_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_FIFO_ADDR_FS, TUSB_HS_DAP_EP0_TX_FIFO_SIZE_FS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE, TUSB_HS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Rx(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_TYPE, TUSB_HS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_Tx(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TYPE, TUSB_HS_DAP_EP2_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_FIFO_ADDR_FS, TUSB_HS_DAP_EP2_TX_FIFO_SIZE_FS);  \
  } \
  }while(0)

#else  // USB_OTG_FS
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_FS(dev) 

#endif  // USB_OTG_FS

#if defined(USB_OTG_HS)
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_HS) { \
    SET_RX_FIFO(dev, TUSB_HS_DAP_OTG_RX_FIFO_ADDR_HS, TUSB_HS_DAP_OTG_RX_FIFO_SIZE_HS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE, TUSB_HS_DAP_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TX_FIFO_ADDR_HS, TUSB_HS_DAP_EP0_TX_FIFO_SIZE_HS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, TUSB_HS_DAP_EP0_TYPE, TUSB_HS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Rx(dev, PCD_ENDP1, TUSB_HS_DAP_EP1_TYPE, TUSB_HS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_Tx(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TYPE, TUSB_HS_DAP_EP2_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP2, TUSB_HS_DAP_EP2_TX_FIFO_ADDR_HS, TUSB_HS_DAP_EP2_TX_FIFO_SIZE_HS);  \
  } \
  }while(0)

#else  // USB_OTG_HS
#define TUSB_HS_DAP_TUSB_INIT_EP_OTG_HS(dev) 

#endif // USB_OTG_HS

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


// Enable WCID related code
#define  HAS_WCID


#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif

// Descriptor declare
#define  TUSB_HS_DAP_DEVICE_DESCRIPTOR_SIZE  (18)
extern const uint8_t TUSB_HS_DAP_DeviceDescriptor [18];
#define  TUSB_HS_DAP_CONFIG_DESCRIPTOR_SIZE  (32)
extern const uint8_t TUSB_HS_DAP_ConfigDescriptor [32];
#define TUSB_HS_DAP_DEV_STATUS0      (0)
#define TUSB_HS_DAP_DEV_STATUS1      (0)
#define TUSB_HS_DAP_DEV_STATUS    ((TUSB_HS_DAP_DEV_STATUS0) |(TUSB_HS_DAP_DEV_STATUS1) )

extern const uint8_t* const TUSB_HS_DAP_StringDescriptors[TUSB_HS_DAP_STRING_COUNT];
extern const tusb_descriptors TUSB_HS_DAP_descriptors;


/////////////////////////////////////////////////////////////////////
//// EndPoint for device1 define end
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
//// EndPoint for device2 define begin
/////////////////////////////////////////////////////////////////////
#define TUSB_FS_DAP_VID                                     0x0d28
#define TUSB_FS_DAP_PID                                     0x0204
#define TUSB_FS_DAP_STRING_COUNT                            (4)
// Endpoint usage:
#ifdef TUSB_FS_DAP_BTABLE_ADDRESS
#undef TUSB_FS_DAP_BTABLE_ADDRESS
#endif
#define TUSB_FS_DAP_BTABLE_ADDRESS                          (0)
#define TUSB_FS_DAP_MAX_EP                                  (2)
#define TUSB_FS_DAP_EP_NUM                                  (TUSB_FS_DAP_MAX_EP + 1)
#define TUSB_FS_DAP_EP_BUF_DESC_TABLE_SIZE                  (8)

// PMA buffer reserved for buffer description table  
#define TUSB_FS_DAP_USB_BUF_START                           (TUSB_FS_DAP_EP_BUF_DESC_TABLE_SIZE * TUSB_FS_DAP_EP_NUM)

// EndPoints 0 defines
#define TUSB_FS_DAP_EP0_RX_SIZE                             (64)
#define TUSB_FS_DAP_EP0_RX_ADDR                             (TUSB_FS_DAP_USB_BUF_START + 0)
#define TUSB_FS_DAP_EP0_TX_SIZE                             (64)
#define TUSB_FS_DAP_EP0_TX_ADDR                             (TUSB_FS_DAP_USB_BUF_START + 64)
#define TUSB_FS_DAP_EP0_TYPE                                USB_EP_CONTROL

// EndPoints 1 defines
#define TUSB_FS_DAP_EP1_RX_SIZE                             (64)
#define TUSB_FS_DAP_EP1_RX_ADDR                             (TUSB_FS_DAP_USB_BUF_START + 128)
#define TUSB_FS_DAP_EP1_TYPE                                USB_EP_INTERRUPT

// EndPoints 2 defines
#define TUSB_FS_DAP_EP2_TX_SIZE                             (64)
#define TUSB_FS_DAP_EP2_TX_ADDR                             (TUSB_FS_DAP_USB_BUF_START + 192)
#define TUSB_FS_DAP_EP2_TYPE                                USB_EP_INTERRUPT


// Endpoint define for OTG core
#define TUSB_FS_DAP_OTG_MAX_OUT_SIZE                        (64)
#define TUSB_FS_DAP_OTG_CONTROL_EP_NUM                      (1)
#define TUSB_FS_DAP_OTG_OUT_EP_NUM                          (1)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33
#define TUSB_FS_DAP_OTG_RX_FIFO_SIZE_FS                     (256)
#define TUSB_FS_DAP_OTG_RX_FIFO_ADDR_FS                     (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 
#define TUSB_FS_DAP_EP0_TX_FIFO_ADDR_FS                     (256)
#define TUSB_FS_DAP_EP0_TX_FIFO_SIZE_FS                     (TUSB_FS_DAP_EP0_TX_SIZE * 7)
#define TUSB_FS_DAP_EP2_TX_FIFO_ADDR_FS                     (704)
#define TUSB_FS_DAP_EP2_TX_FIFO_SIZE_FS                     (TUSB_FS_DAP_EP2_TX_SIZE * 7)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 33
#define TUSB_FS_DAP_OTG_RX_FIFO_SIZE_HS                     (512)
#define TUSB_FS_DAP_OTG_RX_FIFO_ADDR_HS                     (0)
// Sum of IN ep max packet size is 128
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 
#define TUSB_FS_DAP_EP0_TX_FIFO_ADDR_HS                     (512)
#define TUSB_FS_DAP_EP0_TX_FIFO_SIZE_HS                     (TUSB_FS_DAP_EP0_TX_SIZE * 7)
#define TUSB_FS_DAP_EP2_TX_FIFO_ADDR_HS                     (960)
#define TUSB_FS_DAP_EP2_TX_FIFO_SIZE_HS                     (TUSB_FS_DAP_EP2_TX_SIZE * 7)

// EndPoint max packed sizes
extern const uint8_t TUSB_FS_DAP_txEpMaxSize[];
#define TUSB_FS_DAP_TXEP_MAX_SIZE                           \
const uint8_t TUSB_FS_DAP_txEpMaxSize[] = \
{TUSB_FS_DAP_EP0_TX_SIZE, 0, TUSB_FS_DAP_EP2_TX_SIZE, };
extern const uint8_t TUSB_FS_DAP_rxEpMaxSize[];
#define TUSB_FS_DAP_RXEP_MAX_SIZE                           \
const uint8_t TUSB_FS_DAP_rxEpMaxSize[] = \
{TUSB_FS_DAP_EP0_RX_SIZE, TUSB_FS_DAP_EP1_RX_SIZE, 0, };

// EndPoints init function for USB FS core
#define TUSB_FS_DAP_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_RxOnly(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_TYPE);       \
    SET_RX_ADDR(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TYPE);       \
    SET_TX_ADDR(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_ADDR);  \
  }while(0)


// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_FS) { \
    SET_RX_FIFO(dev, TUSB_FS_DAP_OTG_RX_FIFO_ADDR_FS, TUSB_FS_DAP_OTG_RX_FIFO_SIZE_FS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE, TUSB_FS_DAP_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_FIFO_ADDR_FS, TUSB_FS_DAP_EP0_TX_FIFO_SIZE_FS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE, TUSB_FS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Rx(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_TYPE, TUSB_FS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_Tx(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TYPE, TUSB_FS_DAP_EP2_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_FIFO_ADDR_FS, TUSB_FS_DAP_EP2_TX_FIFO_SIZE_FS);  \
  } \
  }while(0)

#else  // USB_OTG_FS
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_FS(dev) 

#endif  // USB_OTG_FS

#if defined(USB_OTG_HS)
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
  if(GetUSB(dev) == USB_OTG_HS) { \
    SET_RX_FIFO(dev, TUSB_FS_DAP_OTG_RX_FIFO_ADDR_HS, TUSB_FS_DAP_OTG_RX_FIFO_SIZE_HS);  \
    /* Init ep0 */ \
    INIT_EP_Tx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE, TUSB_FS_DAP_EP0_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TX_FIFO_ADDR_HS, TUSB_FS_DAP_EP0_TX_FIFO_SIZE_HS);  \
    INIT_EP_Rx(dev, PCD_ENDP0, TUSB_FS_DAP_EP0_TYPE, TUSB_FS_DAP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_Rx(dev, PCD_ENDP1, TUSB_FS_DAP_EP1_TYPE, TUSB_FS_DAP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_Tx(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TYPE, TUSB_FS_DAP_EP2_TX_SIZE);  \
    SET_TX_FIFO(dev, PCD_ENDP2, TUSB_FS_DAP_EP2_TX_FIFO_ADDR_HS, TUSB_FS_DAP_EP2_TX_FIFO_SIZE_HS);  \
  } \
  }while(0)

#else  // USB_OTG_HS
#define TUSB_FS_DAP_TUSB_INIT_EP_OTG_HS(dev) 

#endif // USB_OTG_HS

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


#define  TUSB_FS_DAP_DEVICE_DESCRIPTOR_SIZE  (18)
extern const uint8_t TUSB_FS_DAP_DeviceDescriptor [18];
#define TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0 33
extern const uint8_t TUSB_FS_DAP_ReportDescriptor_if0[TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0];
#define  TUSB_FS_DAP_CONFIG_DESCRIPTOR_SIZE  (41)
extern const uint8_t TUSB_FS_DAP_ConfigDescriptor [41];
#define TUSB_FS_DAP_DEV_STATUS0      (0)
#define TUSB_FS_DAP_DEV_STATUS1      (0)
#define TUSB_FS_DAP_DEV_STATUS    ((TUSB_FS_DAP_DEV_STATUS0) |(TUSB_FS_DAP_DEV_STATUS1) )

extern const uint8_t* const TUSB_FS_DAP_StringDescriptors[TUSB_FS_DAP_STRING_COUNT];
extern const tusb_descriptors TUSB_FS_DAP_descriptors;


/////////////////////////////////////////////////////////////////////
//// EndPoint for device2 define end
/////////////////////////////////////////////////////////////////////




#endif

