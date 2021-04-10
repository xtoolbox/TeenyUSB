/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2021-04-10 11:29:10

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  winusb30_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return {
Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB30",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0001,
    prefix = "BULK30",
    bcdUSB = 0x300,
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 1024, 1, 4),
            EndPoint(OUT(1), BulkDouble, 1024, 1, 4),
        },
   },
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB20",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0002,
    prefix = "BULK20",
    bcdUSB = 0x210,
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 512),
            EndPoint(OUT(1), BulkDouble, 512),
        },
   },
}

}


  ------------- lua script end   ------------
 */
/////////////////////////////////////////
//// Device 0
/////////////////////////////////////////
#include "teeny_usb_desc.h"

#define  BULK30_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t BULK30_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x03,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x09,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x01, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  BULK30_CONFIG1_DESCRIPTOR_SIZE  (44)
__ALIGN_BEGIN  const uint8_t BULK30_ConfigDescriptor1 [44] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x2c, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0xff,                                             /* bInterfaceClass */
  0xff,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x00, 0x04,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// Endpoint Companion descriptor
  ///////////////////////////////////////
  0x06,                                             /* bLength */
  USB_SS_ENDPOINT_COMPANION_TYPE,                   /* bDescriptorType */
  0x03,                                             /* bMaxBurst */
  0x00,                                             /* bmAttributes */
  0x00, 0x00,                                       /* wBytesPerInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x00, 0x04,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// Endpoint Companion descriptor
  ///////////////////////////////////////
  0x06,                                             /* bLength */
  USB_SS_ENDPOINT_COMPANION_TYPE,                   /* bDescriptorType */
  0x03,                                             /* bMaxBurst */
  0x00,                                             /* bmAttributes */
  0x00, 0x00,                                       /* wBytesPerInterval */
};
#define BULK30_CONFIG_COUNT 1
const desc_t BULK30_ConfigDescriptors[BULK30_CONFIG_COUNT] = {
  BULK30_ConfigDescriptor1,
};
#define  BULK30_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  BULK30_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t BULK30_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  BULK30_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  BULK30_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t BULK30_StringDescriptor1 [18] __ALIGN_END = {
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
#define  BULK30_STRING_DESCRIPTOR2_STR   "TeenyUSB WinUSB30"
#define  BULK30_STRING_DESCRIPTOR2_SIZE   (36)
WEAK __ALIGN_BEGIN  const uint8_t BULK30_StringDescriptor2 [36] __ALIGN_END = {
  0x24,                                             /* bLength */
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
  'W', 0x00,                                        /* wcChar9 */
  'i', 0x00,                                        /* wcChar10 */
  'n', 0x00,                                        /* wcChar11 */
  'U', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  'B', 0x00,                                        /* wcChar14 */
  '3', 0x00,                                        /* wcChar15 */
  '0', 0x00,                                        /* wcChar16 */
};
#define  BULK30_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  BULK30_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t BULK30_StringDescriptor3 [22] __ALIGN_END = {
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
#define BULK30_STRING_COUNT    (4)
const uint8_t* const BULK30_StringDescriptors[4] = {
  BULK30_StringDescriptor0,
  BULK30_StringDescriptor1,
  BULK30_StringDescriptor2,
  BULK30_StringDescriptor3,
};
#define HAS_WCID
#define HAS_WCID_20
#define WCID_VENDOR_CODE 0x17
//////////////////////////////////////////////////////
///// WCID 2.0 descripors
/////////////////////////////////////////////////////
#if defined(HAS_WCID_20)

