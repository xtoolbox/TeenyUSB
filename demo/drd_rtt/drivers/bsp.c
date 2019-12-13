/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     ZYH          first implementation
 */

#include <rtthread.h>
#include "board.h"
//#include "drv_mpu.h"
#include <rthw.h>


void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

uint32_t HAL_GetTick(void)
{
    return rt_tick_get() * 1000 / RT_TICK_PER_SECOND;
}

void tusb_delay_ms(uint32_t Delay)
{
    rt_thread_mdelay(Delay);
}

void HAL_Delay(__IO uint32_t Delay)
{
    rt_thread_mdelay(Delay);
}

void HAL_SuspendTick(void)
{
    /* we should not suspend tick */
}

void HAL_ResumeTick(void)
{
    /* we should not resume tick */
}
#if defined(BSP_USING_SDRAM) && defined(RT_USING_MEMHEAP_AS_HEAP)
static struct rt_memheap system_heap;
#endif

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* Configure the MPU attributes as Write Through */
    //bsp_mpu_hw_init();

    /* Enable I-Cache-------------------------------------------------------------*/
    //rt_hw_cpu_icache_enable();

    /* Enable D-Cache-------------------------------------------------------------*/
    //rt_hw_cpu_dcache_enable();

    /* STM32F7xx HAL library initialization:
    - Configure the Flash ART accelerator on ITCM interface
    - Configure the Systick to generate an interrupt each 1 msec
    - Set NVIC Group Priority to 4
    - Global MSP (MCU Support Package) initialization
    */
    /* Configure the system clock @ 216 Mhz */

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);

#ifdef RT_USING_HEAP

#if defined(BSP_USING_SDRAM) && defined(RT_USING_MEMHEAP_AS_HEAP)
    bsp_sdram_hw_init();
    rt_system_heap_init((void *)SDRAM_BEGIN, (void *)SDRAM_END);
    rt_memheap_init(&system_heap, "sram", (void *)HEAP_BEGIN, HEAP_SIZE);
#else
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif

#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}
