/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2019-09-13 20:54:04
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
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
        },
   }
}  

  ------------- lua script end   ------------
 */
#include "teeny_usb.h"
#define  BULK_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t BULK_DeviceDescriptor [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x00, 0x02,                                   /* bcdUSB */
    0x00,                                         /* bDeviceClass */
    0x00,                                         /* bDeviceSubClass */
    0x00,                                         /* bDeviceProtocol */
    0x40,                                         /* bMaxPacketSize */
    LOBYTE(BULK_VID), HIBYTE(BULK_VID),           /* idVendor */
    LOBYTE(BULK_PID), HIBYTE(BULK_PID),           /* idProduct */
    0x00, 0x01,                                   /* bcdDevice */
    0x01,                                         /* iManufacture */
    0x02,                                         /* iProduct */
    0x03,                                         /* iSerial */
    0x01,                                         /* bNumConfigurations */
};

// Configs 
#define  BULK_CONFIG_DESCRIPTOR_SIZE  (32)
__ALIGN_BEGIN const uint8_t BULK_ConfigDescriptor [32] __ALIGN_END = {
    0x09,                                         /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,            /* bDescriptorType */
    0x20, 0x00,                                   /* wTotalLength */
    0x01,                                         /* bNumInterfaces */
    0x01,                                         /* bConfigurationValue */
    0x00,                                         /* iConfiguration */
    0x80,                                         /* bmAttributes */
    0x64,                                         /* bMaxPower */
    /* Interface descriptor, len: 23*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x00,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0xff,                                         /* bInterfaceClass */
    0xff,                                         /* bInterfaceSubClass */
    0x00,                                         /* bInterfaceProtocol */
    0x00,                                         /* iInterface */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x81,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x02,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
};

// Power status
#define BULK_DEV_STATUS0      (0)
#define BULK_DEV_STATUS1      (0)
#define BULK_DEV_STATUS    ((BULK_DEV_STATUS0) |(BULK_DEV_STATUS1) )

BULK_TXEP_MAX_SIZE
BULK_RXEP_MAX_SIZE

// Strings 
#define BULK_STRING_DESCRIPTOR0_STR          "\x09\x04"
#define BULK_STRING_DESCRIPTOR0_SIZE          (4)
WEAK __ALIGN_BEGIN const uint8_t  BULK_StringDescriptor0 [4] __ALIGN_END = {
    0x04,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x09, 0x04,                                   /* wLangID0 */
};
#define BULK_STRING_DESCRIPTOR1_STR          "TeenyUSB"
#define BULK_STRING_DESCRIPTOR1_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  BULK_StringDescriptor1 [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'e', 0x00,                                    /* wcChar1 */
    'e', 0x00,                                    /* wcChar2 */
    'n', 0x00,                                    /* wcChar3 */
    'y', 0x00,                                    /* wcChar4 */
    'U', 0x00,                                    /* wcChar5 */
    'S', 0x00,                                    /* wcChar6 */
    'B', 0x00,                                    /* wcChar7 */
};
#define BULK_STRING_DESCRIPTOR2_STR          "TeenyUSB Custom Bulk"
#define BULK_STRING_DESCRIPTOR2_SIZE          (42)
WEAK __ALIGN_BEGIN const uint8_t  BULK_StringDescriptor2 [42] __ALIGN_END = {
    0x2a,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'e', 0x00,                                    /* wcChar1 */
    'e', 0x00,                                    /* wcChar2 */
    'n', 0x00,                                    /* wcChar3 */
    'y', 0x00,                                    /* wcChar4 */
    'U', 0x00,                                    /* wcChar5 */
    'S', 0x00,                                    /* wcChar6 */
    'B', 0x00,                                    /* wcChar7 */
    ' ', 0x00,                                    /* wcChar8 */
    'C', 0x00,                                    /* wcChar9 */
    'u', 0x00,                                    /* wcChar10 */
    's', 0x00,                                    /* wcChar11 */
    't', 0x00,                                    /* wcChar12 */
    'o', 0x00,                                    /* wcChar13 */
    'm', 0x00,                                    /* wcChar14 */
    ' ', 0x00,                                    /* wcChar15 */
    'B', 0x00,                                    /* wcChar16 */
    'u', 0x00,                                    /* wcChar17 */
    'l', 0x00,                                    /* wcChar18 */
    'k', 0x00,                                    /* wcChar19 */
};
#define BULK_STRING_DESCRIPTOR3_STR          "TUSB123456"
#define BULK_STRING_DESCRIPTOR3_SIZE          (22)
WEAK __ALIGN_BEGIN const uint8_t  BULK_StringDescriptor3 [22] __ALIGN_END = {
    0x16,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'U', 0x00,                                    /* wcChar1 */
    'S', 0x00,                                    /* wcChar2 */
    'B', 0x00,                                    /* wcChar3 */
    '1', 0x00,                                    /* wcChar4 */
    '2', 0x00,                                    /* wcChar5 */
    '3', 0x00,                                    /* wcChar6 */
    '4', 0x00,                                    /* wcChar7 */
    '5', 0x00,                                    /* wcChar8 */
    '6', 0x00,                                    /* wcChar9 */
};
const uint8_t* const BULK_StringDescriptors[BULK_STRING_COUNT] = {
BULK_StringDescriptor0,
BULK_StringDescriptor1,
BULK_StringDescriptor2,
BULK_StringDescriptor3,
};



