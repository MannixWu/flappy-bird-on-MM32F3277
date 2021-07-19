/*
 * @Author: your name
 * @Date: 2021-04-10 20:32:02
 * @LastEditTime: 2021-06-30 21:58:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Seekfree_MM32F3277_Opensource_Library\Project\MDK\pid.c
 */

#include "headfile.h"
float dir_P = 3.45;   //6
float dir_D = 1.25; //1.2
float error_old = 0;

float_t dir_pid(float_t dz,float_t MBdz)
{
    float_t dir_error,dir;
    dir_error = dz - MBdz;
    dir = dir_P * dir_error + dir_D * (dir_error - error_old);
    error_old = dir_error;
		//ips114_showuint16(190, 0, ()dir);
		// ips114_showfloat(0,0,dir,5,3);
    return dir;
}