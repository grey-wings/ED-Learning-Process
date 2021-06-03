
#include "ADF4351.h"
#include "math.h"
#include "spi.h"
///////////////////////////////////////////////////////////////////////////////////////////////
// 用户配置区域
void ADF4351_Spi_Transmit(adf4351* device, uint8_t data);
void ADF4351_LE_Reset();
void ADF4351_LE_Set();
void ADF4351_CE_Set();
void ADF4351_CE_Reset();

// 因为目前Framework, F7的引脚还没有支持
void ADF4351_LE_Reset(){
    /* USER CODE BEGIN */
    HAL_GPIO_WritePin(ADF4351_LE_GPIO_Port, ADF4351_LE_Pin, GPIO_PIN_RESET);
    /* USER CODE END */
}

void ADF4351_LE_Set(){
    /* USER CODE BEGIN */
    HAL_GPIO_WritePin(ADF4351_LE_GPIO_Port, ADF4351_LE_Pin, GPIO_PIN_SET);
    /* USER CODE END */
}

void ADF4351_CE_Reset(){
    /* USER CODE BEGIN */
    HAL_GPIO_WritePin(ADF4351_CE_GPIO_Port, ADF4351_CE_Pin, GPIO_PIN_RESET);
    /* USER CODE END */
}

void ADF4351_CE_Set(){
    /* USER CODE BEGIN */
    HAL_GPIO_WritePin(ADF4351_CE_GPIO_Port, ADF4351_CE_Pin, GPIO_PIN_SET);
    /* USER CODE END */
}

