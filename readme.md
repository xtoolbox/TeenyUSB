Teeny USB
==========
Not only a teeny USB stack for STM32 devices, but also a toolset to create USB descriptors and drivers.

## 特性
1. 支持中断传输，批量传输，批量双缓冲传输，同步传输。
2. 支持的例程：串口(CDC)，多串口(CDC x 7)，HID，多HID(HID x 7)，自定义批量(bulk)。
3. 使用lua脚本生成USB相关描述符，并根据描述符自动生成初始化配置代码。
4. 根据USB描述自动生成驱动文件，并对驱动文件签名。

## 目录结构说明
[usb_stack](./usb_stack) TeenyUSB协议栈的源码

[TeenyDT](./TeenyDT) 描述符自动生成工具及源码

[pc_test_tool](./pc_test_tool) PC端测试工具及源码
