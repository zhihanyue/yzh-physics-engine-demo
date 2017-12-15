
// BirdsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Birds.h"
#include "BirdsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBirdsDlg 对话框



CBirdsDlg::CBirdsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBirdsDlg::IDD, pParent), m_go(false), m_num(5), m_posDown(-1, -1), m_m1(13.0), m_k(35.5)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBirdsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBirdsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CBirdsDlg::OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CBirdsDlg 消息处理程序

BOOL CBirdsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CBitmap m_backgnd;
	m_backgnd.LoadBitmapW(IDB_BACKGND);
	BITMAP bmpInfo;
	m_backgnd.GetBitmap(&bmpInfo);

	CRect rect1,rect2;
	GetWindowRect(&rect1);
	GetClientRect(&rect2);
	rect1.right += bmpInfo.bmWidth - rect2.right;
	rect1.bottom += bmpInfo.bmHeight - rect2.bottom;
	MoveWindow(&rect1);
	CenterWindow();
	//SetWindowPos(NULL, 0, 0, , , SWP_NOMOVE);
	InitEnv();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBirdsDlg::OnPaint()
{
	CPaintDC dc(this);

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBirdsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBirdsDlg::OnBnClickedOk()
{
	
	//CDialogEx::OnOK();
}

BOOL CBirdsDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CBirdsDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}

DWORD WINAPI CBirdsDlg::GameLoop(_In_ LPVOID lpParameter)
{
	CBirdsDlg *dlg = (CBirdsDlg*)(theApp.m_pMainWnd);
	const int FPS = 70;
	const LONGLONG T = 1000 / FPS;
	LONGLONG nextT = theApp.GetTime();
	int sleepT = 0;
	CClientDC dc(dlg);
	while (dlg->m_running) {
		CBitmap m_backgnd;
		int m_height, m_width;

		m_backgnd.LoadBitmapW(IDB_BACKGND);
		BITMAP bmpInfo;
		m_backgnd.GetBitmap(&bmpInfo);
		m_height = bmpInfo.bmHeight;
		m_width = bmpInfo.bmWidth;
		
		CDC mdc;
		mdc.CreateCompatibleDC(&dc);
		mdc.SelectObject(&m_backgnd);
		dlg->m_env.Update();
		dlg->m_env.Display(&mdc);
		dc.BitBlt(0, 0, m_width, m_height, &mdc, 0, 0, SRCCOPY);
		
		nextT += T;
		sleepT = (int)(nextT - theApp.GetTime());
		if (sleepT >= 0) {
			TRACE("%d\n", sleepT);
			Sleep(sleepT);
		}
		else TRACE("卡！\n");
	}
	return 0;
}

void CBirdsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_running = false;
	WaitForSingleObject(m_hThread,INFINITE);
}


void CBirdsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CPhyObj *tobj = m_env.Route(SPointF(point.x, point.y));
	if (!m_go && m_posDown != CPoint(-1, -1))
		m_env.m_objs[1].m_pos = SPointF(BIRDX, BIRDY) + SPointF(point.x, point.y) - SPointF(m_posDown.x, m_posDown.y);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CBirdsDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPhyObj *tobj = m_env.Route(SPointF(point.x, point.y));
	if (tobj == &(m_env.m_objs[1])) {
		if (!m_go && tobj->m_pos == SPointF(BIRDX,BIRDY) )
			m_posDown = point;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CBirdsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPhyObj *tobj = m_env.Route(SPointF(point.x, point.y));
	if (tobj == &(m_env.m_objs[1])) {
		if (m_go || m_posDown == CPoint(-1, -1)) return;

		CPoint temp = point - m_posDown;
		SVecF delta(temp.x,temp.y);
		double len = delta.Norm();
		
		if (len <= 10.0) {
			m_env.m_objs[1].m_pos.x = BIRDX;
			m_env.m_objs[1].m_pos.y = BIRDY;
		}
		else {
			double angle = delta.Angle() + PI;
			//能量守恒
			double v = m_k * len * len / m_m1 + 2 * 9.8 * (BIRDY - point.y);
			if (v <= 0) {
				AfxMessageBox(_T("弹性势能太小，鸟飞不出去...\n\n请尝试 尽可能拉长弹弓 或 增大劲度系数。"));
				m_env.m_objs[1].m_pos.x = BIRDX;
				m_env.m_objs[1].m_pos.y = BIRDY;
			} else {
				v = sqrt(v);
				m_go = true;
				m_env.m_objs[1].m_vel = SVecF(v * cos(angle), v * sin(angle));
				m_env.m_objs[1].m_acc = SVecF(0, 9.8);
			}
		}
	} else if (tobj == &(m_env.m_objs[2])) {
		m_go = false;
		m_posDown = CPoint(-1, -1);
		m_running = false;
		WaitForSingleObject(m_hThread, INFINITE);
		m_env = CPhyEnv();
		InitEnv();
	}
	m_posDown = CPoint(-1, -1);
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CBirdsDlg::InitEnv()
{
	Image *pImg;

	theApp.ImageFromIDResource(IDB_GND, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(1000000.0, SPointF(0, 395), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_B1, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(m_m1, SPointF(BIRDX, BIRDY), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_RESTART, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(1000000.0, SPointF(510, 3), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_M1, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(0.5, SPointF(450, 260), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_M2, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(0.7, SPointF(500, 275), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_M3, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(0.2, SPointF(495, 337), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	theApp.ImageFromIDResource(IDB_M4, _T("PNG"), pImg);
	m_env.m_objs.push_back(CPhyObj(0.8, SPointF(560, 275), SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

	//ADDED 7

	for (int i = 1; i <= m_num; ++i) {
		theApp.ImageFromIDResource(theApp.Rand(IDB_G1, IDB_G7), _T("PNG"), pImg);
		SPointF pt(theApp.Rand(200, 600), theApp.Rand(250, 360));
		m_env.m_objs.push_back(CPhyObj(theApp.Rand(1.0, 5.0), pt, SVecF(0, 0), SVecF(0, 0), pImg, 1.0));

		int cntTimes = 0;
		while(true) {
			bool isOK = true;
			for (int j = 0, _m = m_env.m_objs.size(); j < _m - 1; ++j) {
				if (m_env.m_objs.back().IsCollision(m_env.m_objs[j], false)) {
					isOK = false;
					break;
				}
			}
			if (isOK) break;
			++cntTimes;
			pt = SPointF(theApp.Rand(300, 600), theApp.Rand(250, 360));
			m_env.m_objs.back().m_pos = pt;
			if (cntTimes > 10) break;
		}
	}
	theApp.ImageFromIDResource(IDB_B1, _T("PNG"), pImg);

	//theApp.ImageFromIDResource(IDB_G1, _T("PNG"), pImg);
	//m_env.m_objs.push_back(CPhyObj(10.0,SPointF(100,50),SVecF(5,8),SVecF(0,3), pImg, 1.0));
	//theApp.ImageFromIDResource(IDB_GND, _T("PNG"), pImg);
	//m_env.m_objs.push_back(CPhyObj(10000.0, SPointF(0, 300), SVecF(0, 0), SVecF(0,0.5), pImg, 1.0));

	m_env.m_speed = 0.005;
	m_running = true;
	m_hThread = CreateThread(NULL, 0, GameLoop, NULL, 0, NULL);
}