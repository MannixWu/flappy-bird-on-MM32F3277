/*
 * @Author: MannixWu
 * @Date: 2021-07-19 09:23:27
 * @LastEditTime: 2021-08-08 12:11:20
 * @LastEditors: Please set LastEditors
 * @QQ: 3068758340
 * @Description: ת���޸���ע������
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
//֧���Զ����޸ĵĳ���
#define GAME_SPEED 10   //��Ϸ�ٶ�
#define RND_SEED 237162 //���������
#define BIRD_X 30       //���ʼx����
#define BIRD_Y 30       //���ʼy����
#define PIPE_OFFSET 400 //�ܵ���ʼλ��
#define PIPE_SPACE 32   //�ܵ����¼��
#define PIPE_REFRESH 0  //�ܵ����Ҽ������
//����16λ��ɫͼ�����Ļ���ͼ��
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

    srand(RND_SEED); //��������ӳ�ʼ��
    device_init();   //�豸��ʼ��
    //��Ϸ����ѭ��
    while (TRUE)
    {
        bird_init();
        int bird_off_x = BIRD_X;              //���ʼx����
        int bird_off_y = BIRD_Y;              //���ʼy����
        int pipeTick = PIPE_OFFSET;           //�ܵ�λ��
        int score = 0;                        //����
        int pipe_space = PIPE_SPACE;          //���¹ܵ��Ŀ�϶
        int pipe_offset = random() % 32 + 97; //����ܵ��߶�
        //������Ϸѭ��
        while (birdIsAlive)
        {
            tick++;
            get_icm20602_accdata();
            get_icm20602_gyro();
            speed_read();
            //���Ʊ���
            bird_draw_background();
            //���Ƶ���
            bird_draw_floor();
            //���˶��ж�
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
            //������ж�
            if (bird_off_y > 195 - 24)
            {
                bird_off_y = 195 - 24;
            }
            if (bird_off_y < 0)
            {
                bird_off_y = 0;
            }
            //������
            bird_draw_body(bird_off_x, bird_off_y);
            //����ˮ��
            if (pipeTick < -52)
            {
                pipeTick = 135;
                pipe_offset = (random() % 64) - 32 + 97;
                score = -score;
            }
            //����ˮ��
            bird_draw_pipe(pipeTick, pipe_offset, pipe_space);
            //�ж�����
            if (bird_off_x + 34 > pipeTick && bird_off_x < pipeTick + 52)
            {
                //�Ѿ�����ܵ���Χ
                if (bird_off_y < pipe_offset - pipe_space || bird_off_y + 24 > pipe_offset + pipe_space)
                {
                    birdIsAlive = FALSE;
                }
            }
            //�Ʒ��ж�
            if (bird_off_x > pipeTick + 52 && score >= 0)
            {
                score = -score - 1;
            }
            //���Ʒ���
            bird_draw_score(score);
            //�ƶ�ˮ������
            pipeTick -= gameSpeed;
            //systick_delay_ms(50);
            ips114_displayimageRGB(screen[0], WIDTH, HEIGHT);
        }
        //��Ϸ���㻭��
        while (TRUE)
        {
            speed_read();
            //����gameover
            bird_draw_gameover();
            //��ʾ���
            ips114_displayimageRGB(screen[0], WIDTH, HEIGHT);
            //���¿�ʼ�ж�
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
 * @brief: ���Ʊ���
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
 * @brief: ���Ƶ���
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
 * @brief: ������
 * @param {int} bird_off_x  ��x����
 * @param {int} bird_off_y  ��y����
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
 * @brief: ����ˮ��
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
 * @brief: ������Ϸ����ui
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
 * @brief: ���ƼǷְ�
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
 * @brief: �����ݳ�ʼ��
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
 * @brief: �豸Ӳ����ʼ��
 * @param {*}
 * @return {*}
 */
void device_init()
{
    board_init(true);
    ips114_init(); //240*135  MT9V03X_H	MT9V03X_W
    //icm20602_init();
    seekfree_wireless_init();
    uart_init(WIRELESS_UART, WIRELESS_UART_BAUD, WIRELESS_UART_TX, WIRELESS_UART_RX); // ��ʼ������
    void Date_Send(void);
    int8 Date_Recive(void);
    tim_counter_init(TIM_3, TIM_3_ENC1_B04);
    tim_counter_init(TIM_4, TIM_4_ENC1_B06);
    gpio_init(B5, GPI, 1, GPI_PULL_UP);
    gpio_init(B7, GPI, 1, GPI_PULL_UP);
}

/**
 * @brief: ���������
 * @note rand�������ɴ������˻��ظ���ÿ�θ�������ʵ�����
 * @return {int}    ��������
 */
int random()
{
    int temp = rand();
    srand(temp);
    return temp;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief  ˫���������ݷ���
// @param  none
// @param  none
// @return  none
// Sample usage:    Date_Send();     //ֱ���þͺ�
//-------------------------------------------------------------------------------------------------------------------
void Date_Send(void)
{
    uint8 date[5] = {'c', 'y', '1', 'j', 'y'};
    seekfree_wireless_send_buff(date, 5);
    //uart_putbuff(WIRELESS_UART, date, 5);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief  ˫���������ݽ���
// @param  none
// @param  none
// @return  -1 or 1   1��ƥ��ɹ�  -1��ƥ��ʧ��
// Sample usage:    Date_Recive();     //ֱ���þͺ�
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