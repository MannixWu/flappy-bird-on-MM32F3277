#include "headfile.h"

int32 Left_speed;
int32 Right_speed;
float KP = 2.4;  //2.4
float KI = 0.35; //0.35
float KD = 0.3;  //0.3
_pid_ Left_motor;
_pid_ Right_motor;

uint8 mid;
int32 dir;


void pid_init(void){
	Left_motor.kp = KP;
	Left_motor.ki = KI;
	Left_motor.kd = KD;

	Left_motor.Wucha = 0;
	Left_motor.LWcha = 0;
	Left_motor.LLWcha = 0;
	Left_motor.result = 0;

	Right_motor.kp = KP;
	Right_motor.ki = KI;
	Right_motor.kd = KD;

	Right_motor.Wucha = 0;
	Right_motor.LWcha = 0;
	Right_motor.LLWcha = 0;
	Right_motor.result = 0;
}



void speed_control(_pid_ *pid,int16 MBsu, int16 su) {
	int32  A, B, C;

	pid->Wucha = MBsu - su;
	A = pid->Wucha - pid->LWcha;
	B = pid->Wucha;
	C = pid->Wucha - 2 * pid->LWcha + pid->LLWcha;
	pid->result += (int32) (pid->kp * A + pid->ki * B + pid->kd * C);
	if (pid->result >= 9900) {
		pid->result = 9900;
	}
	if (pid->result < -9900) {
		pid->result = -9900;
	}
	pid->LLWcha = pid->LWcha;
	pid->LWcha = pid->Wucha;

}



void speed_read(void)
{
	int32 speed;
	float k = 201 / (2330 * 0.005);
	if(gpio_get(B5))
	{
		Left_speed=(k*tim_counter_get_count(TIM_3));
	}
	else
	{
		Left_speed=-(k*tim_counter_get_count(TIM_3));
	}
	
		if(gpio_get(B7))
	{
		Right_speed=-(k*tim_counter_get_count(TIM_4));
	}
	else
	{
		Right_speed=(k*tim_counter_get_count(TIM_4));
	}
	
	tim_counter_rst(TIM_3);
	tim_counter_rst(TIM_4);
}


void dir_control(int mid)
{
	//mid=search_mid(60);
	//ips114_showuint8(190,5,mid);
	dir=dir_pid(mid,94);
	//ips114_showuint16(190, 1, dir);
}
