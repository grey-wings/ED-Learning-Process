/***************************************************************************
*|------------------------------------------------------------------------
*|-------------------------------ADF4351驱动库------------------------------
****************************************************************************/
#ifndef BASEPROJECT_ADF4351_H
#define BASEPROJECT_ADF4351_H

#include <stdint.h>
#include "Framework.h"

/*============================================ADF4351寄存器定义=========================================================*/
/********************Register0**************************/
typedef struct{
    uint16_t int_value;     // 整数部分
    uint16_t frac_value;    // 小数部分
}adf4351_register_0;

/********************Register1**************************/
typedef enum {
    PRESCALER_4_5 = 0,
    PRESCALER_8_9
} adf4351_prescaler_type;

typedef struct{
    uint8_t                 phase_adjust;       // 相位调整，0: 关闭, 1: 开启
    adf4351_prescaler_type  prescaler;          // 预分频，0: 4/5, 1: 8/9
    uint16_t                phase_value;        // 相位值，0 ~ 4096
    uint16_t                mod_value;          // mode value
}adf4351_register_1;

/********************Register2**************************/
typedef enum {
    LOW_NOISE_MODE = 0,
    LOW_SPUR_MODE = 3
} adf4351_low_noise_spur_mode_type;

typedef enum {
    MUXOUT_3_STATE = 0,
    MUXOUT_DVDD,
    MUTOUT_DGND,
    MUXOUT_R_DIVIDER,
    MUXOUT_N_DIVIDER,
    MUXOUT_ANALOG_LOCK,
    MUXOUT_DIGITAL_LOCK
} adf4351_muxout_type;

typedef enum {
    CHARGE_PUMP_CURRENT_0_31 = 0,
    CHARGE_PUMP_CURRENT_0_63,
    CHARGE_PUMP_CURRENT_0_94,
    CHARGE_PUMP_CURRENT_1_25,
    CHARGE_PUMP_CURRENT_1_56,
    CHARGE_PUMP_CURRENT_1_88,
    CHARGE_PUMP_CURRENT_2_19,
    CHARGE_PUMP_CURRENT_2_50,
    CHARGE_PUMP_CURRENT_2_81,
    CHARGE_PUMP_CURRENT_3_13,
    CHARGE_PUMP_CURRENT_3_44,
    CHARGE_PUMP_CURRENT_3_75,
    CHARGE_PUMP_CURRENT_4_06,
    CHARGE_PUMP_CURRENT_4_38,
    CHARGE_PUMP_CURRENT_4_69,
    CHARGE_PUMP_CURRENT_5_00
} adf4351_charge_pump_current_type;

typedef enum {
    LDF_FRAC_N = 0,
    LDF_INT_N
} adf4351_ldf_type;

typedef enum {
    LDP_10ns = 0,
    LDP_6ns
} adf4351_ldp_type;

typedef enum {
    PD_POLARITY_NEGTIVE = 0,
    PD_POLARITY_POSTIVE
} adf4351_pd_polarity_type;

typedef struct{
    adf4351_low_noise_spur_mode_type    low_noise_spur_mode;    // 低噪声和低杂散模式
    adf4351_muxout_type                 muxout;                 // 多路复用
    uint8_t                             ref_double;             // 输入2倍频，0: 关闭, 1: 开启
    uint8_t                             ref_div2;               // 输入2分频，0: 关闭, 1: 开启
    uint16_t                            r_counter;              // 输入R分频，1 ~ 1023
    uint8_t                             double_buffer;          // 双缓冲，0: 关闭, 1: 开启
    adf4351_charge_pump_current_type    charge_pump_current;    // 电荷泵电流
    adf4351_ldf_type                    ldf;                    // 锁定检测功能，对于小数N分频模式，建议设为0;对于整数N分频模式，建议设为1
    adf4351_ldp_type                    ldp;                    // 锁定检测精度，对于小数N分频模式，建议设为0;对于整数N分频模式，建议设为1
    adf4351_pd_polarity_type            pd_polarity;            // 鉴相器极性
    uint8_t                             power_down;             // 关断，0: 正常工作，1: 关断
    uint8_t                             cp_3_state;             // 电荷泵三态，设置为1时，电荷泵进入三态模式。正常工作时，应设置为0
    uint8_t                             counter_reset;          // 分频器复位，当设为1时，RF频率合成器N分频器和R分频器处于复位状态。正常工作时，应设置为0
}adf4351_register_2;

