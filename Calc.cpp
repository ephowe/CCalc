// Calc.cpp: implementation of the CCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Calc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalc::CCalc()
{	
	this->LoadErrorInfo();		//������Ϣ

	coder = NULL;

	mValue = 0.0;

	lpFirstTok = NULL;
	lpCurrentTok = NULL;

	lpFirstErr = NULL;
	lpCurrentErr = NULL;

	mTo = 0;

}

CCalc::~CCalc()
{
	this->FreeError();
	this->FreeTokenLink(NULL); 
}

BOOL CCalc::SetError(S_Err& iErr)
{
	if (!lpFirstErr)
	{
		if ( !(lpFirstErr = new S_Err) ) return FALSE; 
		*lpFirstErr = iErr;
		lpFirstErr->Next = NULL;
		lpCurrentErr = lpFirstErr;
	}
	else
	{
		if ( !(lpCurrentErr->Next = new S_Err) ) return FALSE;
		*(lpCurrentErr->Next) = iErr;
		lpCurrentErr = lpCurrentErr->Next;
		lpCurrentErr->Next = NULL;	
	}
	return TRUE;
}

BOOL CCalc::GetError()
{
	char colBuf[16] = {0};
	char codBuf[2] = {0};
	const char eof = '\0';

	if (!lpFirstErr) return FALSE;

	//::printf("\n���ʽ: %s \n",coder);

	lpCurrentErr = lpFirstErr;

	while (lpCurrentErr)
	{
		//
		mMsg[mTo] = new char[128];

		switch (lpCurrentErr->Cod)
		{
		case '(': 
			//::printf("error: %c %s -> �� %d ������ \n",lpCurrentErr->Cod,lpCurrentErr->lpMsg,lpCurrentErr->Col);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: ") , 
						codBuf , TEXT(" ") , lpCurrentErr->lpMsg , 
						TEXT(" -> �� ") ,itoa(lpCurrentErr->Col ,colBuf , 10) , 
						TEXT(" ������\r\n"), &eof);
			
			if ( mTo < 256 ) mTo++;

			break;
		case ')':
			//::printf("error: %c %s -> �� %d ������ \n",lpCurrentErr->Cod,lpCurrentErr->lpMsg,lpCurrentErr->Col);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: ") ,
						codBuf , TEXT(" ") ,
						lpCurrentErr->lpMsg , TEXT(" -> �� ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , 
						TEXT(" ������\r\n"), &eof);
			if ( mTo < 256 ) mTo++;

			break;
		case '\0':
			//::printf("error: �� %d �� -> %s \n",lpCurrentErr->Col,lpCurrentErr->lpMsg);
			Combo_Str( mMsg[mTo] , TEXT("error: �� ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" �� -> ") ,
						lpCurrentErr->lpMsg , TEXT("\r\n") , &eof);
			if ( mTo < 256 ) mTo++;
			break;
		case 'N':
			//::printf("error: �� %d �� -> %s \n",lpCurrentErr->Col,lpCurrentErr->lpMsg);
			Combo_Str( mMsg[mTo] , TEXT("error: �� ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" �� -> ") ,
						lpCurrentErr->lpMsg , TEXT("\r\n") , &eof);
			if ( mTo < 256 ) mTo++;
			break;		
		default:
			//::printf("error: �� %d �� -> %s  %c \n",lpCurrentErr->Col,lpCurrentErr->lpMsg,lpCurrentErr->Cod);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: �� ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" �� -> ") ,
						lpCurrentErr->lpMsg , TEXT(" ") , codBuf , TEXT("\r\n") , &eof);
			if ( mTo < 256 ) mTo++;
		}
		lpCurrentErr = lpCurrentErr->Next;
	}

	FreeError();

	return TRUE;
}

BOOL CCalc::FreeError() 
{
	if (!lpFirstErr) return FALSE;
	
	while (lpFirstErr)
	{
		lpCurrentErr = lpFirstErr->Next;
		delete lpFirstErr;
		lpFirstErr = lpCurrentErr;
	}

	lpFirstErr = lpCurrentErr = NULL;

	return TRUE;
}

