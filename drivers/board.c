/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-01     RealThread   first version
 */

#include <rtthread.h>
#include <board.h>
#include <drv_common.h>

RT_WEAK void rt_hw_board_init()
{
    extern void hw_board_init(char *clock_src, int32_t clock_src_freq, int32_t clock_target_freq);

    /* Heap initialization */
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *) HEAP_BEGIN, (void *) HEAP_END);
#endif

    hw_board_init(BSP_CLOCK_SOURCE, BSP_CLOCK_SOURCE_FREQ_MHZ, BSP_CLOCK_SYSTEM_FREQ_MHZ);

    /* Set the shell console output device */
#if defined(RT_USING_DEVICE) && defined(RT_USING_CONSOLE)
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Board underlying hardware initialization */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif


    GPIO_InitTypeDef GPIO_InitStruct = {0};


            __HAL_RCC_TIM1_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            /**TIM1 GPIO Configuration
            PA8     ------> TIM1_CH1
            */
            GPIO_InitStruct.Pin = GPIO_PIN_8;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            __HAL_AFIO_REMAP_TIM1_ENABLE();


            __HAL_RCC_TIM8_CLK_ENABLE();
            __HAL_RCC_GPIOC_CLK_ENABLE();
               /**TIM8 GPIO Configuration
               PC6     ------> TIM8_CH1
               */
               GPIO_InitStruct.Pin = GPIO_PIN_6;
               GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
               GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
               HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


               __HAL_RCC_TIM4_CLK_ENABLE();
               __HAL_RCC_GPIOB_CLK_ENABLE();
                   /**TIM4 GPIO Configuration
                   PB6     ------> TIM4_CH1
                   */
                   GPIO_InitStruct.Pin = GPIO_PIN_6;
                   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                   __HAL_AFIO_REMAP_TIM4_ENABLE();

                   __HAL_RCC_GPIOD_CLK_ENABLE();
                   __HAL_RCC_GPIOF_CLK_ENABLE();


                      __HAL_RCC_SPI2_CLK_ENABLE();

                      __HAL_RCC_GPIOB_CLK_ENABLE();
                      __HAL_RCC_GPIOG_CLK_ENABLE();
                      /**SPI2 GPIO Configuration
                      PB13     ------> SPI2_SCK
                      PB14     ------> SPI2_MISO
                      PB15     ------> SPI2_MOSI
                      */
                      GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
                      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                      GPIO_InitStruct.Pin = GPIO_PIN_14;
                      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                      GPIO_InitStruct.Pull = GPIO_NOPULL;
                      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



}
