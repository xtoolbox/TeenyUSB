# 无RTOS的主机例程 

host demo without RTOS

通过串口进行简单的交互，默认串口配置为115200 N 8 1，当USB口接入键盘时，可以通过键盘做为Console的输入，不支持ANSI转义字符输入。

A simple terminal through serial port with default setting "115200 N 8 1". The terminal input could be a USB keyboard. ANSI escape character is not support.

## 显示连接的设备 list attched device
```
TeenyUSB > lsusb
```

## 显示内存使用情况 list memory usage
```
TeenyUSB > showmem
```



