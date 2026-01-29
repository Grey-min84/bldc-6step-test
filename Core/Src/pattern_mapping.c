#include "main.h"
#include "tiny_printf.h"
#include "motorCtl.h"

#if 0


#define PWM_VALUE		(800)

void PatternFindWay(){

    uint8_t hallSens = 0;

    



    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(1) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(2) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(3) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(4) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);



    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(5) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(6) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);





    SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_HiZ, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Low

    
}


void PatternFindWay2(){
 uint8_t hallSens = 0;

    



    


    


    


    



    


    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(1) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(2) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(3) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_PWM_IN, PWM_VALUE);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(4) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(5) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);


    SetPhase(POLE_U, _6STEP_PWM_IN, PWM_VALUE);  // A: PWM
    SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_PWM_IN, PWM_VALUE);  // C: Low
    HAL_Delay(2000);
    hallSens = ReadHallSensors();
    printf("(6) Hall State: %d\r\n\r\n", hallSens);
    HAL_Delay(500);





    SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: PWM
    SetPhase(POLE_V, _6STEP_HiZ, 0);  // B: Off
    SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Low

}

#endif