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
		int Grayvalue;//�Ҷ�ֵ
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
		int sign;//loopΪ1��������տռ�Ϊ2;
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

			statisticsrgbcolor();//��������ɫ��������л��֣��������ֺ������صĺϲ�����
			//�жϸ�ͼƬ�Ƿ���Ҫȥ�߿򣬷�ֹ��������Ե����ĸ��ȥ
			judgeframe();
			dividetreatmentimage();   //������֤��
		}
		void statisticsrgbcolor()
		{
			int firstcountamount;
			firstcountamount = originalstatisticsrgbcolor();
			proceddingstatisticsrgbcolor(firstcountamount);
		}
		void judgeframe()
		{
			/*�����ж���֤��ͼƬ�Ƿ���Ҫȥ�߿򣬸��������߿����ɫ���ص����Ƿ񳬹�ͼƬ4/5�Ŀ��/�߶����ж�
			���������Ϊ���Ǳ߿���Ҫȥ�߿򣬷�����Ϊ������ĸ�����߿����������ȥ�߿�*/
			int i, j, amountdiffer;
			int  amountfirsti, amountendi, amountfirstj, amountendj;
			int acountspotamountone, acountspotamounttwo;

			imagergb  p;
			amountfirsti = 0;
			amountendi = 0;
			amountfirstj = 0;
			amountendj = 0;
			acountspotamountone = calculatedifferspot(10, 5);/*num��������ͳ����Χ��ͬ������������Χ��ͬ�㳬��num�����ص���*/
			acountspotamounttwo = calculatedifferspot(20, 5);



			imagergb Imagergb = getImagergb();

			for (i = 0, p = Imagergb; i < width; i++, p++)
			{
				//ͳ�Ƶ�һ�в�ͬ�ڱ���ɫ�����ص�����
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
				//ͳ�Ƶ�һ�в�ͬ�ڱ���ɫ�����ص�����
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
				//û�б߿��ͼƬ����ͼƬ��С��
				if (acountspotamountone - acountspotamounttwo <= 0 && width <= 40)
				{
					amplify = 1;
				}
			}
		}

		void discardframe()
		{
			//ȥ�߿�
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
			////���ຯ��
			int bgcolor;
			if (rgbReversedture == 0)
			{
				bgcolor = acountbackgroundcolorquantity();
				if ((bgcolor == 1 || bgcolor == 2 || bgcolor == 3) && width <= 200)
				{
					//���ֱ�����ɫСͼ
					treatmentimageone(bgcolor);
				}
				else if (bgcolor == 0 && width <= 200)
				{
					//һ�ֱ�����ɫСͼ
					treatmentimagetwo();
				}
				else if (bgcolor == 0 && width > 200)
				{
					//һ�ֱ�����ɫ�Ĵ�ͼ	
					treatmentimagethree();
				}
			}
			else
			{
				//ͼ����ɫ���ٵ�ͼ		
				treatmentimagefour();
			}
		}

		int acountbackgroundcolorquantity()
		{
			/*�жϸ�ͼƬ�Ƕ౳�����ǵ�����:���ݲ�ͬ�ڱ���ɫ����ɫ�����������Լ������к���ɫ�������������ۺ��ж�*/
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
						//̫��ı�����ɫ����Ϊ��˫����
						return 0;
					}
					else if (amountdiffer > 20)
					{
						//̫�����ɫ����Ϊ�������ڶ��ֱ���
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
										countnum++;//�ܹ��ж�����������ɫ
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
										countnumwidth++;//��������
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
										countnumheight++;//�ܹ�������������ɫ
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
			//�౳��Сͼ����
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
				//���
				imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 2, 13, 0);
				blackwhitediscardrgbspot(5, 255);
			}
			else
			{
				if (coloramount < 145)
				{
					//����
					imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 13, 0);
					blackwhitediscardrgbspot(5, 255);
				}
				else
				{
					//58,̨��,����
					if (colortype >= 4)
					{
						if (width < 100)
						{
							//̨��
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
							//����
							imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 12, 0);//yuan7->xian13
							blackwhitediscardrgbspot(5, 255);
						}
						else
						{
							//̨��
							imagetransferblackwhitetwo(rgbamount, rgbamount + backlocation, 6, 8, 0);//yuan7->xian13
							blackwhitediscardrgbspot(7, 255);
						}
					}
				}
			}
		}
		void treatmentimagetwo()
		{
			//������Сͼ
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

			//ֻ��һ����ɫ��ͼƬ��������ͼƬ�ֿ����������ж��Ƿ���������㣬��������
			if (colornum1 > 350 && colornum2 < 102)
			{
				if ((color1->numb + color1->numg + color1->numr) >= 3)
				{
					//üɽ���Ĵ�
					treatmentsamecolorline();
				}
				else
				{
					//ͭ�����Ӱ������ݡ�����
					colortoblackwhite(10, 0, width - 1, 0);
					DropFall();
				}
			}
			else
			{
				noisespot = judgenoisespot();
				//�ж����ߺ�����Ŀǰֻ�����Ƿ������ߣ����ִ�ϸ
				noiseline = judgenoiseline();
				if (noiseline == 0)
				{
					if (noisespot == 0)
					{
						//�����������
						//���ݣ���ɽ
						treatmentimagetwowithoutspot();
					}
					else
					{
						//�����������
						//����
					}
				}
				else
				{
					if (noiseline == 1)
					{
						//ϸ����
						//�𻪣���������ͨ�����ݣ�����,������,1�ŵ�
						type = separateimagenoiseline(noisespot);
						if (type == 1)
						{
							//��
							treatmentthinline();
						}
						if (type == 2)
						{
							//��ͨ
							treatmentimagetwoline();
						}
						if (type == 3)
						{
							//����,������
							treatmentimagetwospot(noisespot);
						}
						if (type == 4)
						{
							//����
							treatmentthinlineliber();
						}
						if (type == 5)
						{
							//����
							discardcolorlump(4);
						}
						if (type == 6)
						{
							//1�ŵ�--�����ַ�
							treatmenthollowchar();
							blackAndWhiteExchange();//�������֮��ɫ�ʶԻ�
							//colortoblackwhite(3, 0, width - 1, 255);
							//	grayprocess(128);
							//DropFall();
						}
					}
					else
					{
						//������
						//����
					}
				}
			}
		}
		void treatmentimagethree()
		{
			//��������ͼ
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
			//��͸���ȣ���ɫ�ٵ�ͼƬ
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
				//�Ͼ�
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

			//����
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
				//���������
				imagetransferblackwhite(rgbamount, 7);//7->12
			}
			else
			{
				//ǳ���������ݣ���ɽ
				imagetransferblackwhite(rgbamount, 12);//7->12
			}
			maxwidth = calculatemaxamountwidth();
			if (maxwidth >= 2)//��ĸ�Ͽ��¼�
				blackwhitediscardrgbspot(6, 255);
			else//��ĸ��ϸ
				blackwhitediscardrgbspot(7, 255);//yuan7->xian5������ĸ��,yuan5->xian7(ϸ��ĸ)
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
				//58����
				//����βû�к��ֲ���ֱ����Ϊ����ɫ
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
				//58�Ӽ�����ĸ
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
			//������ͨ�ĺ�������ͨ��֤���Ѿ������ı䣬��Ҫ����ʶ�𣬴˺�����ɾ��
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
			//��������ϸ����λ�ò��̶�
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

			//ˮƽ��ֱ����ͶӰ
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


			//��ֱˮƽ������������ɫ������
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

			//ȥ����
			if (verti == 0 && horzi == 0)
			{
				//���߶�,����ϸ����ɫȥ���ߣ����Ժ��ԣ�
			}
			else
			{
				if (horzi != 0)
				{
					//ˮƽ����תΪ��ֱ���꣬����ֱȥ����
					verti = horzi;
					vertj = horzj;
					mark = 1;
				}


				if (verti != 0)
				{
					//��ֱȥ����
					spotj = vertj;
					middle = spotj;
					if (mark == 0)
					{
						//��ֱ����ȥ����
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
						//ˮƽת��Ϊ��ֱ����ȥ����
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
								//����ĸճ������
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
								//�հ׵��У�i������һ����Ŀ�ͽ���
								blacknum++;
								if (blacknum > 3)
								{
									break;
								}
							}
							else
							{
								//û������ĸճ������
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
								//�հ׵��У�i������һ����Ŀ�ͽ���
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


			//ȥ���������:��ֱ
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

			//�ָ��ַ��Լ���������
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
						//�����ַ������ַ���Ⱦ����뱳��ɫ�ĶԱȶ�
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
						//�����ַ�ճ�����ָ��ַ�(�ַ���ɫ��ͬ��
						Lettersegment(startposition[i], endposition[i], i);
						i--;
					}
				}
			}
		}
		void treatmentthinline()
		{
			//�㽭��
			int i, j, numb, numg, numr, amountdifferone, amountdiffertwo;
			//�������ǰ�벿��/��벿�ֵ�������ɫ�Ƿ��ҵ���
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
							//��¼ǰ�벿�����ߵ���ɫ
							priorlinecolor->numb = p->rgbBlue / 15;
							priorlinecolor->numg = p->rgbGreen / 15;
							priorlinecolor->numr = p->rgbRed / 15;
							priormark = j;
						}
						if ((i > width / 2) && (j == 1) && (endmark == 0))
						{
							//��¼��벿�����ߵ���ɫ
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
				//��ǰ�벿�����߽���ȥ��
				for (i = 1; i < width / 2; i++)
				{
					for (j = priormark, p = rgb + j*width + i; j <= priormark; j++)
					{
						amountdifferone = imagecolordiffer(p, priorlinecolor);
						amountdiffertwo = imagecolordiffer(p, rgbamount);
						if (amountdifferone < 4)
						{
							//������λ����������ɫ��ͬ��ֱ��ȥ��
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;

						}
						if ((amountdifferone>4) && (amountdiffertwo > 4))
						{
							//������λ���ҵ��Ȳ��Ǳ���ɫҲ��������ɫ����Ϊ��ĸ��ɫ��Ҫ����
							continue;
						}
						if (amountdiffertwo < 4)
						{
							//������λ���ҵ�����ɫ������λ���������������Ҵ�λ�øı�����²���һ��
							priormark = priormark + 1;
							i--;
							break;
						}
					}
				}
			}
			if (endmark != 0)
			{
				//�Ժ�벿�����߽���ȥ��
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
			//����,����Ч������û�ύ
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

						if (amountdifferone <= 15)//��ɫ
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;
						}
						else if (amountdiffertwo <= 5)//��ɫ�������
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
						{//������ɫ��Ϊ��ɫ
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


			//ͳ�Ƹ������ص�����
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
			//����洢��ĸ���ҿ�ʼ����λ�ã��У�

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
			//�Ĵ���üɽ��������������������Ч������û�ύ
			int i, j, k, s, t = 0, amountdiffer, markup = 0, markdown = 0;
			int mark1 = 0, mark2 = 0, mark3 = 0;
			int num = 0, num1 = 0;
			imagergb p, q, r;
			rgbcolor linecolor;
			//linecolor��ŵ���ͼƬ�е���ɫ����ɫ�����ǳɫ�����Ƚ϶�
			linecolor = rgbamount + 1;

			//��������ɨ��
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i + j*width; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if (amountdiffer <= 4)
					{
						//����ɫ
						p->sign = 0;
						continue;//����ɫ���ֲ���
					}
					else
					{
						amountdiffer = imagecolordiffer(p, linecolor);
						if (amountdiffer >= 3)
						{
							//ǳɫ
							p->sign = 1;
							//��ͬ���е�һЩ��ɫ��Ϊǳɫ
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
								//���ͳ�Ƶĸ��е�ɫ����3���򽫸��к�������Ϊ��ɫ
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
								if (amountdiffer > 4)//��ȻΪ��ɫ��������²���
								{
									q->sign = 1;
									continue;
								}
								else//���ҵĵ�Ϊ����ɫ����ʼ��һ�εĲ���
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
							//��ɫ
							//������ɫͬ����Χ�Ƿ���ǳɫ
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

							//��ɫ�����Ҹ��н�����λ���Ƿ�Ϊǳɫ�����߽���λ���Ƿ�Ϊ�߽磬��������
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


			//��������ɨ��
			for (i = 0; i < width; i++)
			{
				for (j = height - 1, p = rgb + j*width + i; j >= 0; j--, p = p - width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					//����ɫ
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
							//��ɫ
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
							//ǳɫ
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


			//��ʾͼƬ
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


		//�ڰ׵��ɫ�ʶԻ�
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
			//�ϵ������յ�
			int starti, startj, endi, endj;
			//���ߵ�б�ʺ͵�ǰҪ��������
			float ratio;
			int currenti, currentj;
			imagergb p, q, r;
			//������������������������begin  �����Դ��ע�Ϳɲο�downRepatching������ע�ͣ������Ĵ���һ�£���������ϸ΢���޸�

			//�϶����޲���ͼƬ(ȱ��С)������
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
					//����ַ��뿪���ַ����ֿ�
					if (num < 7)
					{
						i = j - 1;
					}
					else
					{
						//�����ַ���ȱ
						int next;
						for (m = i; m < j; m++)
						{
							next = m + 1;
							if (next == 83){

								next = next;
							}
							if (sign[next] - sign[m] >= 5)
							{

								//��¼���


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

								//		//��¼�յ�
								//		endi = n;
								//		endj = sign[endi];
								//		m = n - 1;
								//		break;
								//	}
								//}
								//����
								lineEquationRepatching(starti, startj, endi, endj);

							}
						}
						i = m - 1;
					}
				}
			}
		}

		//�������㹹��һ��ֱ�ߣ�ֱ�߾����ĵ��޲�Ϊ��ɫ
		void lineEquationRepatching(int starti, int startj, int  endi, int endj){


			int t;
			imagergb q;
			//����
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


		//�˺�������Ϊ�˺����Ŀ��ظ�����,�����ж�ȷ���޲����̵��յ㣬��㣻��Ҫ������ô˺����������߼��ſɿ��ⲿ�֣�����ָ�봫�Σ�ֻҪ�����޸�ԭֵ
		void repatchingSolution(int *sign, int *m, int* next, int *starti, int *startj, int  *endi, int *endj, int duanJ){
		
			//��¼���
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
				else if (abs(sign[n] - sign[*starti]) >= 3)     //��������������sign[n] ����뿪ʼ��sign[starti]��x���������������������֪��С��3���߱��������һ��
				{
					continue;
				}
				else
				{
					//��¼�յ�
					*endi = n;
					*endj = sign[*endi];
					*m = n - 1;
					break;
				}
			}
		
		}


		//�ַ����¶��߶�ȱ�ٵĲ��䣬˼·���϶��߲���Ļ���֮���޸ģ��϶�����treatmenthollowchar���������֡�
		void downRepatching(){
			int i, j;
			int m, n;
			int t;
			int amountdiffer;
			int num;
			int mark;
			//���ߵ�б�ʺ͵�ǰҪ��������
			float ratio;
			int currenti, currentj;
			//�ϵ������յ�
			int starti, startj, endi, endj;
			int sign[Nlength];
			imagergb p, q, r;
			for (i = 0; i < width - 1; i++)
			{
				sign[i] = 0;
				int nn = 0;   //ͳ��ÿ���ڵ��ܱߵľŸ����Ƿ񶼲��ǰ׵㣬��nn=9;
				bool kongzhiyu = false;
				for (j = height - 1, p = rgb + j*width + i; j > 0; j--, p = p - width)
				{
					if (p->rgbBlue == 0) //��ǰ��Ϊ�ڵ�
					{
						//if (r->rgbBlue != 0)continue;
						if (sign[i] == 0)          //��ʾ���������ݣ��򱣴�����Ϊj
						{
							sign[i] = j;
						}

						r = p - 5 * width;   //�����¶˴�����Ӱ��߽磬p - 5 * width��ʾȥ��5����Ӱ�㣬��ʼ����
						mark = 0;

						for (m = -1; m <= 1; m++)   ///ɨ�赱ǰ�����ά����9����
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
							kongzhiyu = true;   //�����н��������ɹ�һ�κ�ֱ������
						}

						nn = 0;
					}

					if (kongzhiyu)break;
				}
			}


			//�޲���ͼƬ(ȱ��С)������
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
					//����ַ��뿪���ַ����ֿ�
					if (num < 7)                                 //����7��sign[i]>1�ſ�ʼ�޸�
					{
						i = j - 1;
					}
					else
					{
						//�����ַ���ȱ
						int next;
						for (m = i; m < j; m++)
						{
							next = m + 1;
							if (sign[next] - sign[m] <= -5 )      //��������sign[next] �� sign[m]���x�����С����
							{
								//��¼���
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

								//		//��¼�յ�
								//		endi = n;
								//		endj = sign[endi];
								//		m = n - 1;
								//		break;
								//	}
								//}
								//����
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
		//�¶��߿�ʼ�޲�ǰ����Ҫ����һЩ����ɫ��Ŀ��Ϊ�˻ص��޲�����ʱ��ͬ���ĳ���
		void	dealBeforeDownRepatching(){

			ColorFillingbinary(0, 0, 100, 180);

			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp",100));
			imagergb p, q;
			int i, j, m, n;
			int sum;
			for (i = 0; i < width - 1; i++)
			{
				for (j = height - 1, p = rgb + j*width + i; j > 0; j--, p = p - width)
				{
					if (p->rgbBlue == 0) //��ǰ��Ϊ�ڵ�
					{
						int mark = 0;
						for (m = -1; m <= 1; m++)   ///ɨ�赱ǰ�����ά����9����
						{
							for (n = -1, q = p + m*width + n; n <= 1; n++, q++)
							{
								if (q->rgbBlue == 100)
								{
									mark = 1;
								}
							}
						}
						if (mark == 1) ///ɨ�赱ǰ�����ά����9���㣬�����һ����Ϊ��ͬ�����ʾΪ1����ǰ��������ɫ�任,���Ϊ��ɫ��255��0��0��
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;

						}
					}
				}
			}
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp", 99));
			changColor(100, 255);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp", 101));
			ColorFillingbinary(0, 0, 180, 100);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp", 102));
		}

		//��ֱ����ȱ���޲�
		void 	verticalRepatching()
		{

			imagergb p;
			int i, j;
			//��ֱ����ȱ�ڲ����������ȱ�ڵĴ�С����������֮�ڵġ�������ÿ���ڵ�������������Ƿ�Ϊ��ɫ������²���
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
			//��ֱ����ȱ�ڲ����������ȱ�ڵĴ�С����������֮�ڵġ�������ÿ���ڵ�������������Ƿ�Ϊ��ɫ������²���
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

		//���һ������������ֹ����ַ��ײ���߽��γɴ����
		void upEndline(){
			imagergb imagerRGB = getImagergb();

			//���һ������������ֹ����ַ��ײ���߽��γɴ����
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

		//��sign���鸳ֵ�����޸�start��end��ֵ
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
					if (p->rgbBlue == 0) //��ǰ��Ϊ�ڵ�
					{
						if (sign[i] == 0)          //��ʾ���������ݣ��򱣴�����Ϊj
						{
							sign[i] = j;
						}
						mark = 0;

						for (m = -1; m <= 1; m++)   ///ɨ�赱ǰ�����ά����9���㣬�����һ����Ϊ�׵����ʾΪ1
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
							kongzhiyu = true;//���Ƶ�һ�θ�ֵsign[i]��һ����ֵ
						}
						if (mark == 1) ///ɨ�赱ǰ�����ά����9���㣬�����һ����Ϊ��ͬ�����ʾΪ1����ǰ��������ɫ�任,���Ϊ��ɫ��255��0��0��
						{
							p->rgbBlue = 0;
							p->rgbGreen = 0;
							p->rgbRed = 255;


							if (!kongzhiyu){ sign[i] = -1; }                                //��ʾ���±�Ϊ-1

						}
						nn = 0;
					}
				}
				if (*start == 0 && sign[i] != 0)                            //����start end  �����ַ������ֵ�������Ŀ�ʼ�������������
				{
					//��¼�ַ���ʼλ�ã������ƶ������ַ������
					*start = i;
				}
				if (sign[i] != 0 && i < width - 1)
				{
					//��¼�ַ�����λ�ã����ں����ƶ�
					*end = i;
				}

			}


		}


		void treatmenthollowchar()
		{
			//��������ַ�����1�ŵ꣩
			int i, j;
			int m, n;
			int t;
			int amountdiffer;
			int num;
			int mark;
			//���ߵ�б�ʺ͵�ǰҪ��������
			float ratio;
			int currenti, currentj;
			//�ϵ������յ�
			int starti, startj, endi, endj;
			//�ַ���Χ����¼������
			int start, end;
			int sign[Nlength];
			imagergb p, q, r;

			//��ֵ��     ������6 �ڶ�εĳ��Ժ�ѡ��ģ�û�о���Ĳο���Ϣ
			colortoblackwhite(6, 0, width - 1, 255);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}��ֵ��.bmp", 0));
			//ͼƬ���
			int newwidth = 150, newheight = 100; //�賬��ԭͼ���15���߶�
			rgb = toBigImagergb(rgb, newwidth, newheight);

			//width = newwidth;
			//height = newheight;

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}jiada.bmp", 0));


			////���һ������������ֹ����ַ��ײ���߽��γɴ����
			upEndline();

			//////////////////////////

			for (i = 0, p = rgb + (height - 1)*width; i < width; i++, p++)     //���һ������Ϊȫ�ױ���
			{
				p->rgbBlue = 255;
				p->rgbGreen = 255;
				p->rgbRed = 255;
			}

			for (j = 0, p = rgb + width - 1; j < height; j++, p = p + width)      ///���һ������Ϊȫ�ڱ���
			{
				p->rgbBlue = 0;
				p->rgbGreen = 0;
				p->rgbRed = 0;
			}

			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}���һ��.bmp", 1));

			ColorFillingbinary(0, 0, 255, 100);
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}�ݹ�����ɫ.bmp", 2));

			loopnum = 0;
			discardbinarylump(60);
			start = 0;
			end = 0;

			dealSignArry(sign, &start, &end);
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}����ַ��߽���.bmp", 3));

			//������������������������begin  �����Դ��ע�Ϳɲο�downRepatching������ע�ͣ������Ĵ���һ�£���������ϸ΢���޸�

			//�϶����޲���ͼƬ(ȱ��С)������
			upRepatching(sign);
			verticalRepatching();

			//ColorFillingbinary(0, 0, 255, 100);
			//	writeimagergb()->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}shangxianxiubu.bmp", 301));
			dealBeforeDownRepatching();
			downRepatching();
		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}�����޲�֮��.bmp", 5));
			//���
			ColorFillingbinary(0, 0, 100, 0);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}��ɫ�ݹ��ɫ����ɫ.bmp", 6));

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


		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp", 7));

			//ȥ���
			discardbinarylump(60);

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}ȥ���.bmp", 8));



			//�ƶ��ַ�

			//1)�����ƶ���ĩβλ��
			int distance, interval;
			distance = width - end - 2;
			Lettermove(0, width - 1, distance, 0, 1);//1)�����ַ����ƶ���ĩβλ��

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}�ƶ�λ�ú�.bmp", 9));
			//2)λ���ص����ַ�����
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
						//	if (num>4)break;  //���ƽ��������ĸ���Ч�����������֮����������������⡣
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


			//��תǰ�޲�
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

		//	writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}����.bmp", 10));
			//��ת
			rotate();
