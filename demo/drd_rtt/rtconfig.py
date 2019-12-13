import os
from SCons.Script import *

# toolchains options
ARCH='arm'
CPU='cortex-m7'
FPU=' -mfpu=fpv5-d16 -mfloat-abi=hard'
CROSS_TOOL='gcc'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')
if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')

try:
    AddOption('--demo',
            dest = 'demo',
            type = 'string',
            help = 'set demo target: f107/f407/f723')
except:
    x=1
DEMO_TARGET = GetOption('demo')
if not DEMO_TARGET:
    DEMO_TARGET='f723'

BOARD_PATH='../boards/stm32f723e_disco/'
LD_FILE="STM32F723IEKx_FLASH.ld"
BOARD_DEFINES=['USE_HAL_DRIVER','STM32F723xx','HSE_VALUE=25000000']
BOARD_FILES='''
../boards/stm32f723e_disco/system_stm32f7xx_HSE25MHz.c
../boards/stm32f723e_disco/board.c
'''
START_UP_FILE={
    'gcc':'../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f723xx.s',
    'keil':'../boards/stm32f723e_disco/startup_stm32f723xx.s',
}
if DEMO_TARGET=='f723':
    BOARD_PATH='../boards/stm32f723e_disco/'
    LD_NAME="STM32F723IEKx_FLASH.ld"
    CPU='cortex-m7'
    FPU=' -mfpu=fpv5-d16 -mfloat-abi=hard'
    BOARD_DEFINES=['USE_HAL_DRIVER', 'STM32F723xx', 'HSE_VALUE=25000000']
    BOARD_HEADER=[BOARD_PATH, '../../mcu_lib/st_driver/STM32F7xx_HAL_Driver/Inc', '../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F7xx/Include']
    BOARD_FILES='''
../boards/stm32f723e_disco/system_stm32f7xx_HSE25MHz.c
../boards/stm32f723e_disco/board.c
'''
    START_UP_FILE={
        'gcc':'../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f723xx.s',
        'keil':'../boards/stm32f723e_disco/startup_stm32f723xx.s',
    }
elif DEMO_TARGET=='f407':
    BOARD_PATH='../boards/stm32f407_evk/'
    LD_NAME="STM32F407VGTx_FLASH.ld"
    CPU='cortex-m4'
    FPU=''
    BOARD_DEFINES=['USE_HAL_DRIVER', 'STM32F407xx', 'HSE_VALUE=8000000']
    BOARD_HEADER=[BOARD_PATH, '../../mcu_lib/st_driver/STM32F4xx_HAL_Driver/Inc', '../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F4xx/Include']
    BOARD_FILES='''
../boards/stm32f407_evk/system_stm32f4xx.c
../boards/stm32f407_evk/board.c
../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s
'''
    START_UP_FILE={
    'gcc':'../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s',
    'keil':'../boards/stm32f407_evk/startup_stm32f407xx.s',
    }
elif DEMO_TARGET=='f107':
    BOARD_PATH='../boards/stm32f107vc/'
    LD_NAME="STM32F107VCTx_FLASH.ld"
    FPU=''
    CPU='cortex-m3'
    BOARD_DEFINES=['USE_HAL_DRIVER', 'STM32F107xC', 'HSE_VALUE=25000000']
    BOARD_HEADER=[BOARD_PATH, '../../mcu_lib/st_driver/STM32F1xx_HAL_Driver/Inc', '../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F1xx/Include']
    BOARD_FILES='''
../boards/stm32f107vc/system_stm32f1xx.c
../boards/stm32f107vc/board.c
'''
    START_UP_FILE={
    'gcc':'../../mcu_lib/st_driver/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f107xc.s',
    'keil':'../boards/stm32f107vc/startup_stm32f107xc.s',
    }
else:
    print("Unknown target: " + str(DEMO_TARGET))
    exit(-1)

Export('BOARD_HEADER')
Export('BOARD_FILES')
Export('BOARD_DEFINES')
Export('START_UP_FILE')

LD_FILE=BOARD_PATH+LD_NAME

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if  CROSS_TOOL == 'gcc':
    PLATFORM 	= 'gcc'
    EXEC_PATH 	= '/usr/local/Cellar/arm-none-eabi-gcc/7-2017-q4-major/gcc/bin/'