BOOL CCalc::Scan(char *mcode)
{
	S_Err ErrStt;		//������Ϣ�ṹ
	int blkh = 0;		//����������
	int rbidx = 1; //, lbidx = 1;		//��������
	int col = 0;		//��ȡ�к�
	BOOL blret = TRUE;	//
	char *p = mcode;	//�����ַ����

	if (!mcode) return (!blret);	//�ж��Ƿ���ڱ��ʽ

	coder = mcode;					//����ָ�븱��

	//ɨ���������ʽ
	while( *mcode )
	{
		col++;
		if ( *mcode == '(' )		//Ѱ��������
		{
			blkh++;
		}
		else if ( *mcode == ')' )	//���������
		{
			if ( !blkh )
			{
				ErrStt.lpMsg = ErrMsg[1];
				ErrStt.Cod = *mcode;
				ErrStt.Col = rbidx;	//
				SetError(ErrStt);		//���ô�����Ϣ
				if (blret) blret = !blret;
			}
			else
			{
				blkh--;
			}
			rbidx++;
		}
		else if ( !Is_Num(*mcode) && !Is_Op(*mcode) && *mcode != ' ' && *mcode != '.' )		//�Ƿ��ǷǷ��ַ�
		{
			ErrStt.lpMsg = ErrMsg[0];
			ErrStt.Cod = *mcode;
			ErrStt.Col = col;
			SetError(ErrStt);		//���ô�����Ϣ
			if (blret) blret = !blret;
		}
		mcode++;
	}

	//ɨ����Ϻ�,���ù���������ԵĴ�����Ϣ
	//δ���ƣ����޸�
	if ( blkh != 0 )
	{
		while ( blkh )
		{
			if ( blkh > 0 )
			{
				ErrStt.lpMsg = ErrMsg[1];
				ErrStt.Cod = '(';
				blkh--;
			}
			else
			{
				ErrStt.lpMsg = ErrMsg[2];
				ErrStt.Cod = ')';
				blkh++;
			}
			ErrStt.Col = -1;	//
			SetError(ErrStt);		//���ô�����Ϣ
		}
		if ( blret ) blret = !blret;
	}

	if ( blret ) return ScanEx(p);
	
	return blret;
}

int CCalc::DeCoder()
{
	int mret = 0;

	SetValue(NULL);

	mValue = 0.0;	//��0
	
	if ( Originate() )	//��ʼ����
	{
		//��֤����Ĵ�С
		if ( lpFirstTok->Value <= 9223372036854775808 )
		{
			//printf("\n���ʽ: %s \nscan pass! \n",coder);
			
			//printf("= %f \n",lpFirstTok->Value); //��ʾ���

			mValue = lpFirstTok->Value;			//�Ѽ���������MVALUE

			FreeError();

			mret = 1;	//Success
		}
		else
		{
			//��ֵ����
		}
	}
	else
	{
		GetError();
	}

	FreeTokenLink(NULL);	//�ͷ���

	return mret;
}

double CCalc::Calcer(char op , double m1 , double m2)
{
	switch (op)
	{
	case '+': m1 = m1 + m2; break;
	case '-': m1 = m1 - m2; break;
	case '*': m1 = m1 * m2; break;
	case '/': m1 = m1 / m2; break;
	case '^': m1 = pow(m1,m2); break;
	case '%': m1 = double( (int)m1 % (int)m2 ); break;
	default: ;	///
	}	
	return m1;
}

void CCalc::LoadErrorInfo()
{
	ErrMsg[0] =	"���ڷǷ��ַ�!";
	ErrMsg[1] =	"����δ���!";
	ErrMsg[2] =	"���ݳ��ȳ�������!";
	ErrMsg[3] =	"�������������䲻��������!";
	ErrMsg[4] = "���������ݼ���ڿո�,�Ƿ��������!";
	ErrMsg[5] = "�Ƿ���С����!";
	ErrMsg[6] = "���ʽ���������3��!";
	ErrMsg[7] = "��ֵ������(��������!";
	ErrMsg[8] = "�����������)��������!";
	ErrMsg[9] = ")������(���ż��������!";
	ErrMsg[10] = "(������)���ż�����,�Ƿ��������!";
	ErrMsg[11] = ")���Ų�����ֵ������!";
	ErrMsg[12] = "(���Ų��������������,�������������һ������!";
	ErrMsg[13] = "���ʽ���һ���Ϊ�������(����!";
	ErrMsg[14] = "���ʽ��һ���Ϊ�����!";
	ErrMsg[15] = "��ʾ�����������ڵ���ֵ������1��!";
	ErrMsg[16] = "��������Ϊ0!";
	ErrMsg[17] = "��������Ƕ���Ѿ�����������ƵĲ���!";

}

