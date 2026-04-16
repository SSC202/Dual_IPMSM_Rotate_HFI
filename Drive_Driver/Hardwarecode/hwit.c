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
        if (Drive1_curr.sample_flag == CURR_SAMPLE_GET_OFFSET)
        {
            adc_u1_offset_sum += hwadc_value[0];
            adc_v1_offset_sum += hwadc_value[1];
            adc_u2_offset_sum += hwadc_value[2];
            adc_v2_offset_sum += hwadc_value[3];
            adc_cnt++;
            if (adc_cnt == 20)
            {
                adc_cnt = 0;
                Drive1_curr.adc_val_u_offset = adc_u1_offset_sum / 20.0f;
                Drive1_curr.adc_val_v_offset = adc_v1_offset_sum / 20.0f;
                Drive1_curr.sample_flag = CURR_SAMPLE_RUNNING;
                adc_u1_offset_sum = 0;
                adc_v1_offset_sum = 0;
                Drive2_curr.adc_val_u_offset = adc_u2_offset_sum / 20.0f;
                Drive2_curr.adc_val_v_offset = adc_v2_offset_sum / 20.0f;
                Drive2_curr.sample_flag = CURR_SAMPLE_RUNNING;
                adc_u2_offset_sum = 0;
                adc_v2_offset_sum = 0;
            }
        }
        // Read Current
        else
        {
            Drive1_curr.adc_val_u = hwadc_value[0];
            Drive1_curr.adc_val_v = hwadc_value[1];
            Drive2_curr.adc_val_u = hwadc_value[2];
            Drive2_curr.adc_val_v = hwadc_value[3];
            adc_2_curr(&Drive1_curr);
            adc_2_curr(&Drive2_curr);
            Drive1_iabc.a = Drive1_curr.curr_u;
            Drive1_iabc.b = Drive1_curr.curr_v;
            Drive1_iabc.c = Drive1_curr.curr_w;
            Drive2_iabc.a = Drive2_curr.curr_u;
            Drive2_iabc.b = Drive2_curr.curr_v;
            Drive2_iabc.c = Drive2_curr.curr_w;
            // software protection
            if ((Drive1_iabc.a * Drive1_iabc.a > DRIVE1_MAX_CURRENT * DRIVE1_MAX_CURRENT) || (Drive1_iabc.b * Drive1_iabc.b > DRIVE1_MAX_CURRENT * DRIVE1_MAX_CURRENT) || (Drive1_iabc.c * Drive1_iabc.c > DRIVE1_MAX_CURRENT * DRIVE1_MAX_CURRENT) || (Drive2_iabc.a * Drive2_iabc.a > DRIVE2_MAX_CURRENT * DRIVE2_MAX_CURRENT) || (Drive2_iabc.b * Drive2_iabc.b > DRIVE2_MAX_CURRENT * DRIVE2_MAX_CURRENT) || (Drive2_iabc.c * Drive2_iabc.c > DRIVE2_MAX_CURRENT * DRIVE2_MAX_CURRENT))
            {
                TIM1->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE);
                MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_FORCED_INACTIVE);
                MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, TIM_OCMODE_FORCED_INACTIVE << 8);
                MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, TIM_OCMODE_FORCED_INACTIVE);

                HAL_GPIO_WritePin(STOP_PIN_GPIO_Port, STOP_PIN_Pin, GPIO_PIN_SET);
                system_enable = 0;
            }
        }
    }
}