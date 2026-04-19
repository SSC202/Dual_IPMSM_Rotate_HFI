#include "usermain.h"

/**
 * @brief   Init Program
 */
static void init(void)
{
    // User init
    user_init();
    // Hardware init
    hw_init();
}

/**
 * @brief   主函数
 */
void usermain(void)
{
    init();
    while (1)
    {
        // DAC Print
        // Usart Print
        switch (system_print)
        {
        case 0:
            printf("u: %.3f, %.3f\r\n", Drive1_hfi.theta_obs, Drive1_hfi.theta_true);
            break;
        case 1:
            printf("u: %.3f, %.3f\r\n", Drive1_hfi.speed_obs, Drive1_hfi.speed_true);
            break;
        case 2:
            printf("u: %.3f\r\n", Drive1_hfi.theta_true - Drive1_hfi.theta_obs);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief   ADC Callback function
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 0); // Caculate running time

    /**********************************
     * @brief   Command update
     */
    if (system_enable == 0)
    {
        Drive1_hfi.enable = 0;
    }

    /**********************************
     * @brief   Sample Calculate
     */
    hw_curr_sample(hadc);

    // Angle and Speed Sample
    AD2S1210_Angle_Get(); // Angle Sample
    AD2S1210_Speed_Get(); // Speed Sample
    /**********************************
     * @brief   FOC Calculate
     */
    drive_foc_calc();

    /**********************************
     * @brief   Voltage-Source Inverter Control
     */
    if (system_enable == 0)
    {
        TIM1->CCR1 = 0;
        TIM1->CCR2 = 0;
        TIM1->CCR3 = 0;
    }
    else
    {
        TIM1->CCR1 = Drive1_duty_abc.dutya * TIM1->ARR;
        TIM1->CCR2 = Drive1_duty_abc.dutyb * TIM1->ARR;
        TIM1->CCR3 = Drive1_duty_abc.dutyc * TIM1->ARR;
    }

    /**********************************
     * @brief   DAC Output
     */
    hw_dac_output();
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, system_dac_value1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, system_dac_value2);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 1); // Caculate running time
}

/**
 * @brief   Driver Interrput
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //
{
    if (GPIO_Pin == PROT_EXTI_Pin)
    {
        uint8_t cnt = 0;
        for (int i = 0; i < 5; i++)
        {
            if (HAL_GPIO_ReadPin(GPIOE, PROT_EXTI_Pin) == GPIO_PIN_RESET)
            {
                cnt++;
            }
            delay_us(10);
        }
        if (cnt >= 3)
        {
            TIM1->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE);
            MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_FORCED_INACTIVE);
            MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, TIM_OCMODE_FORCED_INACTIVE << 8);
            MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, TIM_OCMODE_FORCED_INACTIVE);

            system_enable = 0;
        }
    }
}