BOOL CCalc::ScanEx(char *mcode)
{
	LP_S_Token lpTokStt;		//����Ϣ�ṹ
	S_Err mErr;					//ERROR_MSG		
	int toknum;					//�����Ŀ
	BOOL blret = TRUE;			//����ֵ

	///////
	//�������㸺���ӱ��ʽǶ�׵�����
#define MAXTF	128				//���Ƕ�׵Ĳ���	

	int tfop[MAXTF] = {0};			//�ж���������,��������������ֵ����
	int lgkh[MAXTF] = {0};			//�ж�(-(x-y)) or (-((x-y)-z)) or (-((x-(-(y-e))-z))
	int idx = 0;				//�������

	tfop[idx] = lgkh[idx] = 0;	//��һ��Ԫ��Ϊ���� ��ʼ��Ϊ0
	///////

	if ( ( toknum = SetTokenLink(mcode) ) != -1 )
	{
		if ( toknum < 3 )		//�����Ŀ��������3��
		{
			//ERROR
			mErr.lpMsg = ErrMsg[6];
			mErr.Cod = '\0';
			mErr.Col = 0;
			SetError(mErr);

			//������� �ͷ���
			FreeTokenLink(NULL);
			return FALSE;
		}
		
		lpTokStt = lpFirstTok;
	
		while ( lpTokStt->Next )
		{
			switch ( lpTokStt->Type )
			{
			case NUMBER:
				
				if ( tfop[idx] > 1 && !lgkh[idx] )
				{
					//ERROR
					mErr.lpMsg = ErrMsg[15];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);
					blret = FALSE;
				}

				if (tfop[idx] && !lgkh[idx] ) tfop[idx]++;

				switch ( lpTokStt->Next->Type )
				{
				case NUMBER:
					//ERROR;
					mErr.lpMsg = ErrMsg[4];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);

					blret = FALSE;
					break;
				case LBRACKET:
					mErr.lpMsg = ErrMsg[7];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);

					///
					if ( tfop[idx] > 1 && !lgkh[idx] )
					{
						//ERROR
						mErr.lpMsg = ErrMsg[15];
						mErr.Cod = '\0';
						mErr.Col = lpTokStt->Next->Idx;
						SetError(mErr);
						blret = FALSE;
					}

					if ( tfop[idx] ) lgkh[idx]++;	//

					blret = FALSE;
					//ERROR;
					break;
				case RBRACKET:		
					//������������ű�ʾ���������ж��ѽ���
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//���ϱ��
						//������һ�㲢�ݼ��Ǻ�
						if ( lgkh[idx] ) lgkh[idx]--;	
					}

					break;
				}
				break;
			case OPERANDS:
				/////
				if ( !lpTokStt->Prior )
				{
					//ERROR;
					mErr.lpMsg = ErrMsg[14];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);
					blret = FALSE;
					break;
				}

				switch ( lpTokStt->Next->Type )
				{
				case OPERANDS:
					//ERROR;
					mErr.lpMsg = ErrMsg[3];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);
					blret = FALSE;
					break;
				case RBRACKET:
					//ERROR;
					mErr.lpMsg = ErrMsg[8];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);
					blret = FALSE;

					//������������ű�ʾ���������ж��ѽ���
					//�д���
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//���ϱ��
						if ( lgkh[idx] ) lgkh[idx]--;
					}							

					break;
				case LBRACKET:
					if ( tfop[idx] > 1 && !lgkh[idx] )
					{
						//ERROR
						mErr.lpMsg = ErrMsg[15];
						mErr.Cod = '\0';
						mErr.Col = lpTokStt->Next->Idx;
						SetError(mErr);
						blret = FALSE;
					}

					if ( tfop[idx] && !lgkh[idx] ) tfop[idx]++;

					if ( tfop[idx] ) lgkh[idx]++;

					break;
				}
				break;
			case LBRACKET:
				switch ( lpTokStt->Next->Type )
				{
				case OPERANDS:
					//�迼����������ʾ
					if ( lpTokStt->Next->Tok[0] != '-' && lpTokStt->Next->Tok[0] != '+' )
					{
						//ERROR;
						mErr.lpMsg = ErrMsg[12];
						mErr.Cod = '\0';
						mErr.Col = lpTokStt->Next->Idx;
						SetError(mErr);
						blret = FALSE;
					}
					else
					{
						if ( idx >= 0 && idx < MAXTF )
						{
							tfop[++idx] = 1;		//��ʼ�жϲ����ϱ��
							lgkh[idx] = 0;
						}
						else if ( idx < 0 )			//�������С��0
						{
							idx = 1;
						}
						else						//��Ŵ���MAXTF
						{
							mErr.lpMsg = ErrMsg[17];
							mErr.Cod = '\0';
							mErr.Col = lpTokStt->Next->Idx;
							SetError(mErr);
							blret = FALSE;
						}
					}
					break;
				case RBRACKET:
					//WARNING;
					mErr.lpMsg = ErrMsg[10];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);

					//������������ű�ʾ���������ж��ѽ���
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//���ϱ��
						if ( lgkh[idx] ) lgkh[idx]--;
					}

					blret = FALSE;
					break;
				case LBRACKET:
					if ( tfop[idx] ) lgkh[idx]++;
					break;
				}
				break;
			case RBRACKET:
				switch ( lpTokStt->Next->Type )
				{
				case NUMBER:
					//ERROR;
					mErr.lpMsg = ErrMsg[11];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);
					blret = FALSE;
					break;
				case LBRACKET:
					//ERROR;
					mErr.lpMsg = ErrMsg[9];
					mErr.Cod = '\0';
					mErr.Col = lpTokStt->Idx;
					SetError(mErr);

					///
					if ( tfop[idx] > 1 && !lgkh[idx] )
					{
						//ERROR
						mErr.lpMsg = ErrMsg[15];
						mErr.Cod = '\0';
						mErr.Col = lpTokStt->Next->Idx;
						SetError(mErr);
						blret = FALSE;
					}

					if ( tfop[idx] ) lgkh[idx]++;

					blret = FALSE;
					break;
				case RBRACKET:		
					//������������ű�ʾ���������ж��ѽ���
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//���ϱ��
						if ( lgkh[idx] ) lgkh[idx]--;
					}
					break;
				}
				break;				
			}
			lpTokStt = lpTokStt->Next; 		
		}
		//������һ��
		if ( ( lpTokStt->Type == OPERANDS ) || ( lpTokStt->Type == LBRACKET ) )
		{
			//ERROR
			mErr.lpMsg = ErrMsg[13];
			mErr.Cod = '\0';
			mErr.Col = lpTokStt->Idx;
			SetError(mErr);
			blret = FALSE;			
		}
	}
	else blret = FALSE;	//�д���
	
	if ( !blret ) FreeTokenLink(NULL);	//������

	return blret;
}

