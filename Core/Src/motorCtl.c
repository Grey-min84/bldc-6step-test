#include "motorCtl.h"


#if 0

uint8_t ReadHallSensors(void)
{
    uint8_t state = 0;

    if (HAL_GPIO_ReadPin(GPE_HALL_1_GPIO_Port, GPE_HALL_1_Pin) == GPIO_PIN_SET)
        state |= 0x01;

    if (HAL_GPIO_ReadPin(GPE_HALL_2_GPIO_Port, GPE_HALL_2_Pin) == GPIO_PIN_SET)
        state |= 0x02;

    if (HAL_GPIO_ReadPin(GPE_HALL_3_GPIO_Port, GPE_HALL_3_Pin) == GPIO_PIN_SET)
        state |= 0x04;


    return state;
}




/**
 * @brief Set phase state for UNIPOLAR drive
 * @param phase Phase number (1=A, 2=B, 3=C)
 * @param state 0=Off, 1=PWM High-side only, 2=Low-side only
 */
void SetPhase(uint8_t phase, uint8_t state, uint16_t pwmVal)
{
	/**TIM1 GPIO Configuration
	PA8     ------> TIM1_CH1
	PA9     ------> TIM1_CH2
	PA10     ------> TIM1_CH3
	*/

    switch (phase)
    {
        case POLE_U:  // Phase A
            if (state == _6STEP_HiZ) {
                // Off - both switches off
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            }
            else if (state == _6STEP_PWM_IN) {
                // PWM High-side, Low-side OFF
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmVal);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            }
            else {
                // High-side OFF, Low-side ON
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            break;

        case POLE_V:  // Phase B
            if (state == _6STEP_HiZ) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
            }
            else if (state == _6STEP_PWM_IN) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwmVal);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
            }
            else {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            }
            break;

        case POLE_W:  // Phase C
            if (state == _6STEP_HiZ) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
            }
            else if (state == _6STEP_PWM_IN) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwmVal);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
            }
            else {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
            }
            break;
    }
}




/**
 * @brief Apply 6-step commutation based on hall sensor state and drive mode
 * @param state Hall sensor state (0-7)
 */
void ApplyCommutation(uint8_t state, uint16_t pwmVal)
{
	switch (state)
	{
		case 1:  // Hall: 001 -> B-PWM, C-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: Off
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  // B: PWM
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		case 2:  // Hall: 010 -> A-PWM, C-Low
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  // A: PWM
			SetPhase(POLE_V, _6STEP_HiZ, 0);  // B: Off
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		case 3:  // Hall: 011 -> A-PWM, B-Low
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  // A: PWM
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;

		case 4:  // Hall: 100 -> C-PWM, A-Low
			SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			SetPhase(POLE_V, _6STEP_HiZ, 0);  // B: Off
			SetPhase(POLE_W, _6STEP_PWM_IN, pwmVal);  // C: PWM
			break;

		case 5:  // Hall: 101 -> C-PWM, B-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: Off
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			SetPhase(POLE_W, _6STEP_PWM_IN, pwmVal);  // C: PWM
			break;

		case 6:  // Hall: 110 -> B-PWM, A-Low
			SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  // B: PWM
			SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;

		default:  // Invalid states (0, 7)
			SetPhase(1, 0, 0);  // A: Off
			SetPhase(2, 0, 0);  // B: Off
			SetPhase(3, 0, 0);  // C: Off
			break;
	}
}


void ApplyCommutation2(uint8_t state, uint16_t pwmVal)
{
	switch (state)
	{
		case 1:  // Hall: 001 -> B-PWM, C-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: Off
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  // B: PWM
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		case 2:  // Hall: 010 -> A-PWM, C-Low  -----
            SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  // A: PWM
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;

		case 3:  // Hall: 011 -> A-PWM, B-Low
			
			break;

		case 4:  // Hall: 100 -> C-PWM, A-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  // A: Off
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			SetPhase(POLE_W, _6STEP_PWM_IN, pwmVal);  // C: PWM
			break;

		case 5:  // Hall: 101 -> C-PWM, B-Low
			SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  // B: PWM
			SetPhase(POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;

		case 6:  // Hall: 110 -> B-PWM, A-Low
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  // A: PWM
			SetPhase(POLE_V, _6STEP_HiZ, 0);  // B: Off
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		default:  // Invalid states (0, 7)
			SetPhase(1, 0, 0);  // A: Off
			SetPhase(2, 0, 0);  // B: Off
			SetPhase(3, 0, 0);  // C: Off
			break;
	}
}



void ApplyCommutation_DRV832x(uint8_t state, uint16_t pwmVal)
{
	switch (state)
	{
		case 4:  // Hall: 001 -> B-PWM, C-Low
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  	// A: PWM
			SetPhase(POLE_V, _6STEP_HiZ, 0);  		// B: Off
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  		// C: Low
			break;

		case 2:  // Hall: 010 -> A-PWM, C-Low  -----
            SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  		// A: Low
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  	// B: PWM
			SetPhase(POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 6:  // Hall: 011 -> A-PWM, B-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  		// A: Off
			SetPhase(POLE_V, _6STEP_PWM_IN, pwmVal);  	// B: PWM
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  		// C: Low
			break;

		case 1:  // Hall: 100 -> C-PWM, A-Low
			SetPhase(POLE_U, _6STEP_HiZ, 0);  		// A: Off
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  		// B: Low
			SetPhase(POLE_W, _6STEP_PWM_IN, pwmVal);  	// C: PWM
			break;

		case 5:  // Hall: 101 -> C-PWM, B-Low
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  	// A: PWM
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  		// B: Low
			SetPhase(POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 3:  // Hall: 110 -> B-PWM, A-Low
			SetPhase(POLE_U, _6STEP_LOWSIDE_ON, 0);  		// A: Low
			SetPhase(POLE_V, _6STEP_HiZ, 0);  		// B: Off
			SetPhase(POLE_W, _6STEP_PWM_IN, pwmVal);  	// C: PWM
			break;

        case 7:  // align
			SetPhase(POLE_U, _6STEP_PWM_IN, pwmVal);  	// A: PWM
			SetPhase(POLE_V, _6STEP_LOWSIDE_ON, 0);  		// B: Off
			SetPhase(POLE_W, _6STEP_LOWSIDE_ON, 0);  		// C: Low
			break;

		default:  // Invalid states (0, 7)
			SetPhase(1, 0, 0);  // A: Off
			SetPhase(2, 0, 0);  // B: Off
			SetPhase(3, 0, 0);  // C: Off
			break;
	}
}
#endif

