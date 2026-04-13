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
        printf("I:%.4f,%.4f\r\n", Load2_iabc.a, Load2_iabc.b);
    }
}

/**
 * @brief   ADC Callback function
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 0); // Caculate running time

    /**********************************
     * @brief   Command update
     */
    Load1_iq_command.user_command = load1_iq_ref;
    Command_update(&Load1_iq_command);

    Load2_iq_command.user_command = load2_iq_ref;
    Command_update(&Load2_iq_command);

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
    load_foc_calc();

    /**********************************
     * @brief   Voltage-Source Inverter Control
     */
    if (system_enable == 0)
    {
        TIM8->CCR1 = 0;
        TIM8->CCR2 = 0;
        TIM8->CCR3 = 0;
        TIM1->CCR1 = 0;
        TIM1->CCR2 = 0;
        TIM1->CCR3 = 0;
    }
    else
    {
        TIM8->CCR1 = Load2_duty_abc.dutya * TIM8->ARR;
        TIM8->CCR2 = Load2_duty_abc.dutyb * TIM8->ARR;
        TIM8->CCR3 = Load2_duty_abc.dutyc * TIM8->ARR;
        TIM1->CCR1 = Load1_duty_abc.dutya * TIM1->ARR;
        TIM1->CCR2 = Load1_duty_abc.dutyb * TIM1->ARR;
        TIM1->CCR3 = Load1_duty_abc.dutyc * TIM1->ARR;
    }

    /**********************************
     * @brief   DAC Output
     */
    hw_dac_output();
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, system_dac_value1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, system_dac_value2);

    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 1); // Caculate running time
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

            TIM8->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE);
            MODIFY_REG(TIM8->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_FORCED_INACTIVE);
            MODIFY_REG(TIM8->CCMR1, TIM_CCMR1_OC2M, TIM_OCMODE_FORCED_INACTIVE << 8);
            MODIFY_REG(TIM8->CCMR2, TIM_CCMR2_OC3M, TIM_OCMODE_FORCED_INACTIVE);
            HAL_GPIO_WritePin(STOP_PIN_GPIO_Port, STOP_PIN_Pin, GPIO_PIN_RESET);
            system_enable = 0;
        }
    }
}
