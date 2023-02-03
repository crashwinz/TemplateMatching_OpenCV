// HMBitmapWndFindCD.cpp: implementation of the CHMBitmapWndFindCD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMBitmapWndFindCD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMBitmapWndFindCD::CHMBitmapWndFindCD()
{
	m_dThresholdOffset = 1.0;
}

CHMBitmapWndFindCD::~CHMBitmapWndFindCD()
{

}

int CHMBitmapWndFindCD::FindCD(CHMBitmap* pBitmap, double dResolution, double* pfCDum, double* pfCDPixel, double* pfAngle)
{
	// 주어진 이미지에서 두 직선을 찾는다.
	LineEquation LineFirst, LineSecond;
	LineDirection LineDir = ImageFindLineDirection();
	if (LineDir == LineUnknown)
		return CD_RESULT_LINEUNKNOWN;

	if (!ImageFindLine(pBitmap, LineDir, LineFirst, LineSecond))//rtFirst, rtSecond, fTopFirst, fBottomFirst, fTopSecond, fBottomSecond);
		return CD_RESULT_NOTFINDLINE;

	if (pfAngle)
		*pfAngle = (LineFirst.s_fAngle + LineSecond.s_fAngle) / 2.0;

	// ax + by + c = 0 와 점 (x_0, y_0) 와의 거리는 
	// |ax_0 + by_0 + c |/√(a^2 + b^2) 
	float fTemp1, fTemp2, fCDPixelFirst, fCDPixelSecond;
	
	fTemp1 = fabs(LineFirst.s_fA * LineSecond.s_fStartX + LineFirst.s_fB * LineSecond.s_fStartY + LineFirst.s_fC);
	fTemp2 = sqrt(LineFirst.s_fA * LineFirst.s_fA + LineFirst.s_fB * LineFirst.s_fB);
	fCDPixelFirst = fTemp1 / fTemp2;
	
	fTemp1 = fabs(LineFirst.s_fA * LineSecond.s_fEndX + LineFirst.s_fB * LineSecond.s_fEndY + LineFirst.s_fC);
	fCDPixelSecond = fTemp1 / fTemp2;

	float fCDPixel = (fCDPixelFirst + fCDPixelSecond) / 2;
	*pfCDum = fCDPixel * dResolution;
	if (pfCDPixel)
		*pfCDPixel = fCDPixel;

	// Matched Image에 Line 그리기.
	EraseLine();
	CRect rt;
	rt.SetRect((int)LineFirst.s_fStartX, (int)LineFirst.s_fStartY, (int)LineFirst.s_fEndX, (int)LineFirst.s_fEndY);
	DrawLine(rt);
	rt.SetRect((int)LineSecond.s_fStartX, (int)LineSecond.s_fStartY, (int)LineSecond.s_fEndX, (int)LineSecond.s_fEndY);
	DrawLine(rt);

	// 법선의 방정식 구하기.
	LineEquation LineCenter;
	LineCenter.s_fStartX = (LineFirst.s_fEndX + LineFirst.s_fStartX) / 2;
	LineCenter.s_fStartY = (LineFirst.s_fEndY + LineFirst.s_fStartY) / 2;
	LineCenter.s_fA = -1 / LineSecond.s_fA;
	LineCenter.s_fB = -1.0;
	LineCenter.s_fC = LineCenter.s_fStartY - (LineCenter.s_fA * LineCenter.s_fStartX);
	
	LineCenter.s_fEndX = (LineSecond.s_fC - LineCenter.s_fC) / (LineCenter.s_fA - LineSecond.s_fA);
	LineCenter.s_fEndY = LineSecond.s_fA * LineCenter.s_fEndX + LineSecond.s_fC;

	rt.SetRect((int)LineCenter.s_fStartX, (int)LineCenter.s_fStartY, (int)LineCenter.s_fEndX, (int)LineCenter.s_fEndY);
	DrawLine(rt);

	return CD_RESULT_SUCCESS;
}