BOOL CCalc::AddToken(S_Token &tok)
{
	if (!lpFirstTok)
	{
		if ( !(lpFirstTok = new S_Token) ) return FALSE; 
		*lpFirstTok = tok;
		lpFirstTok->Idx = 1;
		lpFirstTok->Next = NULL;
		lpFirstTok->Prior = NULL;
		lpCurrentTok = lpFirstTok;
	}
	else
	{
		if ( !(lpCurrentTok->Next = new S_Token) ) return FALSE;
		*(lpCurrentTok->Next) = tok;
		lpCurrentTok->Next->Prior = lpCurrentTok;
		lpCurrentTok = lpCurrentTok->Next;
		lpCurrentTok->Idx = lpCurrentTok->Prior->Idx + 1; 
		lpCurrentTok->Next = NULL;	
	}
	return TRUE;
}

BOOL CCalc::Is_Num(char num)
{
	if ( num >= '0' && num <= '9') return TRUE;
	else return FALSE;
}

BOOL CCalc::Is_Op(char op)
{
	char *Operand = "^*/%+-";		//��������
	
	while ( *Operand ) if ( op == *Operand++ ) return TRUE;

	return FALSE;
}

BOOL CCalc::Is_Bra(char bra)
{
	if ( bra == '(' || bra == ')' ) return TRUE;
	return FALSE;
}

int CCalc::GetNumber(char *dst, char *src)
{
	char *p = src;
	int i = 0;	//�±���
	int dn = 1;	//��¼С���������
	BOOL blerr = FALSE;
	S_Err mErr;

	while ( *p )
	{
		if ( i > 18 )
		{
			if (!blerr)
			{
				mErr.lpMsg = ErrMsg[2];
				mErr.Col = int( src - coder + 1 );
				mErr.Cod = 'N';
				SetError(mErr);
				blerr = !blerr;
			}
			i--;		//����ʹ�����*�ŵ��������±����
		}

		if ( Is_Num(*p) )
		{
			dst[i++] = *p++;	//*-------	
		}
		else if ( *p == '.' )
		{
			if ( Is_Num(p[1]) && dn < 2 ) //С�����������ܴ���2
			{
				dst[i++] = *p++;	//*-------
				dn++;
			}
			else break;
		}
		else 
		{
			break;		
		}
	}
	dst[i] = '\0';		//
	return int( p - src - 1 );
}

