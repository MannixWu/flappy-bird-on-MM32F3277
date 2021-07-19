#include "headfile.h"


void Right_lun(int32 pwm)
{
	if(pwm>=0)
	{
		//gpio_set(A2, GPIO_LOW);
		//pwm_duty_updata(TIM_5, TIM_5_CH4_A03, pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH3_A02, pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH1_A00, 0);					//左轮正转
	}
	else
	{
		//gpio_set(A2, GPIO_HIGH);
		//pwm_duty_updata(TIM_5, TIM_5_CH4_A03, -pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH3_A02, 0);					//左轮倒转
		pwm_duty_updata(TIM_5, TIM_5_CH1_A00, -pwm);
	}
}


void Left_lun(int32 pwm)
{
	if(pwm>=0)
	{
		//gpio_set(A0, GPIO_LOW);												
		//pwm_duty_updata(TIM_5, TIM_5_CH2_A01, pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH4_A03, pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH2_A01, 0);					//右轮正转
	}
	else
	{
		//gpio_set(A0, GPIO_HIGH);												
		//pwm_duty_updata(TIM_5, TIM_5_CH2_A01, -pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH2_A01, -pwm);
		pwm_duty_updata(TIM_5, TIM_5_CH4_A03,0);
	}
	
}