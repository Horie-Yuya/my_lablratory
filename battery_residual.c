/*
 * File:  battery_residual.c
 * Author:　堀江勇冶*
 * Created on 2013/03/26
 */


#include <xc.h>
#include <adc.h>
#include <timer.h>
#define VOLTAGE_ADC_SETTING 0x0268
#difine ANNOUNCEMENT_PORT

/*ADC_config*/
/* ********************************** */

unsignedint AD_config1 = ADC_MODULE_ON & ADC_IDLE_STOP & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON & ADC_SIMULTANEOUS & ADC_SAMP_OFF;

unsignedint AD_config2 = ADC_VREF_AVDD_AVSS & ADC_SCAN_ON & ADC_SELECT_CHAN_0 & ADC_SAMPLES_PER_INT_3 & ADC_ALT_BUF_OFF & ADC_ALT_INPUT_OFF;

unsignedint AD_config3 = ADC_SAMPLE_TIME_2 & ADC_CONV_CLK_SYSTEM & ADC_CONV_CLK_7Tcy;

unsignedint AD_configPORT = ENABLE_AN10_ANA & ENABLE_AN11_ANA & ENABLE_AN12_ANA;     /* 電源検知で1PORT追加　*/

unsignedint AD_configSCAN = SKIP_SCAN_AN0 & SKIP_SCAN_AN1 & SKIP_SCAN_AN2 & SKIP_SCAN_AN3 & SKIP_SCAN_AN4 &　SKIP_SCAN_AN5 & SKIP_SCAN_AN6 & SKIP_SCAN_AN7 & SKIP_SCAN_AN8 & SKIP_SCAN_AN9 & SKIP_SCAN_AN13 & SKIP_SCAN_AN14 & SKIP_SCAN_AN15;

unsignedint AD_configCHANNEL = ADC_CH0_POS_SAMPLEA_AN0 & ADC_CH0_NEG_SAMPLEA_NVREF;

/* ********************************** */

/*Timer3_config*/
/* ********************************** */

unsignedint T3config = T3_ON & GATE_OFF & T3_PS_1_256 & T3SOURCE_INT;

unsignedint SampleTime = 39063;                  /*(0.5秒)16ビットint型の最大値「65535」を超えないように！！*/

/* ********************************** */

/* グローバル関数 */
unsigned int buzzer_flag;
/* ************ */


int  battery_residual(void)
{
    unsignedint volatile_adc_value;
    /*
     ----------------------------
     ADCを呼び出して電圧を数値化
     分圧比をdspicの電源電圧（3.3{V}この電圧を超えないように（17{V}~14{V}）の範囲を分圧しA/D変換できるようにする）
     
     R1=2.2kΩ
     R2=470Ω                                       16進（4{mV}を１ビットとした場合）
     17{V}を印可したときR2の電圧は2.992{V}                             0x02EC
     14{V}を印可したときR2の電圧は2.464{V}                             0x0268
     ----------------------------
     */
    OpenADC1(config1,config2,config3,configPort,configScan);
    OpenTimer3(T3config,SampleTime);
    
    ConvertADC1();//ADC開始
    
    while(BusyADC1());//アクィジション時間
    
    volatile_adc_value = ReadADC1(0);
    
    if(volatile_adc_value < VOLTAGE_ADC_SETTING )
    {
        /*highとlowの分け方（timer3を使って)*/
        buzzer_flag = 1;
        
    }
    else{
        buzzer_flag = 0;
    }
}

void _ISR _T3Interrupt(void){
    
    unsigned int buzzer_count;
    
    IFS0bit.T3IF = 0;
    
    if(buzzer_flag == 1){
        
        if(buzzer_count % 2 == 1){
            
            ANNOUNCEMENT_PORT = 1;
        }
        else if(buzzer_count % 2 == 0 ){
            
            ANNOUNCEMENT_PORT = 0;
        }
        
    }
    
    buzzer_count++;
    
    
}