/********************Register3**************************/
typedef enum {
    BAND_SELECT_CLOCK_MODE_LOW = 0,
    BAND_SELECT_CLOCK_MODE_HIGH
} adf4351_band_select_clock_mode_type;

typedef enum {
    ABP_6ns = 0,
    ABP_3ns
} adf4351_abp_type;

typedef enum {
    CLOCK_DIV_MODE_CLOCK_DIVIDER_OFF = 0,
    CLOCK_DIV_MODE_FAST_LOCK_ENABLE,
    CLOCK_DIV_MODE_RESYNC_ENABLE
} adf4351_clock_div_mode_type;

typedef struct{
    adf4351_band_select_clock_mode_type band_select_clock_mode;     // 频段选择时钟模式
    adf4351_abp_type                    abp;                        // 防反冲脉冲宽度，设为1时，PFD防反冲脉冲宽度为3ns，可改善整数N分频操作的相位噪声和杂散性能。对于小数N分频操作，不建议使用3ns设置
    uint8_t                             charge_cancellation;        // 电荷消除，设为1将使能电荷泵电荷消除功能，这可以降低整 数N分频模式下的PFD杂散。在小数N分频模式下，应设置为0
    uint8_t                             csr;                        // 周跳减少使能，0关闭，1开启
    adf4351_clock_div_mode_type         clock_div_mode;             // 时钟分频器模式
    uint16_t                            clock_divider_value;        // 时钟分频器值，0 ～ 4095
}adf4351_register_3;

/********************Register4**************************/
typedef enum {
    FEEDBACK_SIGNAL_DIVIDED = 0,
    FEEDBACK_SIGNAL_FUNDAMENTAL
} adf4351_feedback_select_type;

typedef enum {
    AUX_OUTPUT_SELECT_DIVIDED = 0,
    AUX_OUTPUT_SELECT_FUNDAMENTAL
} adf4351_aux_output_select_type;

typedef enum {
    AUX_OUTPUT_POWER_neg_4 = 0,
    AUX_OUTPUT_POWER_neg_1,
    AUX_OUTPUT_POWER_pos_2,
    AUX_OUTPUT_POWER_pos_5
} adf4351_aux_output_power_type;

typedef enum {
    RF_OUTPUT_POWER_neg_4 = 0,
    RF_OUTPUT_POWER_neg_1,
    RF_OUTPUT_POWER_pos_2,
    RF_OUTPUT_POWER_pos_5
} adf4351_rf_output_power_type;

typedef enum {
    OUTPUT_DIVIDER_1 = 0,
    OUTPUT_DIVIDER_2,
    OUTPUT_DIVIDER_4,
    OUTPUT_DIVIDER_8,
    OUTPUT_DIVIDER_16,
    OUTPUT_DIVIDER_32,
    OUTPUT_DIVIDER_64,
} adf4351_output_divider_type;

typedef struct{
    adf4351_feedback_select_type    feedback_select;            // 反馈选择，0: 分频后, 1: 原始
    uint8_t                         vco_power_down;             // VCO关断，0上电，1关断
    uint8_t                         mtld;                       // 静音至检测到锁定，如果设置为1，则切断RF输出级的电源电流，直到数字锁定检测电路检测到器件实现锁定为止
    adf4351_aux_output_select_type  aux_output_select;          // 辅助输出选择
    uint8_t                         aux_output_enable;          // 辅助输出使能，0禁用输出，1启用输出
    adf4351_aux_output_power_type   aux_output_power;           // 辅助输出功率
    uint8_t                         rf_output_enable;           // RF输出使能，0禁用输出，1启用输出
    adf4351_rf_output_power_type    rf_output_power;            // RF输出功率
    uint8_t                         band_select_clock_divider;  // 频段选择时钟分频器值，1 ~ 255，R分频器的输出默认用作频段选择逻辑时钟，但如果此值太大(>125 kHz)，则可以启用一个分频器，以将R分频器输出细分为较小的值
    adf4351_output_divider_type     output_divider;
}adf4351_register_4;

