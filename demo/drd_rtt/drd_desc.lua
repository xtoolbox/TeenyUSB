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