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