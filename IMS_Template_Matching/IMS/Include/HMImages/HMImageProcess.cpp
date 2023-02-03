// HMImageProcess.cpp: implementation of the CHMImageProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMImageProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PI  3.141592

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMImageProcess::CHMImageProcess()
{

}

CHMImageProcess::~CHMImageProcess()
{

}

//////////////////////////////////////////////////////////////////////////////
// 이미지 프로세싱 함수들...
BOOL CHMImageProcess::VSobelBoundary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	int MaskBox[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
	int nHeightm1 = nHeight - 1;
	int nWidthm1 = nWidth - 1;
	int mr, mc, newValue, i, j, min, max;
	int* pTmpImg;
	float constVal1, constVal2;
	pTmpImg = new int[nHeight * nBufWidth];	// 정수값을 갖는 이미지 동적 메모리 할당
	ZeroMemory(pTmpImg, nHeight * nBufWidth * sizeof(int));

	for (i = 1; i < nHeightm1; i++)
	{
		for (j= 1; j < nWidthm1; j++)
		{
			newValue = 0;
			for (mr = 0; mr < 3; mr++)
			{
				for (mc = 0; mc < 3; mc++)
					newValue += (MaskBox[mr][mc] * lpInImg[(i + mr - 1) * nBufWidth + (j + mc - 1)]);
			}

			// 값을 양수로 변환
			if (newValue < 0)
				newValue = -newValue;
			pTmpImg[i * nBufWidth + j] = newValue;
		}
	}

	// 디스플레이를 위해 0에서 255사이로 값의 범위를 매핑
	// 이를 위해 먼저 최대, 최소값을 찾은 후 이를 이용하여 매핑한다.
	min = 0x0fffffff;
	max = -(0x0fffffff);

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = pTmpImg[i * nBufWidth + j];
			if (newValue < min)
				min = newValue;
			if (newValue > max)
				max = newValue;
		}
	}

	// 변환시 상수값을 미리 계산
	constVal1 = (float)(255.0 / (max - min));
	constVal2 = (float)(-255.0 * min / (max - min));

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			// [min, max] 사이의 값을 [0, 255] 값으로 변환
			newValue = pTmpImg[i * nBufWidth + j];
			newValue = (int)(constVal1 * newValue + constVal2);

			lpOutImg[i * nBufWidth + j] = (BYTE)newValue;
		}
	}

	// Find Max Sum
	for (i = 1; i < nWidthm1; i++)
	{
		for (j = 1; j < nHeightm1; j++)
		{
			if (lpOutImg[j * nBufWidth + i] > 127)
				lpOutImg[j * nBufWidth + i] = 255;
			else
				lpOutImg[j * nBufWidth + i] = 0;
		}
	}

	if (pTmpImg)
	{
		delete [] pTmpImg;
		pTmpImg = NULL;
	}
	

	return TRUE;
}

BOOL CHMImageProcess::HSobelBoundary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	int MaskBox[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
	int nHeightm1 = nHeight - 1;
	int nWidthm1 = nWidth - 1;
	int mr, mc, newValue, i, j, min, max;
	int* pTmpImg;
	float constVal1, constVal2;
	pTmpImg = new int[nHeight * nBufWidth];	// 정수값을 갖는 이미지 동적 메모리 할당
	ZeroMemory(pTmpImg, nHeight * nBufWidth * sizeof(int));

	for (i = 1; i < nHeightm1; i++)
	{
		for (j= 1; j < nWidthm1; j++)
		{
			newValue = 0;
			for (mr = 0; mr < 3; mr++)
			{
				for (mc = 0; mc < 3; mc++)
					newValue += (MaskBox[mr][mc] * lpInImg[(i + mr - 1) * nBufWidth + (j + mc - 1)]);
			}

			// 값을 양수로 변환
			if (newValue < 0)
				newValue = -newValue;
			pTmpImg[i * nBufWidth + j] = newValue;
		}
	}

	// 디스플레이를 위해 0에서 255사이로 값의 범위를 매핑
	// 이를 위해 먼저 최대, 최소값을 찾은 후 이를 이용하여 매핑한다.
	min = 0x0fffffff;
	max = -(0x0fffffff);

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = pTmpImg[i * nBufWidth + j];
			if (newValue < min)
				min = newValue;
			if (newValue > max)
				max = newValue;
		}
	}

	// 변환시 상수값을 미리 계산
	constVal1 = (float)(255.0 / (max - min));
	constVal2 = (float)(-255.0 * min / (max - min));

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			// [min, max] 사이의 값을 [0, 255] 값으로 변환
			newValue = pTmpImg[i * nBufWidth + j];
			newValue = (int)(constVal1 * newValue + constVal2);

			lpOutImg[i * nBufWidth + j] = (BYTE)newValue;
		}
	}

	// Find Max Sum
	for (i = 1; i < nWidthm1; i++)
	{
		for (j = 1; j < nHeightm1; j++)
		{
			if (lpOutImg[j * nBufWidth + i] > 127)
				lpOutImg[j * nBufWidth + i] = 255;
			else
				lpOutImg[j * nBufWidth + i] = 0;
		}
	}

	if (pTmpImg)
	{
		delete [] pTmpImg;
		pTmpImg = NULL;
	}

	return TRUE;
}

