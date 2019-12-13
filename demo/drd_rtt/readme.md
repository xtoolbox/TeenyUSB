双角色设备rt-thread例程 Dual role device demo with rt-thread
==========

## 如何编译此例程 How to build this DEMO

1. 配置scons环境 Setup scons env.
2. 配置arm-none-eabi-gcc环境 Setup arm-none-eabi-gcc env.
```
git clone https://github.com/RT-Thread/rt-thread.git
git clone https://github.com/xtoolbox/TeenyUSB.git
cd TeenyUSB
git submodule update --init
cd demo/drd_rtt
set RTT_ROOT=../../../rt-thread
set RTT_EXEC_PATH=<your gcc binary folder e.g. set RTT_EXEC_PATH=D:/tools/gcc-arm-none-eabi/bin>
scons -j8
```

## 功能 feature

| type          | Device |  Host  |
|---------------|--------|--------|
| MSC           |  YES   |  YES   |
| WinUSB        |  YES   |  NO    |
| CDC ACM       |  YES   |  NO    |
| CDC RNDIS     |  YES   |  YES   |
| HID Keyboard  |  YES   |  YES   |
| HID Mouse     |  YES   |  NO    |
| HUB           |  NO    |  YES   |

## 如何使用此例程 How to use this DEMO

使用OTG线时(ID线接地)自动切换为主机模式。使用普通连时自动切换为设备模式。

when OTG cable plug, it will auto switch to host mode.

```
Host thread runnig
msh />USB ID check thread runnig
Change FS to device mode
Open USB FS device
Change HS to device mode
Open USB HS device
Change HS to host mode
Close USB HS device
Change FS to host mode
Close USB FS device
```

设备角色可以在运行时进行切换，无需插拔USB线。 Device will auto enumurate after changed without re-plug the cable.
```
msh />
usbfs_cdc        - set USB FS to CDC device
usbfs_msc        - set USB FS to MSC device
usbfs_kb         - set USB FS to keyboard device
usbfs_mouse      - set USB FS to mouse device
usbfs_winusb     - set USB FS to WinUSB device
usbfs_rndis      - set USB FS to RNDIS device
usbhs_cdc        - set USB HS to CDC device
usbhs_msc        - set USB HS to MSC device
usbhs_kb         - set USB HS to keyboard device
usbhs_mouse      - set USB HS to mouse device
usbhs_winusb     - set USB HS to WinUSB device
usbhs_rndis      - set USB HS to RNDIS device

```


## U盘功能 MSC feature
### 主机模式 host mode
自动mount, 使用文件命令进行操作。 auto mount, use standard file operation.

```
msh />RHUB HS:0      Connect
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
Mount /udisk0, (Kingston - DataTraveler G3  - PMAP lun=0 )
ITF  HS.01.0-0 MSC interface init
DEV  HS.01     Device has 1 interfaces

msh />ls udisk0
Directory udisk0:
T1                  8                        
T2                  15                       
T3                  21                       
System Volume Information<DIR>                    
FF                  22                       
R2                  8                        
hh                  11                       
12312312312312312   9
msh />cat udisk0/t1
112233
msh />
```

### 设备模式 device mode

先使用msc_medium命令选择要模拟为U盘的块设备名称，然后将设备切换为U盘模式。
```
msh />list_device
device           type         ref count
-------- -------------------- ----------
udisk0   Block Device         1       
root_dev Block Device         1       
uart1    Character Device     2       
e0       Network Interface    0       
msh />msc_medium udisk0
msh />usbfs_msc
Change USB FS core MSC device
msh />
```

## 键盘功能 Keyboard feature

### 主机模式 host mode

插入后自动配置为boot模式，可以使用连接的键盘进行命令输入。

```
msh />RHUB HS:0      Connect
RHUB HS:0      Enable
DEV  HS.00     Allocate pipe, in = 0, out = 1
DEV  HS.01     Re-allocate pipe, in = 0, out = 1
DEV  HS.01     LS device, VID:1a2c PID:2124
ITF  HS.01.0-0 HID interface init
ITF  HS.01.0-0 Ep in 81
ITF  HS.01.1-1 HID interface init
ITF  HS.01.1-1 Ep in 82
DEV  HS.01     Device has 2 interfaces

msh />
```

### 设备模式 device mode

使用usbfs_kb命令切换为键盘模式，使用key命令模拟按键。

```
msh />usbfs_kb
Change USB FS core Keyboard device
msh />key a
msh />a
```

## 鼠标功能 Mouse feature

### 设备模式 device mode

使用usbfs_mouse命令切换为键盘模式，使用mouse_move命令模拟鼠标移动，使用mouse_down命令模拟点击。

```
msh />usbfs_mouse
Change USB FS core Mouse device
msh />mouse_move -10 0
```

## 虚拟串口与WinUSB功能 CDC ACM and WinUSB device feature

### 设备模式 device mode

使用usbfs_cdc/usbfs_winusb 命令切换，使用cdc_send/winusb_send命令发送数据。


## 虚拟网卡功能 CDC RNDIS feature

### 主机模式 host mode
接入rndis网卡后会自动将数据与协议栈进行关联。

```
msh />RHUB HS:0      Connect
RHUB HS:0      Enable
DEV  HS.00     Allocate pipe, in = 0, out = 1
DEV  HS.01     Re-allocate pipe, in = 0, out = 1
DEV  HS.01     FS device, VID:0483 PID:0031
ITF  HS.01.0-0 CDC RNDIS interface init
ITF  HS.01.0-0 CDC RNDIS Ep in 81, out 01
ITF  HS.01.0-0 CDC RNDIS Notify EP 83
DEV  HS.01     Device has 1 interfaces
ITF  HS.01.0-0 RNDIS Ver 1.0, Medium: 0, xfer size: 1568 x 1
ITF  HS.01.0-0 RNDIS Vendor TeenyUSB
ITF  HS.01.0-0 RNDIS MAC 00-80-e1-11-22-33
ITF  HS.01.0-0 RNDIS Set filter success
```

### 设备模式 device mode

使用usbfs_rndis命令切换，接入电脑后会显示rndis网卡，在浏览器中输入192.168.1.30可以看到一个简单的页面。