LineDirection CHMBitmapWndFindCD::ImageFindLineDirection()
{
	int i, j, nVert = 0, nHoriz = 0, nValue;

	// 세로방향으로 인접픽셀과의 밝기차 제곱의 합.
	// 이 값이 크면 선은 가로방향이다.
	int* pnVertSum = new int[m_nImageWidth];
	ZeroMemory(pnVertSum, m_nImageWidth * sizeof(int));
	for (i = 0; i < m_nImageWidth; i++)
	{
		for (j = 1; j < m_nImageHeight; j++)
		{
			nValue = m_pBuffer[j * m_nBufferWidth + i] - m_pBuffer[(j - 1) * m_nBufferWidth + i];
			pnVertSum[i] += (nValue * nValue);
		}
	}
	for (i = 0; i < m_nImageWidth; i++)
		nHoriz += pnVertSum[i];

	if (pnVertSum)
	{
		delete [] pnVertSum;
		pnVertSum = NULL;
	}


	// 가로방향으로 인접픽셀과의 밝기차 제곱의 합.
	// 이 값이 크면 선은 세로방향이다.
	int* pnHorizSum = new int[m_nImageHeight];
	ZeroMemory(pnHorizSum, m_nImageHeight * sizeof(int));
	for (i = 0; i < m_nImageHeight; i++)
	{
		for (j = 1; j < m_nImageWidth; j++)
		{
			nValue = m_pBuffer[i * m_nBufferWidth + j] - m_pBuffer[i * m_nBufferWidth + (j - 1)];
			pnHorizSum[i] += (nValue * nValue);
		}
	}
	for (i = 0; i < m_nImageHeight; i++)
		nVert += pnHorizSum[i];

	if (pnHorizSum)
	{
		delete [] pnHorizSum;
		pnHorizSum = NULL;
	}
	

	if (nVert >= nHoriz)
	{
		TRACE("\n\nVert\n\n");
		return LineVert;
	}
	else if (nHoriz > nVert)
	{
		TRACE("\n\nHoriz\n\n");
		return LineHoriz;
	}
	else
		return LineUnknown;
}

