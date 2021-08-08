/*
 * @Author: MannixWu
 * @Date: 2021-07-19 09:23:27
 * @LastEditTime: 2021-08-08 12:11:20
 * @LastEditors: Please set LastEditors
 * @QQ: 3068758340
 * @Description: 转载修改请注明出处
 * @FilePath: \flappy bird\Project\USER\src\main.c
 */
#include "headfile.h"
#include "math.h"
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define Pi 3.1415
#define WIDTH 135
#define HEIGHT 240
//支持自定义修改的常量
#define GAME_SPEED 10   //游戏速度
#define RND_SEED 237162 //随机数种子
#define BIRD_X 30       //鸟初始x坐标
#define BIRD_Y 30       //鸟初始y坐标
#define PIPE_OFFSET 400 //管道初始位置
#define PIPE_SPACE 32   //管道上下间隔
#define PIPE_REFRESH 0  //管道左右间隔增量
//储存16位彩色图像的屏幕输出图像
uint16 screen[HEIGHT][WIDTH] = WHITE;

int tick;
int birdFlyTick;
int birdAcc;
int gameSpeed;
int birdIsAlive;

int main()
{
    int random();
    void device_init();
    void bird_init();
    void bird_draw_background();
    void bird_draw_floor();
    void bird_draw_body();
    void bird_draw_pipe();
    void bird_draw_gameover();
    void bird_draw_score();

    srand(RND_SEED); //随机数种子初始化
    device_init();   //设备初始化
    //游戏进程循环
    while (TRUE)
    {
        bird_init();
        int bird_off_x = BIRD_X;              //鸟初始x坐标
        int bird_off_y = BIRD_Y;              //鸟初始y坐标
        int pipeTick = PIPE_OFFSET;           //管道位置
        int score = 0;                        //分数
        int pipe_space = PIPE_SPACE;          //上下管道的空隙
        int pipe_offset = random() % 32 + 97; //随机管道高度
        //单局游戏循环
        while (birdIsAlive)
        {
            tick++;
            get_icm20602_accdata();
            get_icm20602_gyro();
            speed_read();
            //绘制背景
            bird_draw_background();
            //绘制地面
            bird_draw_floor();
            //鸟运动判定
            if (icm_gyro_z < -200 || abs(Left_speed) > 1000 || abs(Right_speed) > 1000)
            {
                birdAcc = 5;
            }
            else if (uart_index[WIRELESS_UART]->CSR & UART_CSR_RXAVL)
            {
                birdAcc = 5;
                char ccc = (uint8)uart_index[WIRELESS_UART]->RDR;
            }
            else
            {
                birdAcc -= 1;
            }
            bird_off_y -= birdAcc;
            //鸟落地判定
            if (bird_off_y > 195 - 24)
            {
                bird_off_y = 195 - 24;
            }
            if (bird_off_y < 0)
            {
                bird_off_y = 0;
            }
            //绘制鸟
            bird_draw_body(bird_off_x, bird_off_y);
            //重置水管
            if (pipeTick < -52)
            {
                pipeTick = 135;
                pipe_offset = (random() % 64) - 32 + 97;
                score = -score;
            }
            //绘制水管
            bird_draw_pipe(pipeTick, pipe_offset, pipe_space);
            //判定死亡
            if (bird_off_x + 34 > pipeTick && bird_off_x < pipeTick + 52)
            {
                //已经进入管道范围
                if (bird_off_y < pipe_offset - pipe_space || bird_off_y + 24 > pipe_offset + pipe_space)
                {
                    birdIsAlive = FALSE;
                }
            }
            //计分判定
            if (bird_off_x > pipeTick + 52 && score >= 0)
            {
                score = -score - 1;
            }
            //绘制分数
            bird_draw_score(score);
            //移动水管坐标
            pipeTick -= gameSpeed;
            //systick_delay_ms(50);
            ips114_displayimageRGB(screen[0], WIDTH, HEIGHT);
        }
        //游戏结算画面
        while (TRUE)
        {
            speed_read();
            //绘制gameover
            bird_draw_gameover();
            //显示输出
            ips114_displayimageRGB(screen[0], WIDTH, HEIGHT);
            //重新开始判定
            if (icm_gyro_z < -200 || abs(Left_speed) > 1000 || abs(Right_speed) > 1000)
            {
                break;
            }
            else if (uart_index[WIRELESS_UART]->CSR & UART_CSR_RXAVL)
            {
                char ccc = (uint8)uart_index[WIRELESS_UART]->RDR;
                break;
            }
        }
    }
    return 0;
}
/**
 * @brief: 绘制背景
 * @param {*}
 * @return {*}
 */