#if defined(HAS_WCID)

// Define WCID os string descriptor 
#ifndef WCID_MSOS_STRING
#define WCID_MSOS_STRING
#define WCID_STRING_DESCRIPTOR_MSOS_STR          "MSFT100"
#define WCID_STRING_DESCRIPTOR_MSOS_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'M', 0x00,                                    /* wcChar0 */
    'S', 0x00,                                    /* wcChar1 */
    'F', 0x00,                                    /* wcChar2 */
    'T', 0x00,                                    /* wcChar3 */
    '1', 0x00,                                    /* wcChar4 */
    '0', 0x00,                                    /* wcChar5 */
    '0', 0x00,                                    /* wcChar6 */
    WCID_VENDOR_CODE,                             /* bVendorCode */
    0x00,                                         /* bReserved */
};
#endif

#define  BULK_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t BULK_WCIDDescriptor [40] __ALIGN_END = {
    0x28, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x04, 0x00,                                   /* wIndex */
    0x01,                                         /* bCount */
    0,0,0,0,0,0,0,                                /* Reserved */
    /* WCID Function  */
    0x00,                                         /* bFirstInterfaceNumber */
    0x01,                                         /* bReserved */
    /* CID */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 
    /* sub CID */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0,0,0,0,0,0,                                  /* Reserved */
};


#define  BULK__IF0_WCID_PROPERTIES_SIZE  (142)
WEAK __ALIGN_BEGIN const uint8_t BULK__IF0_WCIDProperties [142] __ALIGN_END = {
    0x8e, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x05, 0x00,                                   /* wIndex */
    0x01, 0x00,                                   /* wCount */
    /*WCID property field */
  /* DeviceInterfaceGUID = 
       {30da925a-2f8e-8a2e-fd7a-4c784db044d6}  */
    0x84, 0x00, 0x00, 0x00,                       /* dwSize */
    0x01, 0x00, 0x00, 0x00,                       /* dwPropertyDataType */
    0x28, 0x00,                                   /* wPropertyNameLength */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00, 
    
    0x4e, 0x00, 0x00, 0x00,                       /* dwPropertyDataLength */
    '{', 0x00, '3', 0x00, '0', 0x00, 'd', 0x00, 
    'a', 0x00, '9', 0x00, '2', 0x00, '5', 0x00, 
    'a', 0x00, '-', 0x00, '2', 0x00, 'f', 0x00, 
    '8', 0x00, 'e', 0x00, '-', 0x00, '8', 0x00, 
    'a', 0x00, '2', 0x00, 'e', 0x00, '-', 0x00, 
    'f', 0x00, 'd', 0x00, '7', 0x00, 'a', 0x00, 
    '-', 0x00, '4', 0x00, 'c', 0x00, '7', 0x00, 
    '8', 0x00, '4', 0x00, 'd', 0x00, 'b', 0x00, 
    '0', 0x00, '4', 0x00, '4', 0x00, 'd', 0x00, 
    '6', 0x00, '}', 0x00, 0x00, 0x00, 
};


#define BULK_WCID_PROPERTIES_SIZE (1)
WEAK __ALIGN_BEGIN const desc_t BULK_WCIDProperties[ BULK_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
BULK__IF0_WCIDProperties,

};

#endif

//  Device descriptors
const tusb_descriptors BULK_descriptors = {
  .device = BULK_DeviceDescriptor,
  .config = BULK_ConfigDescriptor,
  .strings = BULK_StringDescriptors,
  .string_cnt = BULK_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(BULK_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = BULK_WCIDDescriptor,
#else
  .wcid_desc = 0,  
#endif // BULK_WCID_DESCRIPTOR_SIZE)

#if defined(BULK_WCID_PROPERTIES_SIZE)
  .wcid_properties = BULK_WCIDProperties,
#else
  .wcid_properties = 0,  
#endif // BULK_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(BULK_WCID_BOS_SIZE)
  .wcid_bos = BULK_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // BULK_WCID_BOS_SIZE)

#if defined(BULK_WCID_DESC_SET_SIZE)
  .wcid_desc_set = BULK_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // BULK_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};


