// CCalcDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CCCalcDlg 对话框
class CCCalcDlg : public CDialog
{
// 构造
public:
	CCCalcDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CCALC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// EDIT1
	CEdit m_edit_01;
	// EDIT02
	CEdit m_edit_02;
	afx_msg void OnBnClickedRun();
//	afx_msg void OnEnChangeEdit1();
};
