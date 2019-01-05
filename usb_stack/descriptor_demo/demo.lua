-- DEMO USB descriptor for teeny USB stack
return {
-- Demo descriptor of Bulk device
Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb bulk demo",
    strSerial = "tu123456",
    idVendor = 0x4322,
    idProduct = 0x4321,
    prefix = "BULK",
    bMaxPacketSize = 64,
    Config {
        bMaxPower = 100,
        SelfPower = true,
        RemoteWakeup = true,
        --Interface{
        --    WCID = WinUSB,
        --    EndPoint(IN(1),  BulkDouble, 64),
        --    EndPoint(OUT(2), BulkDouble, 64),
        --    strInterface = "teenyUsb1",
        --},
        Interface{
            WCID = WinUSB,
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
            strInterface = "TeenyUsb2",
        },
   }
},


Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb iso demo",
    strSerial = "tu123456",
    idVendor = 0x4322,
    idProduct = 0x4322,
    prefix = "ISO",
    bMaxPacketSize = 64,
    Config {
        bMaxPower = 100,
        SelfPower = true,
        RemoteWakeup = true,
        Interface{
            WCID = WinUSB,
            EndPoint(IN(1),  ISO, 64),
            EndPoint(OUT(2), ISO, 64),
            strInterface = "TeenyUsb2",
        },
   }
},

-- Demo descriptor of CDC device
Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb cdc demo",
    strSerial = "tu123456",
    idVendor = 0x03EB,    -- VID PID for Ateml CDC
    idProduct = 0x6124,
    prefix = "CDC",
    Config {
        bMaxPower = 100,
        SelfPower = true,
        RemoteWakeup = true,
        CDC_ACM{
            CtrlEp =  EndPoint(IN(3),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(1),  BulkDouble, 64),
            WriteEp = EndPoint(OUT(2), BulkDouble, 64),
        },
    },
},

-- Demo descriptor of 7 CDC device
Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb cdc7 demo",
    strSerial = "tu123456",
    idVendor = 0x2017,   -- Win10 can drive the multiple CDC, VID PID can be any one
    idProduct = 0x0926,
    prefix = "CDC7",
    Config {
        bMaxPower = 100,
        SelfPower = true,
        RemoteWakeup = true,
        CDC_ACM{
            CtrlEp =  EndPoint(IN(8),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(1),  BulkDouble, 64),
            WriteEp = EndPoint(OUT(1), BulkDouble, 64),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(9),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(2),  BulkDouble, 32),
            WriteEp = EndPoint(OUT(2), BulkDouble, 32),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(10),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(3),  BulkDouble, 32),
            WriteEp = EndPoint(OUT(3), BulkDouble, 32),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(11),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(4),  BulkDouble, 32),
            WriteEp = EndPoint(OUT(4), BulkDouble, 32),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(12),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(5),  BulkDouble, 16),
            WriteEp = EndPoint(OUT(5), BulkDouble, 16),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(13),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(6),  BulkDouble, 16),
            WriteEp = EndPoint(OUT(6), BulkDouble, 16),
        },
        CDC_ACM{
            CtrlEp =  EndPoint(IN(14),  Interrupt,  8, 1),
            ReadEp =  EndPoint(IN(7),  BulkDouble, 16),
            WriteEp = EndPoint(OUT(7), BulkDouble, 16),
        },
    },
},

-- Demo descriptor of HID device
Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb HID demo",
    strSerial = "tu123456",
    idVendor = 0x2017,
    idProduct = 0x0927,
    prefix = "HID",
    Config {
        bMaxPower = 100,
        SelfPower = true,
        RemoteWakeup = true,
        USB_HID{
            isKey = false,
            isMouse = false,
            isBoot = false,
            ReadEp = EndPoint(IN(1),  Interrupt, 64),
            WriteEp = EndPoint(OUT(1), Interrupt, 64),
            report = HID_InOut(64),
        },
   }
},

-- Demo descriptor of 7 HID device
Device {
    strManufacture = "teeny usb",
    strProduct = "teeny usb 7 HID demo",
    strSerial = "tu123456",
    idVendor = 0x2017,
    idProduct = 0x0928,
    prefix = "HID7",
    Config {
        USB_HID{
            ReadEp = EndPoint(IN(1),  Interrupt, 64),
            WriteEp = EndPoint(OUT(1), Interrupt, 64),
            report = HID_InOut(64),
        },
        USB_HID{
            ReadEp = EndPoint(IN(2),  Interrupt, 32),
            WriteEp = EndPoint(OUT(2), Interrupt, 32),
            report = HID_InOut(32),
        },
        USB_HID{
            ReadEp = EndPoint(IN(3),  Interrupt, 32),
            WriteEp = EndPoint(OUT(3), Interrupt, 32),
            report = HID_InOut(32),
        },
        USB_HID{
            ReadEp = EndPoint(IN(4),  Interrupt, 32),
            WriteEp = EndPoint(OUT(4), Interrupt, 32),
            report = HID_InOut(32),
        },
        USB_HID{
            ReadEp = EndPoint(IN(5),  Interrupt, 16),
            WriteEp = EndPoint(OUT(5), Interrupt, 16),
            report = HID_InOut(16),
        },
        USB_HID{
            ReadEp = EndPoint(IN(6),  Interrupt, 16),
            WriteEp = EndPoint(OUT(6), Interrupt, 16),
            report = HID_InOut(16),
        },
        USB_HID{
            ReadEp = EndPoint(IN(7),  Interrupt, 16),
            WriteEp = EndPoint(OUT(7), Interrupt, 16),
            report = HID_InOut(16),
        },
   }
},
}