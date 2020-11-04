/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2020-08-20 15:05:12

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
            EndPoint(IN(2),  Interrupt, 8),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        },
   }
}

  ------------- lua script end   ------------
 */
#include "teeny_usb_desc.h"

#define  RNDIS_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t RNDIS_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0xef,                                             /* bDeviceClass */
  0x02,                                             /* bDeviceSubClass */
  0x01,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x31, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  RNDIS_CONFIG1_DESCRIPTOR_SIZE  (75)
__ALIGN_BEGIN  const uint8_t RNDIS_ConfigDescriptor1 [75] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x4b, 0x00,                                       /* wTotalLength */
  0x02,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface association descriptor
  ///////////////////////////////////////
  0x08,                                             /* bLength */
  USB_IAD_DESCRIPTOR_TYPE,                          /* bDescriptorType */
  0x00,                                             /* bFirstInterface */
  0x02,                                             /* bInterfaceCount */
  0x02,                                             /* bFunctionClass */
  0x02,                                             /* bFunctionSubClass */
  0xff,                                             /* bFunctionProtocol */
  0x00,                                             /* iFunction */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x01,                                             /* bNumEndpoints */
  0x02,                                             /* bInterfaceClass */
  0x02,                                             /* bInterfaceSubClass */
  0xff,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// cdc acm header descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x00,                                             /* bDescriptorSubtype */
  0x10, 0x01,                                       /* bcdCDC */
  
  ///////////////////////////////////////
  /// cdc acm call management descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x01,                                             /* bDescriptorSubtype */
  0x00,                                             /* bmCapabilities */
  0x01,                                             /* bDataInterface */
  
  ///////////////////////////////////////
  /// cdc acm descriptor
  ///////////////////////////////////////
  0x04,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x02,                                             /* bDescriptorSubtype */
  0x02,                                             /* bmCapabilities */
  
  ///////////////////////////////////////
  /// cdc acm union descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x06,                                             /* bDescriptorSubtype */
  0x00,                                             /* bMasterInterface */
  0x01,                                             /* bSlaveInterface0 */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x82,                                             /* bEndpointAddress */
  0x03,                                             /* bmAttributes */
  0x08, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x01,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0x0a,                                             /* bInterfaceClass */
  0x00,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define RNDIS_CONFIG_COUNT 1
const desc_t RNDIS_ConfigDescriptors[RNDIS_CONFIG_COUNT] = {
  RNDIS_ConfigDescriptor1,
};
#define  RNDIS_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  RNDIS_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  RNDIS_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  RNDIS_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  RNDIS_STRING_DESCRIPTOR2_STR   "TeenyUSB RNDIS DEMO"
#define  RNDIS_STRING_DESCRIPTOR2_SIZE   (40)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor2 [40] __ALIGN_END = {
  0x28,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'R', 0x00,                                        /* wcChar9 */
  'N', 0x00,                                        /* wcChar10 */
  'D', 0x00,                                        /* wcChar11 */
  'I', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  ' ', 0x00,                                        /* wcChar14 */
  'D', 0x00,                                        /* wcChar15 */
  'E', 0x00,                                        /* wcChar16 */
  'M', 0x00,                                        /* wcChar17 */
  'O', 0x00,                                        /* wcChar18 */
};
#define  RNDIS_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  RNDIS_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define RNDIS_STRING_COUNT    (4)
const uint8_t* const RNDIS_StringDescriptors[4] = {
  RNDIS_StringDescriptor0,
  RNDIS_StringDescriptor1,
  RNDIS_StringDescriptor2,
  RNDIS_StringDescriptor3,
};
#define HAS_WCID
#define HAS_WCID_20
#define WCID_VENDOR_CODE 0x17

//////////////////////////////////////////////////////
///// WCID descripors
//////////////////////////////////////////////////////
#if defined(HAS_WCID)

// Define WCID os string descriptor 
#ifndef WCID_MSOS_STRING
#define WCID_MSOS_STRING
#define WCID_STRING_DESCRIPTOR_MSOS_STR          "MSFT100"
#define WCID_STRING_DESCRIPTOR_MSOS_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// MS OS string descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  /* MSFT100 */
  'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00,       /* wcChar_7 */
  '1', 0x00, '0', 0x00, '0', 0x00,                  /* wcChar_7 */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bReserved */
};

#endif // WCID_MSOS_STRING
#define  RNDIS_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t RNDIS_WCIDDescriptor [40] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID descriptor
  ///////////////////////////////////////
  0x28, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x04, 0x00,                                       /* wIndex */
  0x01,                                             /* bCount */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* bReserved_7 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x00,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* RNDIS */
  'R', 'N', 'D', 'I', 'S', 0x00, 0x00, 0x00,        /* cCID_8 */
  /* 5162001 */
  '5', '1', '6', '2', '0', '0', '1', 0x00,          /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
};
#define RNDIS_WCID_PROPERTIES_SIZE (2)
WEAK __ALIGN_BEGIN const desc_t RNDIS_WCIDProperties[ RNDIS_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
  0,    // No WCID in Interface 0
  0,    // No WCID in Interface 1
};
#endif    // #if defined(HAS_WCID)

#if defined(RNDIS_WCID_DESCRIPTOR_SIZE) || \
    defined(RNDIS_WCID_PROPERTIES_SIZE) || \
    defined(RNDIS_WCID_BOS_SIZE) || \
    defined(RNDIS_WCID_DESC_SET_SIZE)

const tusb_extra_desc_t RNDIS_extra_descriptor = {
#if defined(RNDIS_WCID_DESCRIPTOR_SIZE)
    .wcid_desc = RNDIS_WCIDDescriptor,
#else
    .wcid_desc = 0,
#endif // RNDIS_WCID_DESCRIPTOR_SIZE)

#if defined(RNDIS_WCID_PROPERTIES_SIZE)
    .wcid_properties = RNDIS_WCIDProperties,
#else
    .wcid_properties = 0,
#endif // RNDIS_WCID_PROPERTIES_SIZE

#if defined(RNDIS_WCID_BOS_SIZE)
    .wcid_bos = RNDIS_WCIDBOS,
#else
    .wcid_bos = 0,  
#endif // RNDIS_WCID_BOS_SIZE)

#if defined(RNDIS_WCID_DESC_SET_SIZE)
    .wcid_desc_set = RNDIS_WCIDDescriptorSet,
#else
    .wcid_desc_set = 0,  
#endif // RNDIS_WCID_DESC_SET_SIZE
};
#define RNDIS_EXT_DESC  (&RNDIS_extra_descriptor)
#else
#define RNDIS_EXT_DESC  (0)
#endif


//RNDIS_TXEP_MAX_SIZE
//RNDIS_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors_t RNDIS_descriptors = {
  .device = RNDIS_DeviceDescriptor,
  .configs = RNDIS_ConfigDescriptors,
  .config_count = RNDIS_CONFIG_COUNT,
  .strings = RNDIS_StringDescriptors,
  .string_cnt = RNDIS_STRING_COUNT,
  .extra = RNDIS_EXT_DESC,
};
