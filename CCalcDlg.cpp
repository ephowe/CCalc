// CCalcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CCalc.h"
#include "Calc.h"
#include "CCalcDlg.h"
#include ".\ccalcdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCCalcDlg �Ի���



CCCalcDlg::CCCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCCalcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//
}

void CCCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit_01);
	DDX_Control(pDX, IDC_EDIT2, m_edit_02);
}

BEGIN_MESSAGE_MAP(CCCalcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedRun)
//	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// CCCalcDlg ��Ϣ�������

BOOL CCCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_edit_02.SetWindowText("�˼�����!֧�ּ򵥵��������ʽ,֧������Ƕ��(���128��)!!"); 

	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CCCalcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCCalcDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CCCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CCCalcDlg::OnBnClickedRun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

#define CODE_LEN	512		//���ʽ��󳤶�

	char mBuf[CODE_LEN] = {0};

	int mLen = m_edit_01.GetWindowTextLength();	//�õ����ʽ����

	if ( mLen++ > 511 )
	{
		m_edit_02.SetWindowText("error: ���ʽ����!!");
	
		return;
	}


	m_edit_01.GetWindowText( LPTSTR(mBuf) , mLen ); 

	CCalc* mCalc = new CCalc();	//����ʵ��

	mCalc->Scan(mBuf);

	if ( !mCalc->GetError() )
	{			
		if ( mCalc->DeCoder() )
		{
			
			::gcvt(mCalc->mValue,19,mBuf);

			m_edit_02.SetWindowText(LPCTSTR(mBuf));

		}
		else goto Err_Addr;
	}
	else
	{
		//ERROR MESSAGE
Err_Addr:

		m_edit_02.SetWindowText("");

		for ( int i = 0 ; i < mCalc->mTo ; i++ )
		{
			m_edit_02.ReplaceSel(mCalc->mMsg[i]);
		}
		
	}

	delete mCalc;
}

//void CCCalcDlg::OnEnChangeEdit1()
//{
//	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
//	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
//	// ���������� CRichEditCtrl().SetEventMask()��
//	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
//
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//}
