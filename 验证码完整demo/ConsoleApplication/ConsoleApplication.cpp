// ConsoleApplication.cpp: ����Ŀ�ļ���

#include "stdafx.h"
#include<stdio.h>
#include<malloc.h>  
#include<string.h> 
#include<stdlib.h>
#include<iostream>  

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace System::Net;
using namespace System::IO;


int processingimagergb(int number)
{
#pragma region 
	//WebClient^ wc = gcnew WebClient();
	//wc->Headers->Add("Referer", "http://jjzd.mas.gov.cn:8090/frmVehVio.aspx");//��ɽ
	//wc->Headers->Add("Referer", "http://www.hfjjzd.gov.cn:9080/hfwebquery/Default.aspx");//�Ϸ�

	//WebClient^ wc = gcnew WebClient();
	//MemoryStream^ ms = gcnew MemoryStream(wc->DownloadData("http://fort.yhd.com/validcode/valid_code.do?t=0.19241086835972965"));
	//if (wc->ResponseHeaders->Get("Content-Type") != "image/PNG")
	//{
	//	Bitmap^ originalimage = gcnew Bitmap(ms);
	//Bitmap^ originalimage = gcnew Bitmap(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\ԭͼ\\{0}.jpg", number));
	////originalimage->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\ԭͼ\\{0}.jpg", number));
	//Bitmap^ processingimge;
	//processingimge = OcrHelper::Helper::Process(originalimage);
	//processingimge->Save(String::Format("C:\\Users\\pxf\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.bmp", number));
	//return 1;
	//}
	//else
	//{
	//	Console::WriteLine((wc->ResponseHeaders->Get("Content-Type"))->ToString());
	//	return 0;
	//}  
#pragma endregion


	//���ػ�ȡͼƬ

	Bitmap^ originalimage = gcnew Bitmap(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\ԭͼ\\{0}.jpg", number));
	Bitmap^ processingimge;
	processingimge = OcrHelper::Helper::Process(originalimage);
	processingimge->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.jpg", number));
	processingimge->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ2\\{0}.jpg", number));
	return 1;


	////�����ȡͼƬ

	//WebClient^ wc = gcnew WebClient();
	//MemoryStream^ ms = gcnew MemoryStream(wc->DownloadData("http://fortjq.yhd.com/validcode/valid_code.do?t=0.19241086835972965"));
	//if (!ms)
	//{
	//	return 0;
	//}
	//Bitmap^ originalimage = gcnew Bitmap(ms);

	//originalimage->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\ԭͼ\\{0}.jpg", number));
	//Bitmap^ processingimge;
	//processingimge = OcrHelper::Helper::Process(originalimage);
	//processingimge->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ\\{0}.jpg", number));
	//processingimge->Save(String::Format("D:\\Documents and Settings\\Administrator\\Desktop\\ͼ�δ���2\\�����ͼ2\\{0}.jpg", number));
	//return 1;
}

int main()
{

	//processingimagergb(9);

	int i;
	for (i = 1; i < 100; i++)
	{
		int state;
		state = processingimagergb(i);
		if (state == 0)
		{
			i--;
		}
		cout << i << endl;

	//	break;
	}
	system("pause");
	return 0;
}
