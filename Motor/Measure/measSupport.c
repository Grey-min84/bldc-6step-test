#include "measSupport.h"



/**
  * @brief 특정 채널의 모든 샘플을 추출
  * @param channel: 채널 번호 (0~7)
  * @param output: 출력 버퍼 (크기: SAMPLES_PER_CHANNEL)
  */
void Extract_Channel_Data(uint16_t* adcBuf, uint16_t *output, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels)
{
    //if(channel >= NUM_CHANNELS) return;
    
    for(int i = 0; i < usSamplesPerCh; i++)
    {
        output[i] = adcBuf[channel + (i * usNumChannels)];
    }
}
    
/**
  * @brief 특정 채널의 평균값 계산
  * @param channel: 채널 번호 (0~7)
  * @retval 평균값
  */
float Calculate_Average_float(uint16_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels)
{
    //if(channel >= NUM_CHANNELS) return 0.0f;
    
    uint32_t sum = 0;
    for(int i = 0; i < usSamplesPerCh; i++)
    {
        sum += adcBuf[channel + (i * usNumChannels)];
    }
    
    return (float)sum / usSamplesPerCh;
}

uint32_t Calculate_Average_u32(uint16_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels)
{
    //if(channel >= NUM_CHANNELS) return 0.0f;
    
    uint32_t sum = 0;
    for(int i = 0; i < usSamplesPerCh; i++)
    {
        sum += adcBuf[channel + (i * usNumChannels)];
    }
    
    return sum / usSamplesPerCh;
}



uint32_t Calculate_AverageU32_lower(uint32_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels)
{
    //if(channel >= NUM_CHANNELS) return 0.0f;

    //(uint16_t)(adc_multimode_buffer[i] & 0xFFFF)
    
    uint32_t sum = 0;
    for(int i = 0; i < usSamplesPerCh; i++)
    {
        sum = (adcBuf[channel + (i * usNumChannels)]  & 0xFFFF) + sum;
    }
    
    return sum / usSamplesPerCh;
}


uint32_t Calculate_AverageU32_upper(uint32_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels)
{
    //if(channel >= NUM_CHANNELS) return 0.0f;

    //(uint16_t)(adc_multimode_buffer[i] & 0xFFFF)
    
    uint32_t sum = 0;
    for(int i = 0; i < usSamplesPerCh; i++)
    {
        sum = ((adcBuf[channel + (i * usNumChannels)] >> 16) & 0xFFFF) + sum;
    }
    
    return sum / usSamplesPerCh;
}