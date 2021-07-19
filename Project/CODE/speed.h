#ifndef _SPEED_H_
#define _SPEED_H_



typedef struct
{
  float Wucha;
  float LWcha;
  float LLWcha;
  float result;
  float kp,ki,kd;
}_pid_;

extern _pid_ Left_motor;
extern _pid_ Right_motor;

extern int32 Left_speed;
extern int32 Right_speed;
extern float KP;
extern float KI;
extern float KD;

extern uint8 mid;
extern int32 dir;
void speed_control(_pid_ *pid,int16 MBsu, int16 su);
void speed_read(void);
void pid_init(void);
void dir_control(int mid);



#endif