BOOL CHMBitmapWndFindCD::ImageFindLine(CHMBitmap* pDest, LineDirection& LineDir, LineEquation& LineFirst, LineEquation& LineSecond)// CRect& rtFirst, CRect& rtSecond)
{
	// 방향에 따른 변수값 변경.
	int nParallel, nCross;
	if (LineDir == LineHoriz)
	{
		nParallel = m_nImageWidth;
		nCross = m_nImageHeight;
	}
	else if (LineDir == LineVert)
	{
		nParallel = m_nImageHeight;
		nCross = m_nImageWidth;
	}

	CRect rt(0, 0, m_nImageWidth, m_nImageHeight);

	// Matching이 되는 이미지 조각을 잘라온다.
	if (!GetBitmapPiece(pDest, rt))
		return FALSE;

	// 중간값으로 이치화한다.
	int nAvg = pDest->GetAverage();
	nAvg *= (int)m_dThresholdOffset;
	pDest->ImageThreshold(nAvg);

	// 결과 저장 버퍼 생성.
	float* pfPeekFirst = new float[nParallel];
	ZeroMemory(pfPeekFirst, nParallel * sizeof(float));
	float* pfPeekSecond = new float[nParallel];
	ZeroMemory(pfPeekSecond, nParallel * sizeof(float));

	// 직선 외 좌우(또는 상하) 외곽의 값을 저장.
	int nLineStart = -1, nLineEnd = -1, nTempOrigin, nTempReverse;
	BOOL bScanBlackOrigin, bScanBlackReverse, bIsFirst;
	LPBYTE lpDest = pDest->GetBuffer();
	int i;
	for (i = 0; i < nParallel; i++)
	{
		bScanBlackOrigin = FALSE;
		bScanBlackReverse = FALSE;
		bIsFirst = TRUE;
		for (int j = 0; j < nCross; j++)
		{
			if (LineDir == LineHoriz)
			{
				nTempOrigin = pDest->GetValue(i, j);
				nTempReverse = pDest->GetValue(i, nCross - j - 1);
			}
			else if (LineDir == LineVert)
			{
				nTempOrigin = pDest->GetValue(j, i);
				nTempReverse = pDest->GetValue(nCross - j - 1, i);
			}

			if (nTempOrigin == 0 && !bScanBlackOrigin)
			{
				pfPeekFirst[i] = (float)j;
				bScanBlackOrigin = TRUE;
			}
			if (nTempReverse == 0 && !bScanBlackReverse)
			{
				pfPeekSecond[i] = (float)(nCross - j - 1);
				bScanBlackReverse = TRUE;
			}
		}
		TRACE("순방향 = %d, First - %3.3f, Second - %3.3f\n", i, pfPeekFirst[i], pfPeekSecond[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	// 사각형과 경계인 부분 제거
	BOOL bLeft = FALSE, bRight = FALSE;
	for (i = 0; i <= nCross / 2; i++)
	{
		if (pfPeekFirst[nCross / 2 - i] == 0 || pfPeekFirst[nCross / 2 - i] >= nCross - 1
			|| pfPeekSecond[nCross / 2 - i] == 0 || pfPeekSecond[nCross / 2 - i] >= nCross - 1)
		{
			TRACE("%d 왼쪽을 버림 i = %d\n", nCross / 2 - i, i);
			bLeft = TRUE;
		}
		if (pfPeekFirst[nCross / 2 + i - 1] == 0 || pfPeekFirst[nCross / 2 + i - 1] >= nCross - 1
			|| pfPeekSecond[nCross / 2 + i - 1] == 0 || pfPeekSecond[nCross / 2 + i - 1] >= nCross - 1)
		{
			TRACE("%d 오른쪽을 버림 i = %d\n", nCross / 2 + i - 1, i);
			bRight = TRUE;
		}

		if (bLeft)
		{
			pfPeekSecond[nCross / 2 - i] = 0;
			pfPeekFirst[nCross / 2 - i] = 0;
		}
		if (bRight)
		{
			pfPeekSecond[nCross / 2 + i - 1] = 0;
			pfPeekFirst[nCross / 2 + i - 1] = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 직선이 하나인 부분 제거
	int nCount = 0, nStart = -1, nEnd = 0;	// CD가 있는 픽셀 수, 시작점, 끝점.
	float fSum = 0, fAvg;
	for (i = 0; i < nCross; i++)
	{
		if (pfPeekFirst[i] == 0 || pfPeekSecond[i] == 0
			|| pfPeekFirst[i] >= nCross - 1 || pfPeekSecond[i] >= nCross - 1)
		{
			pfPeekSecond[i] = -1;
			pfPeekFirst[i] = -1;
		}
		else
		{
			nCount++;
			fSum += (pfPeekSecond[i] - pfPeekFirst[i]);
			if (nStart == -1)
				nStart = i;
			nEnd = i;
		}
	}
	if (nCount < nParallel / 2)
		return FALSE;

	fAvg = (float)(fSum / (float)nCount + 3.0);

	// 결함 부분을 제거한다.
	for (i = 0; i < nCross; i++)
	{
		if (pfPeekFirst[i] == -1 || pfPeekSecond[i] == -1)
			continue;
		if (pfPeekSecond[i] - pfPeekFirst[i] > fAvg + 3)
		{
			pfPeekFirst[i] = -1;
			pfPeekSecond[i] = -1;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 직선의 위쪽 절반과 아래쪽 절반의 중심점을 구한다.
	float	fTopFirst = 0.0,	fBottomFirst = 0.0,	fTopSecond = 0.0,	fBottomSecond = 0.0;
	int		nTopFirst = 0,		nBottomFirst = 0,	nTopSecond = 0,		nBottomSecond = 0;
	int nHalf = (int)((nEnd - nStart) / 2) + nStart;
	int nCountCalc = 0;
	for (i = 0; i < nHalf - nStart; i++)
	{
		if (pfPeekFirst[nStart + i] != -1)
		{
			fTopFirst += pfPeekFirst[nStart + i];
			nTopFirst++;
		}
		if (pfPeekFirst[nEnd - i - 1] != -1)
		{
			fBottomFirst += pfPeekFirst[nEnd - i - 1];
			nBottomFirst++;
		}
		if (pfPeekSecond[nStart + i] != -1)
		{
			fTopSecond += pfPeekSecond[nStart + i];
			nTopSecond++;
		}
		if (pfPeekSecond[nEnd - i - 1] != -1)
		{
			fBottomSecond += pfPeekSecond[nEnd - i - 1];
			nBottomSecond++;
		}
	}
	fTopFirst /= nTopFirst;
	fBottomFirst /= nBottomFirst;
	fTopSecond /= nTopSecond;
	fBottomSecond /= nBottomSecond;
//	TRACE("TopFirst - %3.3f, BottomFirst - %3.3f, TopSecond - %3.3f, BottomSecond - %3.3f\n", fTopFirst, fBottomFirst, fTopSecond, fBottomSecond);

	// 화면에 그릴 네 점을 리턴한다.
	if (LineDir == LineVert)
	{
		LineFirst.SetTwoPoint(fTopFirst, (float)((nHalf - nStart) / 2 + nStart), fBottomFirst, (float)(nEnd - (nEnd - nHalf) / 2));
		LineSecond.SetTwoPoint(fTopSecond, (float)((nHalf - nStart) / 2 + nStart), fBottomSecond, (float)(nEnd - (nEnd - nHalf) / 2));
	}
	else if (LineDir == LineHoriz)
	{
		LineFirst.SetTwoPoint((float)((nHalf - nStart) / 2 + nStart), fTopFirst, (float)(nEnd - (nEnd - nHalf) / 2), fBottomFirst);
		LineSecond.SetTwoPoint((float)((nHalf - nStart) / 2 + nStart), fTopSecond, (float)(nEnd - (nEnd - nHalf) / 2), fBottomSecond);
	}

	if (pfPeekFirst)
	{
		delete [] pfPeekFirst;
		pfPeekFirst = NULL;
	}
	

	if (pfPeekSecond)
	{
		delete [] pfPeekSecond;
		pfPeekSecond = NULL;
	}

	return TRUE;
}
