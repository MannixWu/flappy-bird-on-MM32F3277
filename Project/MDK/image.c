/*
 * image.c
 *
 *  Created on: 2021��3��29��
 *      Author: linzhiying
 */

#include "headfile.h"
uint8 h = 93;
uint8 Bian_left[60];  //��߽��
uint8 Bian_right[60]; //�ұ߽��
uint8 Zuo_min = 0;    //��߽���Сֵ
uint8 You_max = 0;    //�ұ߽����ֵ
int Deng_len = 0;     //�ű�ƿ��
int Deng_mid = 0;     //�ű���е�
uint8 DiuDeng = 0;
char getstr;
char comdata[30];
char data_p;
int left;
int right;
int zhong;

uint8 GetOSTU(uint8 tmImage[MT9V03X_H][MT9V03X_W])
{
    int16 i, j;
    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint16 HistoGram[256]; //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < MT9V03X_H; j += 2)
    {
        for (i = 0; i < MT9V03X_W; i += 2)
        {
            HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }
    //�˲�
    for (int i = 0; i < 256; i++)
    {
        HistoGram[i] = (HistoGram[between(i - 3, 0, 255)] * 10 + HistoGram[between(i - 2, 0, 255)] * 20 + HistoGram[between(i - 1, 0, 255)] * 30 + HistoGram[between(i - 0, 0, 255)] * 40) / 100;
    }
    //end

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++)
        ; //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--)
        ; //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)
        return (uint8)MaxValue; // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)
        return (uint8)MinValue; // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j]; //  ��������

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j; //�Ҷ�ֵ����
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];                                              //ǰ�����ص���
        PixelFore = Amount - PixelBack;                                                    //�������ص���
        OmegaBack = (double)PixelBack / Amount;                                            //ǰ�����ذٷֱ�
        OmegaFore = (double)PixelFore / Amount;                                            //�������ذٷֱ�
        PixelIntegralBack += HistoGram[j] * j;                                             //ǰ���Ҷ�ֵ
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             //�����Ҷ�ֵ
        MicroBack = (double)PixelIntegralBack / PixelBack;                                 //ǰ���ҶȰٷֱ�
        MicroFore = (double)PixelIntegralFore / PixelFore;                                 //�����ҶȰٷֱ�
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); //������䷽��
        if (Sigma > SigmaB)                                                                //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            SigmaB = Sigma;
            Threshold = (uint8)j;
        }
    }
    return Threshold; //���������ֵ;
}

uint8 search_mid(uint8 high)
{
    uint8 Threshold, left = 1, right = 187, mid, i = 1, j = 187;
    Threshold = GetOSTU(mt9v03x_image);
    //Threshold=117;
    for (i = 94; i >= 1; i--)
    {
        if ((mt9v03x_image[high][i] < Threshold) && (mt9v03x_image[high][i - 1] < Threshold) && (mt9v03x_image[high][i - 2] < Threshold))
        {
            left = i;
            break;
        }
    }
    for (j = 94; j <= 187; j++)
    {
        if ((mt9v03x_image[high][j] < Threshold) && (mt9v03x_image[high][j + 1] < Threshold) && (mt9v03x_image[high][j + 2] < Threshold))
        {
            right = j;
            break;
        }
    }
    mid = (left + right) / 2;
    //h=mid;
    return mid;
}

void showLD()
{

    uint8 th = GetOSTU(mt9v03x_image);
    for (int i = 0; i < 100; i++)
    {
        if (mt9v03x_image[120 - i - 1 - 4][4] < th)
        {
            drawPointL(10, 120 - i - 1, RED, 1);
            break;
        }
        if (mt9v03x_image[120 - 1 - 4][4 + i] < th)
        {
            drawPointL(i + 10, 120 - 1, RED, 1);
            break;
        }
    }
}