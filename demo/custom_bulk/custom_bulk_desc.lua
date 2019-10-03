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