BOOL CHMImageProcess::PrewittMasking(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	int MaskPrewittX[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
	int MaskPrewittY[3][3] = {{1, 1, 1}, {0, 0, 0}, {-1, -1, -1}};
	int nHeightm1 = nHeight - 1, nWidthm1 = nWidth - 1;
	int mr, mc, newValue, i, j;
	int min, max, where;
	float constVal1, constVal2;

	// 정수값을 갖는 이미지 동적 메모리 할당
	int* pImgPrewittX = new int[nHeight * nBufWidth];
	ZeroMemory(pImgPrewittX, nHeight * nBufWidth);
	int* pImgPrewittY = new int[nHeight * nBufWidth];
	ZeroMemory(pImgPrewittY, nHeight * nBufWidth);

	// X방향 에지 강도 계산
	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = 0;	// 0으로 초기화
			for (mr = 0; mr < 3; mr++)
				for (mc = 0; mc < 3; mc++)
 					newValue += (MaskPrewittX[mr][mc] * lpInImg[(i + mr - 1) * nBufWidth + (j + mc - 1)]);

				pImgPrewittX[i * nBufWidth + j] = newValue;
		}
	}

	// Y방향 에지 강도 계산
	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = 0;	// 0으로 초기화
			for (mr = 0; mr < 3; mr++)
				for (mc = 0; mc < 3; mc++)
					newValue += (MaskPrewittY[mr][mc] * lpInImg[(i + mr - 1) * nBufWidth + (j + mc - 1)]);
				
				pImgPrewittY[i * nBufWidth + j] = newValue;
		}
	}

	// 에지 강도 계산 절대값(X) + 절대값(Y) 후 pImgPrewittX[]에 저장
	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			where = i * nBufWidth + j;
			constVal1 = (float)pImgPrewittX[where];
			constVal2 = (float)pImgPrewittY[where];
			if (constVal1 < 0)
				constVal1 = -constVal1;
			if (constVal2 < 0)
				constVal2 = -constVal2;

			pImgPrewittX[where] = (int)(constVal1 + constVal2);
		}
	}

	// 디스플레이를 위해 0에서 255 사이로 값의 범위를 매핑
	// 이를 위해 먼저 최대, 최소값을 찾은 후 이를 이용하여 매핑한다.
	min = 0x0fffffff;
	max = -(0x0fffffff);

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = pImgPrewittX[i * nWidth + j];
			if (newValue < min)
				min = newValue;
			if (newValue > max)
				max = newValue;
		}
	}

	// 변환시 상수값을 미리 계산
	constVal1 = (float)(255.0 / (max - min));
	constVal2 = (float)(-255.0 * min / (max - min));
	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			// [min, max] 사이의 값을 [0, 255] 값으로 변환
			newValue = pImgPrewittX[i * nBufWidth + j];
			newValue = (int)(constVal1 * newValue + constVal2);
			lpOutImg[i * nBufWidth + j] = (BYTE)newValue;
		}
	}

	if (pImgPrewittX)
	{
		delete [] pImgPrewittX;
		pImgPrewittX = NULL;
	}
	if (pImgPrewittY)
	{
		delete [] pImgPrewittY;
		pImgPrewittY = NULL;
	}

	return TRUE;
}

