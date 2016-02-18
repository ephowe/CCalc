// Calc.h: interface for the CCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_)
#define AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////
//��������ڱ��ʽ����ɨ�ֳɵ���������.��:
//1.����ɨ����ʽ�Ƿ���ڷǷ��ַ�.�ɹ�����.
//2.(1)ͨ����,����ɨ����Ĳ����������ŵ����з�ʽ�Ƿ�Ϸ�.�ɹ�����.
//3.��ʼ���㲢�����ֵ��С������Ƿ�Ϊ0.
//����ɨ�軥Ϊ����,������һ�������,�����ж�ɨ�貢��ʾERRMSG,���ٽ�ȥ��ȥ.���֮��Ĵ��󲻻ᱻ���.



//////////////////////////////////////////////////////////////////////
#define	NUMBER		1		//�����
#define OPERANDS	2		//����
#define LBRACKET	4		//������
#define RBRACKET	8		//������
#define VARIABLE	16		//����		//��ʱδ�õ�

//��ṹ
typedef struct tag_S_Token
{
	char Tok[18];			//����һ����
	char Type;				//�������
	int	 Idx;				//������
	double Value;			//�����������ֵ
	tag_S_Token *Prior;		//ָ��ǰ
	tag_S_Token *Next;		//ָ���
}
S_Token , *LP_S_Token;

//������Ϣ�ṹ
typedef struct tag_S_Err
{
	char	*lpMsg;		//ָ�������Ϣ
	char	Cod;		//����Ƿ��ַ�
	int		Col;		//�����к�
	tag_S_Err *Next;	//ָ����һ������ṹ
} 
S_Err , *LP_S_Err;

class CCalc  
{
private:
		
	LP_S_Token lpFirstTok , lpCurrentTok;	//����ָ��
	LP_S_Err lpFirstErr , lpCurrentErr;	//������ָ��

	char *ErrMsg[20];				//��Ŵ�����Ϣ		
	char *coder;					//ָ����ʽ

	BOOL Is_Bra(char bra);		//������?
	BOOL Is_Op(char op);		//�������?
	BOOL Is_Num(char num);		//������?

	int SetTokenLink(char *mcode);			//������TOKEN��,�����ɹ� RET �����Ŀ,ȡ������ RET -1 ,��SETERROR
	int GetNumber(char *dst , char *src);	//ȡ�ַ����е���ֵ,RET ��ֵLEN - 1

	BOOL AddToken(S_Token& tok);	//�����
	BOOL FreeTokenLink(LP_S_Token lpTok);	//�ͷ�TOK����

	void LoadErrorInfo();			//���ش�����Ϣ
	BOOL FreeError();				//������Ϣ���ͷ��ڴ�
	BOOL SetError(S_Err& iErr);		//���ô�����Ϣ��

	BOOL SetValue(LP_S_Token p);			//�������ֵ
	BOOL RstIndex(LP_S_Token lpTokStt);		//����������
	int GetBracket();				//ȡ��������������ڵ��ӱ��(lpCurrentTokΪ���ӱ��ʽ��ͷһ��)ʽ��֮����(������������)
	double Calcer(char op , double m1 , double m2);		//��������������

	BOOL Originate();				//��ʼ
	BOOL Level1(int n);				//������ʽ�еĴη���
	BOOL Level2(int n);				//������ʽ�еĳ� �� ȡ��
	BOOL Level3(int n);				//������ʽ�еļӼ�
	BOOL Expire(int n);				//��ֹ���ʽ

	//
	//�ַ����ϲ� ; ���������ַ����ϲ�,�������ַ��ͱ���&eof='\0'��β,
	//�׸�����Ϊ�û��ṩ��BUF,�����ַ�������COPY��BUF��.
	BOOL Combo_Str(char* , ...);	


public:

	CCalc();						//����
	virtual ~CCalc();				//����

	BOOL Scan(char* mcode);		//����ɨ����ʽ���������GetError()ȡ����Ϣ
	BOOL ScanEx(char *mcode);	//����ɨ����ʽ���������GetError()ȡ����Ϣ

	int DeCoder();				//�������ʽ
	
	BOOL GetError();				//ȡ�ô�����Ϣ

	double mValue;				//���
	char *mMsg[256];			//�����Ϣ
	char mTo;					//����MSG�����

protected:

};

#endif // !defined(AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_)
