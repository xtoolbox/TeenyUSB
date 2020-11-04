Teeny USB 
==========
[![Build Status](https://travis-ci.org/xtoolbox/teenyusb.svg?branch=master)](https://travis-ci.org/xtoolbox/teenyusb)

一个简易的STM32 USB主机和设备协议栈

A teeny USB device and host stack for STM32 MCUs

关于TeenyUSB的实现细节和使用方式请阅读《STM32 USB设备开发指南》 [Github下载](https://github.com/xtoolbox/TeenyUSB/releases/download/0.1/STM32_USB_desgin_guide.pdf) , [21IC下载](http://dl.21ic.com/download/stm32_usb-285543.html)

## 设备栈 Device Stack
- HID class
- MSC 大容量存储设备，U盘。 Mass storage class
- CDC ACM 虚拟串口。 Communication device class, abstract control mode, known as virtual serial port.
- CDC RNDIS 虚拟网口。 Communication device class, remote NDIS
- Vendor 厂商自定义设备。 vendor class

USB描述符由TeenyDT生成。 [在线试用TeenyDT](http://dt1.tusb.org)

USB descriptor is generate by TeenyDT. [Try TeenyDT online](http://dt.tusb.org)

### USB设备例程 Demo for device

- 复合设备，包含HID CDC(虚拟串口) WinUSB MSC(U盘) Composite device with HID+CDC+WinUSB+MSC
- WinUSB设备，Bulk传输免驱动。 Bulk device with WinUSB support
- CMSIS DAP on STM32F723 discovery
- 虚拟网卡, CDC RNDIS device

## 主机栈 Host Stack
- HUB class, 支持多设备级连, support multiple device and hub
- HID class, 支持键盘、鼠标以及自定义HIDden设备。support keyboard, mouse, custom device
- MSC class, 大容量存储类(U盘)，使用BOT协议。 mass storage class with BOT protocol
- CDC ACM class, 虚拟串口。 Communication device class, abstract control mode
- CDC RNDIS class,  虚拟网口。 Communication device class, remote NDIS
- Vendor class，厂商自定义设备

### USB主机例程 Demo for host

- 简易交互式主机，支持U盘、HUB、键盘、鼠标以及自定义设备 [更多说明][host_readme]。 Interactive host, support HUB,MSC,Keyboard,Mouse and generic device. [more info][host_readme]
- rt-thread双角色设备, 支持运行时切换设备类行，支持动态mount U盘，[更多说明][drd_readme]。Dual role device with rt-thread, [more info][drd_readme].

[host_readme]: https://github.com/xtoolbox/TeenyUSB/blob/master/demo/host/readme.md
[drd_readme]: https://github.com/xtoolbox/TeenyUSB/blob/master/demo/drd_rtt/readme.md

## 如何生成示例程序 How to make demo

编译工具为 [arm-none-eabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) is the compile toolchain.

调试使用VSCode+OpenOCD，[如何在VSCode中配置STM32开发调试环境?](http://blog.xtoolbox.org/stm32_open_source_toolchain/)

``` batch
git clone https://github.com/xtoolbox/TeenyUSB.git
cd TeenyUSB
git submodule update --init
cd sample
make all -j8
```
如果更新mcu库很慢，可以使用gitee.com镜像。[https://gitee.com/xtoolbox/st_driver.git](https://gitee.com/xtoolbox/st_driver.git)

### demo编译后各字段大小信息 Section size info of each demo
```
$ echo "   text    data     bss     dec     hex filename" && make all -j8 LOG_INFO=0 | grep '^[^l].*output/.*.elf'
   text    data     bss     dec     hex filename
  12072    1084   69296   82452   14214 output/f723dap.elf
   4480    1080    6068   11628    2d6c output/f072bulk.elf
   6392    1084    6416   13892    3644 output/f407bulk.elf
   5396    1084    6064   12544    3100 output/f303bulk.elf
   4972    1080    6068   12120    2f58 output/f103bulk.elf
   6524    1084    6416   14024    36c8 output/f723bulk.elf
   6284    1084    6416   13784    35d8 output/f767bulk.elf
   8100    3500    2200   13800    35e8 output/f103comp.elf
   8324    3500    2200   14024    36c8 output/f072comp.elf
  10132    3504    2588   16224    3f60 output/f407comp.elf
  10240    3504    2588   16332    3fcc output/f723comp.elf
   8528    3504    2196   14228    3794 output/f303comp.elf
  10000    3504    2588   16092    3edc output/f767comp.elf
  27560    1092   35776   64428    fbac output/f107host.elf
  28704    1092   28608   58404    e424 output/f407host.elf
  28860    1092   28832   58784    e5a0 output/f723host.elf
  28504    1092   28792   58388    e414 output/f767host.elf
```

## 目录结构说明 Folder
```
.
├── TeenyDT         # 基于lua的USB描述符生成工具 A lua based USB descriptor generator 
├── sample          # 示例代码 Sample projects
├── mcu_lib         # MCU文件子仓库，MCU library sub module
├── core            # USB核心文件， TeenyUSB core file
├── class           # USB设备类和主机类文件, TeenyUSB device and host class file
├── driver_stm32    # STM32驱动文件。 STM32 Driver file
├── pc_test_tool    # 基于lua Qt的Windows USB 测试程序 A luaQt based Windows program to test CDC/HID/WinUSB devices
└── third_part      # 第三方代码,如FatFs,rt-thread。 Third part source code, e.g. FatFs, rt-thread.
```

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
| stm32h743_openmv | [stm32h743_openmv][h743]    | STM32H743VIT6 | 12 MHz  | OTG_FS              |


[767]: https://www.st.com/en/evaluation-tools/nucleo-f767zi.html
[723]: https://www.st.com/en/evaluation-tools/32f723ediscovery.html
[407]: http://www.waveshare.net/wiki/EVK407I
[303]: https://www.st.com/en/evaluation-tools/stm32f3discovery.html
[h743]: https://github.com/Kevincoooool/OpenMV_PCB

## 其它支持STM32的开源USB协议栈 Other open source USB stack for STM32
- [tinyusb](https://github.com/hathach/tinyusb.git)  全静态内存分配，不支持多设备，不支持同一设备上使用多个同类型接口，暂时不支持STM32主机模式。
- [libopencm3](https://github.com/libopencm3/libopencm3.git) 动态生成描述符，不依赖官方库。USB设备类型少，不支持主机模式。
- [libusb_stm32](https://github.com/dmitrystu/libusb_stm32.git) 资源占用极少的USB设备库，不支持主机模式。

## 其它嵌入式开源USB协议栈
- [lufa](https://github.com/abcminiuser/lufa) 运行于AVR的轻量级USB主机和从机栈。
