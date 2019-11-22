Teeny USB
==========
A teeny USB stack for STM32 MCUs, also contain a toolset to create USB descriptors and drivers.

# 如何生成示例程序 How make demo
``` batch
git clone https://github.com/xtoolbox/TeenyUSB.git
cd TeenyUSB
git submodule update --init
cd demo
make all -j8
```

关于TeenyUSB的实现细节和使用方式请阅读《STM32 USB设备开发指南》 [Github下载](https://github.com/xtoolbox/TeenyUSB/releases/download/0.1/STM32_USB_desgin_guide.pdf) , [21IC下载](http://dl.21ic.com/download/stm32_usb-285543.html)

# 特性
1. 设备例程：U盘+串口+自定义HID+WinUSB 复合设备，自定义设备批量传输(无需驱动), F723 DapLink。
2. 主机例程：无协议栈主机，键盘鼠标+HUB。
3. 使用lua脚本生成USB相关描述符，并根据描述符自动生成初始化配置代码，[在线试用](http://dt1.tusb.org)。
# Feature
1. Device demo: Composite(MSC+CDC+HID+WinUSB) , Custom Bulk (No custom driver need, even no inf), DAPLink V1/V2 in STM32F723
2. Host demo: raw host, hub+keyboard+mouse
3. USB descriptor generation by lua script, [Try it online](http://dt.tusb.org).

# 目录结构说明 Folder
[usb_stack](./usb_stack) TeenyUSB协议栈的源码 Source code of TeenyUSB

[Demo](./demo) 演示代码目录，demo folder. "make all" 编译所有DEMO，make all demo.

[pc_test_tool](./pc_test_tool) PC端测试工具及源码 Source code for USB device test tool

# TeenyUSB协议栈 TeenyUSB Stack

## 示例 demos

## USB设备例程 Demo for device

| Demo Folder      |      Demo Type       |  Tested Board  |
|------------------|----------------------|----------------|
| custom_bulk      | 自定义 Bulk 设备      | All Below      |
| f723dap          | stm32f723 DapLink    | All Below      |
| composite        | HID+MSC+WinUSB+HID  | All Below      |

## USB主机例程 Demo for host

| Demo Folder      |      Demo Type       |  Tested Board  |
|------------------|----------------------|----------------|
| host_raw         | 无协议栈主机 Raw Host| 723            |
| host_input       | HUB+KeyBoard+Mouse   | 723            |

## USB OTG例程 Demo for OTG
| Demo Folder      |      Demo Type       |  Tested Board  |
|------------------|----------------------|----------------|
| host_rtt         | OTG in [rtthread][rtt]   | [stm32f723e_disco][723] |

接入普通USB线时，工作在设备模式，FS设备默认为CDC串口，HS设备默认为U盘，使用板载QSPI，可以通过命令行更改设备角色，重新插拔后生效。

接入OTG线时，工作在主机模式，主机采用修改后的[rtthread][rtt] USB主机协议栈，目前支持HUB，U盘，键盘和鼠标。

When plug with normal USB cable, work in device mode, the FS core is CDC and HS core is MSC by default. The device role can be changed through the command.

When plug with OTG cable, work in host mode, support HUB, Mass storage, keyboard and mouse devices.


## Demo测试用的开发板 Demo tested on boards

| Board Folder     |      Board Type             |      Chip     |HSE Freq | Tested USB Core     |
|------------------|-----------------------------|---------------|---------|---------------------|
| stm32f0          | Custom board                | STM32F072C8T6 | No HSE  | USB FS              |
| stm32f1          | Custom board                | STM32F103RET6 | 8 MHz   | USB FS              |
| stm32f407_evk    | [Waveshare EVK407I][407]    | STM32F407IGT6 | 8 MHz   | OTG_FS/OTG_HS_ULPI  |
| stm32f723e_disco | [stm32f723e discovery][723] | STM32F723IEK6 | 25 MHz  | OTG_FS/OTG_HS_Embed |
| stm32767zi_nucleo| [stm32f767zi nucleo][767]   | STM32F767ZIT6 | 8 MHz   | OTG_FS              |

[767]: https://www.st.com/en/evaluation-tools/nucleo-f767zi.html
[723]: https://www.st.com/en/evaluation-tools/32f723ediscovery.html
[407]: http://www.waveshare.net/wiki/EVK407I
[303]: https://www.st.com/en/evaluation-tools/stm32f3discovery.html
[rtt]: https://github.com/RT-Thread/rt-thread



# USB测试工具 USB Test Tool
串口测试 Serial Port Viewer

![test_cdc5](images/test_tool_cdc5.png)

HID测试 HID Viewer

![test_hid](images/test_tool_hid.png)