elif CROSS_TOOL == 'keil':
    PLATFORM 	= 'armcc'
    EXEC_PATH 	= r'C:/Keil_v5'
elif CROSS_TOOL == 'iar':
	PLATFORM 	= 'iar'
	EXEC_PATH 	= r'C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.0'

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC = PREFIX + 'gcc'
    CXX = PREFIX + 'g++'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    LINK = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'
    STRIP = PREFIX + 'strip'

    DEVICE = ' -mcpu=' + CPU + ' -mthumb -ffunction-sections -fdata-sections' + FPU
    CFLAGS = DEVICE + ' -std=c99 -g -Wall'
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -Wa,-mimplicit-it=thumb '
    LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler,-eentry -T '+LD_FILE

    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2 -Os'

    POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
    
    # module setting 
    CXXFLAGS = ' -Woverloaded-virtual -fno-exceptions -fno-rtti '
    M_CFLAGS = CFLAGS + ' -mlong-calls -fPIC '
    M_CXXFLAGS = CXXFLAGS + ' -mlong-calls -fPIC'
    M_LFLAGS = DEVICE + CXXFLAGS + ' -Wl,--gc-sections,-z,max-page-size=0x4' +\
                                    ' -shared -fPIC -nostartfiles -static-libgcc'
    M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'

elif PLATFORM == 'armcc':
    # toolchains
    CC = 'armcc'
    CXX = 'armcc'
    AS = 'armasm'
    AR = 'armar'
    LINK = 'armlink'
    TARGET_EXT = 'axf'

    DEVICE = ' --cpu Cortex-M7.fp.sp --fpu=FPv4-SP'
    CFLAGS = DEVICE + ' --apcs=interwork '
    AFLAGS = DEVICE
    LFLAGS = DEVICE + ' --info sizes --info totals --info unused --info veneers --list rtthread.map --scatter rtthread.sct'

    CFLAGS += ' -I' + EXEC_PATH + '/ARM/ARMCC/INC'
    LFLAGS += ' --libpath "' + EXEC_PATH + '/ARM/ARMCC/lib"'

    EXEC_PATH += '/arm/bin40/'

    if BUILD == 'debug':
        CFLAGS += ' -g -O0'
        AFLAGS += ' -g'
    else:
        CFLAGS += ' -O2 -Otime'

    CXXFLAGS = CFLAGS
    POST_ACTION = 'fromelf --bin $TARGET --output rtthread.bin \nfromelf -z $TARGET'

elif PLATFORM == 'iar':
    # toolchains
    CC = 'iccarm'
    CXX = 'iccarm'
    AS = 'iasmarm'
    AR = 'iarchive'
    LINK = 'ilinkarm'
    TARGET_EXT = 'out'

    DEVICE = ''

    CFLAGS = DEVICE
    CFLAGS += ' --diag_suppress Pa050'
    CFLAGS += ' --no_cse' 
    CFLAGS += ' --no_unroll' 
    CFLAGS += ' --no_inline' 
    CFLAGS += ' --no_code_motion' 
    CFLAGS += ' --no_tbaa' 
    CFLAGS += ' --no_clustering' 
    CFLAGS += ' --no_scheduling' 
    CFLAGS += ' --debug' 
    CFLAGS += ' --endian=little' 
    CFLAGS += ' --cpu=Cortex-M7' 
    CFLAGS += ' -e' 
    CFLAGS += ' --fpu=None'
    CFLAGS += ' --dlib_config "' + EXEC_PATH + '/arm/INC/c/DLib_Config_Normal.h"'    
    CFLAGS += ' -Ol'    
    CFLAGS += ' --use_c++_inline'
    CFLAGS += ' --silent'
    
    AFLAGS = ''
    AFLAGS += ' -s+' 
    AFLAGS += ' -w+' 
    AFLAGS += ' -r' 
    AFLAGS += ' --cpu Cortex-M7' 
    AFLAGS += ' --fpu None' 
    AFLAGS += ' -S' 
    
    LFLAGS = ' --config rtthread.icf'
    LFLAGS += ' --redirect _Printf=_PrintfTiny' 
    LFLAGS += ' --redirect _Scanf=_ScanfSmall' 
    LFLAGS += ' --entry __iar_program_start'    
    LFLAGS += ' --silent'

    CXXFLAGS = CFLAGS

    EXEC_PATH = EXEC_PATH + '/arm/bin/'
    POST_ACTION = ''
