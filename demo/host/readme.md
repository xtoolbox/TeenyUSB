# 无RTOS的主机例程 

host demo without RTOS

通过串口进行简单的交互，默认串口配置为115200 N 8 1，当USB口接入键盘时，可以通过USB键盘做为Console的输入，不支持ANSI转义字符输入(即不支持退格键、方向键)。

A simple terminal through serial port with default setting "115200 N 8 1". The terminal input could be a USB keyboard. ANSI escape character is not support.

## 简易命令行 Simple Terminal

按下Tab键显示支持的命令。 Press Tab to show supported commands.

```
 _______                    _    _  _____ ____  
|__   __|                  | |  | |/ ____|  _ \ 
   | | ___  ___ _ __  _   _| |  | | (___ | |_) |
   | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
   | |  __/  __/ | | | |_| | |__| |____) | |_) |
   |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
                       __/ |                    
                      |___/                     
TeenyUSB > 
lsusb      - list usb devices
showmem    - show memory usage
ls         - ls <dir>   list dir
mkdir      - mkdir <dir>  create dir
mv         - mv <oldfile> <newfile> move file
cat        - cat <file> display file
cp         - cp <source> <dest> copy file
rm         - rm <target> remove file or path
append     - append <file> <data> [data data ...] add data to file
TeenyUSB > 
```

### 显示连接的设备 list attached device
```
TeenyUSB > lsusb
Device of FS root hub
  Device VID:058f PID:9254, Parent: ROOT FS:0
    Interface 0: HUB
  Device VID:10c4 PID:ea60, Parent: Dev 01:3
    Interface 0: vendor class
Device of HS root hub
  Device VID:0930 PID:6545, Parent: ROOT HS:0
    Interface 0: Mass Storage
TeenyUSB > 
```
### 显示内存使用情况 list memory usage
```
TeenyUSB > showmem
  Memory used 176, max 176
  MQ used 1
  Device used 3
  Event used 3
TeenyUSB > 
```
### 文件操作 File operation
当连接U盘时文件操作才有效。 Effective when USB mass storage device connected.

下面是一个mount在[0:]上的U盘例子。 A sample MSC device mount on [0:].

通过日志可以看到第一次获取Capacity失败，读取Sense信息后第二次获取成功。
```
TeenyUSB > RHUB HS:0      Connect
RHUB HS:0      Enable
DEV  HS.00     Allocate pipe, in = 0, out = 1
DEV  HS.01     Re-allocate pipe, in = 0, out = 1
DEV  HS.01     HS device, VID:0930 PID:6545
ITF  HS.01.0-0 MSC lun(0) Kingston - DataTraveler G3  - PMAP
ITF  HS.01.0-0 MSC ep 81 stall
ITF  HS.01.0-0 MSC stall CSW res = 13, residue 8, status 1
ITF  HS.01.0-0 MSC xfer fail, res = -6, residue 8, status 1
ITF  HS.01.0-0 MSC Sense: SENSE_UNIT_ATTENTION, MEDIUM_HAVE_CHANGED, qua 0
ITF  HS.01.0-0 MSC lun(0) Block Count:7669824 Size:512
MSC mount [0:] r = 0
ITF  HS.01.0-0 MSC interface init
DEV  HS.01     Device has 1 interfaces
```
使用文件操作U盘中的文件。
```
TeenyUSB > ls 0:
0:/T1
0:/T2
0:/T3
0:/SYSTEM~1/WPSETT~1.DAT
0:/SYSTEM~1/INDEXE~1
TeenyUSB > cat t1
112233

TeenyUSB > cp t1 r1
TeenyUSB > cat r1
112233

TeenyUSB > mv r1 r2
TeenyUSB > cat r2
112233

TeenyUSB > append ff test_data
TeenyUSB > cat ff
test_data

TeenyUSB > append ff 123123123
TeenyUSB > cat ff
test_data
123123123

TeenyUSB > 
```
