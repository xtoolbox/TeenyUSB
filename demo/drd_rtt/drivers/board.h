/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */
#ifndef __BOARD_H__
#define __BOARD_H__
#include "teeny_usb.h"
#include "board_config.h"

/* whether use board external SDRAM memory */
//#define EXT_SDRAM_BEGIN    (0xC0000000)
//#define EXT_SDRAM_SIZE     (0x800000)
//#define EXT_SDRAM_END      (EXT_SDRAM_BEGIN + EXT_SDRAM_SIZE)

#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define HEAP_BEGIN    (__segment_end("HEAP"))
#elif defined(__GNUC__)
extern int __bss_end__;
#define HEAP_BEGIN    (&__bss_end__)
#else
extern int __bss_end;
#define HEAP_BEGIN    (&__bss_end)
#endif

#if defined(STM32F723xx)
#define STM32_SRAM_SIZE   (256)
#define HEAP_END          (0x20000000 + STM32_SRAM_SIZE * 1024)
#define HEAP_SIZE         (HEAP_END - (rt_uint32_t)HEAP_BEGIN)
#elif defined(STM32F407xx)
#define STM32_SRAM_SIZE   (128)
#define HEAP_END          (0x20000000 + STM32_SRAM_SIZE * 1024)
#define HEAP_SIZE         (HEAP_END - (rt_uint32_t)HEAP_BEGIN)
#elif defined(STM32F107xC)
#define STM32_SRAM_SIZE   (64)
#define HEAP_END          (0x20000000 + STM32_SRAM_SIZE * 1024)
#define HEAP_SIZE         (HEAP_END - (rt_uint32_t)HEAP_BEGIN)
#else
#error unknown board
#endif

extern void rt_hw_board_init(void);
#endif

