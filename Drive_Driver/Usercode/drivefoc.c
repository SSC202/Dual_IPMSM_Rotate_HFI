#include "drivefoc.h"

/**
 * @brief   Drive Motor FOC Control caculate, use it in interrupt
 */
void drive_foc_calc(void)
{
    /**
     * @brief   HFI Caculate
     */
    // abc-to-alpha_beta
    abc_2_alphabeta(&Drive1_iabc, &Drive1_i_alpha_beta);

    // Current HPF
    Drive1_ialpha_lpf.input = Drive1_i_alpha_beta.alpha;
    Drive1_ibeta_lpf.input = Drive1_i_alpha_beta.beta;
    LPF_Calc(&Drive1_ialpha_lpf);
    LPF_Calc(&Drive1_ibeta_lpf);
    Drive1_hfi.i_alpha_betah_now.alpha = Drive1_i_alpha_beta.alpha - Drive1_ialpha_lpf.output;
    Drive1_hfi.i_alpha_betah_now.beta = Drive1_i_alpha_beta.beta - Drive1_ibeta_lpf.output;
    Drive1_hfi.theta_true = Drive1_AD2S.Electrical_Angle;
    Drive1_hfi.speed_true = Drive1_AD2S.Speed;
    HFI_Calc(&Drive1_hfi);

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
     * @brief   HFI Inject
     */
    HFI_Inject(&Drive1_hfi);
    Drive1_u_alpha_betah.alpha = Drive1_hfi.u_alpha_betah.alpha;
    Drive1_u_alpha_betah.beta = Drive1_hfi.u_alpha_betah.beta;
    alphabeta_2_abc(&Drive1_u_alpha_betah, &Drive1_u_abch);
    /************************************************************
     * @brief   SVPWM
     */
    dq_2_abc(&Drive1_udq, &Drive1_uabc, Drive1_AD2S.Electrical_Angle);
    Drive1_uabc.a = Drive1_u_abch.a + Drive1_uabc.a;
    Drive1_uabc.b = Drive1_u_abch.b + Drive1_uabc.b;
    Drive1_uabc.c = Drive1_u_abch.c + Drive1_uabc.c;
    e_svpwm(&Drive1_uabc, 150, &Drive1_duty_abc);


    /************************************************************
     * @brief   FOC Caculate - Drive2
     */
    // current ABC-to-dq
    abc_2_dq(&Drive2_iabc, &Drive2_idq, Drive2_AD2S.Electrical_Angle);
}