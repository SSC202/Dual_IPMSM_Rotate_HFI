#include "hwinit.h"

/**
 * @brief   Hardware Init
 */
void hw_init(void)
{
    // Wait
    HAL_Delay(50);
    // Init AD2S1210
    AD2S1210_Init();
    // system init
    system_enable = 0;
    // Init FOC parameters
    // Drive1 motor
    Drive1_curr.adc_val_u_offset = 0;
    Drive1_curr.adc_val_v_offset = 0;
    Drive1_curr.sample_flag = CURR_SAMPLE_GET_OFFSET; // Read ADC Offset
    PID_init(&Drive1_id_pi, 6, 500, 0, 86);           // Current PI Init
    PID_init(&Drive1_iq_pi, 6, 500, 0, 86);
    PID_init(&Drive1_speed_pi, 0.075, 0.15, 0, 6.2); // Speed PI Init
    // Drive2 motor
    Drive2_curr.adc_val_u_offset = 0;
    Drive2_curr.adc_val_v_offset = 0;
    Drive2_curr.sample_flag = CURR_SAMPLE_GET_OFFSET; // Read ADC Offset
    // 1us Timer
    HAL_TIM_Base_Start(&htim7);
    // Current Sample
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
    HAL_TIM_Base_Start(&htim1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)hwadc_value, 4);
    // Wait for Read ADC Offset
    while (Drive1_curr.sample_flag == CURR_SAMPLE_GET_OFFSET || Drive2_curr.sample_flag == CURR_SAMPLE_GET_OFFSET)
    {
        ;
    }
    // DAC Init
    HAL_DAC_Start(&hdac1, DAC1_CHANNEL_1);
    AD5676_init();
    // Open Inverter
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}
