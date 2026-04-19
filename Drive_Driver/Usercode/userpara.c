#include "userpara.h"

/******************************************************
 * @brief   运行相关的参数,结合 CubeMX 进行定义
 */

#define DRIVE1_MAX_CURRENT 7.0f    // 负载电机1电流限制
#define DRIVE2_MAX_CURRENT 7.0f    // 负载电机2电流限制
#define SYSTEM_SAMPLE_TIME 0.0001f // 系统采样时间,根据 CubeMX 配置

uint8_t system_enable;        // 系统使能参数,0为失能,1为使能
uint8_t system_print;         // 参数打印,用户自行定义串口打印的变量
uint16_t system_dac_value1;   // DAC Channel1 示波器打印变量,用户自行赋值
uint16_t system_dac_value2;   // DAC Channel2 示波器打印变量,用户自行赋值
extern uint16_t hwdac_value1; // 外部DAC1通道值
extern uint16_t hwdac_value2; // 外部DAC2通道值
extern uint16_t hwdac_value3; // 外部DAC3通道值
extern uint16_t hwdac_value4; // 外部DAC4通道值

uint16_t hwadc_value[4] __attribute__((section(".ARM.__at_0x24000000")));

/******************************************************
 * @brief   临时变量
 */

/******************************************************
 * @brief   驱动电机1相关定义
 * @note    驱动电机1变量命名使用 Drive1_xxx 格式, 结构体使用 Drive1, 单变量使用 drive1
 */
// 速度环 PID 相关定义
PID_t Drive1_speed_pi; // 驱动电机速度PI控制器结构体

// 电流环 PID 相关定义
PID_t Drive1_id_pi; // 驱动电机1 d轴电流PI控制器结构体
PID_t Drive1_iq_pi; // 驱动电机1 q轴电流PI控制器结构体

// SVPWM 输出相关定义
dq_t Drive1_udq;            // 驱动电机1 dq 轴指令电压
abc_t Drive1_uabc;          // 驱动电机1 ABC 相指令电压
duty_abc_t Drive1_duty_abc; // 驱动电机1 ABC 相占空比值

// 电流采样相关定义
curr_sample_t Drive1_curr; // 驱动电机1采样电流相关结构体

abc_t Drive1_iabc; // 驱动电机1 ABC 相电流(原始值)
dq_t Drive1_idq;   // 驱动电机1 dq 轴电流(原始值)

// 速度/位置采样相关定义
extern ad2s1210_t Drive1_AD2S; // 负载电机1的旋变解码板结构体

float drive1_speed_ref; // 速度指令值

/******************************************************
 * @brief   驱动电机2相关定义
 * @note    驱动电机2变量命名使用 Drive2_xxx 格式, 结构体使用 Drive2, 单变量使用 drive2
 */

// 电流采样相关定义
curr_sample_t Drive2_curr; // 驱动电机2采样电流相关结构体

abc_t Drive2_iabc; // 负载电机2 ABC 相电流(原始值)
dq_t Drive2_idq;   // 负载电机2 dq 轴电流(原始值)

// 位置采样相关定义
extern ad2s1210_t Drive2_AD2S; // 负载电机2的旋变解码板结构体

/****************************************
 * @brief   以下为无感算法变量相关定义,用户自行定义相关全局变量
 */

HFI_t Drive1_hfi;                 // 旋转正弦注入结构体
alpha_beta_t Drive1_i_alpha_beta; // alpha-beta 轴电流
LPF_t Drive1_ialpha_lpf;          // alpah 轴电流低通滤波器
LPF_t Drive1_ibeta_lpf;           // beta 轴电流低通滤波器

alpha_beta_t Drive1_u_alpha_betah; // alpha-beta 轴高频指令电压
abc_t Drive1_u_abch;               // ABC 相高频指令电压