int CCalc::SetTokenLink(char *mcode)
{
	int lgret = 0;
	char *p = mcode;

	S_Token TokStt;
	S_Err mErr;
	
	while ( *p )
	{
		if ( Is_Num(*p) )
		{
			p += GetNumber(TokStt.Tok,p);	//�������ֵĳ���-1
			TokStt.Type = NUMBER;
			if (lgret != -1)
			{
				AddToken(TokStt);
				lgret++;		
			}
		}
		else if ( Is_Op(*p) )
		{
			TokStt.Tok[0] = *p;
			TokStt.Type = OPERANDS;
			if (lgret != -1)
			{
				AddToken(TokStt);
				lgret++;		
			}
		}
		else if ( Is_Bra(*p) )
		{
			switch ( TokStt.Tok[0] = *p )
			{
			case '(': TokStt.Type = LBRACKET; break;
			case ')': TokStt.Type = RBRACKET; break;			
			}
			if (lgret != -1)
			{
				AddToken(TokStt);
				lgret++;		
			}	
		}
		else if ( *p == '.' )		//���ַǷ���С����
		{
			mErr.lpMsg = ErrMsg[5];
			mErr.Col = (int)( p - mcode + 1 );
			mErr.Cod = *p;
			SetError(mErr);
			if ( lgret != -1 ) lgret = -1;
		}
		p++;
	}

	return lgret;
}

BOOL CCalc::FreeTokenLink(LP_S_Token lpTok)	//����Ϊ��ɾ��������
{
	LP_S_Token p;
	
	if (!lpFirstTok) return FALSE;
	
	if ( lpTok )
	{
		//������Ϊ����ɾ������
		while (lpTok)	
		{
			p = lpTok->Next;
			delete lpTok;
			lpTok = p;	
		}
	}
	else
	{	
		while (lpFirstTok)
		{
			lpCurrentTok = lpFirstTok->Next;
			delete lpFirstTok;
			lpFirstTok = lpCurrentTok;
		}
		lpFirstTok = lpCurrentTok = NULL;	
	}
	
	return TRUE;
}

int CCalc::GetBracket()
{
	LP_S_Token p = lpCurrentTok = lpFirstTok;

	while ( p )
	{
		if ( p->Type == LBRACKET )
		{
			lpCurrentTok = p;   
		}
		else if ( p->Type == RBRACKET )
		{
			break;
		}
		else if ( !p->Next ) break;

		p = p->Next; 
	}

	return ( p->Idx - lpCurrentTok->Idx + 1 );
}

BOOL CCalc::RstIndex(LP_S_Token lpTokStt)
{
	//�д���
	if (!lpTokStt) return FALSE;
	
	while ( lpTokStt = lpTokStt->Next )
	{
		lpTokStt->Idx = lpTokStt->Prior->Idx + 1;  
	}

	return TRUE;
}

BOOL CCalc::SetValue(LP_S_Token p)
{
	if (!p)
	{
		if ( !(p = lpFirstTok) ) return FALSE;

		while ( p )
		{
			if ( p->Type == NUMBER ) p->Value = atof(p->Tok); 
			p = p->Next; 
		}
	}
	else
	{
		//
	}

	return TRUE;
}

BOOL CCalc::Originate()
{
	return Level1(GetBracket());
}

BOOL CCalc::Level1(int n)
{
	LP_S_Token p = lpCurrentTok , tp;
	int m = n;

	for ( int i = 0 ; i < m ; i++ )
	{
		if ( p->Tok[0] == '^' )
		{
			p->Prior->Value = Calcer(p->Tok[0],p->Prior->Value,p->Next->Value);    
			
			//�������
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;	//Ϊ��ɾ�������β��
			tp = p;		//�������������
			p = p->Prior;

			FreeTokenLink(tp);
			RstIndex(p);

			n -= 2;
			i++;
			
		}
		p = p->Next; 
	}

	if ( n == 1 )	//���ڵ���3���ֱ������β
		return Expire(n);
	else
		return Level2(n);	//��ʼ����*/%
}

