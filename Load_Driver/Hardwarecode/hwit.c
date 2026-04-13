#include "hwit.h"

void hw_curr_sample(ADC_HandleTypeDef *hadc)
{
    static int adc_cnt = 0;
    static uint32_t adc_u1_offset_sum = 0;
    static uint32_t adc_v1_offset_sum = 0;
    static uint32_t adc_u2_offset_sum = 0;
    static uint32_t adc_v2_offset_sum = 0;
    UNUSED(hadc);
    /**********************************
     * @brief   Sample
     */
    if (hadc == &hadc1)
    {
        // Read Offset
        if (Load1_curr.sample_flag == CURR_SAMPLE_GET_OFFSET)
        {
            adc_u1_offset_sum += hwadc_value[0];
            adc_v1_offset_sum += hwadc_value[1];
            adc_u2_offset_sum += hwadc_value[2];
            adc_v2_offset_sum += hwadc_value[3];
            adc_cnt++;
            if (adc_cnt == 20)
            {
                adc_cnt = 0;
                Load1_curr.adc_val_u_offset = adc_u1_offset_sum / 20.0f;
                Load1_curr.adc_val_v_offset = adc_v1_offset_sum / 20.0f;
                Load1_curr.sample_flag = CURR_SAMPLE_RUNNING;
                adc_u1_offset_sum = 0;
                adc_v1_offset_sum = 0;
                Load2_curr.adc_val_u_offset = adc_u2_offset_sum / 20.0f;
                Load2_curr.adc_val_v_offset = adc_v2_offset_sum / 20.0f;
                Load2_curr.sample_flag = CURR_SAMPLE_RUNNING;
                adc_u2_offset_sum = 0;
                adc_v2_offset_sum = 0;
            }
        }
        // Read Current
        else
        {
            Load1_curr.adc_val_u = hwadc_value[0];
            Load1_curr.adc_val_v = hwadc_value[1];
            Load2_curr.adc_val_u = hwadc_value[2];
            Load2_curr.adc_val_v = hwadc_value[3];
            adc_2_curr(&Load1_curr);
            adc_2_curr(&Load2_curr);
            Load1_iabc.a = Load1_curr.curr_u;
            Load1_iabc.b = Load1_curr.curr_v;
            Load1_iabc.c = Load1_curr.curr_w;
            Load2_iabc.a = Load2_curr.curr_u;
            Load2_iabc.b = Load2_curr.curr_v;
            Load2_iabc.c = Load2_curr.curr_w;
            // software protection
            if ((Load1_iabc.a * Load1_iabc.a > LOAD1_MAX_CURRENT * LOAD1_MAX_CURRENT) || (Load1_iabc.b * Load1_iabc.b > LOAD1_MAX_CURRENT * LOAD1_MAX_CURRENT) || (Load1_iabc.c * Load1_iabc.c > LOAD1_MAX_CURRENT * LOAD1_MAX_CURRENT) || (Load2_iabc.a * Load2_iabc.a > LOAD2_MAX_CURRENT * LOAD2_MAX_CURRENT) || (Load2_iabc.b * Load2_iabc.b > LOAD2_MAX_CURRENT * LOAD2_MAX_CURRENT) || (Load2_iabc.c * Load2_iabc.c > LOAD2_MAX_CURRENT * LOAD2_MAX_CURRENT))
            {
                TIM1->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE);
                MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_FORCED_INACTIVE);
                MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, TIM_OCMODE_FORCED_INACTIVE << 8);
                MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, TIM_OCMODE_FORCED_INACTIVE);

                TIM8->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE);
                MODIFY_REG(TIM8->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_FORCED_INACTIVE);
                MODIFY_REG(TIM8->CCMR1, TIM_CCMR1_OC2M, TIM_OCMODE_FORCED_INACTIVE << 8);
                MODIFY_REG(TIM8->CCMR2, TIM_CCMR2_OC3M, TIM_OCMODE_FORCED_INACTIVE);

                HAL_GPIO_WritePin(STOP_PIN_GPIO_Port, STOP_PIN_Pin, GPIO_PIN_SET);
                system_enable = 0;
            }
        }
    }
}