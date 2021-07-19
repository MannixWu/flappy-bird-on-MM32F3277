/*
 * image.c
 *
 *  Created on: 2021年3月29日
 *      Author: linzhiying
 */

#include "headfile.h"
uint8 h = 93;
uint8 Bian_left[60];  //左边界点
uint8 Bian_right[60]; //右边界点
uint8 Zuo_min = 0;    //左边界最小值
uint8 You_max = 0;    //右边界最大值
int Deng_len = 0;     //信标灯宽度
int Deng_mid = 0;     //信标灯中点
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
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint16 HistoGram[256]; //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < MT9V03X_H; j += 2)
    {
        for (i = 0; i < MT9V03X_W; i += 2)
        {
            HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }
    //滤波
    for (int i = 0; i < 256; i++)
    {
        HistoGram[i] = (HistoGram[between(i - 3, 0, 255)] * 10 + HistoGram[between(i - 2, 0, 255)] * 20 + HistoGram[between(i - 1, 0, 255)] * 30 + HistoGram[between(i - 0, 0, 255)] * 40) / 100;
    }
    //end

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++)
        ; //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--)
        ; //获取最大灰度的值

    if (MaxValue == MinValue)
        return (uint8)MaxValue; // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)
        return (uint8)MinValue; // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j]; //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j; //灰度值总数
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];                                              //前景像素点数
        PixelFore = Amount - PixelBack;                                                    //背景像素点数
        OmegaBack = (double)PixelBack / Amount;                                            //前景像素百分比
        OmegaFore = (double)PixelFore / Amount;                                            //背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;                                             //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             //背景灰度值
        MicroBack = (double)PixelIntegralBack / PixelBack;                                 //前景灰度百分比
        MicroFore = (double)PixelIntegralFore / PixelFore;                                 //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); //计算类间方差
        if (Sigma > SigmaB)                                                                //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = (uint8)j;
        }
    }
    return Threshold; //返回最佳阈值;
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