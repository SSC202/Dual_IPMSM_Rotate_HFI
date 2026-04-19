#include "userinit.h"

/**
 * @brief   Userinit
 */
void user_init(void)
{
    // 高频注入初始化
    LPF_Init(&Drive1_ialpha_lpf, 50, SYSTEM_SAMPLE_TIME);
    LPF_Init(&Drive1_ibeta_lpf, 50, SYSTEM_SAMPLE_TIME);
    HFI_Init(&Drive1_hfi, 20, SYSTEM_SAMPLE_TIME, 800, 800, 1);
}