BOOL CCalc::Level2(int n)
{
	LP_S_Token p = lpCurrentTok , tp;
	S_Err mErr;					//ERROR_MSG
	BOOL blisop = FALSE;	//�ж��Ƿ���*/%
	int m = n;

	for ( int i = 0 ; i < m ; i++ )
	{
		switch ( p->Tok[0] )
		{
		case '*':
			blisop = TRUE;
			break;
		case '/':
			//�������Ƿ�Ϊ0
			if ( !p->Next->Value )
			{
				mErr.lpMsg = ErrMsg[16];
				mErr.Cod = '\0';
				mErr.Col = p->Next->Idx;	///warning
				SetError(mErr);
				return FALSE;		
			}
			blisop = TRUE;
			break;
		case '%':
			blisop = TRUE;
			break;
		}

		if ( blisop )
		{	
			p->Prior->Value = Calcer(p->Tok[0],p->Prior->Value,p->Next->Value);    
			
			//�������
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;	//Ϊ��ɾ�������β��
			tp = p;		//�������������
			p = p->Prior;

			FreeTokenLink(tp);
			RstIndex(p);

			n -= 2;
			i++;
			blisop = !blisop;
		}

		p = p->Next; 
	}

	if ( n == 1 ) 
		return Expire(n);
	else
		return Level3(n);	//��ʼ����+-
}

BOOL CCalc::Level3(int n)
{
	LP_S_Token p = lpCurrentTok , tp;
	int blisop = 0;	//�ж��Ƿ���+- = 1,���� = 2 ELSE = 0
	int m = n;

	for ( int i = 0 ; i < m ; i++ )
	{
		switch ( p->Tok[0] )
		{
		case '+':
			//����Ƿ�Ϊ���� ���� �Ӻ�
			if ( p->Prior->Type != LBRACKET ) blisop = 1;
			else blisop = 2;
			break;
		case '-':
			//����Ƿ�Ϊ���� ���� ����
			if ( p->Prior->Type != LBRACKET ) blisop = 1;
			else blisop = 2;
			break;
		}

		if ( blisop == 1 )	//���Ӽ�����
		{	
			p->Prior->Value = Calcer(p->Tok[0],p->Prior->Value,p->Next->Value);    
			
			//�������
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;
			tp = p;		//�������������
			p = p->Prior;

			FreeTokenLink(tp);
			RstIndex(p);

			n -= 2;
			i++;
		}
		else if ( blisop == 2 )	//����������
		{
			if ( p->Tok[0] == '-'  )
				p->Value = -p->Next->Value;				
			else
				p->Value = p->Next->Value;

			////

			p->Type = p->Next->Type; 
			
			//������� 
			tp = p->Next;	//�������������
			p->Next = tp->Next;
			if ( p->Next ) p->Next->Prior = p;   
			tp->Next = NULL;

			FreeTokenLink(tp);
			RstIndex(p);
			n--;
			i++;
		}

		blisop = 0;
		p = p->Next; 
	}

	return Expire(n);
}

BOOL CCalc::Expire(int n)
{
	LP_S_Token p = lpCurrentTok , tp;

	if ( n == 3 )	//���ʣ��3�˵���ӱ��ʽ�������	//
	{	
		p->Value = p->Next->Value;
		p->Type = p->Next->Type; 

		//�������
		tp = p->Next;		//�������������
		p->Next = tp->Next->Next;
		if ( p->Next ) p->Next->Prior = p;
		tp->Next->Next = NULL;
		
		FreeTokenLink(tp);
		RstIndex(p);
		
		Originate();		//�ݹ�
	}
	else if ( n == 1 )  //�������ʽ�������
	{
		//SUCESS
		return TRUE; 
	}
	else	
	{
		//����
		return FALSE;
	
	}
}




////

BOOL CCalc::Combo_Str(char* m = NULL , ...)
{
	///
	if ( !m ) return FALSE;

    va_list arg_ptr;

	char* sp = m , *sc = NULL;

    va_start( arg_ptr, m );

    while( *( sc = va_arg( arg_ptr, char* ) ) != '\0' )	//ȡ�ø����������ж��Ƿ���EOF
    {
		::strcpy(sp , sc);

		sp += strlen(sc);
    }

    va_end( arg_ptr );

	return TRUE;
}
