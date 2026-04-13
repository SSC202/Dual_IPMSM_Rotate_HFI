#include "loadfoc.h"

/**
 * @brief   Load Motor FOC Control caculate, use it in interrupt
 */
void load_foc_calc(void)
{
    /************************************************************
     * @brief   FOC Caculate - Load1
     */

    // Current loop
    // current ABC-to-dq
    abc_2_dq(&Load1_iabc, &Load1_idq, Load1_AD2S.Electrical_Angle);

    // (id=0 control)Current PI Controller
    // d-axis
    Load1_id_pi.ref = 0;
    Load1_id_pi.fdb = Load1_idq.d;
    PID_Calc(&Load1_id_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Load1_udq.d = Load1_id_pi.output;

    // q-axis
    Load1_iq_pi.ref = Load1_iq_command.true_command;
    Load1_iq_pi.fdb = Load1_idq.q;
    PID_Calc(&Load1_iq_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Load1_udq.q = Load1_iq_pi.output;

    /************************************************************
     * @brief   SVPWM
     */
    dq_2_abc(&Load1_udq, &Load1_uabc, Load1_AD2S.Electrical_Angle);
    e_svpwm(&Load1_uabc, 150, &Load1_duty_abc);

    /************************************************************
     * @brief   FOC Caculate - Load2
     */

    // Current loop
    // current ABC-to-dq
    abc_2_dq(&Load2_iabc, &Load2_idq, Load2_AD2S.Electrical_Angle);

    // (id=0 control)Current PI Controller
    // d-axis
    Load2_id_pi.ref = 0;
    Load2_id_pi.fdb = Load2_idq.d;
    PID_Calc(&Load2_id_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Load2_udq.d = Load2_id_pi.output;

    // q-axis
    Load2_iq_pi.ref = Load2_iq_command.true_command;
    Load2_iq_pi.fdb = Load2_idq.q;
    PID_Calc(&Load2_iq_pi, system_enable, SYSTEM_SAMPLE_TIME);
    Load2_udq.q = Load2_iq_pi.output;

    /************************************************************
     * @brief   SVPWM
     */
    dq_2_abc(&Load2_udq, &Load2_uabc, Load2_AD2S.Electrical_Angle);
    e_svpwm(&Load2_uabc, 150, &Load2_duty_abc);
}