//#include <random>
// Birds.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CBirdsApp: 
// 有关此类的实现，请参阅 Birds.cpp
//

class CBirdsApp : public CWinApp
{
public:
	CBirdsApp();
	ULONG_PTR m_gdiplusToken;
	std::default_random_engine m_randEng;

	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image *&pImg);
	double Rand(double L, double R) {
		std::uniform_real_distribution<double> rander(L,std::nextafter(R,DBL_MAX));
		return rander(m_randEng);
	}
	int Rand(int L,int R) {
		std::uniform_int_distribution<int> rander(L,R);
		return rander(m_randEng);
	}
	LONGLONG GetTime() {
		LARGE_INTEGER liPerfFreq;
		QueryPerformanceFrequency(&liPerfFreq);
		LARGE_INTEGER liPerfNow;
		QueryPerformanceCounter(&liPerfNow);
		return liPerfNow.QuadPart * 1000 / liPerfFreq.QuadPart;
	}
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBirdsApp theApp;