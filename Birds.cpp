
// Birds.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Birds.h"
#include "BirdsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBirdsApp

BEGIN_MESSAGE_MAP(CBirdsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBirdsApp 构造

CBirdsApp::CBirdsApp() :m_randEng((unsigned int)time(0))
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

}

CBirdsApp theApp;

BOOL CBirdsApp::InitInstance()
{
	srand((unsigned)time(0));
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	CBirdsDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

int CBirdsApp::ExitInstance()
{
	GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}

BOOL CBirdsApp::ImageFromIDResource(UINT nID, LPCTSTR sTR, Image *&pImg)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), sTR);
	if (!hRsrc) return FALSE;
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc) return FALSE;
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	GlobalUnlock(m_hMem);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
	pImg = Gdiplus::Image::FromStream(pstm);
	pstm->Release();
	FreeResource(lpRsrc);
	GlobalFree(m_hMem);
	return TRUE;
}
