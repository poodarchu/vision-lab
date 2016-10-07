#include "stdafx.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>

static bool GetValue(int p[],int size,int &value)
{
	//数组中间的值
	int zxy = p[(size-1)/2];
	//用于记录原数组的下标
	int *a = new int [size];
	int index = 0;
	for(int i = 0;i<size;++i)
		a[index++] = i;

	for(int i=0;i<size -1;i++)
		for(int j=i+1;j<size;j++) 
			if(p[i]>p[j]){  
				int tempA = a[i];
				a[i] = a[j];
				a[j] = tempA;
				int temp=p[i];  
                p[i]=p[j];  
                p[j]=temp; 

            }  
	int zmax = p[size-1];
	int zmin = p[0];
	int zmed = p[(size-1)/2];
	
	if(zmax>zmed&&zmin<zmed){
		if(zxy>zmin&&zxy<zmax)
			value = (size-1)/2;
		else 
			value = a[(size-1)/2];
		delete []a;
		return true;
	}
	else{
		delete []a;
		return false;
	}

}


UINT ImageProcess::medianFilter(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	int startIndex = param->startIndex;
	int endIndex = param->endIndex; 
	int maxSpan = param->maxSpan;
	int maxLength = (maxSpan * 2 + 1) * (maxSpan * 2 + 1);
 
	byte* pRealData = (byte*)param->src->GetBits();  
    int pit = param->src->GetPitch();  
    int bitCount = param->src->GetBPP()/8;

	int *pixel = new int [maxLength];//存储每个像素点的灰度
	int *pixelR = new int [maxLength];
	int *pixelB = new int [maxLength];
	int *pixelG = new int [maxLength];
	int index =0;
	for(int i = startIndex; i <= endIndex; ++i)
	{
			int Sxy = 1;
			int med = 0;
			int state = 0;
			int x = i % maxWidth;
			int y = i / maxWidth;
			while(Sxy <= maxSpan)
			{
				index =0;
				for(int tmpY = y - Sxy; tmpY <= y + Sxy && tmpY <maxHeight; tmpY++)
				{
					if (tmpY < 0) continue;
					for(int tmpX = x - Sxy; tmpX <= x + Sxy && tmpX<maxWidth; tmpX++)
					{
						if (tmpX < 0) continue;
						if(bitCount==1)  
						{  
							pixel[index]= *(pRealData+pit*(tmpY)+(tmpX)*bitCount);
							pixelR[index++] =pixel[index];

						 }
						else
						{
							pixelR[index]= *(pRealData + pit*(tmpY) + (tmpX)*bitCount + 2);
							pixelG[index]= *(pRealData + pit*(tmpY) + (tmpX)*bitCount + 1);
							pixelB[index]= *(pRealData + pit*(tmpY) + (tmpX)*bitCount);
							pixel[index++]= int(pixelB[index]*0.299 + 0.587*pixelG[index] + pixelR[index]*0.144);

						}
					}

				}
				if (index <= 0)
					break;	
				if ((state = GetValue(pixel, index, med))==1)
					break;

				Sxy++;
			};

			if(state)
			{
				if(bitCount==1)
				{
					*(pRealData + pit*y + x*bitCount) = pixelR[med];
					
				}
				else
				{
					*(pRealData + pit*y + x*bitCount+2) = pixelR[med];
					*(pRealData + pit*y + x*bitCount+1) = pixelG[med];
					*(pRealData + pit*y + x*bitCount) = pixelB[med];

				}
			}
			
		}
		
	
	
	delete []pixel;
	delete []pixelR;
	delete []pixelG;
	delete []pixelB;

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MEDIAN_FILTER, 1, NULL); 
	return 0;
}

UINT ImageProcess::addNoise(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex; 
	byte* pRealData = (byte*)param->src->GetBits();
	int bitCount = param->src->GetBPP() / 8;
	int pit = param->src->GetPitch();

	for (int i = startIndex; i <= endIndex; ++i)
	{
		int x = i % maxWidth;
		int y = i / maxWidth;
		if ((rand() % 1000) * 0.001 < NOISE)
		{
			int value = 0;
			if (rand() % 1000 < 500)
			{
				value = 0;
			}
			else
			{
				value = 255;
			}
			if(bitCount == 1)
			{
				*(pRealData + pit * y + x * bitCount) = value;
			}
			else
			{
				*(pRealData + pit * y + x * bitCount) = value;
				*(pRealData + pit * y + x * bitCount + 1) = value;
				*(pRealData + pit * y + x * bitCount + 2) = value;
			}
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NOISE, 1, NULL);
	return 0;
}