Teeny USB
==========
一个简易的STM32 USB主机和设备协议栈

A teeny USB device and host stack for STM32 MCUs

关于TeenyUSB的实现细节和使用方式请阅读《STM32 USB设备开发指南》 [Github下载](https://github.com/xtoolbox/TeenyUSB/releases/download/0.1/STM32_USB_desgin_guide.pdf) , [21IC下载](http://dl.21ic.com/download/stm32_usb-285543.html)

## 设备栈 Device Stack
- HID class
- MSC 大容量存储设备，U盘。 Mass storage class
- CDC 通讯设备类，目前只支持串口。 communication device class
- Vendor 厂商自定义设备。 vendor class

设备描述符由TeenyDT生成。 [在线试用TeenyDT](http://dt1.tusb.org)

Device descriptor is generate by TeenyDT. [Try TeenyDT online](http://dt.tusb.org)

## 主机栈 Host Stack
- HUB class, 支持多设备级连, support multiple device and hub
- HID class, 支持键盘、鼠标以及自定义HIDden设备。support keyboard, mouse, custom device
- Vendor class，厂商自定义设备


## 如何生成示例程序 How make demo

编译工具为 [arm-none-eabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads), compile toolchain is arm-none-eabi-gcc. 

调试使用VSCode+OpenOCD，[何在VSCode中配置STM32开发调试环境?](http://blog.xtoolbox.org/stm32_open_source_toolchain/)

``` batch
git clone https://github.com/xtoolbox/TeenyUSB.git
cd TeenyUSB
git submodule update --init
cd demo
make all -j8
```
如果更新mcu库很慢，可以使用gitee.com镜像。[https://gitee.com/xtoolbox/st_driver.git](https://gitee.com/xtoolbox/st_driver.git)

## 目录结构说明 Folder
```
.
├── TeenyDT         # 基于lua的USB描述符生成工具 A lua based USB descriptor generator 
├── demo            # 示例代码 Sample projects
├── mcu_lib         # MCU文件子仓库，MCU library sub module
├── usb_stack       # USB主机及设备类文件 TeenyUSB host and deivce class
└── pc_test_tool    # 基于lua Qt的Windows USB 测试程序 A luaQt based Windows program to test CDC/HID/WinUSB devices
```

## USB设备例程 Demo for device

- 复合设备，HID MSC WinUSB HID复合设备 Composite device, HID+MSC+WinUSB+HID
- 支持WinUSB的bulk设备, Bulk device with WinUSB support
- CMSIS DAP on STM32F723 discovery

## USB主机例程 Demo for host

- 简易交互式主机，支持HUB、键盘、鼠标以及自定义设备 [更多说明][host_readme]。 Interactive host, support HUB,Keyboard,Mouse and generic device. [more info][host_readme]

[host_readme]: https://github.com/xtoolbox/TeenyUSB/blob/master/demo/host/readme.md

## Demo测试用的开发板 Demo tested boards

| Board Folder     |      Board Type             |      Chip     |HSE Freq | USB Core            |
|------------------|-----------------------------|---------------|---------|---------------------|
| stm32f072c8t6    | Custom board                | STM32F072C8T6 | No HSE  | USB FS              |
| stm32f103ret6    | Custom board                | STM32F103RET6 | 8 MHz   | USB FS              |
| stm32f107vc      | Custom board                | STM32F107VCT6 | 25 MHz  | OTG_FS              |
| stm32f3_disco    | [stm32f3 disco][303]        | STM32F303VCT6 | 8 MHz   | USB FS              |
| stm32f407_evk    | [Waveshare EVK407I][407]    | STM32F407IGT6 | 8 MHz   | OTG_FS/OTG_HS_ULPI  |
| stm32f723e_disco | [stm32f723e discovery][723] | STM32F723IEK6 | 25 MHz  | OTG_FS/OTG_HS_Embed |
| stm32767zi_nucleo| [stm32f767zi nucleo][767]   | STM32F767ZIT6 | 8 MHz   | OTG_FS              |

[767]: https://www.st.com/en/evaluation-tools/nucleo-f767zi.html
[723]: https://www.st.com/en/evaluation-tools/32f723ediscovery.html
[407]: http://www.waveshare.net/wiki/EVK407I
[303]: https://www.st.com/en/evaluation-tools/stm32f3discovery.html

## 其它开源STM32 USB协议栈 Other USB stack for STM32
- [tinyusb](https://github.com/hathach/tinyusb.git)  全静态内存分配，不支持多设备和同类型多接口，暂时不支持STM32主机模式。
- [libopencm3](https://github.com/libopencm3/libopencm3.git) 动态生成描述符，不依赖官方库。设备类型少，不支持主机模式。
- [libusb_stm32](https://github.com/dmitrystu/libusb_stm32.git) 资源占用极少的USB设备库，不支持主机模式。

