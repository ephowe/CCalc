// CCalcDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CCCalcDlg �Ի���
class CCCalcDlg : public CDialog
{
// ����
public:
	CCCalcDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CCALC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