// 用户自己实现的spi发送,每次只写8位,(注意只负责写，不负责CS引脚的拉高拉低)
void ADF4351_Spi_Transmit(adf4351* device, uint8_t data){
    /* USER CODE BEGIN */
    HAL_SPI_Transmit(device->spi.Instance, &data, 1, 1000);
    /* USER CODE END */
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//ADF4351 VCO输出范围
#define VCO_MAX_FREQUENCY 4400.
#define VCO_MIN_FREQUENCY 2200.

//常用寄存器地址定义
#define REG_0	0
#define REG_1   1
#define REG_2	2
#define REG_3	3
#define REG_4   4
#define REG_5   5

/*=====================================函数定义================================================*/
uint32_t    ADF4351_Generate_Register(uint8_t reg_num, adf4351* device);
float       ADF4351_Get_Actual_Frequency(adf4351* device);
void        Method_ADF4351_Set_Output_Frequency(adf4351* device, float frequency);
void        Method_ADF4351_Write_Reg(adf4351* device, uint32_t word);
void        Method_ADF4351_Set_Aux_Output_Power(adf4351* device, adf4351_rf_output_power_type power);
void        Method_ADF4351_Set_Common_Param(adf4351* device, uint32_t reference_frequency, float channel_spacing);
/*======================================数学计算方法==============================================*/
/**
 * 辗转相除法求最大公约数
 */
uint32_t GCD(uint32_t a, uint32_t b) {
    return a == 0 ? b : GCD(b % a, a);
}
/*========================================ADF4351方法===========================================*/

/**
 * 通过配置值计算ADF4351参数值，这个函数不会验证配置是否合理，也不会修改配置的值，即使配置值是错误的
 * @
 * @return ADF4351参数结构体
 */
void ADF4351_Calculate_Parameter(adf4351* device) {
    // output frequency should be less than 4400MHz and grater than 35MHz, VCO frequency is 2200 to 4400
    if (device->output_frequency > VCO_MIN_FREQUENCY) {
        device->register4.output_divider = OUTPUT_DIVIDER_1;
    } else if (device->output_frequency > VCO_MIN_FREQUENCY / 2) {
        device->register4.output_divider = OUTPUT_DIVIDER_2;
    } else if (device->output_frequency > VCO_MIN_FREQUENCY / 4) {
        device->register4.output_divider = OUTPUT_DIVIDER_4;
    } else if (device->output_frequency > VCO_MIN_FREQUENCY / 8) {
        device->register4.output_divider = OUTPUT_DIVIDER_8;
    } else if (device->output_frequency > VCO_MIN_FREQUENCY / 16) {
        device->register4.output_divider = OUTPUT_DIVIDER_16;
    } else if (device->output_frequency > VCO_MIN_FREQUENCY / 32) {
        device->register4.output_divider = OUTPUT_DIVIDER_32;
    } else {
        device->register4.output_divider = OUTPUT_DIVIDER_64;
    }
    // PFD
    const float pfd_frequency = device->reference_frequency * (device->register2.ref_double + 1) / (device->register2.ref_div2 + 1) / device->register2.r_counter;
    // 反馈频率
    const float feedback_frequency = device->register4.feedback_select == FEEDBACK_SIGNAL_FUNDAMENTAL ? (device->output_frequency * (1 << device->register4.output_divider)) : device->output_frequency;
    // N=反馈频率/PFD
    const float N = feedback_frequency / pfd_frequency;
    device->register0.int_value = (uint16_t)N;
    // 四舍五入取整
    device->register1.mod_value = roundf(pfd_frequency * 1e3 / device->channel_spacing);
    // 保证MOD < 0xfff
    if (device->register1.mod_value > 0xfff) {
        device->register1.mod_value = 0xfff;
    }
    // 四舍五入取整
    device->register0.frac_value = round(device->register1.mod_value * (N - device->register0.int_value));
    // 约分
    const uint32_t gcd = GCD(device->register0.frac_value, device->register1.mod_value);
    device->register0.frac_value /= gcd;
    device->register1.mod_value /= gcd;

    // INT必须大于等于75(prescaler=8/9)或23(prescaler=4/5)
    if (device->register1.prescaler == PRESCALER_8_9 && device->register0.int_value < 75) {
        device->register0.int_value = 75;
    }
    if (device->register1.prescaler == PRESCALER_4_5 && device->register0.int_value < 23) {
        device->register0.int_value = 23;
    }
}

/**
 * 生成寄存器值
 * @param reg_num 寄存器号，0 ~ 5
 * @param device ADF4351配置结构体的指针
 * @return 寄存器值
 */
uint32_t ADF4351_Generate_Register(uint8_t reg_num, adf4351* device) {
    uint32_t value = reg_num;
    switch (reg_num) {
        case 0: {
            value |= device->register0.int_value << 15u;
            value |= device->register0.frac_value << 3u;
            break;
        }
        case 1: {
            value |= device->register1.phase_adjust << 28u;
            value |= device->register1.prescaler << 27u;
            value |= device->register1.phase_value<< 15u;
            value |= device->register1.mod_value << 3u;
            break;
        }
        case 2: {
            value |= device->register2.low_noise_spur_mode << 29u;
            value |= device->register2.muxout << 26u;
            value |= device->register2.ref_double << 25u;
            value |= device->register2.ref_div2 << 24u;
            value |= device->register2.r_counter << 14u;
            value |= device->register2.double_buffer << 13u;
            value |= device->register2.charge_pump_current << 9u;
            value |= device->register2.ldf << 8u;
            value |= device->register2.ldp << 7u;
            value |= device->register2.pd_polarity << 6u;
            value |= device->register2.power_down << 5u;
            value |= device->register2.cp_3_state << 4u;
            value |= device->register2.counter_reset << 3u;
            break;
        }
        case 3: {
            value |= device->register3.band_select_clock_mode << 23u;
            value |= device->register3.abp << 22u;
            value |= device->register3.charge_cancellation << 21u;
            value |= device->register3.csr << 18u;
            value |= device->register3.clock_div_mode << 15u;
            value |= device->register3.clock_divider_value << 3u;
            break;
        }
        case 4: {
            value |= device->register4.feedback_select << 23u;
            value |= device->register4.output_divider << 20u;
            value |= device->register4.band_select_clock_divider << 12u;
            value |= device->register4.vco_power_down << 11u;
            value |= device->register4.mtld << 10u;
            value |= device->register4.aux_output_select << 9u;
            value |= device->register4.aux_output_enable << 8u;
            value |= device->register4.aux_output_power << 6u;
            value |= device->register4.rf_output_enable << 5u;
            value |= device->register4.rf_output_power << 3u;
            break;
        }
        case 5: {
            value |= 0x180000u; // reserved
            value |= device->register5.ld_pin_mode << 22u;
            break;
        }
        default: {
            // unknow reg_num, set to 0
            reg_num = 0;
            break;
        }
    }
    return value;
}

/**
 * 获得真实频率
 * @param device ADF4351配置结构体的指针
 * @return 真实的频率，单位: MHz
 */
float ADF4351_Get_Actual_Frequency(adf4351* device) {
    float pfd_frequency = device->reference_frequency * (device->register2.ref_double + 1) / (device->register2.ref_div2 + 1);
    if (device->register2.r_counter) {
        pfd_frequency /= device->register2.r_counter;
    }
    return pfd_frequency * (device->register0.int_value + (float)device->register0.frac_value / device->register1.mod_value) / (1u << device->register4.output_divider);
}

/*==========================================================================================================**/
/**
 * 写寄存器
 * @param word 寄存器值，包含控制位
 */
void Method_ADF4351_Write_Reg(adf4351* device, uint32_t word) {
    // 一次写8位，从最高位开始写
//    printf("%x\r\n",word);
    ADF4351_LE_Reset();
    SystemTimer_Delay_Ms(5);
    ADF4351_Spi_Transmit(device, (0xff&(word>>24)));
    ADF4351_Spi_Transmit(device, (0xff&(word>>16)));
    ADF4351_Spi_Transmit(device, (0xff&(word>>8)));
    ADF4351_Spi_Transmit(device, (0xff&(word>>0)));
    SystemTimer_Delay_Ms(5);
    ADF4351_LE_Set();
    SystemTimer_Delay_Ms(5);
    ADF4351_LE_Reset();
    SystemTimer_Delay_Ms(5);
}

void Method_ADF4351_Set_Output_Frequency(adf4351* device, float frequency){
    device->output_frequency = frequency;
    ADF4351_Calculate_Parameter(device);
    printf("Actual output frequency: %lf\r\n", ADF4351_Get_Actual_Frequency(device));
    // 分别要写0、1、4寄存器
    device->Write_Reg(device, ADF4351_Generate_Register(REG_4, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_1, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_0, device));
}

void Method_ADF4351_Set_Common_Param(adf4351* device, uint32_t reference_frequency, float channel_spacing){
    device->reference_frequency = reference_frequency;
    device->channel_spacing     = channel_spacing;
}

void Method_ADF4351_Set_Aux_Output_Power(adf4351* device, adf4351_rf_output_power_type power){
    device->register4.rf_output_power = power;
    device->Write_Reg(device, ADF4351_Generate_Register(REG_4, device));
}

/*================================================================================================*/
void ADF4351_Prepare(adf4351* device, S_H_type mode, uint32_t value, void* reg0, void* reg1,
                     inst_type instance, gpio_type sclk, gpio_type mosi, gpio_type le, gpio_type ce){
//    device->spi.Instance        = instance;

    // 配置一些参数
    device->reference_frequency = 100;
    device->output_frequency    = 556;
    device->channel_spacing     = 100;

    _RO_WRITE(device->spi.Value, uint32_t, value);
    _RO_WRITE(device->spi.Register0, void*, reg0);
    _RO_WRITE(device->spi.Register1, void*, reg1);
    _RO_WRITE(device->spi.Instance, void*, instance);

    // Register 1
    device->register1.prescaler = PRESCALER_4_5;
    device->register1.phase_adjust = 0;
    device->register1.phase_value = 1;
    // Register 2
    device->register2.r_counter                     = 1;
    device->register2.ref_double                    = 0;
    device->register2.ref_div2                      = 1;
    device->register2.low_noise_spur_mode           = LOW_NOISE_MODE;
    device->register2.muxout                        = MUXOUT_ANALOG_LOCK;
    device->register2.double_buffer                 = 0;
    device->register2.charge_pump_current           = CHARGE_PUMP_CURRENT_2_50;
    device->register2.ldf                           = LDF_FRAC_N;
    device->register2.ldp                           = LDP_10ns;
    device->register2.pd_polarity                   = PD_POLARITY_POSTIVE;
    device->register2.power_down                    = 0;
    device->register2.cp_3_state                    = 0;
    device->register2.counter_reset                 = 0;
    // Register 3
    device->register3.band_select_clock_mode        = BAND_SELECT_CLOCK_MODE_LOW;
    device->register3.abp                           = ABP_6ns;
    device->register3.charge_cancellation           = 0;
    device->register3.csr                           = 0;
    device->register3.clock_div_mode                = CLOCK_DIV_MODE_CLOCK_DIVIDER_OFF;
    device->register3.clock_divider_value           = 150;
    //Register 4
    device->register4.feedback_select               = FEEDBACK_SIGNAL_FUNDAMENTAL;
    device->register4.vco_power_down                = 0;
    device->register4.mtld                          = 0;
    device->register4.aux_output_select             = AUX_OUTPUT_SELECT_DIVIDED;
    device->register4.aux_output_enable             = 0;
    device->register4.aux_output_power              = AUX_OUTPUT_POWER_neg_4;
    device->register4.rf_output_enable              = 1;
    device->register4.rf_output_power               = RF_OUTPUT_POWER_pos_5;
    device->register4.band_select_clock_divider     = 200;
    // Register 5
    device->register5.ld_pin_mode                   = LD_PIN_MODE_DIGITAL_LOCK_DETECT;
    // 方法
    device->Write_Reg               = Method_ADF4351_Write_Reg;
    device->Set_Output_Frequency    = Method_ADF4351_Set_Output_Frequency;
    device->Set_Common_Param        = Method_ADF4351_Set_Common_Param;
    device->Set_Aux_Output_Power    = Method_ADF4351_Set_Aux_Output_Power;

    // 计算出ADF4351的参数
    ADF4351_Calculate_Parameter(device);
    ADF4351_CE_Set();
    SystemTimer_Delay_Ms(5);
    // 写ADF4351寄存器
    device->Write_Reg(device, ADF4351_Generate_Register(REG_5, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_4, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_3, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_2, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_1, device));
    device->Write_Reg(device, ADF4351_Generate_Register(REG_0, device));
}