void bird_draw_background()
{
    for (int y = 0; y < 140; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            screen[y][x] = 19993;
        }
    }
    for (int y = 140; y < 190; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            screen[y][x] = backGround[y - 140][(x + (tick * 1) % 135) % 135];
        }
    }
    for (int y = 190; y < 195; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            screen[y][x] = 24334;
        }
    }
}
/**
 * @brief: 绘制地面
 * @param {*}
 * @return {*}
 */
void bird_draw_floor()
{

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            screen[y + 195][x] = land[y][(x + (tick * gameSpeed) % 135) % 135];
        }
    }
    for (int y = 8; y < 45; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            screen[y + 195][x] = 57042;
        }
    }
}
/**
 * @brief: 绘制鸟
 * @param {int} bird_off_x  鸟x坐标
 * @param {int} bird_off_y  鸟y坐标
 * @return {*}
 */
void bird_draw_body(int bird_off_x, int bird_off_y)
{
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 34; x++)
        {
            if (bird_off_y + y >= 0 && bird_off_y + y < HEIGHT)
            {
                if (tick % 4 == 0)
                {
                    if (bird_0[y][x] != 2047)
                    {
                        screen[y + bird_off_y][x + bird_off_x] = bird_0[y][x];
                    }
                }
                if (tick % 4 == 1 || tick % 4 == 3)
                {
                    if (bird_1[y][x] != 2047)
                    {
                        screen[y + bird_off_y][x + bird_off_x] = bird_1[y][x];
                    }
                }
                if (tick % 4 == 2)
                {
                    if (bird_2[y][x] != 2047)
                    {
                        screen[y + bird_off_y][x + bird_off_x] = bird_2[y][x];
                    }
                }
            }
        }
    }
}
/**
 * @brief: 绘制水管
 * @param {int} pipeTick
 * @param {int} pipe_offset
 * @param {int} pipe_space
 * @return {*}
 */
void bird_draw_pipe(int pipeTick, int pipe_offset, int pipe_space)
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 52; x++)
        {
            if (pipe[y][x] != 2047)
            {
                int pipe_x = x + pipeTick;
                int pipe_y = y + pipe_offset + pipe_space;
                if (pipe_x >= 0 && pipe_x < 135 && pipe_y >= 0 && pipe_y < 240)
                {
                    screen[pipe_y][pipe_x] = pipe[y][x];
                }
            }
        }
    }
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 52; x++)
        {
            if (pipe[y][x] != 2047)
            {
                int pipe_x = x + pipeTick;
                int pipe_y = -y + pipe_offset - pipe_space;
                if (pipe_x >= 0 && pipe_x < 135 && pipe_y >= 0 && pipe_y < 240)
                {
                    screen[pipe_y][pipe_x] = pipe[y][x];
                }
            }
        }
    }
    for (int y = 32 + pipe_offset + pipe_space; y < 195; y++)
    {
        for (int x = 0; x < 52; x++)
        {
            if (pipe[31][x] != 2047)
            {
                int pipe_x = x + pipeTick;
                int pipe_y = y;
                if (pipe_x >= 0 && pipe_x < 135 && pipe_y >= 0 && pipe_y < 240)
                {
                    screen[pipe_y][pipe_x] = pipe[31][x];
                }
            }
        }
    }
    for (int y = -32 + pipe_offset - pipe_space; y >= 0; y--)
    {
        for (int x = 0; x < 52; x++)
        {
            if (pipe[31][x] != 2047)
            {
                int pipe_x = x + pipeTick;
                int pipe_y = y;
                if (pipe_x >= 0 && pipe_x < 135 && pipe_y >= 0 && pipe_y < 240)
                {
                    screen[pipe_y][pipe_x] = pipe[31][x];
                }
            }
        }
    }
}
/**
 * @brief: 绘制游戏结束ui
 * @param {*}
 * @return {*}
 */
