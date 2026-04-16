#include "drivefoc.h"

/**
 * @brief   Drive Motor FOC Control caculate, use it in interrupt
 */
void drive_foc_calc(void)
{
    /************************************************************
     * @brief   FOC Caculate - Drive1
     */

    // Speed loop
    Drive1_speed_pi.ref = drive1_speed_ref;
    Drive1_speed_pi.fdb = Drive1_AD2S.Speed;
    PID_Calc(&Drive1_speed_pi, system_enable, SYSTEM_SAMPLE_TIME);

    // Current loop
    // current ABC-to-dq
    abc_2_dq(&Drive1_iabc, &Drive1_idq, Drive1_AD2S.Electrical_Angle);

    // (id=0 control)Current PI Controller
    // d-axis
    Drive1_id_pi.ref = 0;
    Drive1_id_pi.fdb = Drive1_idq.d;
    PID_Calc(&Drive1_id_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Drive1_udq.d = Drive1_id_pi.output;

    // q-axis
    Drive1_iq_pi.ref = Drive1_speed_pi.output;
    Drive1_iq_pi.fdb = Drive1_idq.q;
    PID_Calc(&Drive1_iq_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Drive1_udq.q = Drive1_iq_pi.output;

    /************************************************************
     * @brief   SVPWM
     */
    dq_2_abc(&Drive1_udq, &Drive1_uabc, Drive1_AD2S.Electrical_Angle);
    e_svpwm(&Drive1_uabc, 150, &Drive1_duty_abc);

    /************************************************************
     * @brief   FOC Caculate - Drive2
     */

    // current ABC-to-dq
    abc_2_dq(&Drive2_iabc, &Drive2_idq, Drive2_AD2S.Electrical_Angle);
}