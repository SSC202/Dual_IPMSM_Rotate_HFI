#ifndef __USERPARA_H
#define __USERPARA_H

#include "coordinate_transform.h"
#include "svpwm.h"
#include "curr_sample.h"
#include "ad2s1210.h"
#include "hwdac.h"
#include "user_transfunc.h"
/** User include **/

/** End Include **/

/******************************************************
 * @brief   运行相关的参数,结合 CubeMX 进行定义
 */
#ifndef LOAD1_MAX_CURRENT
#define LOAD1_MAX_CURRENT 7.0f // 电流限制
#endif
#ifndef LOAD2_MAX_CURRENT
#define LOAD2_MAX_CURRENT 7.0f // 电流限制
#endif
#ifndef SYSTEM_SAMPLE_TIME
#define SYSTEM_SAMPLE_TIME 0.0001f // 系统采样时间,根据 CubeMX 配置
#endif

extern uint8_t system_enable;      // 系统使能参数,0为失能,1为使能
extern uint8_t system_print;       // 参数打印,用户自行定义串口打印的变量
extern uint16_t system_dac_value1; // DAC Channel1 示波器打印变量,用户自行赋值
extern uint16_t system_dac_value2; // DAC Channel1 示波器打印变量,用户自行赋值
extern uint16_t hwdac_value1;      // 外部DAC1通道值
extern uint16_t hwdac_value2;      // 外部DAC2通道值
extern uint16_t hwdac_value3;      // 外部DAC3通道值
extern uint16_t hwdac_value4;      // 外部DAC4通道值

extern uint16_t hwadc_value[4] __attribute__((section(".ARM.__at_0x24000000")));

/******************************************************
 * @brief   临时变量
 */

/******************************************************
 * @brief   负载电机1相关定义
 * @note    负载电机1变量命名使用 Load1_xxx 格式, 结构体使用 Load1, 单变量使用 load1
 */

// 电流环 PID 相关定义
extern PID_t Load1_id_pi; // 负载电机1 d轴电流PI控制器结构体
extern PID_t Load1_iq_pi; // 负载电机1 q轴电流PI控制器结构体

// SVPWM 输出相关定义
extern dq_t Load1_udq;            // 负载电机1 dq 轴指令电压
extern abc_t Load1_uabc;          // 负载电机1 ABC 相指令电压
extern duty_abc_t Load1_duty_abc; // 负载电机1 ABC 相占空比值

// 电流采样相关定义
extern curr_sample_t Load1_curr; // 负载电机1采样电流相关结构体

extern abc_t Load1_iabc; // 负载电机1 ABC 相电流(原始值)
extern dq_t Load1_idq;   // 负载电机1 dq 轴电流(原始值)

// 速度/位置采样相关定义
extern ad2s1210_t Load1_AD2S; // 负载电机1的旋变解码板结构体

extern float load1_iq_ref;         // 电流指令值
extern Command_t Load1_iq_command; // 电流指令结构体

/******************************************************
 * @brief   负载电机2相关定义
 * @note    负载电机2变量命名使用 Load2_xxx 格式, 结构体使用 Load2, 单变量使用 load2
 */

// 电流环 PID 相关定义
extern PID_t Load2_id_pi; // 负载电机2 d轴电流PI控制器结构体
extern PID_t Load2_iq_pi; // 负载电机2 q轴电流PI控制器结构体

// SVPWM 输出相关定义
extern dq_t Load2_udq;            // 负载电机2 dq 轴指令电压
extern abc_t Load2_uabc;          // 负载电机2 ABC 相指令电压
extern duty_abc_t Load2_duty_abc; // 负载电机2 ABC 相占空比值

// 电流采样相关定义
extern curr_sample_t Load2_curr; // 负载电机2采样电流相关结构体

extern abc_t Load2_iabc; // 负载电机2 ABC 相电流(原始值)
extern dq_t Load2_idq;   // 负载电机2 dq 轴电流(原始值)

// 位置采样相关定义
extern ad2s1210_t Load2_AD2S; // 负载电机2的旋变解码板结构体

extern float load2_iq_ref;         // 电流指令值
extern Command_t Load2_iq_command; // 电流指令结构体

/****************************************
 * @brief   以下为无感算法变量相关定义,用户自行定义相关全局变量
 */

#endif