#define  BULK30_WCID_DESC_SET_SIZE  (162)
WEAK __ALIGN_BEGIN const uint8_t BULK30_WCIDDescriptorSet [162] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 descriptor set descriptor
  ///////////////////////////////////////
  0x0a, 0x00,                                       /* wLength */
  0x00, 0x00,                                       /* wDescriptorType */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  0xa2, 0x00,                                       /* wDescriptorSetTotalLength */
  
  ///////////////////////////////////////
  /// WCID20 compatible ID descriptor
  ///////////////////////////////////////
  0x14, 0x00,                                       /* wLength */
  0x03, 0x00,                                       /* wDescriptorType */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  
  ///////////////////////////////////////
  /// WCID20 registry property descriptor
  ///////////////////////////////////////
  0x84, 0x00,                                       /* wLength */
  0x04, 0x00,                                       /* wDescriptorType */
  0x07, 0x00,                                       /* wPropertyDataType */
  0x2a, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUIDs */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcPropertyName_21 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcPropertyName_21 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcPropertyName_21 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcPropertyName_21 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,       /* wcPropertyName_21 */
  0x00, 0x00,                                       /* wcPropertyName_21 */
  0x50, 0x00,                                       /* wPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcPropertyData_40 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcPropertyData_40 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcPropertyData_40 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcPropertyData_40 */
  '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,     /* wcPropertyData_40 */
};
#define  BULK30_WCID_BOS_SIZE  (50)
WEAK __ALIGN_BEGIN const uint8_t BULK30_WCIDBOS [50] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 BOS descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  USB_DESC_TYPE_BOS,                                /* bDescriptorType */
  0x32, 0x00,                                       /* wTotalLength */
  0x03,                                             /* bNumDeviceCaps */
  
  ///////////////////////////////////////
  /// USB 2.0 Extension Capability Descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  0x10,                                             /* bDescriptorType */
  0x02,                                             /* bDevCapabilityType */
  0x06, 0x00, 0x00, 0x00,                           /* dwAttributes */
  
  ///////////////////////////////////////
  /// SuperSpeed Device Capability Descriptor
  ///////////////////////////////////////
  0x0a,                                             /* bLength */
  0x10,                                             /* bDescriptorType */
  0x03,                                             /* bDevCapabilityType */
  0x00,                                             /* bAttributes */
  0x0e, 0x00,                                       /* wSpeedsSupported */
  0x01,                                             /* bFunctionalitySupport */
  0x0a,                                             /* bU1DevExitLat */
  0xff, 0x07,                                       /* wU2DevExitLat */
  
  ///////////////////////////////////////
  /// WCID20 device capability descriptor
  ///////////////////////////////////////
  0x1c,                                             /* bLength */
  0x10,                                             /* bDescriptorType */
  0x05,                                             /* bDevCapabilityType */
  0x00,                                             /* bReserved */
  0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c,   /* bPlatformCapabilityUUID_16 */
  0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f,   /* bPlatformCapabilityUUID_16 */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  LO_BYTE(BULK30_WCID_DESC_SET_SIZE), HI_BYTE(BULK30_WCID_DESC_SET_SIZE),/* wDescriptorSetTotalLength */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bAltEnumCode */
};


#endif    // #if defined(HAS_WCID_20)

#if defined(BULK30_WCID_DESCRIPTOR_SIZE) || \
    defined(BULK30_WCID_PROPERTIES_SIZE) || \
    defined(BULK30_WCID_BOS_SIZE) || \
    defined(BULK30_WCID_DESC_SET_SIZE)

const tusb_extra_desc_t BULK30_extra_descriptor = {
#if defined(BULK30_WCID_DESCRIPTOR_SIZE)
    .wcid_desc = BULK30_WCIDDescriptor,
#else
    .wcid_desc = 0,
#endif // BULK30_WCID_DESCRIPTOR_SIZE)

#if defined(BULK30_WCID_PROPERTIES_SIZE)
    .wcid_properties = BULK30_WCIDProperties,
#else
    .wcid_properties = 0,
#endif // BULK30_WCID_PROPERTIES_SIZE

#if defined(BULK30_WCID_BOS_SIZE)
    .wcid_bos = BULK30_WCIDBOS,
#else
    .wcid_bos = 0,  
#endif // BULK30_WCID_BOS_SIZE)

#if defined(BULK30_WCID_DESC_SET_SIZE)
    .wcid_desc_set = BULK30_WCIDDescriptorSet,
#else
    .wcid_desc_set = 0,  
#endif // BULK30_WCID_DESC_SET_SIZE
};
#define BULK30_EXT_DESC  (&BULK30_extra_descriptor)
#else
#define BULK30_EXT_DESC  (0)
#endif


//BULK30_TXEP_MAX_SIZE
//BULK30_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors_t BULK30_descriptors = {
  .device = BULK30_DeviceDescriptor,
  .configs = BULK30_ConfigDescriptors,
  .config_count = BULK30_CONFIG_COUNT,
  .strings = BULK30_StringDescriptors,
  .string_cnt = BULK30_STRING_COUNT,
  .extra = BULK30_EXT_DESC,
};
/////////////////////////////////////////
//// Device 1
/////////////////////////////////////////
#include "teeny_usb_desc.h"

#define  BULK20_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t BULK20_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x10, 0x02,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x02, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  BULK20_CONFIG1_DESCRIPTOR_SIZE  (32)
__ALIGN_BEGIN  const uint8_t BULK20_ConfigDescriptor1 [32] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x20, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0xff,                                             /* bInterfaceClass */
  0xff,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x00, 0x02,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x00, 0x02,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define BULK20_CONFIG_COUNT 1
const desc_t BULK20_ConfigDescriptors[BULK20_CONFIG_COUNT] = {
  BULK20_ConfigDescriptor1,
};
#define  BULK20_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  BULK20_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t BULK20_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  BULK20_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  BULK20_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t BULK20_StringDescriptor1 [18] __ALIGN_END = {
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
#define  BULK20_STRING_DESCRIPTOR2_STR   "TeenyUSB WinUSB20"
#define  BULK20_STRING_DESCRIPTOR2_SIZE   (36)
WEAK __ALIGN_BEGIN  const uint8_t BULK20_StringDescriptor2 [36] __ALIGN_END = {
  0x24,                                             /* bLength */
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
  'W', 0x00,                                        /* wcChar9 */
  'i', 0x00,                                        /* wcChar10 */
  'n', 0x00,                                        /* wcChar11 */
  'U', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  'B', 0x00,                                        /* wcChar14 */
  '2', 0x00,                                        /* wcChar15 */
  '0', 0x00,                                        /* wcChar16 */
};
#define  BULK20_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  BULK20_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t BULK20_StringDescriptor3 [22] __ALIGN_END = {
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
#define BULK20_STRING_COUNT    (4)
const uint8_t* const BULK20_StringDescriptors[4] = {
  BULK20_StringDescriptor0,
  BULK20_StringDescriptor1,
  BULK20_StringDescriptor2,
  BULK20_StringDescriptor3,
};
#define HAS_WCID
#define HAS_WCID_20
#define WCID_VENDOR_CODE 0x17
//////////////////////////////////////////////////////
///// WCID 2.0 descripors
/////////////////////////////////////////////////////
#if defined(HAS_WCID_20)

