/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup.
 */

#include <rtthread.h>

#if defined(__GNUC__)
int entry(void);
int main_thread(void);
int main(void)
{
    static int xxx = 0;
    if(xxx == 0){
        xxx = 1;
        entry();
        return 0;
    }
    return main_thread();
}
#define main main_thread
#endif

int main(void)
{
    /* put user application code here */
    return 0;
}






