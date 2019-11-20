## 如何去掉不需要的接口

1. 在 composite_desc.lua 删除不需要的接口描述符，然后使用 make desc 重新生成描述符.

2. 在 composite_device.c 的 device_interfaces 数组中，去掉不需要的接口，然后重新编译工程.


## How remove unused interface

1. remove unused interface descriptor in composite_desc.lua, use "make desc" generate descriptor.

2. remove unused interface structure in device_interfaces array in composite_device.c, remake the project.