/********************Register5**************************/
typedef enum {
    LD_PIN_MODE_LOW = 0,
    LD_PIN_MODE_DIGITAL_LOCK_DETECT = 1,
    LD_PIN_MODE_HIGH = 3
} adf4351_ld_pin_mode_type;

typedef struct{
    adf4351_ld_pin_mode_type        ld_pin_mode;       // 锁定检测引脚工作方式
}adf4351_register_5;

/*=================================================ADF4351 Class======================================================*/
/*
 * ADF4351类
 * 因为ADF4351的寄存器里面东西实在太多了，所以单独设置了寄存器的结构体
 */
typedef struct adf4351_class{
    /*===================================成员变量===============================*/
    adf4351_register_0          register0;                  // 寄存器配置
    adf4351_register_1          register1;
    adf4351_register_2          register2;
    adf4351_register_3          register3;
    adf4351_register_4          register4;
    adf4351_register_5          register5;
    uint32_t                    reference_frequency;        // 输入频率, 单位: MHz
    float                       output_frequency;           // 输出频率，单位: MHz
    float                       channel_spacing;            // 频率间隔，单位: KHz(一般设置100k)

    ro      periperal                   spi;                        // ADF4351 SPI

    /*===================================方法==========================================*/
    /********************************************************************
	*功能：设置锁相环输出													*
	*参数：1.device：类的实例											    *
	*	   2.frequency：设置的输出频率										*
	*说明：无															    *
	*********************************************************************/
    void    (*Set_Output_Frequency)             (struct adf4351_class* device, float frequency);

    /********************************************************************
	*功能：设置增益值													    *
	*参数：1.device：类的实例											    *
	*	   2.reference_frequency:输入参考频率							    *
    *	    3.channel_spacing:计算的频率间隔(一般设置100k)                   *
	*说明：无															    *
	*********************************************************************/
    void    (*Set_Common_Param)                 (struct adf4351_class* device, uint32_t reference_frequency, float channel_spacing);

    /********************************************************************
    *功能：设置													        *
    *参数：1.device：类的实例											    *
    *	   2.power: 输出功率，分别有四种模式                                 *
    *	            (RF_OUTPUT_POWER_neg_4,RF_OUTPUT_POWER_neg_1,       *
    *	            RF_OUTPUT_POWER_pos_2,RF_OUTPUT_POWER_pos_5         *
    *说明：无															    *
    *********************************************************************/
    void    (*Set_Aux_Output_Power)             (struct adf4351_class* device, adf4351_rf_output_power_type power);

    /********************************************************************
    *功能：写寄存器													*
    *参数：1.device：类的实例											*
    *	   2.word: 需要写入的寄存器值（要求输入位32位的数）					*
    *说明：无															*
    *********************************************************************/
    void    (*Write_Reg)                        (struct adf4351_class*, uint32_t word);

} adf4351;


// 类定义
//typedef struct adf4351_class* adf4351;


/*================================================================================*/
/********************************************************************

*********************************************************************/
//void New_ADF4351(adf4351* device, uint16_t value, peripheral_register_type reg0, peripheral_register_type reg1,
//                 inst_type instance);
void ADF4351_Prepare(adf4351* device, S_H_type mode, uint32_t value, void* reg0, void* reg1,
                   inst_type instance, gpio_type sclk, gpio_type mosi, gpio_type le, gpio_type ce);

#endif //BASEPROJECT_ADF4351_H

