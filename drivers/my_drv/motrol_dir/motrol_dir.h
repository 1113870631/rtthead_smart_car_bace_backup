/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-04     111       the first version
 */
#ifndef DRIVERS_MY_DRV_MOTROL_DIR_MOTROL_DIR_H_
#define DRIVERS_MY_DRV_MOTROL_DIR_MOTROL_DIR_H_
/*pwm3 TIM4 PB6
 * 20000000 20ms 为周期
 * 1500000 为高电平为直行
 *dir 0~180
 * */

/*方向复位*/
int dir_init(void* device);
/*设置方向 设置pwm占空比*/
int ch_dir(double angle ,int speed,void* device);
/*获取占空比 获取方向*/
int get_dir(void);

#endif /* DRIVERS_MY_DRV_MOTROL_DIR_MOTROL_DIR_H_ */