void bird_draw_gameover()
{
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 94; x++)
        {
            if (gameover[y][x] != 2047)
            {
                int off_x = x + 20;
                int off_y = y + 70;
                if (off_x >= 0 && off_x < 135 && off_y >= 0 && off_y < 240)
                {
                    screen[off_y][off_x] = gameover[y][x];
                }
            }
        }
    }
}
/**
 * @brief: 绘制记分板
 * @param {int} score
 * @return {*}
 */
void bird_draw_score(int score)
{
    int numCount = 0;
    int scoreNum = abs(score);
    int numSpace = 20;
    if (scoreNum != 0)
    {
        while (scoreNum > 0)
        {
            scoreNum = scoreNum / 10;
            numCount++;
        }
    }
    else
    {
        numCount = 1;
    }
    scoreNum = abs(score);
    for (int i = 0; i < numCount; i++)
    {
        int num_off_x = (135 / 2) + (numCount - 1) * (numSpace / 2) - (numSpace / 2) - i * numSpace;
        int num_off_y = 10;
        for (int y = 0; y < 28; y++)
        {
            for (int x = 0; x < 18; x++)
            {
                if (numbers[scoreNum % 10][y][x] != 2047)
                {
                    screen[num_off_y + y][num_off_x + x] = numbers[scoreNum % 10][y][x];
                }
            }
        }
        scoreNum /= 10;
    }
}

/**
 * @brief: 鸟数据初始化
 * @param {*}
 * @return {*}
 */
void bird_init()
{
    tick = 0;
    birdFlyTick = 0;
    birdAcc = 0;
    gameSpeed = GAME_SPEED;
    birdIsAlive = TRUE;
}

/**
 * @brief: 设备硬件初始化
 * @param {*}
 * @return {*}
 */
void device_init()
{
    board_init(true);
    ips114_init(); //240*135  MT9V03X_H	MT9V03X_W
    //icm20602_init();
    seekfree_wireless_init();
    uart_init(WIRELESS_UART, WIRELESS_UART_BAUD, WIRELESS_UART_TX, WIRELESS_UART_RX); // 初始化串口
    void Date_Send(void);
    int8 Date_Recive(void);
    tim_counter_init(TIM_3, TIM_3_ENC1_B04);
    tim_counter_init(TIM_4, TIM_4_ENC1_B06);
    gpio_init(B5, GPI, 1, GPI_PULL_UP);
    gpio_init(B7, GPI, 1, GPI_PULL_UP);
}

/**
 * @brief: 生成随机数
 * @note rand函数生成次数多了会重复，每次更换种子实现随机
 * @return {int}    随机数结果
 */
int random()
{
    int temp = rand();
    srand(temp);
    return temp;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief  双车接力数据发送
// @param  none
// @param  none
// @return  none
// Sample usage:    Date_Send();     //直接用就好
//-------------------------------------------------------------------------------------------------------------------
void Date_Send(void)
{
    uint8 date[5] = {'c', 'y', '1', 'j', 'y'};
    seekfree_wireless_send_buff(date, 5);
    //uart_putbuff(WIRELESS_UART, date, 5);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief  双车接力数据接收
// @param  none
// @param  none
// @return  -1 or 1   1：匹配成功  -1：匹配失败
// Sample usage:    Date_Recive();     //直接用就好
//-------------------------------------------------------------------------------------------------------------------
int8 Date_Recive(void)
{
    uint8 flage = 0;
    uint8 str3[10] = {0};

    for (uint8 i = 0; i < 6; i++)
    {
        uart_getchar(WIRELESS_UART, &str3[i]);
    }

    if (str3[1] == 'c' && str3[2] == 'y' && str3[3] == '1' && str3[4] == 'j' && str3[5] == 'y')
        flage = 0;
    else
        flage = 1;

    if (flage)
        return 0;
    else
        return 1;
}