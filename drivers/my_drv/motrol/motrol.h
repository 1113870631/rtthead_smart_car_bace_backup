/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-03     111       the first version
 *
 * 电机控制底层驱动
 * L298N
 * in1
 * in2
 * in3
 * in4
 *
 *
 * pwm1   PA8
 *
 * pwm2   PC6
 *
 *
 *
 */
#ifndef DRIVERS_MY_DRV_MOTROL_MOTROL_H_
#define DRIVERS_MY_DRV_MOTROL_MOTROL_H_
#define MOTROL_FORHEAD 1
#define MOTROL_BACKWORD 2
#define MOTROL_STOP 0

/* MOTROL_FORHEAD  前进
 * MOTROL_BACKWORD 回退
 * MOTROL_STOP     刹车
 * speed 0~100
 * */
void set_motrol_pin(void);
int motrol_1_con(int dir,int speed);
int motrol_2_con(int dir,int speed);



#endif /* DRIVERS_MY_DRV_MOTROL_MOTROL_H_ */
