// OcrHelper.h

#pragma once

#include "stdafx.h"
#include<stdio.h>
#include<malloc.h>  
#include<string.h> 
#include<math.h>
#include<stdlib.h>
#include<iostream>  

using namespace std;
using namespace System;
using namespace System::Drawing;
const int Nlength = 150;
namespace OcrHelper {
	typedef struct RGB_QUAD
	{
		int rgbBlue;
		int rgbGreen;
		int rgbRed;
		int Grayvalue;//灰度值
		int rgbReversed;
		int sign;
	} RGBQUADS, *imagergb;
	typedef struct RGB_COLOR
	{
		int numb;
		int numg;
		int numr;
		int amount;
		int location;
	}RGB_AMOUNT, *rgbcolor;
	typedef struct ENCLOSURE_SPACE
	{
		int leftdownx;
		int leftdowny;
		int rightupx;
		int rightupy;
		int sign;//loop为1，其它封闭空间为2;
	}ENCLOSUSE, *closurespace;


	public ref class Helper
	{
	public:
		static Bitmap^ Process(Bitmap^ img)
		{
			Bitmap^ proimg;
			Helper proprocess;
			proprocess.readimagergb(img);
			proprocess.pretreatmentimage();
			proimg = proprocess.writeimagergb();
			proprocess.freememoryzone();
			return proimg;
		}
	private:

		imagergb rgb, freergb;
		imagergb copyrgb;
		rgbcolor rgbamount;
		closurespace clospace;
		int width, height, origincountcolorvarietyamount, countcolorvarietyamount, rgbReversedture = 0, amplify = 0;
		int *startposition, *endposition;
		int *spotamount;
		int *list;
		int loopnum = 0;

		void pretreatmentimage()
		{

			statisticsrgbcolor();//将各点颜色按区域进行划分（包括划分后进行相关的合并）；
			//判断该图片是否需要去边框，防止将贴近边缘的字母消去
			judgeframe();
			dividetreatmentimage();   //划分验证码
		}
		void statisticsrgbcolor()
		{
			int firstcountamount;
			firstcountamount = originalstatisticsrgbcolor();
			proceddingstatisticsrgbcolor(firstcountamount);
		}
		void judgeframe()
		{
			/*用来判断验证码图片是否需要去边框，根据贴近边框的颜色像素点数是否超过图片4/5的宽度/高度来判断
			如果是则认为它是边框需要去边框，否则认为它是字母贴近边框或者噪线则不去边框；*/
			int i, j, amountdiffer;
			int  amountfirsti, amountendi, amountfirstj, amountendj;
			int acountspotamountone, acountspotamounttwo;

			imagergb  p;
			amountfirsti = 0;
			amountendi = 0;
			amountfirstj = 0;
			amountendj = 0;
			acountspotamountone = calculatedifferspot(10, 5);/*num用来设置统计周围不同点数，返回周围不同点超过num的像素点数*/
			acountspotamounttwo = calculatedifferspot(20, 5);



			imagergb Imagergb = getImagergb();

			for (i = 0, p = Imagergb; i < width; i++, p++)
			{
				//统计第一行不同于背景色的像素点总数
				amountdiffer = imagecolordiffer(p, rgbamount);
				if (amountdiffer > 5)
					amountfirsti++;
			}

			for (i = 0, p = Imagergb + (height - 1)*width; i < width; i++, p++)
			{
				amountdiffer = imagecolordiffer(p, rgbamount);
				if (amountdiffer > 5)
					amountendi++;
			}


			for (j = 0, p = Imagergb; j < height; j++, p = p + width)
			{
				//统计第一列不同于背景色的像素点总数
				amountdiffer = imagecolordiffer(p, rgbamount);
				if (amountdiffer > 5)
					amountfirstj++;
			}
			for (j = 0, p = Imagergb + width - 1; j<height; j++, p = p + width)
			{
				amountdiffer = imagecolordiffer(p, rgbamount);
				if (amountdiffer > 5)
					amountendj++;
			}
			if (amountfirsti > 4 * width / 5 || amountfirstj > 4 * height / 5 || amountendi > 4 * width / 5 || amountendj > 4 * height / 5)
				discardframe();
			else
			{
				//没有边框的图片增加图片大小；
				if (acountspotamountone - acountspotamounttwo <= 0 && width <= 40)
				{
					amplify = 1;
				}
			}
		}

		void discardframe()
		{
			//去边框
			int i, j;
			imagergb p, q;

			imagergb Imagergb = getImagergb();

			for (i = 0, p = Imagergb; i < width; i++, p++)
			{
				p->rgbBlue = rgbamount->numb * 15;
				p->rgbGreen = rgbamount->numg * 15;
				p->rgbRed = rgbamount->numr * 15;
				(p + (height - 1)*width)->rgbBlue = rgbamount->numb * 15;
				(p + (height - 1)*width)->rgbGreen = rgbamount->numg * 15;
				(p + (height - 1)*width)->rgbRed = rgbamount->numr * 15;
			}

			for (j = 0, q = Imagergb; j < height; j++, q = q + width)
			{
				q->rgbBlue = rgbamount->numb * 15;
				q->rgbGreen = rgbamount->numg * 15;
				q->rgbRed = rgbamount->numr * 15;
				(q + width - 1)->rgbBlue = rgbamount->numb * 15;
				(q + width - 1)->rgbGreen = rgbamount->numg * 15;
				(q + width - 1)->rgbRed = rgbamount->numr * 15;
			}
		}

		void dividetreatmentimage()
		{
			////分类函数
			int bgcolor;
			if (rgbReversedture == 0)
			{
				bgcolor = acountbackgroundcolorquantity();
				if ((bgcolor == 1 || bgcolor == 2 || bgcolor == 3) && width <= 200)
				{
					//两种背景颜色小图
					treatmentimageone(bgcolor);
				}
				else if (bgcolor == 0 && width <= 200)
				{
					//一种背景颜色小图
					treatmentimagetwo();
				}
				else if (bgcolor == 0 && width > 200)
				{
					//一种背景颜色的大图	
					treatmentimagethree();
				}
			}
			else
			{
				//图像颜色极少的图		
				treatmentimagefour();
			}
		}

		int acountbackgroundcolorquantity()
		{
			/*判断该图片是多背景还是单背景:根据不同于背景色的颜色列数和行数以及连续列和颜色点总数来进行综合判断*/
			int i, j, ifront, jfront, countnum, countnumwidth, countnumheight, countnumfalse, amountdiffer, numb, numg, numr;
			int coloramount1, coloramount2, colortype;
			rgbcolor s, imagergbcolor;
			imagergb  p;
			imagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			coloramount1 = (rgbamount + 1)->amount;
			coloramount2 = (rgbamount + 2)->amount;
			colortype = calculateimagecoloramount(10);

			if (coloramount1 >= 500 && coloramount2 >= 300 && colortype > 16)
			{
				return 0;
			}
			else
			{
				for (s = rgbamount + 1; s - (rgbamount + 1) < 3; s++)
				{
					amountdiffer = countstatisticscolor(rgbamount, s);
					if (((15 - rgbamount->numb) + (15 - rgbamount->numg) + (15 - rgbamount->numr))>20)
					{
						//太深的背景颜色不认为是双背景
						return 0;
					}
					else if (amountdiffer > 20)
					{
						//太深的颜色不认为可以做第二种背景
						return 0;
					}
					else
					{
						if (amountdiffer >= 3)//yuan>4->xian>=3,3->4
						{
							countnumwidth = 0;
							countnumfalse = 0;
							countnum = 0;
							for (i = 1; i < width - 1; i++)
							{
								for (p = rgb + i + width + 1, j = 0; j < height - 2; p = p + width, j++)
								{
									virtualdiscardrgbspot(p);
									amountdiffer = imagecolordiffer(p, s);
									if (amountdiffer <= 2)
									{
										countnum++;//总共有多少列这种颜色
										break;
									}
								}
							}
							for (i = 1; i < width - 1; i++)
							{
								for (p = rgb + i + width + 1, j = 0; j < height - 2; p = p + width, j++)
								{
									virtualdiscardrgbspot(p);
									amountdiffer = imagecolordiffer(p, s);
									if (amountdiffer <= 2)
									{
										countnumwidth++;//连续的列
										break;
									}
								}
								if (j == height - 2 && countnumfalse > 0 && countnumwidth > 0)
								{
									if (i - ifront == 1)
									{
										countnumfalse++;
										ifront = i;
									}
									else
										countnumfalse = 0;
								}
								if (j == height - 2 && countnumfalse == 0 && countnumwidth > 0)
								{
									countnumfalse = 1;
									ifront = i;
								}
								if (countnumwidth > 0 && countnumfalse > 1 && i<width * 2 / 3)
								{
									countnumwidth = 0;
									countnumfalse = 0;
								}
								if (countnumwidth > width / 3)
									break;
							}
							countnumheight = 0;
							countnumfalse = 0;
							for (i = 1; i < height - 1; i++)
							{
								for (p = rgb + width*i + 1, j = 0; j < width - 2; p++, j++)
								{
									virtualdiscardrgbspot(p);
									amountdiffer = imagecolordiffer(p, s);
									if (amountdiffer <= 2)
									{
										countnumheight++;//总共多少行这种颜色
										break;
									}
								}
								if (j == width - 2 && countnumfalse > 0 && countnumheight > 0)
								{
									if (j - jfront == 1)
									{
										countnumfalse++;
										jfront = j;
									}
									else
										countnumfalse = 0;
								}
								if (j == width - 2 && countnumfalse == 0 && countnumheight > 0)
								{
									countnumfalse = 1;
									jfront = j;
								}
								if (countnumheight > 0 && countnumfalse > 1 && j<height * 2 / 3)
								{
									countnumheight = 0;
									countnumfalse = 0;
								}
								if (countnumheight > height / 3)
									break;
							}

							if ((((countnumwidth > width / 3) && countnumheight > height * 4 / 5 || (countnum > width * 4 / 5)) && rgbamount->amount / s->amount <= 10) || (countnum > width * 3 / 5 && rgbamount->amount < s->amount * 9 / 2))
							{
								free(imagergbcolor);
								return s - rgbamount;
							}
						}
					}
				}
				free(imagergbcolor);
				return 0;
			}
		}
		void treatmentimageone(int backlocation)
		{
			//多背景小图处理
			int colortype, coloramount;
			int numberwidth2, numberwidth3;
			int numberheight2, numberheight3;
			int letterwidth;

			colortype = calculateimagecoloramount(100);
			coloramount = (rgbamount + 1)->amount;
			numberheight2 = discardheightnoise(15, 0, 2);
			numberwidth2 = discardwidthnoise(15, 0, 2);
			numberheight3 = discardheightnoise(15, 0, 3);
			numberwidth3 = discardwidthnoise(15, 0, 3);

			letterwidth = calculateletterwidth();
			if (letterwidth >= 3)
			{
				//天津
				imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 2, 13, 0);
				blackwhitediscardrgbspot(5, 255);
			}
			else
			{
				if (coloramount < 145)
				{
					//安庆
					imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 13, 0);
					blackwhitediscardrgbspot(5, 255);
				}
				else
				{
					//58,台州,其它
					if (colortype >= 4)
					{
						if (width < 100)
						{
							//台州
							imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 7, 0);//yuan7->xian13
							blackwhitediscardrgbspot(7, 255);
						}
						else
						{
							//58
							discardcolorlump(3);
							treatment58();
						}
					}
					else
					{
						if (((numberheight2 <= numberheight3 + 1) || (numberwidth2 <= numberwidth3 + 1)) && numberwidth3 >= 15)
						{
							//其它
							imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 12, 0);//yuan7->xian13
							blackwhitediscardrgbspot(5, 255);
						}
						else
						{
							//台州
							imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 8, 0);//yuan7->xian13
							blackwhitediscardrgbspot(7, 255);
						}
					}
				}
			}
		}
		void treatmentimagetwo()
		{
			//单背景小图
			int acountspotamountone, acountspotamounttwo;
			int heightnum;
			int colornum1, colornum2;
			int coloramount;
			int noisespot, noiseline;
			int type;
			rgbcolor color1;

			color1 = rgbamount + 1;
			acountspotamountone = calculatedifferspot(10, 5);
			acountspotamounttwo = calculatedifferspot(20, 5);
			heightnum = discardheightnoise(8, 0, 1);
			colornum1 = (rgbamount + 1)->amount;
			colornum2 = (rgbamount + 2)->amount;
			coloramount = calculateimagecoloramount(10);

			//只有一种颜色的图片，与其他图片分开处理，不用判断是否有噪线噪点，容易误判
			if (colornum1 > 350 && colornum2 < 102)
			{
				if ((color1->numb + color1->numg + color1->numr) >= 3)
				{
					//眉山，四川
					treatmentsamecolorline();
				}
				else
				{
					//铜川、延安、惠州、福建
					colortoblackwhite(10, 0, width - 1, 0);
					DropFall();
				}
			}
			else
			{
				noisespot = judgenoisespot();
				//判断噪线函数，目前只包括是否有噪线，不分粗细
				noiseline = judgenoiseline();
				if (noiseline == 0)
				{
					if (noisespot == 0)
					{
						//无噪点无噪线
						//常州，马鞍山
						treatmentimagetwowithoutspot();
					}
					else
					{
						//有噪点无噪线
						//暂无
					}
				}
				else
				{
					if (noiseline == 1)
					{
						//细噪线
						//金华，昆明，南通，贵州，湖北,车行易,1号店
						type = separateimagenoiseline(noisespot);
						if (type == 1)
						{
							//金华
							treatmentthinline();
						}
						if (type == 2)
						{
							//南通
							treatmentimagetwoline();
						}
						if (type == 3)
						{
							//湖北,车行易
							treatmentimagetwospot(noisespot);
						}
						if (type == 4)
						{
							//昆明
							treatmentthinlineliber();
						}
						if (type == 5)
						{
							//贵州
							discardcolorlump(4);
						}
						if (type == 6)
						{
							//1号店--空心字符
							treatmenthollowchar();
							blackAndWhiteExchange();//处理完毕之后色彩对换
							//colortoblackwhite(3, 0, width - 1, 255);
							//	grayprocess(128);
							//DropFall();
						}
					}
					else
					{
						//粗噪线
						//暂无
					}
				}
			}
		}
		void treatmentimagethree()
		{
			//单背景大图
			int  i, j, numb, numg, numr, tureposition;
			int  amountdiffer;
			imagergb p;
			rgbcolor s, q;
			findrgbposition();
			discardmakeupcolorspot();
			for (i = 1; i < height - 1; i++)
			{
				for (p = rgb + width*i + 1, j = 1; j < width - 1; p++, j++)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					tureposition = -1;
					q = rgbamount;
					for (s = rgbamount + 1; s - (rgbamount + 1) < 4; s++)
					{
						amountdiffer = imagecolordiffer(p, s);
						if (amountdiffer <= 4 && j - s->location <= 30 && j - s->location >= -30)
						{
							tureposition = 1;
							q = rgbamount;
							break;
						}
						else if (amountdiffer <= 4 && (j - s->location > 30 || j - s->location < -30))
						{
							tureposition = 0;
							q = s;
						}
					}
					if (tureposition == -1)
						discardextraline(p, q, i, j);
					else if (tureposition == 0)
						discardextraline(p, q, i, j);
				}
			}
			imagetransferblackwhite(rgbamount, 2);
			rgbamount->numb = 0;
			rgbamount->numg = 0;
			rgbamount->numr = 0;
			discardwidthnoise(10, 1, 2);
			discardheightnoise(10, 1, 2);
		}
		void treatmentimagefour()
		{
			//有透明度，颜色少的图片
			int i, j, k, s;
			imagergb p, q;
			if (countcolorvarietyamount == 1)
			{
				for (i = 1; i < height - 1; i++)
				{
					for (j = 1, p = rgb + i*width + 1; j < width - 1; j++, p++)
					{
						if (p->rgbReversed == 255)
						{
							p->rgbBlue = 255;
							p->rgbGreen = 255;
							p->rgbRed = 255;
						}
						else
						{
							p->rgbReversed = 255;
							for (k = 0; k < 3; k++)
							{
								for (s = 0, q = p + width*(k - 1) - 1; s < 2; s++, q++)
								{
									q->rgbBlue = 0;
									q->rgbGreen = 0;
									q->rgbRed = 0;
								}
							}
						}
					}
				}
			}
			else
			{
				//南京
				discardrgbspot(7, 10, 0);
				imagetransferblackwhite(rgbamount, 16);
			}
			blackwhitediscardrgbspot(6, 255);
			blackwhitediscardrgbspot(6, 255);
		}

		void treatmentimagetwospot(int spotnum)
		{
			int i, j;
			int amountdifferone, amountdiffertwo;
			int limit;
			imagergb p, q;
			rgbcolor linecolor;
			linecolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			linecolor->numb = 7;
			linecolor->numg = 9;
			linecolor->numr = 10;

			if (spotnum < 20)
			{
				limit = 6;
			}
			else
			{
				limit = 11;
			}

			//湖北
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					amountdifferone = imagecolordiffer(p, linecolor);
					amountdiffertwo = imagecolordiffer(p, rgbamount);
					if (amountdifferone < limit || amountdiffertwo < 10)
					{
						p->rgbBlue = rgbamount->numb * 15;
						p->rgbGreen = rgbamount->numg * 15;
						p->rgbRed = rgbamount->numr * 15;
					}
				}
			}
			if (spotnum >= 20)
			{
				discardcolorspot();
				changecolor(2);
				discardrgbspot(4, 8, 1);
				colortoblackwhite(4, 0, width - 1, 0);
				discardbinarylump(3);
				lettersegmentspace(4);
				lettersegmentline(4);
			}
		}
		void treatmentimagetwowithoutspot()
		{
			int maxwidth;
			int amountdiffer;
			amountdiffer = rgbamount->numb + rgbamount->numg + rgbamount->numr;
			if (amountdiffer < 15)
			{
				//深背景：昆明
				imagetransferblackwhite(rgbamount, 7);//7->12
			}
			else
			{
				//浅背景：常州，马鞍山
				imagetransferblackwhite(rgbamount, 12);//7->12
			}
			maxwidth = calculatemaxamountwidth();
			if (maxwidth >= 2)//字母较宽，新加
				blackwhitediscardrgbspot(6, 255);
			else//字母较细
				blackwhitediscardrgbspot(7, 255);//yuan7->xian5（宽字母）,yuan5->xian7(细字母)
		}
		void treatment58()
		{
			int i, j;
			int letterwidth;
			int acountspotamountone, acountspotamounttwo;
			imagergb p;

			acountspotamountone = calculatedifferspot(10, 5);
			acountspotamounttwo = calculatedifferspot(20, 5);

			letterwidth = calculateletterwidth();

			if (letterwidth <= 1)
			{
				//58汉字
				//将首尾没有汉字部分直接设为背景色
				for (i = 0; i < height; i++)
				{
					for (j = 0, p = rgb + i*width + j; j <= 12; j++, p++)
					{
						p->rgbBlue = rgbamount->numb * 15;
						p->rgbGreen = rgbamount->numg * 15;
						p->rgbRed = rgbamount->numr * 15;
					}
					for (j = 59, p = rgb + i*width + j; j <= 119; j++, p++)
					{
						p->rgbBlue = rgbamount->numb * 15;
						p->rgbGreen = rgbamount->numg * 15;
						p->rgbRed = rgbamount->numr * 15;
					}
				}
				if (acountspotamountone < 100)
				{
					imagetransferblackwhite(rgbamount, 10);
				}
				else if (acountspotamountone < 150)
				{
					imagetransferblackwhite(rgbamount, 14);
				}
				else if (acountspotamountone < 200)
				{
					imagetransferblackwhite(rgbamount, 16);
				}
				else if (acountspotamountone <= 250)
				{
					imagetransferblackwhite(rgbamount, 17);
				}
				else if (acountspotamountone < 300)
				{
					imagetransferblackwhite(rgbamount, 20);
				}
				else
				{
					imagetransferblackwhite(rgbamount, 21);
				}

				blackwhitediscardrgbspot(7, 255);
			}
			else
			{
				//58加减，字母
				if (acountspotamountone < 100)
				{
					imagetransferblackwhite(rgbamount, 16);
				}
				else if (acountspotamountone < 200)
				{
					imagetransferblackwhite(rgbamount, 18);
				}
				else
				{
					imagetransferblackwhite(rgbamount, 20);
				}
				blackwhitediscardrgbspot(6, 255);
				adjustposition();
			}
		}
		void treatmentimagetwoline()
		{
			//处理南通的函数，南通验证码已经发生改变，需要重新识别，此函数可删除
			int acountspotamountone, acountspotamounttwo;
			int heightnum1, heightnum2;
			acountspotamountone = calculatedifferspot(10, 5);
			acountspotamounttwo = calculatedifferspot(20, 5);
			heightnum1 = discardheightnoise(10, 0, 1);
			heightnum2 = discardheightnoise(10, 0, 2);
			cout << "height1:" << heightnum1 << "   " << "height2:" << heightnum2 << endl;
			if (acountspotamountone > 200 && acountspotamounttwo > 100)
			{
				discardrgbspot(4, 10, 0);
				imagetransferblackwhite(rgbamount, 30);
				blackwhitediscardrgbspot(5, 0);
				rotate();
			}
			else
			{
				imagetransferblackwhite(rgbamount, 16);
				blackwhitediscardrgbspot(5, 0);
			}
		}
		void treatmentthinlineliber()
		{
			//处理昆明细噪线位置不固定
			int i, j;
			int m, n;
			int amountdiffer, amountdifferone, amountdiffertwo;
			int vertspot[100], horzspot[50];
			int verti = 0, vertj = 0, horzi = 0, horzj = 0;
			int spotj = 0;
			int middle;
			int mark = 0;
			int firstmark = 0;
			int rgbnum = 0, linenum = 0, letternum = 0;
			int blacknum = 0;
			int start, end;
			imagergb p, q;
			rgbcolor linecolor;

			linecolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));

			//水平竖直方向投影
			for (i = 0; i < width; i++)
			{
				vertspot[i] = 0;
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer>3)
					{
						vertspot[i]++;
					}
				}
			}

			for (j = 0; j < height; j++)
			{
				horzspot[j] = 0;
				for (i = 0, p = rgb + j*width + i; i < width; i++, p++)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer>3)
					{
						horzspot[j]++;
					}
				}
			}


			//竖直水平方向求噪线颜色和坐标
			for (j = 0; j < height - 1; j++)
			{
				if ((horzspot[j] < 25 && horzspot[j + 1] < 25 && j < 9 && horzspot[j] != 0 && horzspot[j + 1] != 0)
					|| (j >= height - 3 && horzspot[j] != 0)
					|| (j >= height - 5 && horzspot[j] != 0 && horzspot[j] < 2))
				{
					if (j < 5)
					{
						horzj = j + 1;
					}
					else
					{
						horzj = j;
					}
					cout << "horzj:" << horzj << "    ";
					for (i = 0, p = rgb + horzj*width + i; i < width; i++, p++)
					{
						amountdiffer = imagecolordiffer(p, rgbamount);
						if (amountdiffer>2)
						{
							horzi = i;
							cout << "horzi:" << horzi << endl;
							linecolor->numb = p->rgbBlue / 15;
							linecolor->numg = p->rgbGreen / 15;
							linecolor->numr = p->rgbRed / 15;
							break;
						}
					}
					break;
				}
			}


			if (horzi == 0)
			{
				for (i = 0; i < width - 1; i++)
				{
					if (vertspot[i] == 1 && vertspot[i + 1] == 1)
					{
						verti = i;
						cout << "verti:" << verti << "    ";
						for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
						{
							amountdiffer = imagecolordiffer(p, rgbamount);
							if (amountdiffer>2)
							{
								vertj = j;
								cout << "vertj:" << vertj << endl;
								linecolor->numb = p->rgbBlue / 15;
								linecolor->numg = p->rgbGreen / 15;
								linecolor->numr = p->rgbRed / 15;
								break;
							}
						}
						break;
					}
				}
			}

			//去噪线
			if (verti == 0 && horzi == 0)
			{
				//噪线短,按粗细和颜色去噪线（可以忽略）
			}
			else
			{
				if (horzi != 0)
				{
					//水平坐标转为竖直坐标，按竖直去噪线
					verti = horzi;
					vertj = horzj;
					mark = 1;
				}


				if (verti != 0)
				{
					//竖直去噪线
					spotj = vertj;
					middle = spotj;
					if (mark == 0)
					{
						//竖直坐标去噪线
						int up, down;
						for (i = verti; i < width; i++)
						{
							vertj = middle;
							rgbnum = 0;
							linenum = 0;
							for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
							{
								amountdiffer = imagecolordiffer(p, linecolor);
								amountdifferone = imagecolordiffer(p, rgbamount);
								if (amountdifferone < 2)
								{
									rgbnum++;
								}
								if (amountdiffer < 3)
								{
									linenum++;
								}
							}
							if (rgbnum == 5)
							{
								break;
							}
							else
							{
								for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
								{
									amountdiffer = imagecolordiffer(p, linecolor);
									if (amountdiffer < 3)
									{
										up = imagecolordiffer(p - width, rgbamount);
										down = imagecolordiffer(p + width, rgbamount);
										if (up < 3 && down < 3)
										{
											p->rgbBlue = 255;
											p->rgbGreen = 255;
											p->rgbRed = 255;
											middle = j;
										}
										if (linenum == 1)
										{
											middle = j;
										}
									}
								}
							}
						}

						middle = spotj;
						for (i = verti - 1; i >= 0; i--)
						{
							vertj = middle;
							rgbnum = 0;
							linenum = 0;
							for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
							{
								amountdiffer = imagecolordiffer(p, linecolor);
								amountdifferone = imagecolordiffer(p, rgbamount);
								if (amountdifferone < 3)
								{
									rgbnum++;
								}
								if (amountdiffer < 3)
								{
									linenum++;
								}
							}
							if (rgbnum == 5)
							{
								break;
							}
							else
							{
								for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
								{
									amountdiffer = imagecolordiffer(p, linecolor);
									if (amountdiffer < 3)
									{
										up = imagecolordiffer(p - width, rgbamount);
										down = imagecolordiffer(p + width, rgbamount);
										if (up < 3 && down < 3)
										{
											p->rgbBlue = 255;
											p->rgbGreen = 255;
											p->rgbRed = 255;
										}
										if (linenum == 1)
										{
											middle = j;
										}
									}
								}
							}
						}
					}



					else
					{
						//水平转化为竖直坐标去噪线
						for (i = verti; i < width; i++)
						{
							rgbnum = 0;
							linenum = 0;
							letternum = 0;
							vertj = middle;
							for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 3; j++, p = p + width)
							{
								amountdifferone = imagecolordiffer(p, rgbamount);
								amountdiffertwo = imagecolordiffer(p, linecolor);
								if (amountdifferone < 3)
								{
									rgbnum++;
								}
								else if (amountdiffertwo < 3)
								{
									linenum++;
								}
								else
								{
									letternum++;
								}
							}

							if (rgbnum == 0)
							{
								//与字母粘连部分
								if (linenum != 0)
								{
									if (linenum < 6)
									{
										for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
										{
											amountdiffer = imagecolordiffer(p, linecolor);
											if (amountdiffer < 3)
											{
												middle = j;
											}
										}
									}
									else
									{
										continue;
									}
								}
								else
								{
									for (m = i + 1; m < width; m++)
									{
										//linenum = 0;
										for (n = vertj - 3, p = rgb + n*width + m; n < height; n++, p = p + width)
										{
											amountdiffer = imagecolordiffer(p, linecolor);
											if (amountdiffer < 3)
											{
												break;
											}
										}
										if (n < height)
										{
											i = m - 1;
											middle = n;
											break;
										}
									}
								}

							}
							else if (rgbnum == 6)
							{
								//空白的行（i）超过一定数目就结束
								blacknum++;
								if (blacknum > 3)
								{
									break;
								}
							}
							else
							{
								//没有与字母粘连部分
								int left, right, down, up;
								for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 3; j++, p = p + width)
								{
									amountdiffer = imagecolordiffer(p, linecolor);
									if (amountdiffer < 3)
									{
										left = imagecolordiffer(p - 1, rgbamount);
										right = imagecolordiffer(p + 1, rgbamount);
										up = imagecolordiffer(p - width, rgbamount);
										down = imagecolordiffer(p + width, rgbamount);
										if ((left < 3 && right < 3) || (up < 3 && down < 3))
										{
											p->rgbBlue = 255;
											p->rgbGreen = 255;
											p->rgbRed = 255;
											middle = j;
										}
										else
										{
											if (abs(j - middle) < 2)
											{
												middle = j;
											}

										}
									}
								}
							}
						}

						middle = spotj;
						blacknum = 0;
						for (i = verti - 1; i >= 0; i--)
						{
							vertj = middle;
							rgbnum = 0;
							linenum = 0;
							letternum = 0;
							for (j = vertj - 3, p = rgb + j*width + i; j <= vertj + 3; j++, p = p + width)
							{
								amountdifferone = imagecolordiffer(p, linecolor);
								amountdiffertwo = imagecolordiffer(p, rgbamount);
								if (amountdiffertwo < 3)
								{
									rgbnum++;
								}
								else if (amountdifferone < 3)
								{
									if (j == 0)
									{
										p->rgbBlue = 255;
										p->rgbGreen = 255;
										p->rgbRed = 255;
										break;
									}
									else
									{
										linenum++;
									}
								}
								else
								{
									letternum++;
								}
							}
							if (rgbnum == 7)
							{
								//空白的行（i）超过一定数目就结束
								blacknum++;
								if (blacknum > 2)
								{
									break;
								}
							}
							else if (rgbnum == 0)
							{
								if (linenum != 0)
								{
									if (linenum < 7)
									{
										for (j = vertj - 2, p = rgb + j*width + i; j <= vertj + 2; j++, p = p + width)
										{
											amountdiffer = imagecolordiffer(p, linecolor);
											if (amountdiffer < 3)
											{
												middle = j;
											}
										}
									}
									else
									{
										continue;
									}
								}
								else
								{
									for (m = i - 1; m > 0; m--)
									{
										//linenum = 0;
										for (n = vertj - 3, p = rgb + n*width + m; n < height; n++, p = p + width)
										{
											amountdiffer = imagecolordiffer(p, linecolor);
											if (amountdiffer < 3)
											{
												break;
											}
										}
										if (n < height)
										{
											i = m + 1;
											middle = n;
											break;
										}
									}
								}
							}
							else
							{
								if (linenum == 0)
								{
									for (m = i - 1; m >= 0; m--)
									{
										rgbnum = 0;
										for (n = 1, p = rgb + n*width + m; n < height - 1; p = p + width, n++)
										{
											amountdiffer = imagecolordiffer(p, linecolor);
											amountdifferone = imagecolordiffer(p, rgbamount);
											if (amountdiffer < 3)
											{
												break;
											}
											if (amountdifferone < 3)
											{
												rgbnum++;
											}
										}
										if (rgbnum >= height - 2)
										{
											m = 0;
											i = 0;
											break;
										}
										if (n < height - 1)
										{
											middle = n;
											i = m + 1;
											break;
										}
									}
								}
								else
								{
									int left, right, down, up;
									firstmark = 0;
									for (j = vertj - 3, p = rgb + j*width + i; j <= vertj + 3; j++, p = p + width)
									{
										amountdiffer = imagecolordiffer(p, linecolor);
										if (amountdiffer < 3)
										{
											left = imagecolordiffer(p - 1, rgbamount);
											right = imagecolordiffer(p + 1, rgbamount);
											up = imagecolordiffer(p - width, rgbamount);
											down = imagecolordiffer(p + width, rgbamount);
											if ((left < 3 && right < 3) || (up < 3 && down < 3))
											{
												p->rgbBlue = 255;
												p->rgbGreen = 255;
												p->rgbRed = 255;
												if (firstmark == 0)
												{
													middle = j;
													firstmark = 1;
												}
											}
											else
											{
												middle = j;
											}
										}
									}
								}
							}
						}
					}
				}
			}


			//去除特殊情况:竖直
			int num = 0;
			for (i = 1; i < width - 1; i++)
			{
				if (vertspot[i - 1] == 0 && vertspot[i]>10 && vertspot[i + 1] < 8)
				{
					for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
				}
				else
				{
					if (vertspot[i - 1] == 0 && vertspot[i] != 0)
					{
						for (j = i; j < width; j++)
						{
							if (vertspot[j] != 0)
							{
								num++;
								continue;
							}
							else
							{
								break;
							}
						}
						if (num < 7)
						{
							for (m = i; m < j; m++)
							{
								for (n = 0, p = rgb + n*width + m; n < height; n++, p = p + width)
								{
									p->rgbBlue = 255;
									p->rgbGreen = 255;
									p->rgbRed = 255;
								}
							}
						}
					}
				}
			}

			//分割字符以及后续处理
			int colorwidth[12];
			int maxinum, letterwidth;
			findletterposition(rgb, 0, 255, 2);

			for (i = 0; i < 6; i++)
			{
				if (startposition[i] != 0)
				{
					if (endposition[i] - startposition[i] <= 16)
					{
						for (j = 0; j < 12; j++)
						{
							colorwidth[j] = 0;
						}
						maxinum = 0;
						letterwidth = 0;
						//单个字符根据字符宽度决定与背景色的对比度
						for (m = 0; m < height; m++)
						{
							for (n = startposition[i], p = rgb + m * width + n; n <= endposition[i]; n++, p++)
							{
								amountdiffer = imagecolordiffer(p, rgbamount);
								if (amountdiffer >= 2)
								{
									num = 1;
									for (j = n + 1, q = p + 1; j <= endposition[i]; j++, q++)
									{
										amountdiffer = imagecolordiffer(q, rgbamount);
										if (amountdiffer > 1)
										{
											num++;
										}
										else
										{
											break;
										}
									}
									if (num <= 11 && num >= 1)
									{
										colorwidth[num]++;
									}
									n = j - 1;
									p = q - 1;

								}
							}
						}
						for (j = 0; j <= 11; j++)
						{
							if (colorwidth[j] >= maxinum)
							{
								maxinum = colorwidth[j];
								letterwidth = j;
							}
						}

						int distance;
						if (i == 0)
						{
							distance = 5;
							start = 0;
							end = endposition[i] - distance;
						}
						else if (i == 1)
						{
							distance = 4;
							start = endposition[i - 1] - distance;
							end = endposition[i] - distance;
						}
						else if (i == 2)
						{
							distance = 3;
							start = endposition[i - 1] - distance;
							end = endposition[i] - distance;
						}
						else
						{
							distance = 2;
							start = endposition[i - 1] - distance;
							end = width - 1;
						}
						Lettermove(startposition[i], endposition[i], distance, 1, -1);
						if (letterwidth <= 4)
						{
							colortoblackwhite(2, start, end, 0);
						}
						else
						{
							colortoblackwhite(20, start, end, 0);
						}
					}
					else
					{
						//存在字符粘连，分割字符(字符颜色不同）
						Lettersegment(startposition[i], endposition[i], i);
						i--;
					}
				}
			}
		}
		void treatmentthinline()
		{
			//浙江金华
			int i, j, numb, numg, numr, amountdifferone, amountdiffertwo;
			//用来标记前半部分/后半部分的噪线颜色是否找到；
			int priormark, endmark;
			int endmarkj = 1;
			rgbcolor priorlinecolor, endlinecolor;
			imagergb  p;
			priorlinecolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			endlinecolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			priormark = 0;
			endmark = 0;
			for (i = 1; i < width - 1; i++)
			{
				for (j = 1, p = rgb + width + i; j < height - 1; j++, p = p + width)
				{
					amountdifferone = imagecolordiffer(p, rgbamount);
					if (amountdifferone < 4)
					{
						continue;
					}
					else
					{
						if ((i == 1) && (priormark == 0))
						{
							//记录前半部分噪线的颜色
							priorlinecolor->numb = p->rgbBlue / 15;
							priorlinecolor->numg = p->rgbGreen / 15;
							priorlinecolor->numr = p->rgbRed / 15;
							priormark = j;
						}
						if ((i > width / 2) && (j == 1) && (endmark == 0))
						{
							//记录后半部分噪线的颜色
							endlinecolor->numb = p->rgbBlue / 15;
							endlinecolor->numg = p->rgbGreen / 15;
							endlinecolor->numr = p->rgbRed / 15;
							endmark = i;
						}
					}
				}
			}
			if (priormark != 0)
			{
				//对前半部分噪线进行去噪
				for (i = 1; i < width / 2; i++)
				{
					for (j = priormark, p = rgb + j*width + i; j <= priormark; j++)
					{
						amountdifferone = imagecolordiffer(p, priorlinecolor);
						amountdiffertwo = imagecolordiffer(p, rgbamount);
						if (amountdifferone < 4)
						{
							//在噪线位置与噪线颜色相同的直接去掉
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;

						}
						if ((amountdifferone>4) && (amountdiffertwo > 4))
						{
							//在噪线位置找到既不是背景色也不是噪线色，即为字母颜色，要保留
							continue;
						}
						if (amountdiffertwo < 4)
						{
							//在噪线位置找到背景色，则列位置往下搜索，并且此位置改变后重新查找一次
							priormark = priormark + 1;
							i--;
							break;
						}
					}
				}
			}
			if (endmark != 0)
			{
				//对后半部分噪线进行去噪
				for (i = endmark; i < width - 1; i++)
				{
					for (j = endmarkj, p = rgb + width * j + i; j <= endmarkj; j++)
					{
						amountdifferone = imagecolordiffer(p, endlinecolor);
						amountdiffertwo = imagecolordiffer(p, rgbamount);
						if (amountdifferone < 4)
						{
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;

						}
						if ((amountdifferone>4) && (amountdiffertwo > 4))
						{
							continue;
						}
						if (amountdiffertwo < 4)
						{
							endmarkj = endmarkj + 1;
							i--;
							break;
						}
					}
				}
			}
			colortoblackwhite(10, 0, width - 1, 0);
		}
		void treatmentboldblackline()
		{
			//厦门,处理效果不好没提交
			int i, j, s, k, amountdifferone, amountdiffertwo;
			int num, amountspot[120], startposition[4], endposition[4], start, end;
			int mark = 0;
			imagergb p, q;
			rgbcolor redcolor, blackcolor;
			redcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			blackcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			redcolor->numb = 0;
			redcolor->numg = 0;
			redcolor->numr = 255 / 15;
			blackcolor->numb = 0;
			blackcolor->numg = 0;
			blackcolor->numr = 0;
			start = 0;
			end = 0;
			for (i = 0; i < 120; i++)
			{
				amountspot[i] = 0;
				if (i <= 3)
				{
					startposition[i] = 0;
					endposition[i] = 0;
				}
			}

			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					if (i < 74 && i >= 20 && j>6 && j < 29)
					{
						amountdifferone = imagecolordiffer(p, redcolor);
						amountdiffertwo = imagecolordiffer(p, blackcolor);

						if (amountdifferone <= 15)//红色
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;
						}
						else if (amountdiffertwo <= 5)//黑色，分情况
						{
							if ((j - 2) >= 0 && (j + 2) < height)
							{
								for (k = j - 2, q = p - 2 * width; k <= j + 2; k++, q = q + width)
								{
									amountdifferone = imagecolordiffer(q, redcolor);
									if (amountdifferone <= 6)
									{
										p->rgbBlue = 0;
										p->rgbGreen = 0;
										p->rgbRed = 255;
										break;
									}
								}
								if (k > j + 2)
								{
									p->rgbBlue = 255;
									p->rgbGreen = 255;
									p->rgbRed = 255;
								}
							}
						}
						else
						{//其它颜色变为白色
							p->rgbBlue = 255;
							p->rgbGreen = 255;
							p->rgbRed = 255;
						}

					}
					else
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}

				}
			}


			//统计各列像素点总数
			for (i = 0; i < width; i++)
			{
				num = 0;
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 0)
					{
						num++;
					}
				}
				amountspot[i] = num;
			}
			//计算存储字母左右开始结束位置（列）

			for (i = 20; i < 74; i++)
			{
				//cout << amountspot[i] << "    ";
				if (amountspot[i] <= 2 && amountspot[i + 1]>2 && start < 4)
				{
					startposition[start++] = i;
					cout << startposition[start - 1] << "   ";
				}
				if (amountspot[i] > 2 && amountspot[i + 1] <= 2 && end < 4)
				{
					endposition[end++] = i + 1;
					cout << endposition[end - 1] << "   ";
				}

			}
			cout << endl;

		}
		void treatmentsamecolorline()
		{
			//四川，眉山，（部分无锡），处理效果不好没提交
			int i, j, k, s, t = 0, amountdiffer, markup = 0, markdown = 0;
			int mark1 = 0, mark2 = 0, mark3 = 0;
			int num = 0, num1 = 0;
			imagergb p, q, r;
			rgbcolor linecolor;
			//linecolor存放的是图片中的深色，深色相对于浅色数量比较多
			linecolor = rgbamount + 1;

			//从上往下扫描
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i + j*width; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer <= 4)
					{
						//背景色
						p->sign = 0;
						continue;//背景色保持不变
					}
					else
					{
						amountdiffer = imagecolordiffer(p, linecolor);
						if (amountdiffer >= 3)
						{
							//浅色
							p->sign = 1;
							//将同行中的一些深色变为浅色
							num = 0;
							num1 = 0;
							for (r = p + 1;; r++)
							{
								amountdiffer = imagecolordiffer(r, rgbamount);
								if (amountdiffer < 4 || num1 > 5)
								{
									break;
								}
								else
								{
									amountdiffer = imagecolordiffer(r, linecolor);
									if (amountdiffer >= 3)
									{
										num++;
										continue;
									}
									else
									{
										num1++;
										continue;
									}
								}
							}
							if (num >= 3)
							{
								//如果统计的该行淡色超过3，则将该行后续都变为淡色
								for (r = p + 1;; r++)
								{
									amountdiffer = imagecolordiffer(r, rgbamount);
									if (amountdiffer < 4)
									{
										break;
									}
									else
									{
										amountdiffer = imagecolordiffer(r, linecolor);
										if (amountdiffer < 3)
										{
											r->rgbBlue = p->rgbBlue;
											r->rgbGreen = p->rgbGreen;
											r->rgbRed = p->rgbRed;
										}
										else
										{
											break;
										}
									}
								}
							}

							for (k = j + 1, q = p + width; k < height; k++, q = q + width)
							{
								amountdiffer = imagecolordiffer(q, rgbamount);
								if (amountdiffer > 4)//仍然为蓝色则继续往下查找
								{
									q->sign = 1;
									continue;
								}
								else//查找的点为背景色，开始下一段的查找
								{
									q->sign = 0;
									p = q;
									j = k;
									break;
								}
							}
							if (k == height)
							{
								break;
							}
						}
						else
						{
							//深色
							//查找深色同行周围是否有浅色
							for (s = i - 1, r = p - 1; s <= i + 1; s++, r++)
							{
								amountdiffer = imagecolordiffer(r, rgbamount);
								if (amountdiffer < 4)
								{
									continue;
								}
								else
								{
									amountdiffer = imagecolordiffer(r, linecolor);
									if (amountdiffer < 3)
									{
										continue;
									}
									else
									{
										mark1 = 1;
										break;
									}
								}
							}

							//深色：查找该列结束的位置是否为浅色，或者结束位置是否为边界，则保留该列
							for (k = j + 1, q = p + width; k < height; k++, q = q + width)
							{
								amountdiffer = imagecolordiffer(q, rgbamount);
								if (amountdiffer < 4)
								{
									break;
								}
								else
								{
									amountdiffer = imagecolordiffer(q, linecolor);
									if (amountdiffer < 3)
									{
										if (k == height)
										{
											mark2 = 1;
											break;
										}
										else
										{
											continue;
										}
									}
									else
									{
										mark2 = 1;
										break;
									}
								}
							}

							if (mark1 == 1 || mark2 == 1)
							{
								markup = 1;
							}

							if (markup == 1)
							{
								p->sign = 1;
							}
							else
							{
								p->sign = 0;
							}
							for (k = j + 1, q = p + width; k < height; k++, q = q + width)
							{
								amountdiffer = imagecolordiffer(q, rgbamount);
								if (amountdiffer < 4)
								{
									p = q;
									j = k;
									p->sign = 0;
									markup = 0;
									mark1 = 0;
									mark2 = 0;
									break;
								}
								else
								{
									if (markup == 1)
									{
										q->sign = 1;
									}
									else
									{
										q->sign = 0;
									}
									continue;
								}
							}
							if (k == height)
							{
								break;
							}
						}
					}
				}
			}


			//从下往上扫描
			for (i = 0; i < width; i++)
			{
				for (j = height - 1, p = rgb + j*width + i; j >= 0; j--, p = p - width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					//背景色
					if (amountdiffer < 4)
					{
						p->sign = 0;
						continue;
					}
					else
					{
						amountdiffer = imagecolordiffer(p, linecolor);
						if (amountdiffer < 3)
						{
							//深色
							for (s = i - 1, r = p - 1; s <= i + 1; s++, r++)
							{
								amountdiffer = imagecolordiffer(r, rgbamount);
								if (amountdiffer < 4)
								{
									continue;
								}
								else
								{
									amountdiffer = imagecolordiffer(p, linecolor);
									if (amountdiffer < 3)
									{
										continue;
									}
									else
									{
										markdown = 1;
										break;
									}
								}
							}
							for (k = j, q = p; k >= 0; k--, q = q - width)
							{
								amountdiffer = imagecolordiffer(q, rgbamount);
								if (amountdiffer > 4)
								{
									if (markdown == 1 || j == height - 1)
									{
										q->sign = 1;
										continue;
									}
									else
									{
										if (q->sign == 1)
											continue;
										else
										{
											q->sign = 0;
											continue;
										}
									}
								}
								else
								{
									q->sign = 0;
									p = q;
									j = k;
									markdown = 0;
									break;
								}
							}
						}
						else
						{
							//浅色
							p->sign = 1;
							for (k = j - 1, q = p - width; k >= 0; k--, q = q - width)
							{
								amountdiffer = imagecolordiffer(q, rgbamount);
								if (amountdiffer > 4)
								{
									q->sign = 1;
									continue;
								}
								else
								{
									q->sign = 0;
									p = q;
									j = k;
									break;
								}
							}
						}
					}
				}
			}


			//显示图片
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					if (p->sign == 0)
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
					else
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
				}
			}
		}


		//黑白点的色彩对换
		void blackAndWhiteExchange(){
			imagergb p;
			int i, j;
			int sum;
			for (i = 0; i < height; i++)
			{
				for (j = 0, p = rgb + i*width; j < width; j++, p++)
				{
					if (p->rgbBlue == 0){
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
					else{

						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
				}
			}

		}

		void upRepatching(int* sign){

			int i, j;
			int m, n;
			int t;
			int amountdiffer;
			int num;
			int mark;
			//断点的起点终点
			int starti, startj, endi, endj;
			//断线的斜率和当前要填补点的坐标
			float ratio;
			int currenti, currentj;
			imagergb p, q, r;
			//、、、、、、、、、、、、begin  下面的源码注释可参考downRepatching函数的注释，两个的大体一致，仅仅在与细微的修改

			//上端线修部分图片(缺口小)轮廓线
			for (i = 0; i < width; i++)
			{
				if (sign[i] != 0 && sign[i] != -1)
				{
					num = 1;
					for (j = i + 1; j < width; j++)
					{
						if (sign[j] != 0 && sign[j] != -1)
						{
							num++;
						}
						else
						{
							break;
						}
					}
					//封闭字符与开放字符区分开
					if (num < 7)
					{
						i = j - 1;
					}
					else
					{
						//开放字符补缺
						int next;
						for (m = i; m < j; m++)
						{
							next = m + 1;
							if (next == 83){

								next = next;
							}
							if (sign[next] - sign[m] >= 5)
							{

								//记录起点


								repatchingSolution(sign, &m, &next, &starti, &startj, &endi, &endj, j);
								//starti = m;
								//startj = sign[starti];

								//for (n = next + 1; n < j; n++)
								//{
								//	if (n == j - 1)
								//	{
								//		endi = n;
								//		endj = sign[endi];
								//		m = n;
								//		break;
								//	}
								//	else if (abs(sign[n] - sign[starti]) >= 3)
								//	{
								//		continue;
								//	}
								//	else
								//	{

								//		//记录终点
								//		endi = n;
								//		endj = sign[endi];
								//		m = n - 1;
								//		break;
								//	}
								//}
								//补线
								lineEquationRepatching(starti, startj, endi, endj);

							}
						}
						i = m - 1;
					}
				}
			}
		}

		//坐标两点构建一条直线，直线经过的点修补为蓝色
		void lineEquationRepatching(int starti, int startj, int  endi, int endj){


			int t;
			imagergb q;
			//补线
			double ratio = (endj - startj)*1.0 / (endi - starti);

			int currentj, currenti;
			if (ratio > 1.0)
			{
				for (int size = startj; size <= endj; size++)
				{
					currentj = size;   //0-height

					currenti = int(ceil((currentj - startj)*1.0 / ratio*1.0 + starti*1.0));  //0-width

					q = rgb + currentj*width + currenti;
					q->rgbBlue = 255;
					q->rgbGreen = 0;
					q->rgbRed = 0;

					if (t < height - 2){     //lai

						(q + 1)->rgbBlue = 255;
						(q + 1)->rgbGreen = 0;
						(q + 1)->rgbRed = 0;

						(q + width + 1)->rgbBlue = 255;
						(q + width + 1)->rgbGreen = 0;
						(q + width + 1)->rgbRed = 0;

						(q + 2 * width + 1)->rgbGreen = 0;
						(q + 2 * width + 1)->rgbRed = 0;
						(q + 2 * width + 1)->rgbBlue = 255;

					}
				}
			}
			else{

				for (t = starti; t <= endi; t++)
				{
					currenti = t;
					currentj = (int)(ratio*(t - starti) + startj);
					q = rgb + currentj*width + currenti;
					q->rgbBlue = 255;
					q->rgbGreen = 0;
					q->rgbRed = 0;

					if (t < height - 2){     //lai

						(q + 1)->rgbBlue = 255;
						(q + 1)->rgbGreen = 0;
						(q + 1)->rgbRed = 0;

						(q + width + 1)->rgbBlue = 255;
						(q + width + 1)->rgbGreen = 0;
						(q + width + 1)->rgbRed = 0;

						(q + 2 * width + 1)->rgbGreen = 0;
						(q + 2 * width + 1)->rgbRed = 0;
						(q + 2 * width + 1)->rgbBlue = 255;

					}




				}

			}
		}


		//此函数单纯为了函数的可重复利用,用于判断确认修补方程的终点，起点；需要结合引用此函数的上面逻辑才可看这部分，引用指针传参，只要在于修改原值
		void repatchingSolution(int *sign, int *m, int* next, int *starti, int *startj, int  *endi, int *endj, int duanJ){
		
			//记录起点
			*starti = *m;
			*startj = sign[*starti];

			for (int n = *next + 1; n < duanJ; n++)
			{
				if (n == duanJ - 1)
				{
					*endi = n;
					*endj = sign[*endi];
					*m = n;
					break;
				}
				else if (abs(sign[n] - sign[*starti]) >= 3)     //连续遍历的两个sign[n] 如果与开始的sign[starti]的x坐标差大于三则继续遍历，知道小于3或者遍历到最后一个
				{
					continue;
				}
				else
				{
					//记录终点
					*endi = n;
					*endj = sign[*endi];
					*m = n - 1;
					break;
				}
			}
		
		}


		//字符的下端线段缺少的补充，思路在上端线补充的基础之上修改，上端线在treatmenthollowchar函数体体现。
		void downRepatching(){
			int i, j;
			int m, n;
			int t;
			int amountdiffer;
			int num;
			int mark;
			//断线的斜率和当前要填补点的坐标
			float ratio;
			int currenti, currentj;
			//断点的起点终点
			int starti, startj, endi, endj;
			int sign[Nlength];
			imagergb p, q, r;
			for (i = 0; i < width - 1; i++)
			{
				sign[i] = 0;
				int nn = 0;   //统计每个黑点周边的九个点是否都不是白点，则nn=9;
				bool kongzhiyu = false;
				for (j = height - 1, p = rgb + j*width + i; j > 0; j--, p = p - width)
				{
					if (p->rgbBlue == 0) //当前点为黑点
					{
						//if (r->rgbBlue != 0)continue;
						if (sign[i] == 0)          //标示不存在数据，则保存数量为j
						{
							sign[i] = j;
						}

						r = p - 5 * width;   //由于下端存在阴影大边界，p - 5 * width表示去除5个阴影点，开始计算
						mark = 0;

						for (m = -1; m <= 1; m++)   ///扫描当前点的三维矩阵9个点
						{
							for (n = -1, q = r + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue != 255 && q->rgbRed != 255)
								{
									nn++;
								}
							}
						}
						if (nn != 9)
						{
							sign[i] = -1;

						}

						else{
							kongzhiyu = true;   //控制行仅仅遍历成功一次后直接跳出
						}

						nn = 0;
					}

					if (kongzhiyu)break;
				}
			}


			//修部分图片(缺口小)轮廓线
			for (i = 0; i < width; i++)
			{
				if (sign[i] != 0 && sign[i] != -1)
				{
					num = 1;
					for (j = i + 1; j < width; j++)
					{
						if (sign[j] != 0 && sign[j] != -1)
						{

							num++;
						}
						else
						{
							break;
						}
					}
					//封闭字符与开放字符区分开
					if (num < 7)                                 //连续7个sign[i]>1才开始修复
					{
						i = j - 1;
					}
					else
					{
						//开放字符补缺
						int next;
						for (m = i; m < j; m++)
						{
							next = m + 1;
							if (sign[next] - sign[m] <= -5 )      //相连两个sign[next] ， sign[m]间的x坐标差小于五
							{
								//记录起点
								repatchingSolution(sign, &m, &next, &starti, &startj, &endi, &endj, j);

								//starti = m;
								//startj = sign[starti];

								//for (n = next + 1; n < j; n++)
								//{
								//	if (n == j - 1)
								//	{
								//		endi = n;
								//		endj = sign[endi];
								//		m = n;
								//		break;
								//	}
								//	else if (abs(sign[n] - sign[starti]) >= 3)
								//	{
								//		continue;
								//	}
								//	else
								//	{

								//		//记录终点
								//		endi = n;
								//		endj = sign[endi];
								//		m = n - 1;
								//		break;
								//	}
								//}
								//补线
								lineEquationRepatching(starti, startj, endi, endj);
							}

							
						}
						i = m - 1;
					}
				}
			}
		}


		void changColor(int colorOne, int colorTwo)
		{
			imagergb p;
			int i, j;
			int sum;
			for (i = 0; i < height; i++)
			{
				for (j = 0, p = rgb + i*width; j < width; j++, p++)
				{
					if (p->rgbBlue == colorOne)
					{
						p->rgbBlue = colorTwo;
						p->rgbGreen = colorTwo;
						p->rgbRed = colorTwo;
					}
				}
			}
		}
		//下端线开始修补前，需要处理一些背景色，目的为了回到修补上线时候同样的场景
		void	dealBeforeDownRepatching(){

			ColorFillingbinary(0, 0, 100, 180);

			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}.bmp",100));
			imagergb p, q;
			int i, j, m, n;
			int sum;
			for (i = 0; i < width - 1; i++)
			{
				for (j = height - 1, p = rgb + j*width + i; j > 0; j--, p = p - width)
				{
					if (p->rgbBlue == 0) //当前点为黑点
					{
						int mark = 0;
						for (m = -1; m <= 1; m++)   ///扫描当前点的三维矩阵9个点
						{
							for (n = -1, q = p + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue == 100)
								{
									mark = 1;
								}
							}
						}
						if (mark == 1) ///扫描当前点的三维矩阵9个点，如果有一个点为不同点则标示为1，则当前点设置颜色变换,标记为红色（255，0，0）
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;

						}
					}
				}
			}
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}.bmp", 99));
			changColor(100, 255);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}.bmp", 101));
			ColorFillingbinary(0, 0, 180, 100);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}.bmp", 102));
		}

		//垂直方向缺口修补
		void 	verticalRepatching()
		{

			imagergb p;
			int i, j;
			//垂直方向缺口补填，仅仅补填缺口的大小在三个像素之内的。，遍历每个黑点的上面三个点是否为灰色的情况下补充
			for (i = 0; i < width - 1; i++)
			{
				for (j = 3, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					int nSum = 0;

					if (p->rgbBlue == 0){
						for (int ni = -1; ni > -4; ni--)
						{

							if (((p + ni * width)->rgbBlue != 0 && (p + ni * width)->rgbBlue != 255 && (p + (ni - 1)* width + 1)->rgbBlue == 0) || ((p + ni * width)->rgbBlue != 0 && (p + ni * width)->rgbBlue != 255 && (p + (ni - 1)* width + 1)->rgbBlue == 0)){
								nSum++;
							}
						}
						if (nSum == 0)break;


						for (int nj = 1; nj <= nSum; nj++)
						{
							(p - nj*width)->rgbBlue = 255;
							(p - nj*width)->rgbRed = 0;
							(p - nj*width)->rgbGreen = 0;

						}
					}
				}
			}
		}

		void 	verticalRepatching2(){
			imagergb p;
			int i, j;
			//垂直方向缺口补填，仅仅补填缺口的大小在三个像素之内的。，遍历每个黑点的上面三个点是否为灰色的情况下补充
			for (i = 3; i < width - 1; i++)
			{

				for (j = 3, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					int nSum = 0;

					if (p->rgbBlue == 0 /*&& (p - width - 1)->rgbBlue != 0 && (p - width + 1)->rgbBlue != 0*/)
					{
						for (int ni = -1; ni >= -4; ni--)
						{

							if ((p + ni * width)->rgbBlue == 100)
							{
								//if ((p + ni * width)->rgbBlue == 0)break;

								if ((p + ni * width - 1)->rgbBlue == 0 || (p + ni * width + 1)->rgbBlue == 0 || (p + (ni - 1) * width)->rgbBlue == 0
									|| (p + (ni - 1) * width - 1)->rgbBlue == 0 || (p + (ni - 1) * width + 1)->rgbBlue == 0 // ||
									/*(p + ni * width - 1)->rgbRed == 100 || (p + ni * width + 1)->rgbRed == 100 || (p + (ni - 1) * width)->rgbRed == 100
									|| (p + (ni - 1) * width - 1)->rgbRed == 100 || (p + (ni - 1) * width + 1)->rgbRed == 100*/
									)
								{
									nSum++;

								}


							}


							if (ni == -4)
							{
								nSum = 0;
							}

						}



						for (int nj = 1; nj <= nSum; nj++)
						{
							(p - nj*width)->rgbBlue = 255;
							(p - nj*width)->rgbRed = 0;
							(p - nj*width)->rgbGreen = 0;

						}
					}
				}
			}

			////////////////////////////////////////


		}

		//最后一行提上来，防止填充字符底部与边界形成大噪块
		void upEndline(){
			imagergb imagerRGB = getImagergb();

			//最后一行提上来，防止填充字符底部与边界形成大噪块
			int i, j, m, num;
			imagergb p, q, r;
			for (i = 0, p = imagerRGB + (height - 1)*width; i < width; i++, p++)
			{
				if (p->rgbBlue == 0)
				{

					if ((p - width)->rgbBlue == 255)
					{
						(p - width)->rgbBlue = 0;
						(p - width)->rgbGreen = 0;
						(p - width)->rgbRed = 0;
					}
				}
				else
				{
					num = 0;

					for (j = i, q = p; j < width; j++, q++)
					{
						if (q->rgbBlue == 255)
						{
							num++;
						}
						else
						{
							break;
						}
					}


					if (num <= 9)
					{

						for (m = i, r = p; m < j; m++, r++)
						{

							r->rgbBlue = 0;
							r->rgbGreen = 0;
							r->rgbRed = 0;

							(r - width)->rgbBlue = 0;
							(r - width)->rgbGreen = 0;
							(r - width)->rgbRed = 0;
						}
					}
					i = j - 1;
					p = --q;
				}
			}

		}

		//给sign数组赋值，并修改start与end的值
		void dealSignArry(int* sign, int *start, int *end){

			imagergb imagerRGB = getImagergb();

			int i, j, m, num, mark, n;
			imagergb p, q, r;
			int nn = 0;
			bool kongzhiyu;
			for (i = 0; i < width - 1; i++)
			{
				sign[i] = 0;
				nn = 0;
				kongzhiyu = false;
				p = imagerRGB + i;
				for (j = 0; j < height - 1; j++, p = p + width)
				{
					if (p->rgbBlue == 0) //当前点为黑点
					{
						if (sign[i] == 0)          //标示不存在数据，则保存数量为j
						{
							sign[i] = j;
						}
						mark = 0;

						for (m = -1; m <= 1; m++)   ///扫描当前点的三维矩阵9个点，如果有一个点为白点则标示为1
						{

							for (n = -1, q = p + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue == 255)
								{
									mark = 1;

								}
								else{
									nn++;
								}

							}

						}

						if (nn == 9){
							kongzhiyu = true;//控制第一次赋值sign[i]的一个阈值
						}
						if (mark == 1) ///扫描当前点的三维矩阵9个点，如果有一个点为不同点则标示为1，则当前点设置颜色变换,标记为红色（255，0，0）
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;


							if (!kongzhiyu){ sign[i] = -1; }                                //标示重新变为-1

						}
						nn = 0;
					}
				}
				if (*start == 0 && sign[i] != 0)                            //整个start end  就是字符串出现的列坐标的开始与结束的纵坐标
				{
					//记录字符开始位置，便于移动设置字符间距离
					*start = i;
				}
				if (sign[i] != 0 && i < width - 1)
				{
					//记录字符结束位置，便于后续移动
					*end = i;
				}

			}


		}


		void treatmenthollowchar()
		{
			//处理空心字符（如1号店）
			int i, j;
			int m, n;
			int t;
			int amountdiffer;
			int num;
			int mark;
			//断线的斜率和当前要填补点的坐标
			float ratio;
			int currenti, currentj;
			//断点的起点终点
			int starti, startj, endi, endj;
			//字符范围，记录的是列
			int start, end;
			int sign[Nlength];
			imagergb p, q, r;

			//二值化     参数中6 在多次的尝试后选择的，没有具体的参考信息
			colortoblackwhite(6, 0, width - 1, 255);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}二值化.bmp", 0));
			//图片变大
			int newwidth = 150, newheight = 100; //需超出原图像的15个高度
			rgb = toBigImagergb(rgb, newwidth, newheight);

			//width = newwidth;
			//height = newheight;

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}jiada.bmp", 0));


			////最后一行提上来，防止填充字符底部与边界形成大噪块
			upEndline();

			//////////////////////////

			for (i = 0, p = rgb + (height - 1)*width; i < width; i++, p++)     //最后一行设置为全白背景
			{
				p->rgbBlue = 255;
				p->rgbGreen = 255;
				p->rgbRed = 255;
			}

			for (j = 0, p = rgb + width - 1; j < height; j++, p = p + width)      ///最后一列设置为全黑背景
			{
				p->rgbBlue = 0;
				p->rgbGreen = 0;
				p->rgbRed = 0;
			}

			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}提高一行.bmp", 1));

			ColorFillingbinary(0, 0, 255, 100);
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}递归填充灰色.bmp", 2));

			loopnum = 0;
			discardbinarylump(60);
			start = 0;
			end = 0;

			dealSignArry(sign, &start, &end);
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}封闭字符边界变红.bmp", 3));

			//、、、、、、、、、、、、begin  下面的源码注释可参考downRepatching函数的注释，两个的大体一致，仅仅在与细微的修改

			//上端线修部分图片(缺口小)轮廓线
			upRepatching(sign);
			verticalRepatching();

			//ColorFillingbinary(0, 0, 255, 100);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\图形处理2\\处理后图\\{0}shangxianxiubu.bmp", 301));
			dealBeforeDownRepatching();
			downRepatching();
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}下线修补之后.bmp", 5));
			//填充
			ColorFillingbinary(0, 0, 100, 0);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}灰色递归灰色填充黑色.bmp", 6));

			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 255 && p->rgbGreen == 255)
					{
						continue;
					}
					else if (p->rgbBlue == 100)
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
					else
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}

				}
			}


		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}.bmp", 7));

			//去噪块
			discardbinarylump(60);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}去噪后.bmp", 8));



			//移动字符

			//1)整体移动到末尾位置
			int distance, interval;
			distance = width - end - 2;
			Lettermove(0, width - 1, distance, 0, 1);//1)整体字符串移动到末尾位置

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}移动位置后.bmp", 9));
			//2)位置重叠的字符拉开
			num = 0;
			start = 0;
			end = 0;
			//interval = start - 2 + distance;




			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						num++;
						//	if (num>4)break;  //控制仅仅分离四个有效，当大于五个之后，下面距离计算出问题。
						ColorFillingbinary(j, i, 255, num * 10);
						findletterposition(rgb, 1, num * 10, 0);
						distance = startposition[0] - end - 12;
						movecolorletter(num * 10, distance);
						ColorFillingbinary(j, i - distance - 1, num * 10, 255);
						end = endposition[0] - distance;
						i = endposition[0] - 1;
						break;
					}
				}


			}


			//旋转前修补
			for (i = 1; i < width - 1; i++)
			{
				for (j = 1, p = rgb + j*width + i; j < height - 1; j++, p = p + width)
				{
					if (p->rgbBlue == 0)
					{
						num = 0;
						for (m = -1; m <= 1; m++)
						{
							for (n = -1, q = p + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue == 255)
								{
									num++;
								}
							}
						}
						if (num >= 5)
						{
							p->rgbBlue = 255;
							p->rgbGreen = 255;
							p->rgbRed = 255;
						}
					}
				}
			}

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}分离.bmp", 10));
			//旋转
			rotate();
