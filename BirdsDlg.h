
// BirdsDlg.h : 头文件
//

#pragma once
#include "PhyEnv.h"
enum{BIRDX=50,BIRDY=260};
// CBirdsDlg 对话框
class CBirdsDlg : public CDialogEx
{
// 构造
public:
	double m_m1, m_k;
	
	bool m_running;
	HANDLE m_hThread;

	int m_num;
	bool m_go;
	CPoint m_posDown;

	CPhyEnv m_env;
	CBirdsDlg(CWnd* pParent = NULL);	// 标准构造函数
	void InitEnv();
	static DWORD WINAPI CBirdsDlg::GameLoop(_In_ LPVOID lpParameter);

// 对话框数据
	enum { IDD = IDD_BIRDS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
