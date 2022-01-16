/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-02     111       the first version
 */
#ifndef _THEAD_CREAT_H_
#define _THEAD_CREAT_H_
#include <rtthread.h>


#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5


int thead_creat(void);


#endif /* APPLICATIONS_THEAD_CREAT_H_ */