//
			writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\图形处理2\\处理后图\\{0}旋转后.bmp", 10));
			//去旋转造成的散点
			discardbinarylump(30);
		}



		/// <summary>
		///  把一个图像转移到一个大背景中去
		/// </summary>
		/// <param name="rgb">旧图形，小的</param>
		/// <param name="newwidth">新图像的宽度</param>
		/// <param name="newheight">新图像的高度，一定要超出旧图像高度的15个像素，否则会报错</param>
		/// <returns>新的大背景图像</returns>
		imagergb toBigImagergb(imagergb rgb, int  newwidth, int newheight){

			//	Bitmap^ bitMap = gcnew Bitmap(newwidth, newheight);
			int imgwidth = newwidth;
			int imgheight = newheight;

			imagergb bigRGB = (imagergb)malloc((imgwidth *imgheight)*sizeof(RGBQUADS));

			imagergb pG = bigRGB;
			for (int i = 0; i < imgheight; i = i + 1)
			{
				for (int j = 0; j < imgwidth; j = j + 1)
				{

					pG->rgbBlue = 255;
					pG->rgbGreen = 255;
					pG->rgbRed = 255;
					pG++;
				}
			}

			imagergb p, q;

			freergb = rgb;
			for (int i = 0; i < width; i = i + 1)
			{
				p = bigRGB + 15 * imgwidth + i;
				q = rgb + i;
				for (int j = 0; j < height; j = j + 1)
				{
					p->rgbBlue = q->rgbBlue;
					p->rgbGreen = q->rgbGreen;
					p->rgbRed = q->rgbRed;
					p->Grayvalue = q->Grayvalue;
					p->rgbReversed = q->rgbReversed;
					p->sign = q->sign;


					p = p + imgwidth;
					q = q + width;

				}
			}
			setImgWidth(newwidth);
			getImgHeight(newheight);
			return bigRGB;
		}

		int separateimagenoiseline(int noisespot)
		{
			//区分金华，昆明，南通，贵州，湖北,车行易，1号店
			int i, j;
			int partamount = 0;
			int num = 0;
			//短线段数量
			int spotnum = 0;
			int smallnum = 0;
			int mark = 0;
			findletterposition(rgb, 0, 255, 3);
			for (i = 0; i < width - 1; i++)
			{
				if (spotamount[i] != 0 && spotamount[i + 1] == 0)
				{
					partamount++;
				}
				if (spotamount[i] >= 10)
				{
					num++;
				}
				if (spotamount[i] <= 1)
				{
					smallnum++;
				}
				if ((spotamount[i] == 0 && spotamount[i + 1] != 0) || (i == 0 && spotamount[i] != 0))
				{
					mark = 0;
					for (j = i + 1; j < width - 1; j++)
					{
						if (spotamount[j] != 0)
						{
							if (spotamount[j] < 5)
							{
								continue;
							}
							else
							{
								mark = 1;
								break;
							}
						}
						else
						{
							break;
						}
					}
					if ((j - i - 1 < 4) && mark == 0)
					{
						spotnum++;
					}
				}
			}

			if (partamount <= 2 && noisespot == 0)
			{
				if (num<4)
				{
					return 1;
				}
				else
				{
					return 6;
				}
			}
			else if (num>100)
			{
				return 2;
			}
			else
			{
				if (noisespot > 16)
				{
					if (num > 20)
					{
						return 3;
					}
					else
					{
						return 5;
					}
				}
				else
				{
					if (partamount <= 3 && smallnum < 20)
					{
						return 3;
					}
					else
					{
						return 4;
					}
				}
			}
		}

		//颜色
		void bubblesort(int countamount)
		{
			//冒泡排序
			int i, j, numb, numg, numr, amount;
			rgbcolor q, s;

			rgbcolor Rgbamount = getRgbamount();

			for (i = 0, q = Rgbamount; i < countamount - 1; i++, q++)
			{
				for (j = i + 1, s = q + 1; j < countamount; j++, s++)
				{
					if (s->amount>q->amount)
					{
						numb = q->numb;
						numg = q->numg;
						numr = q->numr;
						amount = q->amount;
						q->numb = s->numb;
						q->numg = s->numg;
						q->numr = s->numr;
						q->amount = s->amount;
						s->numb = numb;
						s->numg = numg;
						s->numr = numr;
						s->amount = amount;
					}
				}
			}
		}


		int diff(int i, int j){
			return i > j ? (i - j) : (j - i);
		}


		//计算两个像素点间的颜色差
		int countstatisticscolor(rgbcolor p, rgbcolor q)
		{
			return diff(p->numb, q->numb) + diff(p->numr, q->numr) + diff(p->numg, q->numg);
		}


		int CompareTo(RGB_COLOR a, RGB_COLOR b)
		{
			return a.amount.CompareTo(b.amount);
		}


		int imagecolordiffer(imagergb p, rgbcolor q)
		{
			int  amountdiffer;
			rgbcolor imagergbcolor;
			imagergbcolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			setRgbColorDive15(imagergbcolor, p);
			amountdiffer = countstatisticscolor(imagergbcolor, q);
			return amountdiffer;
		}

		int rgbdiffer(imagergb p, imagergb q)
		{
			int  amountdiffer;
			rgbcolor imagergbcolorone, imagergbcolortwo;
			imagergbcolorone = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			imagergbcolortwo = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));

			setRgbColorDive15(imagergbcolorone, p);
			setRgbColorDive15(imagergbcolortwo, q);

			amountdiffer = countstatisticscolor(imagergbcolorone, imagergbcolortwo);
			free(imagergbcolorone);
			free(imagergbcolortwo);
			return amountdiffer;
		}


		//设置颜色数量
		void setRgbamount(int statisticsrgb[17][17][17], int firstcountamount){

			rgbamount = (rgbcolor)malloc((firstcountamount)*sizeof(RGB_AMOUNT));

			int i, j, k;
			rgbcolor q;
			for (i = 0, q = rgbamount; i < 17; i++)
			{
				for (j = 0; j < 17; j++)
				{
					for (k = 0; k < 17; k++)
					{
						if (statisticsrgb[i][j][k]>0)
						{
							q->numb = i;
							q->numg = j;
							q->numr = k;
							q->amount = statisticsrgb[i][j][k];
							q->location = 0;
							q++;
						}
					}
				}
			}

		}

		//获取颜色数量
		rgbcolor getRgbamount(){
			rgbcolor amount = rgbamount;
			return amount;
		}

		int originalstatisticsrgbcolor()
		{
			int i, j, k, numb, numg, numr, firstcountamount = 0;
			int statisticsrgb[17][17][17];
			imagergb p;
			rgbcolor q;
			for (i = 0; i < 17; i++)
			{
				for (j = 0; j < 17; j++)
				{
					for (k = 0; k < 17; k++)
					{
						statisticsrgb[i][j][k] = 0;
					}
				}
			}


			//获取图像的rgb数组，高度，宽度
			imagergb ImgRGB = getImagergb();
			int nheight = getImgHeight();
			int nwidth = getImgWidth();


			for (i = 1; i < nheight - 1; i++)
			{
				for (p = ImgRGB + nwidth*i, j = 0; j < nwidth - 2; p++, j++)
				{
					numb = p->rgbBlue / 15;
					numg = p->rgbGreen / 15;
					numr = p->rgbRed / 15;
					if (numb == 17)
						numb = 16;
					if (numg == 17)
						numg = 16;
					if (numr == 17)
						numr = 16;
					statisticsrgb[numb][numg][numr]++;
					if (p->rgbReversed != 255)
						rgbReversedture = 1;
				}
			}


			for (i = 0; i < 17; i++)
			{
				for (j = 0; j < 17; j++)
				{
					for (k = 0; k < 17; k++)
					{
						if (statisticsrgb[i][j][k]>0)
							firstcountamount++;//几种颜色
					}
				}
			}

			setOrigincountcolorvarietyamount(firstcountamount);

			setRgbamount(statisticsrgb, firstcountamount);

			return firstcountamount;
		}



		void setCountcolorvarietyamount(int nvalue){

			countcolorvarietyamount = nvalue;
		}

		int getCountcolorvarietyamount(){

			return countcolorvarietyamount;
		}

		void expandcolorinterval(int firstcountamount, int intervalvalue)
		{
			int i, j, countamount = 0, amountdiffer;
			rgbcolor q, s;

			rgbcolor Rgbamount = getRgbamount();

			if (firstcountamount > 5)
			{
				for (i = 0, q = Rgbamount; i < firstcountamount - 1; i++, q++)
				{
					if (q->amount != 0)
					{
						for (j = i + 1, s = q + 1; j < firstcountamount; j++, s++)
						{
							amountdiffer = countstatisticscolor(q, s);
							if (amountdiffer <= intervalvalue)
							{
								//将颜色差值小于intervalvalue的区域合并
								q->amount = q->amount + s->amount;
								s->amount = 0;
							}
						}
					}
				}

				int nvalues = 0;

				rgbcolor Rgbamount_ = getRgbamount();
				for (i = 0, q = Rgbamount_; i < firstcountamount - 1; i++, q++)
				{
					if (q->amount> 0)
					{
						nvalues++;
						setCountcolorvarietyamount(nvalues);
					}
				}
				bubblesort(firstcountamount);
			}
			else
			{
				setCountcolorvarietyamount(firstcountamount);

			}
		}


		void proceddingstatisticsrgbcolor(int firstcountamount)
		{
			bubblesort(firstcountamount);
			expandcolorinterval(firstcountamount, 2);
		}
		int findacolorwidthamount(rgbcolor s)
		{
			/*计算颜色s超过4长度的总段数*/
			int i, j, ditance, amountdifferone, amountdiffertwo, rgbwidthamount;
			int rgbwidthquantity[100];
			imagergb p;
			for (i = 0; i < 100; i++)
			{
				rgbwidthquantity[i] = 0;
			}
			for (i = 1; i < height - 1; i++)
			{
				ditance = 0;
				for (p = rgb + width*i + 1, j = 0; j < width - 2; p++, j++)
				{
					amountdifferone = imagecolordiffer(p, s);
					amountdiffertwo = imagecolordiffer(p + 1, s);
					if (amountdifferone <= 2 && amountdiffertwo > 2)
					{
						if (ditance > 0 && ditance <= 100)
						{
							rgbwidthquantity[ditance - 1]++;
						}
						ditance = 0;
					}
					if (ditance > 0 && amountdifferone <= 2 && amountdiffertwo <= 2)
						ditance++;
					if (ditance == 0 && amountdifferone > 2 && amountdiffertwo <= 2)
					{
						ditance = 1;
					}
				}
			}
			rgbwidthamount = 0;
			for (i = 4; i < 100; i++)
			{
				rgbwidthamount = rgbwidthamount + rgbwidthquantity[i];
			}
			return rgbwidthamount;
		}
		void changecolor(int choice)
		{
			/*根据周围点的颜色改变当前点的颜色（某些噪点颜色会存在字符内部）
			choice=1根据上下颜色改变当前
			choice=2根据左右颜色改变当前
			choice=3根据上下左右颜色改变当前*/
			int i, j;
			int amountdiffer1, amountdiffer2;
			imagergb p;
			for (i = 1; i < width - 1; i++)
			{
				for (j = 1, p = rgb + j*width + i; j < height - 1; j++, p = p + width)
				{
					amountdiffer1 = rgbdiffer(p - width, p + width);
					amountdiffer2 = rgbdiffer(p - 1, p + 1);
					if (amountdiffer1 < 4)
					{
						amountdiffer1 = imagecolordiffer(p - width, rgbamount);
						if (amountdiffer1>6 && (choice == 1 || choice == 3))
						{
							p->rgbBlue = (p - width)->rgbBlue;
							p->rgbGreen = (p - width)->rgbGreen;
							p->rgbRed = (p - width)->rgbRed;
						}
					}
					if (amountdiffer2 < 4)
					{
						amountdiffer2 = imagecolordiffer(p - 1, rgbamount);
						if (amountdiffer2>6 && (choice == 2 || choice == 3))
						{
							p->rgbBlue = (p - 1)->rgbBlue;
							p->rgbGreen = (p - 1)->rgbGreen;
							p->rgbRed = (p - 1)->rgbRed;
						}
					}
				}
			}
		}
		void colortoblackwhite(int value, int start, int end, int color)
		{
			//将彩色图转化为黑白图,与value值接近就变为color值，否则变为255-color；
			int i, j, numb, numg, numr, amountdiffer;
			imagergb p;
			for (i = start; i <= end; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if ((amountdiffer < value))
					{
						//与背景色接近
						p->rgbBlue = color;
						p->rgbGreen = color;
						p->rgbRed = color;
					}
					else
					{
						p->rgbBlue = 255 - color;
						p->rgbGreen = 255 - color;
						p->rgbRed = 255 - color;
					}
				}
			}
		}

		//噪点噪线处理函数
		int checknoise(imagergb p, int colordiffer)
		{
			//检查该点是否为噪点,返回不同点数
			int k, s;
			int countnum = 0;
			int amountdiffer = 0;
			imagergb q;
			for (k = 0; k < 3; k++)
			{
				for (s = 0, q = p + width*(k - 1) - 1; s < 3; s++, q++)
				{
					//amountdiffer = rgbdiffer(p, q);
					amountdiffer = imagecolordiffer(q, rgbamount);
					if (amountdiffer < colordiffer)
					{
						countnum++;
					}
				}
			}
			return countnum;
		}
		int judgenoisespot()
		{
			//判断该图片是否有噪点:周围不同点总数
			int i, j;
			int num = 0;
			int differnum = 0;
			int amountdiffer;
			int letterwidth = 0;
			int differspot = 0;
			int spotamount[80];
			imagergb p, q;

			for (i = 1; i < width - 1; i++)
			{
				for (j = 1, p = rgb + i + j * width; j < height - 1; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer>15)
					{
						differnum = checknoise(p, 8);
						if (differnum>7)
						{
							num++;
							//简单去噪
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;
						}
					}
				}
			}
			if (num > 5)
			{
				//该图片有噪点
				return num;
			}
			else
			{
				//该图片没有噪点或在噪点很少可以忽略
				return 0;
			}
		}
		int judgenoiseline()
		{
			//判断该图片是否有噪线(粗噪线返回2，细噪线返回1,无噪线返回0,未实现)；
			int i, j;
			int amountdiffer;
			int vertspotnum[150];
			int horzspotnum[100];
			int vertpart, horzpart;
			int vertamount, horzamount;
			int largeamount;
			imagergb p, q;
			//竖向
			for (i = 0; i < 150; i++)
			{
				vertspotnum[i] = 0;
				if (i < width)
				{
					for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
					{
						amountdiffer = imagecolordiffer(p, rgbamount);
						if (amountdiffer>15)
						{
							vertspotnum[i]++;
						}
					}
				}
			}

			vertamount = 0;
			vertpart = 0;
			largeamount = 0;
			for (i = 1; i < width - 1; i++)
			{
				if (vertspotnum[i] != 0 && vertspotnum[i + 1] == 0)
				{
					//连续的非零数列有几个
					vertpart++;
				}
				if (vertspotnum[i] != 0)
				{
					vertamount++;
				}
				if (vertspotnum[i] > 20)
				{
					//区分云南昭通设置的一个参数
					largeamount++;
				}
			}

			//水平:噪线高为1的情况，昆明、金华细噪线
			for (i = 0; i < 100; i++)
			{
				horzspotnum[i] = 0;
				if (i < height)
				{
					for (j = 0, p = rgb + i*width + j; j<width; j++, p++)
					{
						amountdiffer = imagecolordiffer(p, rgbamount);
						if (amountdiffer>15)
						{
							horzspotnum[i]++;
						}
					}
				}
			}

			horzamount = 0;
			horzpart = 0;
			for (j = 1; j < height - 1; j++)
			{
				if (horzspotnum[j] != 0 && horzspotnum[j + 1] == 0)
				{
					horzpart++;
				}
				if (horzspotnum[j] < 3 && horzspotnum[j] != 0)
				{
					horzamount++;
				}
			}

			if (6 * largeamount>vertamount)
			{
				//区分云南昭通
				return 1;
			}
			if (vertpart != 4 || vertamount > 3 * width / 4)
			{
				return 1;
			}
			else
			{
				if (horzamount >= 2 || horzpart > 1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		int discardwidthnoise(int colordiffer, int selectorder, int rgbcolorwidth)
		{
			/*当selectorder等于1时则去除宽度小于等于rgbcolorwidth的颜色；
			  当selectorder不等于1时，返回宽度小于等于colorwidth的数量*/

			int i, j, k, ditance, numb, numg, numr, amountdiffer, spotamount = 0;
			imagergb p, q;

			for (i = 1; i < height - 1; i++)
			{
				for (p = rgb + width*i + 1, j = 0; j < width - 3; p++, j++)
				{
					ditance = 0;

					amountdiffer = rgbdiffer(p, p + 1);
					if (amountdiffer > colordiffer)
					{
						ditance = 1;
						for (q = p + 1, k = 0; k + j < width - 3; k++, q++)
						{

							amountdiffer = rgbdiffer(q, q + 1);
							if (ditance > 0 && amountdiffer > colordiffer)
								break;
							if (amountdiffer <= colordiffer)
								ditance++;
						}
					}
					if (ditance == rgbcolorwidth)
						spotamount++;
					if (ditance <= rgbcolorwidth && ditance != 0)
					{
						for (p = p + 1, j = j + 1, k = 0; k < ditance; k++, j++, p++)
						{
							if (selectorder == 1)
							{
								p->rgbBlue = rgbamount->numb * 15;
								p->rgbGreen = rgbamount->numg * 15;
								p->rgbRed = rgbamount->numr * 15;
							}
						}
						j--;
						p--;
					}
					else if (ditance > rgbcolorwidth && ditance != 0)
					{
						p = p + ditance;
						j = j + ditance;
					}
				}
			}
			return spotamount;
		}
		int discardheightnoise(int colordiffer, int selectorder, int rgbcolorheight)
		{
			/*同discardwidthnoise，这里指的是高度*/
			int i, j, k, ditance, numb, numg, numr, amountdiffer, spotamount = 0;
			imagergb p, q;
			for (i = 0; i < width - 2; i++)
			{
				for (j = 0, p = rgb + i + width; j < height - 3; p = p + width, j++)
				{
					ditance = 0;

					amountdiffer = rgbdiffer(p, p + width);
					if (amountdiffer > colordiffer)
					{
						ditance = 1;
						for (q = p + width, k = 0; k + j < height - 3; k++, q = q + width)
						{

							amountdiffer = rgbdiffer(q, q + width);
							if (ditance > 0 && amountdiffer > colordiffer)
								break;
							if (amountdiffer <= colordiffer)
								ditance++;
						}
					}
					if (ditance == rgbcolorheight)
						spotamount++;
					if (ditance <= rgbcolorheight && ditance != 0)
					{
						for (p = p + width, j = j + 1, k = 0; k < ditance; k++, j++, p = p + width)
						{
							if (selectorder == 1)
							{
								p->rgbBlue = rgbamount->numb * 15;
								p->rgbGreen = rgbamount->numg * 15;
								p->rgbRed = rgbamount->numr * 15;
							}
						}
						j--;
						p = p - width;
					}
					else if (ditance > rgbcolorheight && ditance != 0)
					{
						p = p + width*ditance;
						j = j + ditance;
					}
				}
			}
			return spotamount;
		}
		void blackwhitediscardrgbspot(int number, int rgbspotcolor)
		{
			/*适用于黑白图，根据周围点数改变当前点的颜色*/
			int i, j, k, s, countnum;
			imagergb  p, q;
			for (i = 1; i < height - 1; i++)
			{
				for (j = 1, p = rgb + i*width + 1; j < width - 1; j++, p++)
				{
					if (p->rgbBlue == rgbspotcolor)
					{
						countnum = 0;
						for (k = 0; k < 3; k++)
						{
							for (s = 0, q = p + width*(k - 1) - 1; s < 3; s++, q++)
							{
								if (q->rgbBlue == 255 - rgbspotcolor)
									countnum++;
							}
						}
						if (countnum>number)
						{
							//当前点周围不同点数超过number则改变当前点颜色
							p->rgbBlue = 255 - rgbspotcolor;
							p->rgbGreen = 255 - rgbspotcolor;
							p->rgbRed = 255 - rgbspotcolor;
						}
					}
				}
			}

		}
		void discardrgbspot(int number, int colordiffer, int choice)
		{
			/*choice为0时当该点周围不同点数超过number就把该点变成背景色;
			  如果choice为1则在此基础上考虑这些不同点中是否有与当前点同色的点，有则不删除（这里的number代表周围背景色的点数）*/
			int i, j, k, s;
			int countnum, countsamenum;
			int rgbnum;
			int amountdifferone, amountdiffertwo;
			imagergb  p, q;
			rgbcolor imagergbcolor, aroundimagergbcolor;
			imagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			aroundimagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			for (i = 1; i < height - 1; i++)
			{
				for (j = 1, p = rgb + i*width + 1; j < width - 1; j++, p++)
				{
					countnum = 0;
					countsamenum = 0;
					rgbnum = 0;
					amountdifferone = imagecolordiffer(p, rgbamount);
					if (amountdifferone>4)
					{


						setRgbColorDive15(imagergbcolor, p);
						for (k = 0; k < 3; k++)
						{
							for (s = 0, q = p + width*(k - 1) - 1; s < 3; s++, q++)
							{
								amountdifferone = imagecolordiffer(q, imagergbcolor);
								amountdiffertwo = imagecolordiffer(q, rgbamount);
								if (amountdifferone > colordiffer)
								{
									countnum++;
								}
								if (amountdifferone < colordiffer)
								{
									countsamenum++;
								}
								if (amountdiffertwo < colordiffer)
								{
									rgbnum++;
								}
							}
						}
						if (choice == 0)
						{
							if (countnum>number)
							{
								p->rgbBlue = rgbamount->numb * 15;
								p->rgbGreen = rgbamount->numg * 15;
								p->rgbRed = rgbamount->numr * 15;
							}
						}
						else
						{
							if (rgbnum > number)
							{
								if (countsamenum > 1)
								{
									continue;
								}
								else
								{
									p->rgbBlue = rgbamount->numb * 15;
									p->rgbGreen = rgbamount->numg * 15;
									p->rgbRed = rgbamount->numr * 15;
								}
							}
						}
					}
				}
			}
			free(imagergbcolor);
			free(aroundimagergbcolor);
		}
		int countonergbspot(imagergb  p, rgbcolor imagergbcolor, int colordiffer)
		{
			//返回当前点周围与它不同的颜色点数
			int  k, s, countnum, numb, numg, numr, amountdiffer;
			imagergb  q;
			countnum = 0;
			for (k = 0; k < 3; k++)
			{
				for (s = 0, q = p + width*(k - 1) - 1; s < 3; s++, q++)
				{
					amountdiffer = imagecolordiffer(q, imagergbcolor);
					if (amountdiffer > colordiffer)
					{
						countnum++;
					}
				}
			}
			return countnum;
		}
		void virtualdiscardrgbspot(imagergb  p)
		{
			/*当前点周围与它不同的点数超过4就将当前点变成背景色*/
			int  countnum;
			rgbcolor imagergbcolor;
			imagergbcolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));

			setRgbColorDive15(imagergbcolor, p);

			countnum = countonergbspot(p, imagergbcolor, 20);
			if (countnum > 4)
			{
				imagergbcolor->numb = rgbamount->numb;
				imagergbcolor->numg = rgbamount->numg;
				imagergbcolor->numr = rgbamount->numr;
			}
		}
		void discardcolorspot()
		{
			//去除彩色单个点
			int i, j;
			int amountdifferup, amountdifferdown, amountdifferleft, amountdifferright, amountdiffer;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer > 4)
					{
						amountdifferup = imagecolordiffer(p - width, rgbamount);
						amountdifferdown = imagecolordiffer(p + width, rgbamount);
						amountdifferleft = imagecolordiffer(p - 1, rgbamount);
						amountdifferright = imagecolordiffer(p + 1, rgbamount);
						if (amountdifferup < 4 && amountdifferdown < 4 && amountdifferleft < 4 && amountdifferright < 4)
						{
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;
						}
					}
				}
			}
		}
		void discardmakeupcolorspot()
		{
			/**/
			int i, j, k, s, countnum, amountdiffer, numb, numg, numr;
			rgbcolor srgba, rrgba, imagergbcolor, sidergbcolor;
			imagergb  p, q;
			imagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			sidergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			for (srgba = rgbamount + 1; srgba - rgbamount >= 10; srgba++)
			{
				for (i = 1; i < height - 1; i++)
				{
					for (j = 1, p = rgb + i*width + 1; j < width - 1; j++, p++)
					{

						amountdiffer = imagecolordiffer(p, srgba);
						if (amountdiffer <= 4)
						{
							countnum = 0;
							sidergbcolor->numb = rgbamount->numb;
							sidergbcolor->numg = rgbamount->numg;
							sidergbcolor->numr = rgbamount->numr;
							for (k = 0; k < 3; k++)
							{
								for (s = 0, q = p + width*(k - 1) - 1; s < 3; s++, q++)
								{
									setRgbColorDive15(imagergbcolor, q);

									amountdiffer = countstatisticscolor(imagergbcolor, srgba);
									if (amountdiffer > 4)
									{
										amountdiffer = countstatisticscolor(imagergbcolor, rgbamount);
										if (amountdiffer > 4)
										{
											sidergbcolor->numb = imagergbcolor->numb;
											sidergbcolor->numg = imagergbcolor->numg;
											sidergbcolor->numr = imagergbcolor->numr;
										}
										countnum++;
									}
								}
							}
							if (countnum > 5)
							{
								p->rgbBlue = sidergbcolor->numb * 15;
								p->rgbGreen = sidergbcolor->numg * 15;
								p->rgbRed = sidergbcolor->numr * 15;
								srgba->amount--;
								for (rrgba = rgbamount; rrgba->amount > 0; rrgba++)
								{
									amountdiffer = countstatisticscolor(sidergbcolor, rrgba);
									if (amountdiffer <= 4)
									{
										rrgba->amount++;
										break;
									}
								}
							}
						}
					}
				}
			}
			free(imagergbcolor);
			free(sidergbcolor);
		}
		void discardextraline(imagergb p, rgbcolor q, int yp, int xp)
		{
			int i, j, numb, numg, numr, countnumleft, countnumright, countnumup, countnumdown, positiondowm, positionup, positionright, positionleft;
			int   amountdifferup, amountdifferdown, amountdifferleft, amountdifferright;
			imagergb qup, qdown, kleft, kright;
			rgbcolor s, imagergbcolorone, imagergbcolortwo, imagergbcolorthree, imagergbcolorfour, rgbleft, rgbright, rgbup, rgbdown;
			imagergbcolorone = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			imagergbcolortwo = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			imagergbcolorthree = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			imagergbcolorfour = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			countnumup = 0;
			countnumdown = 0;
			countnumleft = 0;
			countnumright = 0;
			for (i = 1; i <= 10; i++)
			{
				qdown = p + i*width;
				if (width*(height - 1) < qdown - rgb)
				{
					qdown = rgb + width*(height - 2) + xp;
				}
				qup = p - i*width;
				if (width > qup - rgb)
				{
					qup = rgb + width + xp;
				}
				kright = p + i;//可以修改，防止超出矩阵
				kleft = p - i;
				positiondowm = xp;
				positionup = xp;
				positionright = xp + i;
				positionleft = xp - i;
				if (countnumup == 0)
				{

					setRgbColorDive15(imagergbcolorone, qup);
				}
				if (countnumdown == 0)
				{

					setRgbColorDive15(imagergbcolortwo, qdown);
				}
				if (countnumleft == 0)
				{

					setRgbColorDive15(imagergbcolorthree, kleft);
				}
				if (countnumright == 0)
				{

					setRgbColorDive15(imagergbcolorfour, kright);
				}
				for (j = 0, s = rgbamount; j < 5; s++, j++)
				{
					if (q == rgbamount || s != q)
					{
						amountdifferup = countstatisticscolor(imagergbcolorone, s);
						amountdifferdown = countstatisticscolor(imagergbcolortwo, s);
						if (countnumup == 0 && amountdifferup <= 4)
						{
							if (s == rgbamount || (s != rgbamount && positionup - s->location<50 && positionup - s->location>-50))
							{
								rgbup = s;
								countnumup = 1;
							}
						}
						if (countnumdown == 0 && amountdifferdown <= 4)
						{
							if (s == rgbamount || (s != rgbamount && positiondowm - s->location<50 && positiondowm - s->location>-50))
							{
								rgbdown = s;
								countnumdown = 1;
							}
						}
						amountdifferleft = countstatisticscolor(imagergbcolorthree, s);
						amountdifferright = countstatisticscolor(imagergbcolorfour, s);
						if (countnumleft == 0 && amountdifferleft <= 4)
						{
							if (s == rgbamount || (s != rgbamount && positionleft - s->location<50 && positionleft - s->location>-50))
							{
								rgbleft = s;
								countnumleft = 1;
							}
						}
						if (countnumright == 0 && amountdifferright <= 4)
						{
							if (s == rgbamount || (s != rgbamount && positionright - s->location<50 && positionright - s->location>-50))
							{
								rgbright = s;
								countnumright = 1;
							}
						}
					}
				}
				if ((countnumup == 1 && countnumdown == 1 && rgbamount == rgbup && rgbamount == rgbdown))
				{
					if (!((countnumleft == 1 && rgbleft != rgbamount) || (countnumright == 1 && rgbright != rgbamount)))
					{
						p->rgbBlue = (rgbamount->numb) * 15;
						p->rgbGreen = (rgbamount->numg) * 15;
						p->rgbRed = (rgbamount->numr) * 15;
					}
					break;
				}
				else if ((countnumleft == 1 && countnumright == 1 && rgbamount == rgbleft && rgbamount == rgbright))
				{
					if (!((countnumup == 1 && rgbup != rgbamount) || (countnumdown == 1 && rgbdown != rgbamount)))
					{
						p->rgbBlue = (rgbamount->numb) * 15;
						p->rgbGreen = (rgbamount->numg) * 15;
						p->rgbRed = (rgbamount->numr) * 15;
					}
					break;
				}
			}
			free(imagergbcolorone);
			free(imagergbcolortwo);
			free(imagergbcolorthree);
			free(imagergbcolorfour);
		}
		void imagetransferblackwhite(rgbcolor s, int colordiffer)
		{
			int i, numb, numg, numr, amountdiffer;
			imagergb p;
			for (p = rgb, i = 0; i < width*height; p++, i++)
			{

				amountdiffer = imagecolordiffer(p, s);
				if (amountdiffer <= colordiffer)
				{
					p->rgbBlue = 0;
					p->rgbGreen = 0;
					p->rgbRed = 0;//black
				}
				else
				{
					p->rgbBlue = 255;
					p->rgbGreen = 255;
					p->rgbRed = 255;//white
				}
			}
		}
		void imagetransferblackwhitetwo(rgbcolor s1, rgbcolor s2, int colordifferone, int colordiffertwo, int choice)
		{
			//choice为0变为黑白图，choice为1保持原图只除双背景
			int i, numb, numg, numr, amountdifferone, amountdiffertwo;
			imagergb p;
			for (p = rgb, i = 0; i < width*height; p++, i++)
			{
				amountdifferone = imagecolordiffer(p, s1);
				amountdiffertwo = imagecolordiffer(p, s2);
				if (amountdifferone <= colordifferone || amountdiffertwo <= colordiffertwo)
				{
					if (choice == 0)
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;//black;
					}
					else
					{
						p->rgbBlue = rgbamount->numb * 15;
						p->rgbGreen = rgbamount->numg * 15;
						p->rgbRed = rgbamount->numr * 15;
					}
				}
				else
				{
					if (choice == 0)
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;//white
					}
				}
			}
		}
		void discardonespot()
		{
			//黑白图去除单个噪点
			int i, j, countnum;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					countnum = 0;
					if (i == 0 || i == width - 1 || j == 0 || j == height - 1)
					{
						//边缘去噪
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
					else
					{
						//上下左右都是其它颜色，去除该噪点
						if ((p - 1)->rgbBlue == 0 && (p + 1)->rgbBlue == 0 && (p - width)->rgbBlue == 0 && (p + width)->rgbBlue == 0)
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 0;
						}
					}
				}
			}
		}
		void discardbinarylump(int value)
		{
			//消除图片中的小色块,白色变成黑色
			int i, j;
			int num;
			int amountdiffer;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						ColorFillingbinary(j, i, 255, 128);
						num = loopnum;
						if (num <= value)
						{
							ColorFillingbinary(j, i, 128, 0);
						}
						else
						{
							ColorFillingbinary(j, i, 128, 255);
						}
						loopnum = 0;
					}
				}
			}
		}
		void discardcolorlump(int value)
		{
			//去除原图中的小色块
			int i, j;
			int m, n;
			int amountdiffer, amountdifferone, amountdiffertwo;
			int num, sumnum;
			int mark = 0;
			int samecolor[80], differcolor[80];
			int coloramount[80];

			imagergb p, q, r;
			rgbcolor noisecolor;
			noisecolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));

			//消除周围小色块
			changespotsign(0);
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer>4)
					{
						ColorFillingorigin(j, i, 0);
						num = loopnum;
						if (num < value)
						{
							if (mark == 0)
							{
								noisecolor->numb = p->rgbBlue / 15;
								noisecolor->numg = p->rgbGreen / 15;
								noisecolor->numr = p->rgbRed / 15;
								mark = 1;
							}
							for (m = 0; m < width; m++)
							{
								for (n = 0, q = rgb + n*width + m; n < height; n++, q = q + width)
								{
									if (q->sign == 1)
									{
										q->rgbBlue = rgbamount->numb * 15;
										q->rgbGreen = rgbamount->numg * 15;
										q->rgbRed = rgbamount->numr * 15;
									}
								}
							}
						}
						else
						{
							changespotsign(0);
						}
						loopnum = 0;
					}
				}
			}




		}
		void retaincharcolors()
		{
			//保留字符的颜色（适用于噪点噪线少，颜色排序中前五位是背景和字符）
			int i, j, amountdiffer, k, m, n;
			int mark = 0;
			imagergb p, q;
			rgbcolor s;
			s = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer < 4)
					{
						continue;
					}
					else
					{
						for (s = rgbamount + 1, k = 1; s < rgbamount + 5; s++, k++)
						{
							if (mark == 0)
							{
								for (m = 0; m < 3; m++)
								{
									for (n = 0, q = p + width*(m - 1) - 1; n < 3; n++, q++)
									{
										amountdiffer = imagecolordiffer(q, s);
										if (amountdiffer < 4)
										{
											mark = 1;
											n = 3;
											m = 3;
										}
									}
								}
							}
							else
							{
								break;
							}
						}

						if (mark == 1)
						{
							mark = 0;
							continue;
						}
						else
						{
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;
							mark = 0;
						}
					}
				}
			}
		}

		//分割字符
		int horzpartamount(int start, int end, int choose, int checkposition)
		{
			//字符分割部分函数：返回封闭空间在水平方向的投影部分数量
			//choose:为0表示按原图颜色投影，为1表示图片转为白黑图片的水平投影
			//checkposition:观察运用checkposition-（height-1）位置的数列特征
			int i, j;
			int mark;
			int starty1 = 0, starty2 = 0, endy1 = 0, endy2 = 0;
			int amountdiffer;
			int num;
			imagergb p;
			int horzspotamount[40];
			for (i = 0; i < height; i++)
			{
				horzspotamount[i] = 0;
				for (j = start, p = rgb + i*width + j; j <= end; j++, p++)
				{
					if (choose == 0)
					{
						amountdiffer = imagecolordiffer(p, rgbamount);
						if (amountdiffer>8)
						{
							horzspotamount[i]++;
						}
					}
					else
					{
						if (p->rgbBlue == 0)
						{
							horzspotamount[i]++;
						}
					}
				}
			}

			if (checkposition == 0)
			{
				mark = 0;
				for (i = 1; i < height - 1; i++)
				{
					if (horzspotamount[i - 1] == 0 && horzspotamount[i] != 0)
					{
						if (mark == 0)
						{
							starty1 = i;
						}
						else
						{
							starty2 = i;
						}
					}
					if (horzspotamount[i] != 0 && horzspotamount[i + 1] == 0)
					{
						if (mark == 0)
						{
							endy1 = i;
							//防止单个点被误认为是封闭空间
							if (starty1 == endy1)
							{
								starty1 = 0;
								endy1 = 0;
							}
							else
							{
								mark = 1;
							}

						}
						else
						{
							endy2 = i;
							if (starty2 == endy2)
							{
								starty2 = 0;
								endy2 = 0;
							}

						}
					}
				}
				if (starty2 == 0)
				{
					for (i = starty1 + 2; i < endy1 - 1; i++)
					{
						if (horzspotamount[i] == 1 && horzspotamount[i + 1] != 1 && horzspotamount[i - 1] != 1)
						{
							starty2 = i + 1;
							endy2 = endy1;
							endy1 = i;
							break;
						}
					}
				}
				if (starty1 == 0)
				{
					return 0;
				}
				else if (starty1 != 0 && starty2 == 0)
				{
					return 1;
				}
				else
				{
					return 2;
				}
			}
			else
			{
				num = 0;
				for (i = checkposition; i <= height - 1; i++)
				{
					if (horzspotamount[i] > 2)
					{
						num++;
					}
				}
				if (num > height - checkposition - 1)
				{
					//水平投影从checkposition位置到height-1有噪线
					return 1;
				}
				else
				{
					//从checkposition到height-1位置没有噪线
					return 0;
				}
			}


		}
		void LetterSegmentloop(int amount, int xindex1, int xindex2, int yindex1, int yindex2, int letterwidth)
		{
			//分割字符，首先根据封闭空间进行分割，如果图片没有封闭空间，根据其它方法进行分割
			int i, j;
			int distance1, distance2;
			int dropleftx, droplefty, droprightx, droprighty;
			int leftborder, rightborder;
			//左边-1，右边1
			int left, right;
			imagergb p;
			closurespace s;
			s = clospace;
			leftborder = yindex1;
			rightborder = yindex2;
			if (amount > 0)
			{
				//图片存在封闭空间的情况
				for (i = 0; i < amount; i++, s++)
				{
					if (s->sign == 1)
					{
						//是loop
						distance1 = s->leftdownx - leftborder;
						distance2 = rightborder - s->rightupx;
						if (distance1 <= (yindex2 - yindex1) / 8)
						{
							if (distance2 <= (yindex2 - yindex1) / 8)
							{
								//最后一个字符，不分割
								continue;
							}
							else
							{
								//第一个字符，只需分割右边
								right = 1;
								droprightx = s->rightupx + letterwidth;
								droprighty = s->rightupy;
								//精确分割点
								p = rgb + droprighty*width + droprightx;
								if (p->rgbBlue == 255)
								{
									if ((p - 1)->rgbBlue == 128)
									{
										droprightx -= 1;
									}
									if ((p + 1)->rgbBlue == 128)
									{
										droprightx += 1;
									}
								}
								//采用上下滴水算法
								DropFallroute(droprighty, droprightx, xindex2, yindex2);
								DowndropFallroute(droprighty, droprightx, xindex1, yindex2, right);
								leftborder = droprightx;
							}

						}
						else if (distance2 <= (yindex2 - yindex1) / 8)
						{
							if (distance1 < distance2)
							{
								//若左侧已经被其他字母的滴落路径分割过，则不进行分割
								continue;
							}
							else
							{
								//最后一个字符，只需分割左边
								left = -1;
								dropleftx = s->leftdownx - letterwidth;
								droplefty = s->leftdowny;

								//精确分割点
								p = rgb + droplefty*width + dropleftx;
								if (p->rgbBlue == 255)
								{
									if ((p - 1)->rgbBlue == 128)
									{
										dropleftx -= 1;
									}
									if ((p + 1)->rgbBlue == 128)
									{
										dropleftx += 1;
									}
								}
								DropFallroute(droplefty, dropleftx, xindex2, yindex2);
								DowndropFallroute(droplefty, dropleftx, xindex1, yindex2, left);
							}
						}
						else
						{
							//字符处于中间位置，需要分割左右两边
							droprightx = s->rightupx + letterwidth;
							droprighty = s->rightupy;
							right = 1;
							dropleftx = s->leftdownx - letterwidth;
							droplefty = s->leftdowny;
							left = -1;
							//精确分割点
							p = rgb + droplefty*width + dropleftx;
							if (p->rgbBlue == 255)
							{
								if ((p - 1)->rgbBlue == 128)
								{
									dropleftx -= 1;
								}
								if ((p + 1)->rgbBlue == 128)
								{
									dropleftx += 1;
								}
							}
							p = rgb + droprighty*width + droprightx;
							if (p->rgbBlue == 255)
							{
								if ((p - 1)->rgbBlue == 128)
								{
									droprightx -= 1;
								}
								if ((p + 1)->rgbBlue == 128)
								{
									droprightx += 1;
								}
							}
							DropFallroute(droplefty, dropleftx, xindex2, yindex2);
							DowndropFallroute(droplefty, dropleftx, xindex1, yindex2, left);
							DropFallroute(droprighty, droprightx, xindex2, yindex2);
							DowndropFallroute(droprighty, droprightx, xindex1, yindex2, right);
							leftborder = droprightx;
						}

					}
					else
					{
						//不是loop，由字符粘连形成的封闭空间,任选一边滴水分割
						//droprightx = s->rightupx;
						//droprighty = s->rightupy;
						droprightx = abs(s->rightupx + s->leftdownx) / 2;
						droprighty = abs(s->leftdowny + s->rightupy) / 2;
						right = 1;
						DropFallroute(droprighty, droprightx, xindex2, yindex2);
						DowndropFallroute(droprighty, droprightx, xindex1, yindex2, right);
						leftborder = droprightx;
					}
				}
			}
			else
			{
				//没有封闭空间的情况



			}

		}
		void Lettersegment(int start, int end, int position)
		{
			//分割start-end部分字符，只更新分割位置不移动(昆明)
			int i, j;
			int middle = 0, middle1, middle2;
			int num = 0;
			int partwidth, partwidth1 = 0, partwidth2 = 0;
			int stepwidth;
			int startmark = 0, endmark = 0;
			int mark = 0;
			partwidth = end - start;
			if (partwidth < 30)
			{
				//两个字符
				stepwidth = partwidth / 2;
				middle = start;
				middle1 = 0;

				for (i = start; i <= start + 3; i++)
				{
					if (spotamount[i] >= 13)
					{
						if (startmark == 0 && i > start + 2)
						{
							break;
						}
						else
						{
							startmark++;
						}
					}
				}
				for (i = end; i >= end - 3; i--)
				{
					if (spotamount[i] >= 13)
					{
						if (endmark == 0 && i < end - 2)
						{
							break;
						}
						else
						{
							endmark++;
						}
					}
				}
				if (startmark != 0 || endmark != 0)
				{
					if (startmark != 0 && endmark == 0)
					{
						num = 0;
						for (i = start + 4; i < end; i++)
						{
							if (spotamount[i]>13 && spotamount[i + 1] < 13)
							{
								if (i - start>stepwidth + 4)
								{
									continue;
								}
								else
								{
									middle1 = i;
									break;
								}
							}
						}
						if (i >= end)
						{
							goto loop1;
						}
					}
					else if (endmark != 0 && startmark == 0)
					{
						num = 0;
						for (i = end - 4; i >= start; i--)
						{
							if (spotamount[i] > 10)
							{
								num++;
							}
							if (num == endmark)
							{
								middle1 = i - 1;
								break;
							}
						}
						if (i < start)
						{
							goto loop1;
						}
					}
					else
					{
						num = 0;
						for (i = start + 4; i < end; i++)
						{
							if (spotamount[i]>10)
							{
								num++;
							}
							if (num == startmark)
							{
								middle1 = i;
								break;
							}
							if (i - start > stepwidth + 2 && num == 0)
							{
								mark = 1;
								break;
							}
						}
						if (mark == 1)
						{
							goto loop1;
						}
					}
				}
				else
				{
				loop1:for (i = start + 1; i < end; i++)
				{
						  if (spotamount[i] <= spotamount[i + 1] && spotamount[i] < spotamount[i - 1])
						  {
							  if (i - middle < stepwidth)
							  {
								  continue;
							  }
							  else
							  {
								  if (middle1 == 0)
								  {
									  middle1 = i - 1;
									  cout << "middle:" << middle1 << endl;
									  break;
								  }
							  }
						  }
				}
				}

				for (i = 5; i >= position + 1; i--)
				{
					if (startposition[i] != 0)
					{
						startposition[i + 1] = startposition[i];
						endposition[i + 1] = endposition[i];
					}
				}
				startposition[position] = start;
				endposition[position] = middle1;
				startposition[position + 1] = middle1 + 1;
				endposition[position + 1] = end;
			}
			else
			{
				//三个字符
				stepwidth = partwidth / 3;
				middle = start;
				middle1 = 0;
				middle2 = 0;
				for (i = start + 1; i < end; i++)
				{
					if (spotamount[i] <= spotamount[i + 1] && spotamount[i] < spotamount[i - 1])
					{
						if (i - middle < stepwidth - 1)
						{
							continue;
						}
						else
						{
							middle = i;
							if (middle1 == 0)
							{
								middle1 = i + 1;
								cout << "middle1:" << middle1 << "    ";
							}
							else
							{
								middle2 = i + 1;
								cout << "middle2:" << middle2 << endl;
								break;
							}
						}
					}
				}

				for (i = 5; i >= position + 1; i--)
				{
					if (startposition != 0)
					{
						startposition[i + 2] = startposition[i];
						endposition[i + 2] = endposition[i];
					}
				}
				startposition[position] = start;
				endposition[position] = middle1;
				startposition[position + 1] = middle1 + 1;
				endposition[position + 1] = middle2;
				startposition[position + 2] = middle2 + 1;
				endposition[position + 2] = end;
			}
		}
		void lettersegmentspace(int letteramount)
		{
			//运用填充的方法，用sign和灰色进行标记将字符拉开,适合没有粘连的字母重叠分割
			//letteramount为字符的个数
			int i, j = 0;
			int m, n;
			int starti = 0, endi = 0;
			int priorstart = 0, priorend = 0;
			int distance;
			int partnum = 0;
			int letterposition = 0;
			int graynum[80];
			int passnum = 0;
			imagergb p, q;

			Lettermove(0, width - 1, 10, 0, 1);
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					p->sign = 0;
					//修补一些空缺
					if (p->rgbBlue == 0)
					{
						if ((p - 1)->rgbBlue == 255 && (p + 1)->rgbBlue == 255 && (p - width)->rgbBlue == 255 && (p + width)->rgbBlue == 255)
						{
							p->rgbBlue = 255;
							p->rgbGreen = 255;
							p->rgbRed = 255;
						}
					}
				}
			}

			//多加了4
			distance = 4 * letteramount;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 255 && p->sign == 0)
					{
						distance = distance - 4;
						loopnum = 0;
						ColorFillingbinary(j, i, 255, 128);


						//求灰色封闭空间的范围（方便判断某些小的封闭空间是否是该字符的一部分）
						for (m = 0; m < width; m++)
						{
							graynum[m] = 0;
							for (n = 0, q = rgb + n*width + m; n < height; n++, q = q + width)
							{
								if (q->rgbBlue == 128)
								{
									graynum[m]++;
								}
							}
						}
						for (m = 0; m < width; m++)
						{
							if (graynum[m] == 0 && graynum[m + 1] != 0)
							{
								starti = m + 1;
							}
							if (graynum[m] != 0 && graynum[m + 1] == 0)
							{
								endi = m;
								break;
							}
						}


						if (priorstart != 0)
						{
							if (starti >= priorstart&&endi <= priorend)
							{
								distance = distance + 4;
							}
							else
							{
								if (((loopnum<35 && partnum>40) || (partnum < 35 && loopnum < 40)) && priorend + 1 >= starti)
								{
									distance = distance + 4;
								}
							}
						}

						if (distance < 0)
						{
							distance = 0;
						}
						movecolorletter(128, distance);
						loopnum = 0;
						ColorFillingbinary(j, i - distance, 128, 255);
						partnum = loopnum;
						priorstart = starti;
						priorend = endi;
					}
				}
			}
		}
		void lettersegmentline(int letteramount)
		{
			//存在粘连字符，用分割线标记然后分开
			//letteramount为字符的个数
			int i, j;
			int amount = 0;
			int letterwidth = 0;
			int passamount = 0;
			int start, end;
			findletterposition(rgb, 1, 255, 4);
			for (i = 0; i < 6; i++)
			{
				if (startposition[i] != 0)
				{
					amount++;
					letterwidth = letterwidth + (endposition[i] - startposition[i]);
				}
			}
			letterwidth = letterwidth / letteramount;
			if (amount < letteramount)
			{
				for (i = 0; i < amount; i++)
				{
					if (endposition[i] - startposition[i] - letterwidth>4)
					{
						start = startposition[i];
						end = endposition[i];
						for (j = startposition[i]; j <= endposition[i]; j++)
						{
							if (spotamount[j] <= spotamount[j + 1] && spotamount[j] < spotamount[j - 1] && spotamount[j] <= 5)
							{
								if (j - start>letterwidth - 4 && end - j > letterwidth - 4)
								{
									passamount = markposition(j, 0, -1);
									if (passamount < 5)
									{
										markposition(j, 1, 1);
										start = j;
									}
								}
							}
						}
						markposition(j, 1, 1);
					}
					else
					{
						markposition(endposition[i] + 1, 1, 1);
					}
				}
				lettermovebyline();
			}
		}
		void LetterSegmentcolor()
		{
			//字符的颜色不同，根据颜色填充然后移动分开
			int i, j;
			int amountdifferone, amountdiffertwo;
			imagergb p;

			Lettermove(0, width - 1, 15, 1, 1);
			changecolor(3);
			changecolor(1);

			changespotsign(0);
			loopnum = 0;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					amountdifferone = imagecolordiffer(p, rgbamount);
					if (amountdifferone>4)
					{
						ColorFillingsame(j, i, 1);
						i = width;
						break;
					}
				}
			}


		}
		int markposition(int col, int choice, int sign)
		{
			//对分割路线进行标记，标记为红色
			int i;
			int xrow, ycol;
			int left, right, down, downleft, downright;
			int passnum = 0;
			imagergb p;
			xrow = 0;
			ycol = col;
			while (xrow < height - 1 && ycol < width)
			{
				p = rgb + xrow*width + ycol;
				if (p->rgbBlue == 255)
				{
					passnum++;
				}
				left = (p - 1)->rgbBlue;
				right = (p + 1)->rgbBlue;
				down = (p + width)->rgbBlue;
				downleft = (p + width - 1)->rgbBlue;
				downright = (p + width + 1)->rgbBlue;
				if (choice == 1)
				{
					p->rgbBlue = 0;
					p->rgbGreen = 0;
					p->rgbRed = 255;
				}
				p->sign = sign;
				if (down == 0 && (p + width)->sign != sign)
				{
					xrow = xrow + 1;
				}
				else
				{
					if (downleft == 0 && (p + width - 1)->sign != sign)
					{
						xrow = xrow + 1;
						ycol = ycol - 1;
					}
					else if (downright == 0 && (p + width + 1)->sign != sign)
					{
						xrow = xrow + 1;
						ycol = ycol + 1;
					}
					else if (left == 0 && (p - 1)->sign != sign)
					{
						if (right == 0 && (p + 2 * width)->rgbBlue == 0)
						{
							xrow = xrow + 1;
						}
						else if (right == 0 && (p - 2)->rgbBlue == 0 && (p + 2)->rgbBlue == 0)
						{
							xrow = xrow + 1;
						}
						else
						{
							ycol = ycol - 1;
						}
					}
					else if (right == 0 && (p + 1)->sign != sign)
					{
						if (left == 0 && (p + 2 * width)->rgbBlue == 0)
						{
							xrow = xrow + 1;
						}
						else
						{
							ycol = ycol + 1;
						}
					}
					else
					{
						xrow = xrow + 1;
					}
				}
			}
			return passnum;
		}

		//后期处理，调整位置，旋转等
		void adjustposition()
		{
			//调整黑白图片中字母等上下位置
			int i, j, m, n, amountdiffer;
			int num;
			int differ;
			int amountspot[150];
			int positionstart[6], positionend[6], startp = 0, endp = 0, position[6];
			int controltail, controlhead;
			imagergb p;

			for (i = 0; i < 150; i++)
			{
				amountspot[i] = 0;
				if (i < 6)
				{
					positionstart[i] = 0;
					positionend[i] = 0;
					position[i] = 0;
				}
			}
			//统计各列像素点总数
			for (i = 0; i < width; i++)
			{
				num = 0;
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						num++;
					}
				}
				amountspot[i] = num;
			}
			//计算存储字母左右开始结束位置（列）
			for (i = 0; i < 149; i++)
			{
				if (startp < 6)
				{
					if (amountspot[i] != 0 && amountspot[i - 1] == 0)
					{
						positionstart[startp] = i;
						startp++;
					}
				}
				if (endp < 6)
				{
					if (amountspot[i] != 0 && amountspot[i + 1] == 0)
					{
						positionend[endp] = i;
						endp++;
					}
				}
				if (startp >= 6 && endp >= 6)
				{
					break;
				}
				if (startp<1 && i>70)
				{
					return;
				}

			}

			for (i = 0; i < 6; i++)
			{
				if ((positionstart[i] != 0) && (positionend[i] != 0) && (positionstart[i] != positionend[i]))
				{
					//查找字母顶部开始位置
					for (m = 0; m < height; m++)
					{
						for (n = positionstart[i], p = rgb + m*width + n; n <= positionend[i]; n++, p++)
						{
							if (p->rgbBlue == 255 && ((p - 1)->rgbBlue == 255 || (p + 1)->rgbBlue == 255))
							{
								position[i] = m;
								m = height;
								break;
							}

						}
					}

					differ = 8 - position[i];

					//调整位置
					for (m = positionstart[i]; m <= positionend[i]; m++)
					{
						if (differ > 0)
						{
							//下移
							controltail = 0;
							for (n = height - 1, p = rgb + (height - 1) * width + m; n >= 0; n--, p = p - width, controltail--)
							{
								if (p->rgbBlue == 255)
								{
									//下移需要控制下边，防止出界
									if (controltail + differ <= 0)
									{
										(p + width * differ)->rgbBlue = 255;
										(p + width * differ)->rgbGreen = 255;
										(p + width * differ)->rgbRed = 255;
										p->rgbBlue = 0;
										p->rgbGreen = 0;
										p->rgbRed = 0;
									}
									else
									{
										p->rgbBlue = 0;
										p->rgbGreen = 0;
										p->rgbRed = 0;
									}

								}
							}
						}
						else if (differ < 0)
						{
							//上移
							controlhead = 0;
							for (n = 0, p = rgb + m; n < height; n++, p = p + width, controlhead++)
							{
								if (p->rgbBlue == 255)
								{
									if (controlhead + differ >= 0)
									{
										(p + differ * width)->rgbBlue = 255;
										(p + differ * width)->rgbGreen = 255;
										(p + differ * width)->rgbRed = 255;
										p->rgbBlue = 0;
										p->rgbGreen = 0;
										p->rgbRed = 0;
									}
									else
									{
										p->rgbBlue = 0;
										p->rgbGreen = 0;
										p->rgbRed = 0;
									}

								}
							}
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		int findletterposition(imagergb copy, int dispose, int color, int value)
		{
			/*dispose==1:二值化后的图片；dispose==0:原图
			  color非原图需要确定开始结束位置的颜色;value原图中设置的与背景色的差值大小
			  求图片各字符的开始结束位置*/
			int i, j, num, letterwidth = 0;
			int start = 0, end = 0;
			int amountdiffer;
			int sum = 0;
			imagergb p, q;

			if (spotamount == NULL){

				spotamount = (int *)malloc(200 * sizeof(int));
			}

			if (startposition == NULL){

				startposition = (int *)malloc(6 * sizeof(int));
			}

			if (endposition == NULL){

				endposition = (int *)malloc(6 * sizeof(int));
			}

			for (i = 0; i < 200; i++)
			{
				spotamount[i] = 0;
				if (i <= 5)
				{
					startposition[i] = 0;
					endposition[i] = 0;
				}
			}

			//统计每列字符像素总数
			for (i = 0; i < width; i++)
			{
				num = 0;
				for (j = 0, p = copy + i; j < height; j++, p = p + width)
				{
					if (dispose == 1)
					{
						if (p->rgbBlue == color)
						{
							num++;
						}
					}
					else
					{
						amountdiffer = imagecolordiffer(p, rgbamount);
						if (amountdiffer >= value)
						{
							num++;
						}
					}
				}
				spotamount[i] = num;
				sum += num;
				cout << spotamount[i] << "    ";
			}
			cout << endl;

			//根据每列黑点/彩点总数的数列特征记录整个粘连字符的字母开始结束位置
			for (i = 0; i < 200; i++)
			{
				if (spotamount[i] == 0)
				{
					if (letterwidth>0 && end <= 6)
					{
						if (letterwidth == 1)
						{
							start = start - 1;
							startposition[start] = 0;
						}
						else
						{
							endposition[end++] = i - 1;
							cout << "endposition:" << endposition[end - 1] << "  ";
						}

					}
					letterwidth = 0;
					continue;
				}
				else
				{
					if (spotamount[i] > 1)
					{
						letterwidth++;
						if (letterwidth == 1 && start <= 6)
						{

							startposition[start++] = i;
							cout << "startposition:" << startposition[start - 1] << "    ";
						}
					}
				}
			}
			cout << endl;
			return sum;
		}
		void adjustangle(float centerx, float centery, int angle, int start, int end)
		{
			//绕(centerx,centery)点旋转angle度,旋转的图像放在copy中
			int k, j;
			int newx, newy;
			imagergb p, q;
			for (j = 0; j < width; j++)
			{
				for (k = 0, p = rgb + j; k < height; k++, p = p + width)
				{
					p->sign = 0;
				}
			}
			for (j = start; j <= end; j++)
			{
				for (k = 1, p = rgb + j + width * k; k < height - 1; k++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						//仿射变换
						newx = (j - centerx)*cos(angle * 3.14 / 180) - (k - centery)*sin(angle * 3.14 / 180) + centerx;
						newy = (k - centery)*cos(angle * 3.14 / 180) + (j - centerx)*sin(angle * 3.14 / 180) + centery;
						q = copyrgb + newy*width + newx;
						q->rgbBlue = 255;
						q->rgbGreen = 255;
						q->rgbRed = 255;
					}
					else
					{
						continue;
					}
				}
			}

		}
		void erasebm(int start, int end, imagergb bm)
		{
			//将黑白图中的字符颜色全部擦除（字符为白色）
			int i, j;
			imagergb p;
			for (i = start; i <= end; i++)
			{
				for (j = 0, p = bm + i; j < height; j++, p = p + width)
				{
					p->rgbBlue = 0;
					p->rgbGreen = 0;
					p->rgbRed = 0;
				}
			}
		}
		void fillblack(imagergb bm)
		{
			int i, j;
			imagergb p;
			for (i = 1; i < width - 1; i++)
			{
				for (j = 1, p = bm + i + j*width; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 0)
					{
						if (((p - 1)->rgbBlue == 255 && (p + 1)->rgbBlue == 255) || ((p - width)->rgbBlue == 255 && (p + width)->rgbBlue == 255))
						{
							p->rgbBlue = 255;
							p->rgbGreen = 255;
							p->rgbRed = 255;
						}
					}
				}
			}
		}
		void rotate()
		{
			//倾斜字符角度调整，旋转
			int i, j, k;
			int m, n;
			int xnum = 0, ynum = 0, graynum = 0;
			double centerx, centery;
			int newx = 0, newy = 0;
			int start, end;
			int shadowlength = 0, newshadowlength1 = 0, newshadowlength2 = 0;
			int newshadowlength[10];
			int angle;
			int mid = 100, position = 0;
			imagergb p, q, r;
			imagergb temprgb;
			copyrgb = (imagergb)malloc((width*height)*sizeof(RGBQUADS));
			temprgb = (imagergb)malloc((width*height)*sizeof(RGBQUADS));
			//原图复制到另一片同样大小的空白处；
			for (j = 0; j < width; j++)
			{
				for (k = 0, p = rgb + j, q = copyrgb + j, r = temprgb + j; k < height; k++, p = p + width, q = q + width, r = r + width)
				{
					q->rgbBlue = 0;
					q->rgbGreen = 0;
					q->rgbRed = 0;
					r->rgbBlue = 0;
					r->rgbGreen = 0;
					r->rgbRed = 0;
					q->rgbReversed = p->rgbReversed;
				}
			}

			findletterposition(rgb, 1, 255, 0);
			for (i = 0; (i <= sizeof(startposition)) && (startposition[i] != 0); i++)
			{
				mid = 100;
				position = 0;
				//初始字符投影长度
				shadowlength = endposition[i] - startposition[i];
				if (shadowlength <= 1)
				{
					continue;
				}
				else
				{
					//求字符的质心
					for (j = startposition[i]; j <= endposition[i]; j++)
					{
						for (k = 0, p = rgb + j; k < height; k++, p = p + width)
						{
							if (p->rgbBlue == 255)
							{
								xnum = xnum + 1 * j;
								ynum = ynum + 1 * k;
								graynum = graynum + 1;
							}
						}
					}
					centerx = xnum / graynum;
					centery = ynum / graynum;
					//cout << "centerx:" << centerx << "   " << "centery:" << centery << endl;
					xnum = 0;
					ynum = 0;
					graynum = 0;
				}


				//进行字符角度的调整(调整完一个再进行下一个字符的角度调整),投影最小
				start = startposition[i];
				end = endposition[i];
				//逆时针旋转
				angle = -30;
				adjustangle(centerx, centery, angle, start, end);
				findletterposition(copyrgb, 1, 255, 0);
				newshadowlength1 = endposition[0] - startposition[0];
				erasebm(startposition[0], endposition[0], copyrgb);
				//顺时针旋转
				angle = 30;
				adjustangle(centerx, centery, angle, start, end);
				findletterposition(copyrgb, 1, 255, 0);
				newshadowlength2 = endposition[0] - startposition[0];
				erasebm(startposition[0], endposition[0], copyrgb);
				//比较两个方向的旋转投影结果确定旋转方向
				if (abs(newshadowlength1 - newshadowlength2) > 1)
				{
					if (newshadowlength1 < newshadowlength2)
					{
						//确定按逆时针旋转
						for (j = 0; j <= 9; j++)
						{
							//变化旋转角度求对应的投影长度
							angle = -5 - j * 5;
							adjustangle(centerx, centery, angle, start, end);
							findletterposition(copyrgb, 1, 255, 0);
							newshadowlength[j] = endposition[0] - startposition[0];
							erasebm(startposition[0], endposition[0], copyrgb);
						}
						//求投影长度最小对应的旋转角度
						for (j = 0; j <= 9; j++)
						{
							if (newshadowlength[j] < mid)
							{
								mid = newshadowlength[j];
								position = j;
							}
						}
						angle = -5 - position * 5;
						adjustangle(centerx, centery, angle, start, end);
					}
					else
					{
						//确定按顺时针旋转
						for (j = 0; j <= 9; j++)
						{
							//变化旋转角度求对应的投影长度
							angle = 5 + j * 5;
							adjustangle(centerx, centery, angle, start, end);
							findletterposition(copyrgb, 1, 255, 0);
							newshadowlength[j] = endposition[0] - startposition[0];
							erasebm(startposition[0], endposition[0], copyrgb);
						}
						//求投影长度最小对应的旋转角度
						for (j = 0; j <= 9; j++)
						{
							if (newshadowlength[j] < mid)
							{
								mid = newshadowlength[j];
								position = j;
							}
						}
						angle = 5 + position * 5;
						adjustangle(centerx, centery, angle, start, end);

					}
				}
				else
				{
					//字符没有倾斜，保持原位置
					angle = 0;
					adjustangle(centerx, centery, angle, start, end);
				}

				for (m = start - 1; m <= end + 1; m++)
				{
					for (n = 0, p = copyrgb + m, q = temprgb + m; n < height; n++, p = p + width, q = q + width)
					{
						q->rgbBlue = p->rgbBlue;
						q->rgbGreen = p->rgbGreen;
						q->rgbRed = p->rgbRed;
					}
				}
				erasebm(0, width - 1, copyrgb);
				findletterposition(rgb, 1, 255, 0);
			}

			erasebm(0, width - 1, rgb);
			for (j = 0; j < width; j++)
			{
				for (k = 0, p = rgb + j, q = temprgb + j; k < height; k++, p = p + width, q = q + width)
				{
					p->rgbBlue = q->rgbBlue;
					p->rgbGreen = q->rgbGreen;
					p->rgbRed = q->rgbRed;
				}
			}
			fillblack(rgb);
			free(copyrgb);
			free(temprgb);
		}
		void movecolorletter(int color, int distance)
		{
			//移动指定颜色的字母
			int i, j;
			imagergb p;
			if (distance != 0)
			{
				for (i = 0; i < width; i++)
				{
					for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
					{
						if (p->rgbBlue == color)
						{
							(p - distance)->rgbBlue = p->rgbBlue;
							(p - distance)->rgbGreen = p->rgbGreen;
							(p - distance)->rgbRed = p->rgbRed;
							(p - distance)->sign = 1;
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 0;
							p->sign = 0;
						}
					}
				}
			}
		}
		void lettermovebyline()
		{
			/*图片水平方向从右往左扫描（红色分割线都在字母的右边），在每行中每遇到一根红线就将移动距离减2
			此方法针对图片中字符右边空白多的情况，若是左边空白比较多，可以先将图片中字符和分割线整体左移再使用此函数
			*/
			int i, j, k;
			int amountdiffer;
			int movewidth;
			int num;
			imagergb p, q;
			rgbcolor redmark, blackground;
			redmark = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			blackground = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			redmark->numb = 0;
			redmark->numg = 0;
			redmark->numr = 15;
			blackground->numb = 0;
			blackground->numg = 0;
			blackground->numr = 0;

			for (i = 0; i < height; i++)
			{
				movewidth = 12;
				num = 0;
				for (j = width - 1, p = rgb + i*width + j; j >= 0; j--, p--)
				{
					amountdiffer = imagecolordiffer(p, redmark);
					if (amountdiffer < 4)
					{
						for (k = 1; k < 10; k++)
						{
							amountdiffer = imagecolordiffer(p - k, redmark);
							if (amountdiffer < 4)
							{
								(p - k)->rgbRed = 0;
							}
							else
							{
								break;
							}
						}
						num++;
						p->rgbRed = 0;
						movewidth = 12 - (num - 1) * 4;
					}
					else
					{
						if (num != 0)
						{
							amountdiffer = imagecolordiffer(p, blackground);
							if (amountdiffer > 4)
							{
								if (movewidth != 0)
								{
									(p + movewidth)->rgbBlue = p->rgbBlue;
									(p + movewidth)->rgbGreen = p->rgbGreen;
									(p + movewidth)->rgbRed = p->rgbRed;
									p->rgbBlue = 0;
									p->rgbGreen = 0;
									p->rgbRed = 0;
								}
							}
						}
					}
				}
			}
		}
		void Lettermove(int start, int end, int distance, int choice, int direction)
		{
			/*移动从start到end部分的字符,移动距离为distance
			  choice:1为原图，移动过后原点变为背景色；0为黑白图，移动后的原点变为黑色
			  direction为移动的方向，-1向左，1向右*/
			int i, j;
			int amountdiffer;
			imagergb p;
			if (distance > 0)
			{
				if (direction == -1)
				{
					//向左移动
					for (i = start; i <= end; i++)
					{
						for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
						{

							if (choice == 1)
							{
								amountdiffer = imagecolordiffer(p, rgbamount);
								if (amountdiffer>4)
								{
									(p - distance)->rgbBlue = p->rgbBlue;
									(p - distance)->rgbGreen = p->rgbGreen;
									(p - distance)->rgbRed = p->rgbRed;
									p->rgbBlue = rgbamount->numb * 15;
									p->rgbGreen = rgbamount->numg * 15;
									p->rgbRed = rgbamount->numr * 15;
								}
							}
							else
							{
								if (p->rgbBlue == 255)
								{
									(p - distance)->rgbBlue = p->rgbBlue;
									(p - distance)->rgbGreen = p->rgbGreen;
									(p - distance)->rgbRed = p->rgbRed;
									p->rgbBlue = 0;
									p->rgbGreen = 0;
									p->rgbRed = 0;
								}
							}
						}
					}
				}
				else
				{
					//向右移动
					for (i = end; i >= start; i--)
					{
						for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
						{

							if (choice == 1)
							{
								amountdiffer = imagecolordiffer(p, rgbamount);
								if (amountdiffer>4)
								{
									(p + distance)->rgbBlue = p->rgbBlue;
									(p + distance)->rgbGreen = p->rgbGreen;
									(p + distance)->rgbRed = p->rgbRed;
									p->rgbBlue = rgbamount->numb * 15;
									p->rgbGreen = rgbamount->numg * 15;
									p->rgbRed = rgbamount->numr * 15;
								}
							}
							else
							{
								if (p->rgbBlue == 255)
								{
									(p + distance)->rgbBlue = p->rgbBlue;
									(p + distance)->rgbGreen = p->rgbGreen;
									(p + distance)->rgbRed = p->rgbRed;
									p->rgbBlue = 0;
									p->rgbGreen = 0;
									p->rgbRed = 0;
								}

							}
						}
					}
				}
			}


		}
		void grayprocess(int threshold)
		{
			/*将图像进行灰度化处理
			  threshold:灰度化后根据阈值大小，将小于阈值的点变为黑色，大于阈值的点变为白色；*/
			int i, j;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					p->Grayvalue = 0.29*p->rgbRed + 0.587*p->rgbGreen + 0.114*p->rgbBlue;
					if (p->Grayvalue>threshold)
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
					else
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
				}
			}
		}

		//其它函数
		int calculateimagecoloramount(int num)
		{
			//用来计算满足一定数量的图片颜色种数；
			int coloramount = 0;
			rgbcolor s;
			for (s = rgbamount; s->amount > num; s++)
			{
				coloramount++;
			}
			return coloramount;
		}
		int calculatemaxamountwidth()
		{
			//返回数量最多的宽度;
			int rgbcolorwidth[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			int i, maxwidth;
			for (i = 0; i < 10; i++)
			{
				rgbcolorwidth[i] = discardwidthnoise(10, 0, i + 1);
			}
			for (i = 1, maxwidth = 0; i < 10; i++)
			{
				if (rgbcolorwidth[i]>rgbcolorwidth[maxwidth])
					maxwidth = i;
			}
			return maxwidth + 1;
		}
		int calculateheight()
		{
			//直接返回后半段图片相同颜色超过2的高度
			int i, j, k, amountdiffer, numheight = 0, numwidth = 0;
			imagergb p, q;
			rgbcolor imagergbcolor;
			imagergbcolor = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			for (i = width / 2; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);

					if (amountdiffer>20)
					{
						numheight++;
						imagergbcolor->numb = p->rgbBlue / 15;
						imagergbcolor->numg = p->rgbGreen / 15;
						imagergbcolor->numr = p->rgbRed / 15;
						for (k = j + 1, q = p + width; k < height; k++, q = q + width)
						{
							amountdiffer = imagecolordiffer(q, imagergbcolor);
							if (amountdiffer < 10)
							{
								numheight++;
							}
							else
							{
								p = q - width;
								j = k - 1;
								break;
							}
						}
						if (numheight > 2)
						{
							return numheight;
						}
						else
						{
							numheight = 0;
						}

					}
				}
			}
			return 0;
		}
		int calculateletterwidth()
		{
			//返回图片中数量最多的宽度大小（排除噪点）
			int i, j, k;
			int amountdiffer;
			int letterwidth[5] = { 0, 0, 0, 0, 0 };
			int num = 0;
			int maxinum = 0, maxiwidth = 0;
			int noisenum = 0;
			imagergb p, q;
			for (i = 0; i < height; i++)
			{
				for (j = 0, p = rgb + i * width + j; j < width; j++, p++)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer >= 15)
					{
						//检查该点是否是噪点;
						noisenum = checknoise(p, 8);
						if (noisenum > 7)
						{
							continue;
						}
						else
						{
							num = 1;
							for (k = j + 1, q = p + 1; k <= width; k++, q++)
							{
								amountdiffer = rgbdiffer(p, q);
								if (amountdiffer < 12)
								{
									num++;
								}
								else
								{
									break;
								}
							}
							if (num <= 4 && num >= 1)
							{
								letterwidth[num]++;
							}
							j = k - 1;
							p = q - 1;
						}

					}
				}
			}
			for (k = 0; k <= 4; k++)
			{
				if (letterwidth[k] > maxinum)
				{
					maxinum = letterwidth[k];
					maxiwidth = k;
				}
			}
			return maxiwidth;
		}



		//把像素的取值范围缩小在0-16；通过255/15控制,像素值相差不大的点归类 (lai:理解或许不是很懂前人的想法吗)
		rgbcolor setRgbColorDive15(rgbcolor rgbcolorValue, imagergb p){

			int numb, numg, numr;
			numb = p->rgbBlue / 15;
			numg = p->rgbGreen / 15;
			numr = p->rgbRed / 15;
			if (numb == 17)
				numb = 16;
			if (numg == 17)
				numg = 16;
			if (numr == 17)
				numr = 16;
			rgbcolorValue->numb = numb;
			rgbcolorValue->numg = numg;
			rgbcolorValue->numr = numr;
			return rgbcolorValue;

		}

		int calculatedifferspot(int colordiffer, int num)
		{
			/*num用来设置统计周围不同点数,返回周围不同点超过num的像素点数*/
			int i, j, k, s, countnum, acountspotamount, amountdiffer;
			imagergb  p, q;
			int rgbspot[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			rgbcolor imagergbcolor, aroundimagergbcolor;

			imagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));
			aroundimagergbcolor = (rgbcolor)malloc((1)*sizeof(RGB_AMOUNT));

			imagergb Imagergb = getImagergb();
			int nWidth = getImgWidth();
			int nheight = getImgHeight();

			for (i = 1; i < nheight - 1; i++)
			{
				for (j = 1, p = Imagergb + i*nWidth + 1; j < nWidth - 1; j++)
				{
					countnum = 0;
					setRgbColorDive15(imagergbcolor, p);
					for (k = 0; k < 3; k++)                   //比较的是周围9个像素点
					{
						for (s = 0, q = p + nWidth*(k - 1) - 1; s < 3; s++)
						{
							setRgbColorDive15(aroundimagergbcolor, q);

							amountdiffer = countstatisticscolor(imagergbcolor, aroundimagergbcolor);
							if (amountdiffer > colordiffer)
							{
								countnum++;
							}
							q++;
						}
					}
					rgbspot[countnum]++;
					p++;

				}
			}
			acountspotamount = 0;
			for (i = num; i <= 8; i++)
			{
				acountspotamount += rgbspot[i];
			}
			free(imagergbcolor);
			free(aroundimagergbcolor);
			return acountspotamount;
		}
		void findrgbposition()
		{
			int i, j, inext, jnext, numamount, coloramount = 0, signwidth, location, nextamount;
			int  amountdiffer, heightnumamount, absentwidthamount, rgbwidthamount;
			imagergb p, pnext;
			rgbcolor s, q;
			for (s = rgbamount; s->amount > 50; s++)
			{
				amountdiffer = countstatisticscolor(s, rgbamount);
				rgbwidthamount = findacolorwidthamount(s);
				if (amountdiffer > 4 || (amountdiffer <= 4 && rgbwidthamount > height * 2 / 5))
				{
					numamount = 0;
					absentwidthamount = 0;
					for (i = 1; i < width - 1; i++)
					{
						heightnumamount = 0;
						for (p = rgb + i + j*width + 1, j = 0; j < height - 2; p = p + width, j++)
						{

							amountdiffer = imagecolordiffer(p, s);
							if (amountdiffer <= 2)
								heightnumamount++;
							if (numamount == 0 && heightnumamount > 2)
							{
								numamount = heightnumamount;
								signwidth = i;
							}
						}
						if (numamount > 0)
						{
							numamount = numamount + heightnumamount;
						}
						if (numamount != 0 && heightnumamount < 2)
							absentwidthamount++;
						amountdiffer = countstatisticscolor(s, rgbamount);
						if (absentwidthamount > 20)
						{
							numamount = 0;
							absentwidthamount = 0;
						}
						if (numamount > (s->amount * 3 / 5) && i - signwidth < 100)
						{
							nextamount = 0;
							for (inext = i; inext - i < 3; inext++)
							{
								for (pnext = rgb + inext + 1, jnext = 0; jnext < height - 2; pnext = pnext + width, jnext++)
								{
									amountdiffer = imagecolordiffer(pnext, s);
									if (amountdiffer <= 2)
										nextamount++;
								}
							}
							if (nextamount == 0)
							{
								location = (signwidth + i) / 2;
								if (coloramount == 0)
								{
									s->location = location;
									coloramount++;
								}
								else
								{
									for (q = s - 1; q != rgbamount; q--)
									{
										if (q->location != 0 && (q->location - location<30 && q->location - location>-30))
											break;
									}
									if (q == rgbamount && (!((q + 1)->location - location<30 && (q + 1)->location - location>-30) || (q + 1)->location == 0))
									{
										s->location = location;
										coloramount++;
									}
								}
								numamount = 0;
								absentwidthamount = 0;
							}
						}
					}
				}
				if (coloramount == 4)
					break;
			}
			for (i = 0, q = rgbamount + 1, s = rgbamount + 1; i < 4; q++)
			{
				if (q->location != 0)
				{
					if (s != q)
					{
						s->amount = q->amount;
						s->location = q->location;
						s->numb = q->numb;
						s->numg = q->numg;
						s->numr = q->numr;
					}
					s++;
					i++;
				}
			}
		}
		void changespotsign(int value)
		{
			//改变图片中每个点的sign值
			int i, j;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					p->sign = value;
				}
			}
		}

		//中值滤波函数
		void calculategrayvalue()
		{
			//计算图像的灰度值
			int i, j;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					p->Grayvalue = (p->rgbBlue * 11 + p->rgbGreen * 59 + p->rgbRed * 30 + 50) / 100;
				}
			}
		}
		void MediaFilter()
		{
			//中值滤波3*3
			int x, y;
			int p[9];
			int s;
			int i, j;
			imagergb q;
			calculategrayvalue();
			for (y = 1; y < height - 1; y++)
			{
				for (x = 1, q = rgb + y*width + x; x < width - 1; x++, q++)
				{
					p[0] = (q - width - 1)->Grayvalue;
					p[1] = (q - width)->Grayvalue;
					p[2] = (q - width + 1)->Grayvalue;
					p[3] = (q - 1)->Grayvalue;
					p[4] = q->Grayvalue;
					p[5] = (q + 1)->Grayvalue;
					p[6] = (q + width - 1)->Grayvalue;
					p[7] = (q + width)->Grayvalue;
					p[8] = (q + width + 1)->Grayvalue;
					for (j = 0; j < 5; j++)
					{
						for (i = j + 1; i < 9; i++)
						{
							if (p[j]>p[i])
							{
								s = p[j];
								p[j] = p[i];
								p[i] = s;
							}
						}
					}
					q->Grayvalue = p[4];
				}
			}
		}

		//细化算法函数
		int detectconnect()
		{
			//细化算法中的功能函数
			int connectivity = 0;
			connectivity = list[6] - list[6] * list[7] * list[0];
			connectivity += list[0] - list[0] * list[1] * list[2];
			connectivity += list[2] - list[2] * list[3] * list[4];
			connectivity += list[4] - list[4] * list[5] * list[6];
			return connectivity;
		}
		void Thinning()
		{
			/*Hilditch细化算法
			  满足条件:1）该点是一个边界点
			  2）该点不是一个端点
			  3）该点不是一个断点
			  4）该点的删除不应该造成过度侵蚀
			  退出条件：扫描一次后，将标记的像素点删除；开始下一轮扫描，直到在某次扫描中，没有像素点再被标记，结束。
			  参考网址：http://www.cnblogs.com/xiaotie/archive/2010/08/12/1797760.html
			  */
			int i, j, k;
			int count;
			int connectivity;
			Boolean loop = true;
			imagergb p;
			list = (int*)malloc(8 * sizeof(int));
			while (loop == true)
			{
				loop = false;
				for (i = 1; i < height - 1; i++)
				{
					for (j = 1; j < width - 1; j++)
					{
						p = rgb + i*width + j;
						//P3  P2  P1
						//P4  P   P0
						//P5  P6  P7
						//条件1：p必须是前景色
						if (p->rgbBlue != 255)
						{
							continue;
						}
						//list存储的是补集，字符（白）颜色为0，背景（黑）颜色为1，以方便联结数计算
						list[0] = (p + 1)->rgbBlue == 255 ? 0 : 1;
						list[1] = (p - width + 1)->rgbBlue == 255 ? 0 : 1;
						list[2] = (p - width)->rgbBlue == 255 ? 0 : 1;
						list[3] = (p - width - 1)->rgbBlue == 255 ? 0 : 1;
						list[4] = (p - 1)->rgbBlue == 255 ? 0 : 1;
						list[5] = (p + width - 1)->rgbBlue == 255 ? 0 : 1;
						list[6] = (p + width)->rgbBlue == 255 ? 0 : 1;
						list[7] = (p + width + 1)->rgbBlue == 255 ? 0 : 1;
						//条件2：p0，p2，p4，p6不全是字符点
						if (list[0] == 0 && list[2] == 0 && list[4] == 0 && list[6] == 0)
						{
							continue;
						}
						//条件3：p0~p7至少两个是字符点
						count = 0;
						for (k = 0; k < 8; k++)
						{
							count += list[k];
						}
						if (count>6)
						{
							continue;
						}
						//条件4：联结数等于1
						connectivity = detectconnect();
						if (connectivity != 1)
						{
							continue;
						}
						//条件5：假设p2已经被标记删除，则令p2为背景时，不改变p的联结数
						if ((p - width)->sign == 1)
						{
							list[2] = 1;
							if (detectconnect() != 1)
							{
								continue;
							}
							list[2] = 0;
						}
						// 条件6: 假设p4已标记删除，则令p4为背景，不改变p的联结数 
						if ((p - 1)->sign == 1)
						{
							list[4] = 1;
							if (detectconnect() != 1)
							{
								continue;
							}
							list[4] = 0;
						}
						p->sign = 1; // 标记删除 
						loop = true;
					}
				}


				for (i = 1; i < width - 1; i++)
				{
					for (j = 1, p = rgb + j*width + i; j < height - 1; j++, p = p + width)
					{
						if (p->sign == 1)
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 0;
						}
					}
				}
			}
		}
		//腐蚀算法
		void Erosionforbinary()
		{
			//腐蚀算法
			/*使用1*3的结构元素,为防止越界，不处理最左边和最右边
			  当使用3*1结构元素，为防止越界，不处理最上边和最下边
			  当使用3*3结构时，为防止越界，不处理最左边和最右边以及最上边和最下边
			  */
			int i, j;
			int m, n;
			imagergb p, q;
			for (j = 1; j < height - 1; j++)
			{
				for (i = 1, p = rgb + j*width + i; i < width - 1; i++, p++)
				{
					//使用水平方向结构元素进行腐蚀，当前点先进行标记字符色白色
					//p->sign = 1;
					//如果当前点自身或者左右有一个点不是字符白色，则将该点标记为背景色黑色
					if (p->sign == 2)
					{
						for (m = -1; m <= 1; m++)
						{
							for (n = -1, q = p + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue == 0)
								{
									p->sign = 0;
								}
							}
						}
						//if (((p - 1)->rgbBlue == 0 || p->rgbBlue == 0 || (p + 1)->rgbBlue == 0))
						//{
						//	p->sign = 0;
						//}
					}

				}
			}

			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->sign == 0)
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
					else
					{
						p->rgbBlue = 255;
						p->rgbGreen = 255;
						p->rgbRed = 255;
					}
				}
			}
		}

		//滴水算法以及填充标记封闭空间函数
		void ColorFillingbinary(int nrow, int ncol, int value, int changecolor)
		{
			/*寻找给定坐标点(nrow,ncol)连通的区域，将value颜色改为涂上changecolor颜色*/
			int i, j;
			imagergb p, q;
			for (i = nrow - 1; i < nrow + 2; i++)
			{
				for (j = ncol - 1; j < ncol + 2; j++)
				{
					if (i >= 0 && i < height&&j >= 0 && j < width)
					{
						p = rgb + i*width + j;
						q = p;
						if ((i == nrow || j == ncol) && p->rgbBlue == value)                        // i == nrow || j == ncol 控制在四连域
						{
							loopnum++;
							p->rgbBlue = changecolor;
							p->rgbGreen = changecolor;
							p->rgbRed = changecolor;
							p->sign = 1;
							ColorFillingbinary(i, j, value, changecolor);
						}
					}
				}
			}
		}
		void ColorFillingorigin(int nrow, int ncol, int choice)
		{
			int i, j;
			int amountdiffer;
			imagergb p;
			for (i = nrow - 1; i < nrow + 2; i++)
			{
				for (j = ncol - 1; j < ncol + 2; j++)
				{
					if (i >= 0 && i < height&&j >= 0 && j < width)
					{
						p = rgb + i*width + j;
						amountdiffer = imagecolordiffer(p, rgbamount);
						if ((i == nrow || j == ncol) && p->sign == 0 && amountdiffer > 6)
						{
							loopnum++;
							if (choice == 1)
							{
								p->rgbBlue = rgbamount->numb * 15;
								p->rgbGreen = rgbamount->numg * 15;
								p->rgbRed = rgbamount->numr * 15;
							}
							p->sign = 1;
							ColorFillingorigin(i, j, choice);
						}
					}
				}
			}
		}
		void ColorFillingsame(int nrow, int ncol, int choice)
		{
			//标记当前坐标相同的颜色
			int i, j;
			int amountdifferone, amountdiffertwo;
			imagergb p;
			rgbcolor currentspot;
			currentspot = (rgbcolor)malloc(1 * sizeof(RGB_AMOUNT));
			p = rgb + nrow*width + ncol;
			currentspot->numb = p->rgbBlue / 15;
			currentspot->numg = p->rgbGreen / 15;
			currentspot->numr = p->rgbRed / 15;
			for (i = nrow - 1; i < nrow + 2; i++)
			{
				for (j = ncol - 1; j < ncol + 2; j++)
				{
					if (i >= 0 && i < height&&j >= 0 && j < width)
					{
						p = rgb + i*width + j;
						amountdifferone = imagecolordiffer(p, currentspot);
						amountdiffertwo = imagecolordiffer(p, rgbamount);
						if ((i == nrow || j == ncol) && p->sign == 0 && amountdifferone<10 && amountdiffertwo > 4)
						{
							loopnum++;
							if (choice == 1)
							{
								p->rgbBlue = 128;
								p->rgbGreen = 128;
								p->rgbRed = 128;
							}
							p->sign = 1;
							ColorFillingsame(i, j, 1);
						}
					}
				}
			}

		}
		int LoopBoundary()
		{
			/*求封闭空间的左下角和右上角的坐标(loop字符:ABDOPQ/abdegopq/46890）*/
			int i, j;
			int m, n;
			//竖直方向投影
			int vertspotamount[122];
			int startx, endx;
			//水平方向投影
			int horzspotamount[35];
			int starty1, endy1, starty2, endy2, starty3, endy3;

			//数列上升下降趋势
			int rise, down;
			//粘连位置
			int adjoinposition1 = 0, adjoinposition2 = 0;
			int partamount1 = 0, partamount2 = 0, partamount3 = 0;

			int sum = 0;
			int loopamount = 0;

			imagergb p, q, r;
			closurespace cp;
			clospace = (closurespace)malloc(8 * sizeof(ENCLOSUSE));

			//竖直方向投影
			cout << "blackspot:";
			for (i = 0; i < width; i++)
			{
				vertspotamount[i] = 0;
				for (j = 0, p = rgb + j*width + i; j < height; j++, p = p + width)
				{
					if (p->rgbBlue == 0)
					{
						vertspotamount[i]++;
					}
				}
				cout << vertspotamount[i] << "     ";

			}
			cout << endl;


			//判断是否是loop，并计算封闭空间边界，左下角和右上角坐标
			startx = 0;
			endx = 0;

			for (i = 1, cp = clospace; i < width - 1; i++)
			{
				if (vertspotamount[i] != 0 && vertspotamount[i - 1] == 0)
				{
					//开始位置
					cp->leftdownx = i;
					startx = i;
				}
				if (vertspotamount[i] != 0 && vertspotamount[i + 1] == 0)
				{
					//结束位置
					cp->rightupx = i;
					endx = i;
					loopamount++;
				}
				if (startx != 0 && endx != 0)
				{
					cout << "竖直:" << endl;
					cout << "leftdownx:" << cp->leftdownx << "   ";
					cout << "rightupx:" << cp->rightupx << endl;
					//判断是不是loop
					//rise = 0;
					//down = 0;
					adjoinposition1 = 0;
					adjoinposition2 = 0;
					for (m = startx + 1; m <= endx - 1; m++)
					{
						if ((((vertspotamount[m - 1] - vertspotamount[m]) >= 2) || ((vertspotamount[m + 1] - vertspotamount[m]) >= 2))
							&& vertspotamount[m] <= vertspotamount[m - 1] && vertspotamount[m] <= vertspotamount[m + 1] && vertspotamount[m] < 5
							&& (m - startx)>2 && (endx - m) > 2 && vertspotamount[m] < 10)
						{
							if (adjoinposition1 == 0)
							{
								adjoinposition1 = m;
								cout << "adjoin1:" << adjoinposition1 << "    ";
							}
							else
							{
								if (adjoinposition2 == 0)
								{
									adjoinposition2 = m;
									cout << "adjoin2:" << adjoinposition2 << endl;
								}
							}
						}
					}



					//竖直方向投影存在波峰波谷
					if (adjoinposition1 != 0)
					{
						//向水平方向投影，判断该区域是不是loop,用cp中的sign标记,1是loop，2不是loop
						//水平方向：三部分
						partamount1 = horzpartamount(startx, adjoinposition1, 1, 0);
						if (adjoinposition2 != 0)
						{
							partamount2 = horzpartamount(adjoinposition1, adjoinposition2, 1, 0);
							partamount3 = horzpartamount(adjoinposition2, endx, 1, 0);
							if (partamount1 == 1 && partamount2 == 1 && partamount3 == 1)
							{
								//三部分水平投影皆为1，则取粘连分割点adjoinposition1作为这三部分的分割点
								cp->sign = 2;
								cp->leftdownx = adjoinposition1;
								cp->rightupx = adjoinposition1;
								for (m = 1, p = rgb + m*width + adjoinposition1; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->rightupy = m;
									}
									if ((p + width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}
							}
							if (partamount1 == 1 && partamount2 == 2 && partamount3 == 1)
							{
								//中间部分水平投影为2，中间有可能是字符，两边为字符与字符形成的封闭空间
								cp->sign = 2;
								cp->leftdownx = adjoinposition1;
								cp->rightupx = adjoinposition1;
								for (m = 1, p = rgb + m*width + adjoinposition1; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->rightupy = m;
									}
									if ((p + width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}
								++cp;
								loopamount++;
								cp->sign = 2;
								cp->leftdownx = adjoinposition2;
								cp->rightupx = adjoinposition2;
								for (m = 1, p = rgb + m*width + adjoinposition2; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->rightupy = m;
									}
									if ((p + width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}

							}
							if (partamount1 == 2 && partamount2 == 1 && partamount3 == 2)
							{
								//两边水平投影为2，有可能为字符，中间部分为字符与字符形成的空隙组成的封闭空间
								cp->sign = 2;
								cp->leftdownx = (adjoinposition1 + adjoinposition2) / 2;
								cp->rightupx = (adjoinposition1 + adjoinposition2) / 2;
								for (m = 1, p = rgb + m*width + cp->leftdownx; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->rightupy = m;
									}
									if ((p + width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}
							}
						}
						else
						{
							//水平方向：两部分
							partamount2 = horzpartamount(adjoinposition1, endx, 1, 0);
							//两部分在水平方向的投影部分皆为1，则取粘连分割点adjoinposition1
							if (partamount1 == 1 && partamount2 == 1)
							{
								cp->sign = 2;
								cp->leftdownx = adjoinposition1;
								cp->rightupx = adjoinposition1;
								for (m = 1, p = rgb + m*width + adjoinposition1; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->rightupy = m;
									}
									if ((p + width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}
							}
							//水平方向投影一个为1，一个为2，则认为1是2中的一部分，像8,9等字符，因此取2部分中的上面部分作为划分区域，计算左上角和右下角loop坐标
							if ((partamount1 == 1 && partamount2 == 2) || (partamount1 == 2 && partamount2 == 1))
							{
								cp->sign = 1;
								if (partamount1 == 1)
								{
									cp->leftdownx = adjoinposition1;
									cp->rightupx = endx;
								}
								else
								{
									cp->leftdownx = startx;
									cp->rightupx = adjoinposition1;
								}
								for (m = 1, p = rgb + m*width + cp->leftdownx; m < height - 1; m++, p = p + width)
								{
									if ((p - width)->rgbBlue == 255 && p->rgbBlue == 0)
									{
										cp->leftdowny = m;
										break;
									}
								}
								for (m = 1, p = rgb + m*width + cp->rightupx; m < height - 1; m++, p = p + width)
								{
									if (p->rgbBlue == 0 && (p + width)->rgbBlue == 255)
									{
										cp->rightupy = m;
										break;
									}
								}

							}

						}
						cp++;
						startx = 0;
						endx = 0;
						adjoinposition1 = 0;
						adjoinposition2 = 0;
					}


					//封闭空间竖直投影不具有波峰波谷，根据长宽、投影部分数、黑点数来判断是不是loop
					else
					{
						//对应竖直投影位置求水平坐标
						cout << "水平:" << endl;
						starty1 = 0;
						endy1 = 0;
						starty2 = 0;
						endy2 = 0;
						starty3 = 0;
						endy3 = 0;

						//水平投影
						sum = 0;
						for (m = 0; m < height; m++)
						{
							horzspotamount[m] = 0;
							for (n = startx, q = rgb + m*width + n; n <= endx; n++, q++)
							{
								if (q->rgbBlue == 0)
								{
									horzspotamount[m]++;
									sum++;
								}
							}
							cout << horzspotamount[m] << "     ";
						}
						cout << endl;
						if (sum == 1)
						{
							cp->sign = 0;
							loopamount--;
							startx = 0;
							endx = 0;
							adjoinposition1 = 0;
							adjoinposition2 = 0;
						}
						else
						{
							//水平方向开始、结束位置
							int mark = 0;
							for (m = 1; m < height - 1; m++)
							{
								if (horzspotamount[m - 1] == 0 && horzspotamount[m] != 0)
								{
									if (mark == 0)
									{
										starty1 = m;
									}
									else if (mark == 1)
									{
										starty2 = m;
									}
									else
									{
										starty3 = m;
									}
								}
								if (horzspotamount[m] != 0 && horzspotamount[m + 1] == 0)
								{
									if (mark == 0)
									{
										endy1 = m;
										mark = 1;
									}
									else if (mark == 1)
									{
										endy2 = m;
										mark = 2;
									}
									else
									{
										endy3 = m;
									}
								}
							}


							//根据开始、结束位置进行判断，并进行开始结束位置的修改
							if (starty3 != 0)
							{
								//水平投影：三部分

								cp->sign = 2;
								//求边界坐标
								for (n = height - 1, q = rgb + n*width + startx; n > 1; n--, q = q - width)
								{
									if (q->rgbBlue == 255 && (q - width)->rgbBlue == 0)
									{
										cp->leftdowny = n - 1;
										break;
									}
								}
								for (n = 1, q = rgb + n*width + endx; n < height - 1; n++, q = q + width)
								{
									if (q->rgbBlue == 255 && (q + width)->rgbBlue == 0)
									{
										cp->rightupy = n + 1;
										break;
									}
								}
							}
							else if (starty2 != 0)
							{
								//水平投影：两部分
								//判断是不是loop,上下两部分黑点总数
								int sum1 = 0, sum2 = 0;
								for (m = starty1; m <= endy1; m++)
								{
									sum1 += horzspotamount[m];
								}
								for (m = starty2; m <= endy2; m++)
								{
									sum2 += horzspotamount[m];
								}
								if (sum1 < 10 && sum2 < 10)
								{
									//不是loop
									cp->sign = 2;
									for (n = height - 1, q = rgb + n*width + startx; n > 1; n--, q = q - width)
									{
										if (q->rgbBlue == 255 && (q - width)->rgbBlue == 0)
										{
											cp->leftdowny = n - 1;
											break;
										}
									}
									for (n = 1, q = rgb + n*width + endx; n < height - 1; n++, q = q + width)
									{
										if (q->rgbBlue == 255 && (q + width)->rgbBlue == 0)
										{
											cp->rightupy = n + 1;
											break;
										}
									}

								}
								else
								{
									//根据上下部分判断是不是loop								
									if (sum1 == 1 || sum2 == 1)
									{
										if (sum1 == 1)
										{
											cp->sign = 1;
										}
										else
										{
											//202
											if (sum1 - sum2 > 20)
											{
												cp->sign = 1;
											}
											else
											{
												cp->sign = 2;
											}
										}
										for (n = height - 1, q = rgb + n*width + startx; n > 1; n--, q = q - width)
										{
											if (q->rgbBlue == 255 && (q - width)->rgbBlue == 0)
											{
												cp->leftdowny = n - 1;
												break;
											}
										}
										for (n = 1, q = rgb + n*width + endx; n < height - 1; n++, q = q + width)
										{
											if (q->rgbBlue == 255 && (q + width)->rgbBlue == 0)
											{
												cp->rightupy = n + 1;
												break;
											}
										}
									}
									else if (sum1 < sum2)
									{
										cp->sign = 1;
										//取上面部分定坐标
										for (n = startx, r = rgb + endy1*width + n; n <= endx; n++, r++)
										{
											if (r->rgbBlue == 0 && (r - 1)->rgbBlue == 255)
											{
												cp->leftdownx = n;
												cp->leftdowny = endy1;
												break;
											}
										}
										for (n = endx, r = rgb + starty1*width + n; n >= startx; n--, r--)
										{
											if (r->rgbBlue == 0 && (r + 1)->rgbBlue == 255)
											{
												cp->rightupx = n;
												cp->rightupy = starty1;
												break;
											}
										}
									}
									else
									{
										cp->sign = 1;
										//下面部分定坐标
										for (n = startx, r = rgb + endy2*width + n; n <= endx; n++, r++)
										{
											if (r->rgbBlue == 0 && (r - 1)->rgbBlue == 255)
											{
												cp->leftdownx = n;
												cp->leftdowny = endy2;
												break;
											}
										}
										for (n = endx, r = rgb + starty2*width + n; n >= startx; n--, r--)
										{
											if (r->rgbBlue == 0 && (r + 1)->rgbBlue == 255)
											{
												cp->rightupx = n;
												cp->rightupy = starty2;
												break;
											}
										}
									}
								}
							}
							else
							{
								int sum1 = 0;
								rise = 0;
								down = 0;
								//水平方向起伏情况以及投影为1的数目来判断
								for (m = starty1; m <= endy1 - 1; m++)
								{
									if (horzspotamount[m] == 1)
									{
										sum1++;
									}
									if (horzspotamount[m] < horzspotamount[m + 1] && rise == 0)
									{
										rise = 1;
									}
									if (horzspotamount[m]>horzspotamount[m + 1] && down == 0)
									{
										down = 1;
									}
									if (horzspotamount[m] < horzspotamount[m + 1] && down == 1)
									{
										rise = 2;
									}
									if (horzspotamount[m]>horzspotamount[m + 1] && rise == 2)
									{
										down = 2;
									}

								}
								if (rise == 2 || down == 2 || sum1 >= (endy1 - starty1) / 2)
								{
									//不是loop,选右边作为分割点
									cp->sign = 2;
									cp->leftdownx = cp->rightupx;
									cp->rightupx = cp->leftdownx;
									for (m = 1, r = rgb + m*width + cp->rightupx; m < height - 1; m++, r = r + width)
									{
										if (r->rgbBlue == 255 && (r + width)->rgbBlue == 0)
										{
											cp->rightupy = m;
										}
										if (r->rgbBlue == 0 && (r + width)->rgbBlue == 255)
										{
											cp->leftdowny = m;
										}
									}
								}
								else
								{
									//是loop
									cp->sign = 1;
									for (n = height - 1, q = rgb + n*width + startx; n > 1; n--, q = q - width)
									{
										if (q->rgbBlue == 255 && (q - width)->rgbBlue == 0)
										{
											cp->leftdowny = n - 1;
											break;
										}
									}
									for (n = 1, q = rgb + n*width + endx; n < height - 1; n++, q = q + width)
									{
										if (q->rgbBlue == 255 && (q + width)->rgbBlue == 0)
										{
											cp->rightupy = n + 1;
											break;
										}
									}
								}
							}

							startx = 0;
							endx = 0;
							adjoinposition1 = 0;
							adjoinposition2 = 0;
							cp++;
						}


					}
				}
			}
			cout << "loopamount:" << loopamount << endl;
			return loopamount;
		}
		void DropFallroute(int inipointx, int inipointy, int xindex2, int yindex2)
		{
			//向下滴落,图片是灰度的（白字符、灰背景、封闭区域是黑色）
			imagergb p;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright, pointdownleftleft, pointdownrightright;
			int leftrightflg;
			leftrightflg = 0;
			//标记滴水算法中滴落点流下的路径，p->sign=1;
			while ((inipointx <= xindex2) && (inipointy <= yindex2))
			{
				p = rgb + inipointx*width + inipointy;
				//标记分割路径
				p->sign = 1;
				pointleft = (p - 1)->rgbBlue;
				pointright = (p + 1)->rgbBlue;
				pointdown = (p + width)->rgbBlue;
				pointdownleft = (p + width - 1)->rgbBlue;
				pointdownright = (p + width + 1)->rgbBlue;
				pointdownleftleft = (p + width - 2)->rgbBlue;
				pointdownrightright = (p + width + 2)->rgbBlue;

				if (((pointleft == 128) && (pointright == 128) && (pointdown == 128) && (pointdownleft == 128) && (pointdownright == 128))
					|| ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
					|| ((pointdownleft == 255) && (pointdown == 128)))
				{
					//向下
					inipointx = inipointx + 1;
					leftrightflg = 0;
				}
				else if ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 128 || pointdownleft == 0) && (pointdownright == 255))
				{
					//左下
					if (pointdownleft == 0)
					{
						inipointx = inipointx + 1;
						inipointy = inipointy - 1;
						leftrightflg = 0;
					}
					else
					{
						//查看左左下有没有黑点
						if (pointdownrightright == 0)
						{
							inipointx = inipointx + 1;
							inipointy = inipointy + 2;
							leftrightflg = 0;
						}
						else
						{
							inipointx = inipointx + 1;
							inipointy = inipointy - 1;
							leftrightflg = 0;
						}
					}

				}
				else if ((pointdown == 255) && (pointdownleft == 255) && (pointdownright == 128 || pointdownright == 0))
				{
					//右下
					if (pointdownright == 0)
					{
						inipointx = inipointx + 1;
						inipointy = inipointy + 1;
						leftrightflg = 0;
					}
					else
					{
						if (pointdownleftleft == 0)
						{
							inipointx = inipointx + 1;
							inipointy = inipointy - 2;
							leftrightflg = 0;
						}
						else
						{
							inipointx = inipointx + 1;
							inipointy = inipointy + 1;
							leftrightflg = 0;
						}
					}

				}
				else if ((pointright == 128) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
				{
					//右边，分两种情况，防止不断循环
					if (leftrightflg == 0)
					{
						//第一次
						inipointy = inipointy + 1;
						leftrightflg = 1;
					}
					else
					{
						inipointx = inipointx + 1;
						leftrightflg = 0;
					}
				}
				else if ((pointleft == 128) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
				{
					//左边，分两种情况，防止不断循环
					if (leftrightflg == 0)
					{
						inipointy = inipointy - 1;
						leftrightflg = 1;
					}
					else
					{
						inipointx = inipointx + 1;
						leftrightflg = 0;
					}
				}
				else
				{
					//剩下的情况
					inipointx = inipointx + 1;

				}
			}
		}
		void DowndropFallroute(int inipointx, int inipointy, int xindex1, int yindex2, int trend)
		{
			//向上滴落,图片是灰度的（白字符，灰色背景，黑色封闭区域）
			imagergb p;
			int i, j;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright;
			int leftrightflg;
			leftrightflg = 0;
			//标记滴水算法中滴落点流下的路径，p->sign=1;
			while ((inipointx >= xindex1) && (inipointy <= yindex2))
			{
				p = rgb + inipointx*width + inipointy;
				//标记分割路径
				p->sign = 1;
				pointleft = (p - 1)->rgbBlue;
				pointright = (p + 1)->rgbBlue;
				pointdown = (p - width)->rgbBlue;
				pointdownleft = (p - width - 1)->rgbBlue;
				pointdownright = (p - width + 1)->rgbBlue;

				if (((pointleft == 128) && (pointright == 128) && (pointdown == 128) && (pointdownleft == 128) && (pointdownright == 128))
					|| ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
					|| ((pointdownleft == 255) && (pointdown == 128)))
				{
					//向上
					inipointx = inipointx - 1;
					leftrightflg = 0;
				}
				else if ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 128) && (pointdownright == 255))
				{
					//左上
					inipointx = inipointx - 1;
					inipointy = inipointy - 1;
					leftrightflg = 0;
				}
				else if ((pointdown == 255) && (pointdownleft == 255) && (pointdownright == 128))
				{
					//右上
					inipointx = inipointx - 1;
					inipointy = inipointy + 1;
					leftrightflg = 0;
				}
				else if ((pointright == 128) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
				{
					//右边，分两种情况，防止不断循环
					if (leftrightflg == 0)
					{
						//第一次
						inipointy = inipointy + 1;
						leftrightflg = 1;
					}
					else
					{
						if (pointright == 128 && pointdown == 255)
						{
							inipointy = inipointy - 1;
						}
						else
						{
							inipointx = inipointx - 1;
							leftrightflg = 0;
						}

					}
				}
				else if ((pointleft == 128) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
				{
					//左边，分两种情况，防止不断循环
					if (leftrightflg == 0)
					{
						inipointy = inipointy - 1;
						leftrightflg = 1;
					}
					else
					{
						inipointx = inipointx - 1;
						leftrightflg = 0;
					}
				}
				else
				{
					//剩下的情况
					inipointx = inipointx - 1;
				}


			}
		}
		void DropFall()
		{
			/*滴水算法*/
			int x, y;
			//字符区域上下左右边界
			int xindex1, xindex2, yindex1, yindex2;
			//字符区域宽度和高度
			int validwidth, validheight;
			//标志某些位置可能会出现循环情况；
			int loopflg = 0;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright;
			//滴落点
			int addval;
			int inipointx, inipointy, leftrightflg;
			int loopamount;

			int upedgeheight[90];
			int downedgeheight[90];
			int updropspot[4] = { 0, 0, 0, 0 };
			int downdropspot[4] = { 0, 0, 0, 0 };
			int num = 0;
			int samenum;
			int firstspot;
			int leftposition[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			int rightposition[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			int letterwidth;
			imagergb p;

			imagergb letter1, letter2, letter3, letter4;
			letter1 = (imagergb)malloc((width / 3)*height*sizeof(RGBQUADS));
			letter2 = (imagergb)malloc((width / 3)*height*sizeof(RGBQUADS));
			letter3 = (imagergb)malloc((width / 3)*height*sizeof(RGBQUADS));
			letter4 = (imagergb)malloc((width / 3)*height*sizeof(RGBQUADS));

			xindex1 = 0;
			xindex2 = 0;
			yindex1 = 0;
			yindex2 = 0;


			//求字符区域宽度和高度
			for (x = 0; (x < height) && (loopflg == 0); x++)
			{
				for (y = 0, p = rgb + x*width + y; (y < width) && (loopflg == 0); y++, p++)
				{
					if (p->rgbBlue == 255)
					{
						xindex1 = x;
						loopflg = 1;
					}
				}
			}
			loopflg = 0;
			for (x = height - 1; (x>0) && (loopflg == 0); x--)
			{
				for (y = 0, p = rgb + x*width + y; (y < width) && (loopflg == 0); y++, p++)
				{
					if (p->rgbBlue == 255)
					{
						xindex2 = x;
						loopflg = 1;
					}
				}
			}
			loopflg = 0;
			for (y = 0; (y < width) && (loopflg == 0); y++)
			{
				for (x = 0, p = rgb + x*width + y; (x < height) && (loopflg == 0); x++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						yindex1 = y;
						loopflg = 1;
					}
				}
			}
			loopflg = 0;
			for (y = width - 1; (y>0) && (loopflg == 0); y--)
			{
				for (x = 0, p = rgb + x*width + y; (x < height) && (loopflg == 0); x++, p = p + width)
				{
					if (p->rgbBlue == 255)
					{
						yindex2 = y;
						loopflg = 1;
					}
				}
			}
			validwidth = yindex2 - yindex1;
			validheight = xindex2 - xindex1;

			//计算字符宽度
			//letterwidth = calculateletterwidth();
			ColorFillingbinary(1, 1, 0, 128);
			//查找loop字符位置
			loopamount = LoopBoundary();
			LetterSegmentloop(loopamount, xindex1, xindex2, yindex1, yindex2, 4);

			//滴落路径
			//DropFallroute(inipointx, inipointy, xindex2, yindex2);
			//用颜色标记滴落路径
			for (x = 0; x < width; x++)
			{
				for (y = 0, p = rgb + y*width + x; y < height; y++, p = p + width)
				{
					if (p->sign == 1)
					{
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 255;
					}
				}
			}
		}

		void readimagergb(Bitmap^ img)
		{
			//读取图片信息
			int i, j;
			imagergb p;
			/*width = img->Width;
			height = img->Height;*/

			setImgWidth(img->Width);
			getImgHeight(img->Height);

			rgb = (imagergb)malloc((width *height)*sizeof(RGBQUADS));
			for (i = 0, p = rgb; i < height; i = i + 1)
			{
				for (j = 0; j < width; j = j + 1, p++)
				{
					Color c = img->GetPixel(j, i);
					p->rgbBlue = c.B;
					p->rgbGreen = c.G;
					p->rgbRed = c.R;
					p->rgbReversed = c.A;
				}
			}
		}

		//设置图像宽
		void setImgWidth(int imgWidth){
			width = imgWidth;
		}

		//获取图像宽
		int getImgWidth(){
			return width;
		}
		//设置图像高度
		void getImgHeight(int imgHeight){
			height = imgHeight;
		}

		//获取图像高度
		int getImgHeight(){
			return height;
		}

		void setOrigincountcolorvarietyamount(int value){
			origincountcolorvarietyamount = value;
		}

		int getOrigincountcolorvarietyamount(){

			return origincountcolorvarietyamount;

		}

		//获取一个图像的imagergb 数组
		//param:img 图像
		//returns imagergb数组
		imagergb getImagergb(){
			imagergb  rgbImg;
			rgbImg = rgb;
			return rgbImg;
		}


		Bitmap^ writeimagergb()
		{
			int i, j, m, n, num;
			int newwidth, newheight;
			imagergb  p, q;
			if (amplify == 0)
			{
				Bitmap^ modifybm = gcnew Bitmap(width, height);
				for (i = 0; i < height; i++)
				{
					for (j = 0, p = rgb + i*width; j < width; j++, p++)
					{
						modifybm->SetPixel(j, i, Color::FromArgb(p->rgbRed, p->rgbGreen, p->rgbBlue));
					}
				}
				return modifybm;
			}
			else
			{
				//将原来的图片，空白边缘部分扩宽扩长
				newwidth = width * 2;
				newheight = newwidth*height / width;
				Bitmap^modifybm = gcnew Bitmap(newwidth, newheight);

				for (i = 0, p = rgb; i < newheight; i++)
				{
					for (j = 0; j < newwidth; j++)
					{
						if (i < newheight / 4 || i >= 3 * newheight / 4 || j < newwidth / 4 || j >= 3 * newwidth / 4)
						{
							modifybm->SetPixel(j, i, Color::FromArgb(0, 0, 0));
						}
						else
						{

							modifybm->SetPixel(j, i, Color::FromArgb(p->rgbRed, p->rgbGreen, p->rgbBlue));
							p++;
						}
					}
				}
				return modifybm;
			}
		}
		void freememoryzone()
		{
			//delete rgb;
			//	delete rgbamount;

			free(rgb);
			free(rgbamount);

			delete freergb;
		}
	};
}