BOOL CHMImageProcess::LaplacianMasking(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	int MaskBox[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	int nHeightm1 = nHeight - 1;
	int nWidthm1 = nWidth - 1;
	int mr, mc, newValue, i, j, min, max;
	int* pTmpImg;
	float constVal1, constVal2;
	pTmpImg = new int[nHeight * nBufWidth];	// 정수값을 갖는 이미지 동적 메모리 할당
	ZeroMemory(pTmpImg, nHeight * nBufWidth);

	// 결과 이미지 0으로 초기화
	for (i = 0; i < nHeight; i++)
	{
		for (j = 0; j < nBufWidth; j++)
		{
			lpOutImg[i * nBufWidth + j] = 0;
			pTmpImg[i * nBufWidth + j] = 0;
		}
	}

	for (i = 1; i < nHeightm1; i++)
	{
		for (j= 1; j < nWidthm1; j++)
		{
			newValue = 0;
			for (mr = 0; mr < 3; mr++)
			{
				for (mc = 0; mc < 3; mc++)
					newValue += (MaskBox[mr][mc] * lpInImg[(i + mr - 1) * nBufWidth + (j + mc - 1)]);
			}

			// 값을 양수로 변환
			if (newValue < 0)
				newValue = -newValue;
			pTmpImg[i * nBufWidth + j] = newValue;
		}
	}

	// 디스플레이를 위해 0에서 255사이로 값의 범위를 매핑
	// 이를 위해 먼저 최대, 최소값을 찾은 후 이를 이용하여 매핑한다.
	min = 0x0fffffff;
	max = -(0x0fffffff);

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			newValue = pTmpImg[i * nBufWidth + j];
			if (newValue < min)
				min = newValue;
			if (newValue > max)
				max = newValue;
		}
	}

	// 변환시 상수값을 미리 계산
	constVal1 = (float)(255.0 / (max - min));
	constVal2 = (float)(-255.0 * min / (max - min));

	for (i = 1; i < nHeightm1; i++)
	{
		for (j = 1; j < nWidthm1; j++)
		{
			// [min, max] 사이의 값을 [0, 255] 값으로 변환
			newValue = pTmpImg[i * nBufWidth + j];
			newValue = (int)(constVal1 * newValue + constVal2);

			lpOutImg[i * nBufWidth + j] = (BYTE)newValue;
		}
	}
	
	for (i = 1; i < nWidthm1; i++)
	{
		for (j = 1; j < nHeightm1; j++)
		{
			if (lpOutImg[j * nBufWidth + i] > 127)
				lpOutImg[j * nBufWidth + i] = 255;
			else
				lpOutImg[j * nBufWidth + i] = 0;
		}
	}

	if (pTmpImg)
	{
		delete [] pTmpImg;
		pTmpImg = NULL;
	}

	return TRUE;
}

BOOL CHMImageProcess::ImageBinary(LPBYTE lpInImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg)
		return FALSE;

	//255=white, 0=black
	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(lpInImg[i * nBufWidth + j] >= 128)
				lpInImg[i * nBufWidth + j]=255;
			else
				lpInImg[i * nBufWidth + j]=0;
		}
	}

	return TRUE;
}

BOOL CHMImageProcess::ImageBinary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	//255=white, 0=black
	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(lpInImg[i * nBufWidth + j] >= 128)
				lpOutImg[i * nBufWidth + j]=255;
			else
				lpOutImg[i * nBufWidth + j]=0;
		}
	}

	return TRUE;
}

BOOL CHMImageProcess::ImageThreshold(LPBYTE lpInImg, int nWidth, int nBufWidth, int nHeight, int nThreshold)
{
	if (!lpInImg)
		return FALSE;

	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(lpInImg[i * nBufWidth + j] >= nThreshold)
				lpInImg[i * nBufWidth + j] = 255;
			else
				lpInImg[i * nBufWidth + j] = 0;
		}
	}

	return TRUE;
}

BOOL CHMImageProcess::ImageThreshold(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight, int nThreshold)
{
	if (!lpInImg || !lpOutImg)
		return FALSE;

	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(lpInImg[i * nBufWidth + j] >= nThreshold)
				lpOutImg[i * nBufWidth + j] = 0;
			else
				lpOutImg[i * nBufWidth + j] = 255;
		}
	}

	return TRUE;
}