#define  BULK20_WCID_DESC_SET_SIZE  (162)
WEAK __ALIGN_BEGIN const uint8_t BULK20_WCIDDescriptorSet [162] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 descriptor set descriptor
  ///////////////////////////////////////
  0x0a, 0x00,                                       /* wLength */
  0x00, 0x00,                                       /* wDescriptorType */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  0xa2, 0x00,                                       /* wDescriptorSetTotalLength */
  
  ///////////////////////////////////////
  /// WCID20 compatible ID descriptor
  ///////////////////////////////////////
  0x14, 0x00,                                       /* wLength */
  0x03, 0x00,                                       /* wDescriptorType */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  
  ///////////////////////////////////////
  /// WCID20 registry property descriptor
  ///////////////////////////////////////
  0x84, 0x00,                                       /* wLength */
  0x04, 0x00,                                       /* wDescriptorType */
  0x07, 0x00,                                       /* wPropertyDataType */
  0x2a, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUIDs */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcPropertyName_21 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcPropertyName_21 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcPropertyName_21 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcPropertyName_21 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,       /* wcPropertyName_21 */
  0x00, 0x00,                                       /* wcPropertyName_21 */
  0x50, 0x00,                                       /* wPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcPropertyData_40 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcPropertyData_40 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcPropertyData_40 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcPropertyData_40 */
  '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,     /* wcPropertyData_40 */
};
#define  BULK20_WCID_BOS_SIZE  (33)
WEAK __ALIGN_BEGIN const uint8_t BULK20_WCIDBOS [33] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 BOS descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  USB_DESC_TYPE_BOS,                                /* bDescriptorType */
  0x21, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumDeviceCaps */
  
  ///////////////////////////////////////
  /// WCID20 device capability descriptor
  ///////////////////////////////////////
  0x1c,                                             /* bLength */
  0x10,                                             /* bDescriptorType */
  0x05,                                             /* bDevCapabilityType */
  0x00,                                             /* bReserved */
  0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c,   /* bPlatformCapabilityUUID_16 */
  0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f,   /* bPlatformCapabilityUUID_16 */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  LO_BYTE(BULK20_WCID_DESC_SET_SIZE), HI_BYTE(BULK20_WCID_DESC_SET_SIZE),/* wDescriptorSetTotalLength */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bAltEnumCode */
};


#endif    // #if defined(HAS_WCID_20)

#if defined(BULK20_WCID_DESCRIPTOR_SIZE) || \
    defined(BULK20_WCID_PROPERTIES_SIZE) || \
    defined(BULK20_WCID_BOS_SIZE) || \
    defined(BULK20_WCID_DESC_SET_SIZE)

const tusb_extra_desc_t BULK20_extra_descriptor = {
#if defined(BULK20_WCID_DESCRIPTOR_SIZE)
    .wcid_desc = BULK20_WCIDDescriptor,
#else
    .wcid_desc = 0,
#endif // BULK20_WCID_DESCRIPTOR_SIZE)

#if defined(BULK20_WCID_PROPERTIES_SIZE)
    .wcid_properties = BULK20_WCIDProperties,
#else
    .wcid_properties = 0,
#endif // BULK20_WCID_PROPERTIES_SIZE

#if defined(BULK20_WCID_BOS_SIZE)
    .wcid_bos = BULK20_WCIDBOS,
#else
    .wcid_bos = 0,  
#endif // BULK20_WCID_BOS_SIZE)

#if defined(BULK20_WCID_DESC_SET_SIZE)
    .wcid_desc_set = BULK20_WCIDDescriptorSet,
#else
    .wcid_desc_set = 0,  
#endif // BULK20_WCID_DESC_SET_SIZE
};
#define BULK20_EXT_DESC  (&BULK20_extra_descriptor)
#else
#define BULK20_EXT_DESC  (0)
#endif


//BULK20_TXEP_MAX_SIZE
//BULK20_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors_t BULK20_descriptors = {
  .device = BULK20_DeviceDescriptor,
  .configs = BULK20_ConfigDescriptors,
  .config_count = BULK20_CONFIG_COUNT,
  .strings = BULK20_StringDescriptors,
  .string_cnt = BULK20_STRING_COUNT,
  .extra = BULK20_EXT_DESC,
};