//
			writeimagergb()->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}��ת��.bmp", 10));
			//ȥ��ת��ɵ�ɢ��
			discardbinarylump(30);
		}



		/// <summary>
		///  ��һ��ͼ��ת�Ƶ�һ���󱳾���ȥ
		/// </summary>
		/// <param name="rgb">��ͼ�Σ�С��</param>
		/// <param name="newwidth">��ͼ��Ŀ��</param>
		/// <param name="newheight">��ͼ��ĸ߶ȣ�һ��Ҫ������ͼ��߶ȵ�15�����أ�����ᱨ��</param>
		/// <returns>�µĴ󱳾�ͼ��</returns>
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
			//���ֽ𻪣���������ͨ�����ݣ�����,�����ף�1�ŵ�
			int i, j;
			int partamount = 0;
			int num = 0;
			//���߶�����
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

		//��ɫ
		void bubblesort(int countamount)
		{
			//ð������
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


		//�����������ص�����ɫ��
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


		//������ɫ����
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

		//��ȡ��ɫ����
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


			//��ȡͼ���rgb���飬�߶ȣ����
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
							firstcountamount++;//������ɫ
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
								//����ɫ��ֵС��intervalvalue������ϲ�
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
			/*������ɫs����4���ȵ��ܶ���*/
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
			/*������Χ�����ɫ�ı䵱ǰ�����ɫ��ĳЩ�����ɫ������ַ��ڲ���
			choice=1����������ɫ�ı䵱ǰ
			choice=2����������ɫ�ı䵱ǰ
			choice=3��������������ɫ�ı䵱ǰ*/
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
			//����ɫͼת��Ϊ�ڰ�ͼ,��valueֵ�ӽ��ͱ�Ϊcolorֵ�������Ϊ255-color��
			int i, j, numb, numg, numr, amountdiffer;
			imagergb p;
			for (i = start; i <= end; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					amountdiffer = imagecolordiffer(p, rgbamount);
					if ((amountdiffer < value))
					{
						//�뱳��ɫ�ӽ�
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

		//������ߴ�����
		int checknoise(imagergb p, int colordiffer)
		{
			//���õ��Ƿ�Ϊ���,���ز�ͬ����
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
			//�жϸ�ͼƬ�Ƿ������:��Χ��ͬ������
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
							//��ȥ��
							p->rgbBlue = rgbamount->numb * 15;
							p->rgbGreen = rgbamount->numg * 15;
							p->rgbRed = rgbamount->numr * 15;
						}
					}
				}
			}
			if (num > 5)
			{
				//��ͼƬ�����
				return num;
			}
			else
			{
				//��ͼƬû�������������ٿ��Ժ���
				return 0;
			}
		}
		int judgenoiseline()
		{
			//�жϸ�ͼƬ�Ƿ�������(�����߷���2��ϸ���߷���1,�����߷���0,δʵ��)��
			int i, j;
			int amountdiffer;
			int vertspotnum[150];
			int horzspotnum[100];
			int vertpart, horzpart;
			int vertamount, horzamount;
			int largeamount;
			imagergb p, q;
			//����
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
					//�����ķ��������м���
					vertpart++;
				}
				if (vertspotnum[i] != 0)
				{
					vertamount++;
				}
				if (vertspotnum[i] > 20)
				{
					//����������ͨ���õ�һ������
					largeamount++;
				}
			}

			//ˮƽ:���߸�Ϊ1���������������ϸ����
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
				//����������ͨ
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
			/*��selectorder����1ʱ��ȥ�����С�ڵ���rgbcolorwidth����ɫ��
			  ��selectorder������1ʱ�����ؿ��С�ڵ���colorwidth������*/

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
			/*ͬdiscardwidthnoise������ָ���Ǹ߶�*/
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
			/*�����ںڰ�ͼ��������Χ�����ı䵱ǰ�����ɫ*/
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
							//��ǰ����Χ��ͬ��������number��ı䵱ǰ����ɫ
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
			/*choiceΪ0ʱ���õ���Χ��ͬ��������number�ͰѸõ��ɱ���ɫ;
			  ���choiceΪ1���ڴ˻����Ͽ�����Щ��ͬ�����Ƿ����뵱ǰ��ͬɫ�ĵ㣬����ɾ���������number������Χ����ɫ�ĵ�����*/
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
			//���ص�ǰ����Χ������ͬ����ɫ����
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
			/*��ǰ����Χ������ͬ�ĵ�������4�ͽ���ǰ���ɱ���ɫ*/
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
			//ȥ����ɫ������
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
				kright = p + i;//�����޸ģ���ֹ��������
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
			//choiceΪ0��Ϊ�ڰ�ͼ��choiceΪ1����ԭͼֻ��˫����
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
			//�ڰ�ͼȥ���������
			int i, j, countnum;
			imagergb p;
			for (i = 0; i < width; i++)
			{
				for (j = 0, p = rgb + i; j < height; j++, p = p + width)
				{
					countnum = 0;
					if (i == 0 || i == width - 1 || j == 0 || j == height - 1)
					{
						//��Եȥ��
						p->rgbBlue = 0;
						p->rgbGreen = 0;
						p->rgbRed = 0;
					}
					else
					{
						//�������Ҷ���������ɫ��ȥ�������
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
			//����ͼƬ�е�Сɫ��,��ɫ��ɺ�ɫ
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
			//ȥ��ԭͼ�е�Сɫ��
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

			//������ΧСɫ��
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
			//�����ַ�����ɫ����������������٣���ɫ������ǰ��λ�Ǳ������ַ���
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

		//�ָ��ַ�
		int horzpartamount(int start, int end, int choose, int checkposition)
		{
			//�ַ��ָ�ֺ��������ط�տռ���ˮƽ�����ͶӰ��������
			//choose:Ϊ0��ʾ��ԭͼ��ɫͶӰ��Ϊ1��ʾͼƬתΪ�׺�ͼƬ��ˮƽͶӰ
			//checkposition:�۲�����checkposition-��height-1��λ�õ���������
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
							//��ֹ�����㱻����Ϊ�Ƿ�տռ�
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
					//ˮƽͶӰ��checkpositionλ�õ�height-1������
					return 1;
				}
				else
				{
					//��checkposition��height-1λ��û������
					return 0;
				}
			}


		}
		void LetterSegmentloop(int amount, int xindex1, int xindex2, int yindex1, int yindex2, int letterwidth)
		{
			//�ָ��ַ������ȸ��ݷ�տռ���зָ���ͼƬû�з�տռ䣬���������������зָ�
			int i, j;
			int distance1, distance2;
			int dropleftx, droplefty, droprightx, droprighty;
			int leftborder, rightborder;
			//���-1���ұ�1
			int left, right;
			imagergb p;
			closurespace s;
			s = clospace;
			leftborder = yindex1;
			rightborder = yindex2;
			if (amount > 0)
			{
				//ͼƬ���ڷ�տռ�����
				for (i = 0; i < amount; i++, s++)
				{
					if (s->sign == 1)
					{
						//��loop
						distance1 = s->leftdownx - leftborder;
						distance2 = rightborder - s->rightupx;
						if (distance1 <= (yindex2 - yindex1) / 8)
						{
							if (distance2 <= (yindex2 - yindex1) / 8)
							{
								//���һ���ַ������ָ�
								continue;
							}
							else
							{
								//��һ���ַ���ֻ��ָ��ұ�
								right = 1;
								droprightx = s->rightupx + letterwidth;
								droprighty = s->rightupy;
								//��ȷ�ָ��
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
								//�������µ�ˮ�㷨
								DropFallroute(droprighty, droprightx, xindex2, yindex2);
								DowndropFallroute(droprighty, droprightx, xindex1, yindex2, right);
								leftborder = droprightx;
							}

						}
						else if (distance2 <= (yindex2 - yindex1) / 8)
						{
							if (distance1 < distance2)
							{
								//������Ѿ���������ĸ�ĵ���·���ָ�����򲻽��зָ�
								continue;
							}
							else
							{
								//���һ���ַ���ֻ��ָ����
								left = -1;
								dropleftx = s->leftdownx - letterwidth;
								droplefty = s->leftdowny;

								//��ȷ�ָ��
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
							//�ַ������м�λ�ã���Ҫ�ָ���������
							droprightx = s->rightupx + letterwidth;
							droprighty = s->rightupy;
							right = 1;
							dropleftx = s->leftdownx - letterwidth;
							droplefty = s->leftdowny;
							left = -1;
							//��ȷ�ָ��
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
						//����loop�����ַ�ճ���γɵķ�տռ�,��ѡһ�ߵ�ˮ�ָ�
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
				//û�з�տռ�����



			}

		}
		void Lettersegment(int start, int end, int position)
		{
			//�ָ�start-end�����ַ���ֻ���·ָ�λ�ò��ƶ�(����)
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
				//�����ַ�
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
				//�����ַ�
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
			//�������ķ�������sign�ͻ�ɫ���б�ǽ��ַ�����,�ʺ�û��ճ������ĸ�ص��ָ�
			//letteramountΪ�ַ��ĸ���
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
					//�޲�һЩ��ȱ
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

			//�����4
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


						//���ɫ��տռ�ķ�Χ�������ж�ĳЩС�ķ�տռ��Ƿ��Ǹ��ַ���һ���֣�
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
			//����ճ���ַ����÷ָ��߱��Ȼ��ֿ�
			//letteramountΪ�ַ��ĸ���
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
			//�ַ�����ɫ��ͬ��������ɫ���Ȼ���ƶ��ֿ�
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
			//�Էָ�·�߽��б�ǣ����Ϊ��ɫ
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

		//���ڴ�������λ�ã���ת��
		void adjustposition()
		{
			//�����ڰ�ͼƬ����ĸ������λ��
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
			//ͳ�Ƹ������ص�����
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
			//����洢��ĸ���ҿ�ʼ����λ�ã��У�
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
					//������ĸ������ʼλ��
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

					//����λ��
					for (m = positionstart[i]; m <= positionend[i]; m++)
					{
						if (differ > 0)
						{
							//����
							controltail = 0;
							for (n = height - 1, p = rgb + (height - 1) * width + m; n >= 0; n--, p = p - width, controltail--)
							{
								if (p->rgbBlue == 255)
								{
									//������Ҫ�����±ߣ���ֹ����
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
							//����
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
			/*dispose==1:��ֵ�����ͼƬ��dispose==0:ԭͼ
			  color��ԭͼ��Ҫȷ����ʼ����λ�õ���ɫ;valueԭͼ�����õ��뱳��ɫ�Ĳ�ֵ��С
			  ��ͼƬ���ַ��Ŀ�ʼ����λ��*/
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

			//ͳ��ÿ���ַ���������
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

			//����ÿ�кڵ�/�ʵ�����������������¼����ճ���ַ�����ĸ��ʼ����λ��
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
			//��(centerx,centery)����תangle��,��ת��ͼ�����copy��
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
						//����任
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
			//���ڰ�ͼ�е��ַ���ɫȫ���������ַ�Ϊ��ɫ��
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
			//��б�ַ��Ƕȵ�������ת
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
			//ԭͼ���Ƶ���һƬͬ����С�Ŀհ״���
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
				//��ʼ�ַ�ͶӰ����
				shadowlength = endposition[i] - startposition[i];
				if (shadowlength <= 1)
				{
					continue;
				}
				else
				{
					//���ַ�������
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


				//�����ַ��Ƕȵĵ���(������һ���ٽ�����һ���ַ��ĽǶȵ���),ͶӰ��С
				start = startposition[i];
				end = endposition[i];
				//��ʱ����ת
				angle = -30;
				adjustangle(centerx, centery, angle, start, end);
				findletterposition(copyrgb, 1, 255, 0);
				newshadowlength1 = endposition[0] - startposition[0];
				erasebm(startposition[0], endposition[0], copyrgb);
				//˳ʱ����ת
				angle = 30;
				adjustangle(centerx, centery, angle, start, end);
				findletterposition(copyrgb, 1, 255, 0);
				newshadowlength2 = endposition[0] - startposition[0];
				erasebm(startposition[0], endposition[0], copyrgb);
				//�Ƚ������������תͶӰ���ȷ����ת����
				if (abs(newshadowlength1 - newshadowlength2) > 1)
				{
					if (newshadowlength1 < newshadowlength2)
					{
						//ȷ������ʱ����ת
						for (j = 0; j <= 9; j++)
						{
							//�仯��ת�Ƕ����Ӧ��ͶӰ����
							angle = -5 - j * 5;
							adjustangle(centerx, centery, angle, start, end);
							findletterposition(copyrgb, 1, 255, 0);
							newshadowlength[j] = endposition[0] - startposition[0];
							erasebm(startposition[0], endposition[0], copyrgb);
						}
						//��ͶӰ������С��Ӧ����ת�Ƕ�
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
						//ȷ����˳ʱ����ת
						for (j = 0; j <= 9; j++)
						{
							//�仯��ת�Ƕ����Ӧ��ͶӰ����
							angle = 5 + j * 5;
							adjustangle(centerx, centery, angle, start, end);
							findletterposition(copyrgb, 1, 255, 0);
							newshadowlength[j] = endposition[0] - startposition[0];
							erasebm(startposition[0], endposition[0], copyrgb);
						}
						//��ͶӰ������С��Ӧ����ת�Ƕ�
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
					//�ַ�û����б������ԭλ��
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
			//�ƶ�ָ����ɫ����ĸ
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
			/*ͼƬˮƽ�����������ɨ�裨��ɫ�ָ��߶�����ĸ���ұߣ�����ÿ����ÿ����һ�����߾ͽ��ƶ������2
			�˷������ͼƬ���ַ��ұ߿հ׶�������������߿հױȽ϶࣬�����Ƚ�ͼƬ���ַ��ͷָ�������������ʹ�ô˺���
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
			/*�ƶ���start��end���ֵ��ַ�,�ƶ�����Ϊdistance
			  choice:1Ϊԭͼ���ƶ�����ԭ���Ϊ����ɫ��0Ϊ�ڰ�ͼ���ƶ����ԭ���Ϊ��ɫ
			  directionΪ�ƶ��ķ���-1����1����*/
			int i, j;
			int amountdiffer;
			imagergb p;
			if (distance > 0)
			{
				if (direction == -1)
				{
					//�����ƶ�
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
					//�����ƶ�
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
			/*��ͼ����лҶȻ�����
			  threshold:�ҶȻ��������ֵ��С����С����ֵ�ĵ��Ϊ��ɫ��������ֵ�ĵ��Ϊ��ɫ��*/
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

		//��������
		int calculateimagecoloramount(int num)
		{
			//������������һ��������ͼƬ��ɫ������
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
			//�����������Ŀ��;
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
			//ֱ�ӷ��غ���ͼƬ��ͬ��ɫ����2�ĸ߶�
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
			//����ͼƬ���������Ŀ�ȴ�С���ų���㣩
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
						//���õ��Ƿ������;
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



		//�����ص�ȡֵ��Χ��С��0-16��ͨ��255/15����,����ֵ����ĵ���� (lai:�������Ǻܶ�ǰ�˵��뷨��)
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
			/*num��������ͳ����Χ��ͬ����,������Χ��ͬ�㳬��num�����ص���*/
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
					for (k = 0; k < 3; k++)                   //�Ƚϵ�����Χ9�����ص�
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
			//�ı�ͼƬ��ÿ�����signֵ
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

		//��ֵ�˲�����
		void calculategrayvalue()
		{
			//����ͼ��ĻҶ�ֵ
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
			//��ֵ�˲�3*3
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

		//ϸ���㷨����
		int detectconnect()
		{
			//ϸ���㷨�еĹ��ܺ���
			int connectivity = 0;
			connectivity = list[6] - list[6] * list[7] * list[0];
			connectivity += list[0] - list[0] * list[1] * list[2];
			connectivity += list[2] - list[2] * list[3] * list[4];
			connectivity += list[4] - list[4] * list[5] * list[6];
			return connectivity;
		}
		void Thinning()
		{
			/*Hilditchϸ���㷨
			  ��������:1���õ���һ���߽��
			  2���õ㲻��һ���˵�
			  3���õ㲻��һ���ϵ�
			  4���õ��ɾ����Ӧ����ɹ�����ʴ
			  �˳�������ɨ��һ�κ󣬽���ǵ����ص�ɾ������ʼ��һ��ɨ�裬ֱ����ĳ��ɨ���У�û�����ص��ٱ���ǣ�������
			  �ο���ַ��http://www.cnblogs.com/xiaotie/archive/2010/08/12/1797760.html
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
						//����1��p������ǰ��ɫ
						if (p->rgbBlue != 255)
						{
							continue;
						}
						//list�洢���ǲ������ַ����ף���ɫΪ0���������ڣ���ɫΪ1���Է�������������
						list[0] = (p + 1)->rgbBlue == 255 ? 0 : 1;
						list[1] = (p - width + 1)->rgbBlue == 255 ? 0 : 1;
						list[2] = (p - width)->rgbBlue == 255 ? 0 : 1;
						list[3] = (p - width - 1)->rgbBlue == 255 ? 0 : 1;
						list[4] = (p - 1)->rgbBlue == 255 ? 0 : 1;
						list[5] = (p + width - 1)->rgbBlue == 255 ? 0 : 1;
						list[6] = (p + width)->rgbBlue == 255 ? 0 : 1;
						list[7] = (p + width + 1)->rgbBlue == 255 ? 0 : 1;
						//����2��p0��p2��p4��p6��ȫ���ַ���
						if (list[0] == 0 && list[2] == 0 && list[4] == 0 && list[6] == 0)
						{
							continue;
						}
						//����3��p0~p7�����������ַ���
						count = 0;
						for (k = 0; k < 8; k++)
						{
							count += list[k];
						}
						if (count>6)
						{
							continue;
						}
						//����4������������1
						connectivity = detectconnect();
						if (connectivity != 1)
						{
							continue;
						}
						//����5������p2�Ѿ������ɾ��������p2Ϊ����ʱ�����ı�p��������
						if ((p - width)->sign == 1)
						{
							list[2] = 1;
							if (detectconnect() != 1)
							{
								continue;
							}
							list[2] = 0;
						}
						// ����6: ����p4�ѱ��ɾ��������p4Ϊ���������ı�p�������� 
						if ((p - 1)->sign == 1)
						{
							list[4] = 1;
							if (detectconnect() != 1)
							{
								continue;
							}
							list[4] = 0;
						}
						p->sign = 1; // ���ɾ�� 
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
		//��ʴ�㷨
		void Erosionforbinary()
		{
			//��ʴ�㷨
			/*ʹ��1*3�ĽṹԪ��,Ϊ��ֹԽ�磬����������ߺ����ұ�
			  ��ʹ��3*1�ṹԪ�أ�Ϊ��ֹԽ�磬���������ϱߺ����±�
			  ��ʹ��3*3�ṹʱ��Ϊ��ֹԽ�磬����������ߺ����ұ��Լ����ϱߺ����±�
			  */
			int i, j;
			int m, n;
			imagergb p, q;
			for (j = 1; j < height - 1; j++)
			{
				for (i = 1, p = rgb + j*width + i; i < width - 1; i++, p++)
				{
					//ʹ��ˮƽ����ṹԪ�ؽ��и�ʴ����ǰ���Ƚ��б���ַ�ɫ��ɫ
					//p->sign = 1;
					//�����ǰ���������������һ���㲻���ַ���ɫ���򽫸õ���Ϊ����ɫ��ɫ
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

		//��ˮ�㷨�Լ�����Ƿ�տռ亯��
		void ColorFillingbinary(int nrow, int ncol, int value, int changecolor)
		{
			/*Ѱ�Ҹ��������(nrow,ncol)��ͨ�����򣬽�value��ɫ��ΪͿ��changecolor��ɫ*/
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
						if ((i == nrow || j == ncol) && p->rgbBlue == value)                        // i == nrow || j == ncol ������������
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
			//��ǵ�ǰ������ͬ����ɫ
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
			/*���տռ�����½Ǻ����Ͻǵ�����(loop�ַ�:ABDOPQ/abdegopq/46890��*/
			int i, j;
			int m, n;
			//��ֱ����ͶӰ
			int vertspotamount[122];
			int startx, endx;
			//ˮƽ����ͶӰ
			int horzspotamount[35];
			int starty1, endy1, starty2, endy2, starty3, endy3;

			//���������½�����
			int rise, down;
			//ճ��λ��
			int adjoinposition1 = 0, adjoinposition2 = 0;
			int partamount1 = 0, partamount2 = 0, partamount3 = 0;

			int sum = 0;
			int loopamount = 0;

			imagergb p, q, r;
			closurespace cp;
			clospace = (closurespace)malloc(8 * sizeof(ENCLOSUSE));

			//��ֱ����ͶӰ
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


			//�ж��Ƿ���loop���������տռ�߽磬���½Ǻ����Ͻ�����
			startx = 0;
			endx = 0;

			for (i = 1, cp = clospace; i < width - 1; i++)
			{
				if (vertspotamount[i] != 0 && vertspotamount[i - 1] == 0)
				{
					//��ʼλ��
					cp->leftdownx = i;
					startx = i;
				}
				if (vertspotamount[i] != 0 && vertspotamount[i + 1] == 0)
				{
					//����λ��
					cp->rightupx = i;
					endx = i;
					loopamount++;
				}
				if (startx != 0 && endx != 0)
				{
					cout << "��ֱ:" << endl;
					cout << "leftdownx:" << cp->leftdownx << "   ";
					cout << "rightupx:" << cp->rightupx << endl;
					//�ж��ǲ���loop
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



					//��ֱ����ͶӰ���ڲ��岨��
					if (adjoinposition1 != 0)
					{
						//��ˮƽ����ͶӰ���жϸ������ǲ���loop,��cp�е�sign���,1��loop��2����loop
						//ˮƽ����������
						partamount1 = horzpartamount(startx, adjoinposition1, 1, 0);
						if (adjoinposition2 != 0)
						{
							partamount2 = horzpartamount(adjoinposition1, adjoinposition2, 1, 0);
							partamount3 = horzpartamount(adjoinposition2, endx, 1, 0);
							if (partamount1 == 1 && partamount2 == 1 && partamount3 == 1)
							{
								//������ˮƽͶӰ��Ϊ1����ȡճ���ָ��adjoinposition1��Ϊ�������ֵķָ��
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
								//�м䲿��ˮƽͶӰΪ2���м��п������ַ�������Ϊ�ַ����ַ��γɵķ�տռ�
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
								//����ˮƽͶӰΪ2���п���Ϊ�ַ����м䲿��Ϊ�ַ����ַ��γɵĿ�϶��ɵķ�տռ�
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
							//ˮƽ����������
							partamount2 = horzpartamount(adjoinposition1, endx, 1, 0);
							//��������ˮƽ�����ͶӰ���ֽ�Ϊ1����ȡճ���ָ��adjoinposition1
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
							//ˮƽ����ͶӰһ��Ϊ1��һ��Ϊ2������Ϊ1��2�е�һ���֣���8,9���ַ������ȡ2�����е����沿����Ϊ�������򣬼������ϽǺ����½�loop����
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


					//��տռ���ֱͶӰ�����в��岨�ȣ����ݳ���ͶӰ���������ڵ������ж��ǲ���loop
					else
					{
						//��Ӧ��ֱͶӰλ����ˮƽ����
						cout << "ˮƽ:" << endl;
						starty1 = 0;
						endy1 = 0;
						starty2 = 0;
						endy2 = 0;
						starty3 = 0;
						endy3 = 0;

						//ˮƽͶӰ
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
							//ˮƽ����ʼ������λ��
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


							//���ݿ�ʼ������λ�ý����жϣ������п�ʼ����λ�õ��޸�
							if (starty3 != 0)
							{
								//ˮƽͶӰ��������

								cp->sign = 2;
								//��߽�����
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
								//ˮƽͶӰ��������
								//�ж��ǲ���loop,���������ֺڵ�����
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
									//����loop
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
									//�������²����ж��ǲ���loop								
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
										//ȡ���沿�ֶ�����
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
										//���沿�ֶ�����
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
								//ˮƽ�����������Լ�ͶӰΪ1����Ŀ���ж�
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
									//����loop,ѡ�ұ���Ϊ�ָ��
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
									//��loop
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
			//���µ���,ͼƬ�ǻҶȵģ����ַ����ұ�������������Ǻ�ɫ��
			imagergb p;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright, pointdownleftleft, pointdownrightright;
			int leftrightflg;
			leftrightflg = 0;
			//��ǵ�ˮ�㷨�е�������µ�·����p->sign=1;
			while ((inipointx <= xindex2) && (inipointy <= yindex2))
			{
				p = rgb + inipointx*width + inipointy;
				//��Ƿָ�·��
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
					//����
					inipointx = inipointx + 1;
					leftrightflg = 0;
				}
				else if ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 128 || pointdownleft == 0) && (pointdownright == 255))
				{
					//����
					if (pointdownleft == 0)
					{
						inipointx = inipointx + 1;
						inipointy = inipointy - 1;
						leftrightflg = 0;
					}
					else
					{
						//�鿴��������û�кڵ�
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
					//����
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
					//�ұߣ��������������ֹ����ѭ��
					if (leftrightflg == 0)
					{
						//��һ��
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
					//��ߣ��������������ֹ����ѭ��
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
					//ʣ�µ����
					inipointx = inipointx + 1;

				}
			}
		}
		void DowndropFallroute(int inipointx, int inipointy, int xindex1, int yindex2, int trend)
		{
			//���ϵ���,ͼƬ�ǻҶȵģ����ַ�����ɫ��������ɫ�������
			imagergb p;
			int i, j;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright;
			int leftrightflg;
			leftrightflg = 0;
			//��ǵ�ˮ�㷨�е�������µ�·����p->sign=1;
			while ((inipointx >= xindex1) && (inipointy <= yindex2))
			{
				p = rgb + inipointx*width + inipointy;
				//��Ƿָ�·��
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
					//����
					inipointx = inipointx - 1;
					leftrightflg = 0;
				}
				else if ((pointleft == 255) && (pointright == 255) && (pointdown == 255) && (pointdownleft == 128) && (pointdownright == 255))
				{
					//����
					inipointx = inipointx - 1;
					inipointy = inipointy - 1;
					leftrightflg = 0;
				}
				else if ((pointdown == 255) && (pointdownleft == 255) && (pointdownright == 128))
				{
					//����
					inipointx = inipointx - 1;
					inipointy = inipointy + 1;
					leftrightflg = 0;
				}
				else if ((pointright == 128) && (pointdown == 255) && (pointdownleft == 255) && (pointdownright == 255))
				{
					//�ұߣ��������������ֹ����ѭ��
					if (leftrightflg == 0)
					{
						//��һ��
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
					//��ߣ��������������ֹ����ѭ��
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
					//ʣ�µ����
					inipointx = inipointx - 1;
				}


			}
		}
		void DropFall()
		{
			/*��ˮ�㷨*/
			int x, y;
			//�ַ������������ұ߽�
			int xindex1, xindex2, yindex1, yindex2;
			//�ַ������Ⱥ͸߶�
			int validwidth, validheight;
			//��־ĳЩλ�ÿ��ܻ����ѭ�������
			int loopflg = 0;
			int pointleft, pointright, pointdown, pointdownleft, pointdownright;
			//�����
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


			//���ַ������Ⱥ͸߶�
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

			//�����ַ����
			//letterwidth = calculateletterwidth();
			ColorFillingbinary(1, 1, 0, 128);
			//����loop�ַ�λ��
			loopamount = LoopBoundary();
			LetterSegmentloop(loopamount, xindex1, xindex2, yindex1, yindex2, 4);

			//����·��
			//DropFallroute(inipointx, inipointy, xindex2, yindex2);
			//����ɫ��ǵ���·��
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
			//��ȡͼƬ��Ϣ
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

		//����ͼ���
		void setImgWidth(int imgWidth){
			width = imgWidth;
		}

		//��ȡͼ���
		int getImgWidth(){
			return width;
		}
		//����ͼ��߶�
		void getImgHeight(int imgHeight){
			height = imgHeight;
		}

		//��ȡͼ��߶�
		int getImgHeight(){
			return height;
		}

		void setOrigincountcolorvarietyamount(int value){
			origincountcolorvarietyamount = value;
		}

		int getOrigincountcolorvarietyamount(){

			return origincountcolorvarietyamount;

		}

		//��ȡһ��ͼ���imagergb ����
		//param:img ͼ��
		//returns imagergb����
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
				//��ԭ����ͼƬ���հױ�Ե������������
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
