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
	this->LoadErrorInfo();		//错误信息

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

	//::printf("\n表达式: %s \n",coder);

	lpCurrentErr = lpFirstErr;

	while (lpCurrentErr)
	{
		//
		mMsg[mTo] = new char[128];

		switch (lpCurrentErr->Cod)
		{
		case '(': 
			//::printf("error: %c %s -> 第 %d 个括号 \n",lpCurrentErr->Cod,lpCurrentErr->lpMsg,lpCurrentErr->Col);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: ") , 
						codBuf , TEXT(" ") , lpCurrentErr->lpMsg , 
						TEXT(" -> 第 ") ,itoa(lpCurrentErr->Col ,colBuf , 10) , 
						TEXT(" 个括号\r\n"), &eof);
			
			if ( mTo < 256 ) mTo++;

			break;
		case ')':
			//::printf("error: %c %s -> 第 %d 个括号 \n",lpCurrentErr->Cod,lpCurrentErr->lpMsg,lpCurrentErr->Col);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: ") ,
						codBuf , TEXT(" ") ,
						lpCurrentErr->lpMsg , TEXT(" -> 第 ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , 
						TEXT(" 个括号\r\n"), &eof);
			if ( mTo < 256 ) mTo++;

			break;
		case '\0':
			//::printf("error: 第 %d 项 -> %s \n",lpCurrentErr->Col,lpCurrentErr->lpMsg);
			Combo_Str( mMsg[mTo] , TEXT("error: 第 ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" 项 -> ") ,
						lpCurrentErr->lpMsg , TEXT("\r\n") , &eof);
			if ( mTo < 256 ) mTo++;
			break;
		case 'N':
			//::printf("error: 第 %d 列 -> %s \n",lpCurrentErr->Col,lpCurrentErr->lpMsg);
			Combo_Str( mMsg[mTo] , TEXT("error: 第 ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" 列 -> ") ,
						lpCurrentErr->lpMsg , TEXT("\r\n") , &eof);
			if ( mTo < 256 ) mTo++;
			break;		
		default:
			//::printf("error: 第 %d 列 -> %s  %c \n",lpCurrentErr->Col,lpCurrentErr->lpMsg,lpCurrentErr->Cod);
			codBuf[0] = lpCurrentErr->Cod;

			Combo_Str( mMsg[mTo] , TEXT("error: 第 ") ,
						itoa(lpCurrentErr->Col , colBuf , 10) , TEXT(" 列 -> ") ,
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
	S_Err ErrStt;		//错误信息结构
	int blkh = 0;		//检测括号配对
	int rbidx = 1; //, lbidx = 1;		//括号序列
	int col = 0;		//获取列号
	BOOL blret = TRUE;	//
	char *p = mcode;	//保存地址副本

	if (!mcode) return (!blret);	//判断是否存在表达式

	coder = mcode;					//保存指针副本

	//扫描整个表达式
	while( *mcode )
	{
		col++;
		if ( *mcode == '(' )		//寻找左括号
		{
			blkh++;
		}
		else if ( *mcode == ')' )	//配对右括号
		{
			if ( !blkh )
			{
				ErrStt.lpMsg = ErrMsg[1];
				ErrStt.Cod = *mcode;
				ErrStt.Col = rbidx;	//
				SetError(ErrStt);		//设置错误信息
				if (blret) blret = !blret;
			}
			else
			{
				blkh--;
			}
			rbidx++;
		}
		else if ( !Is_Num(*mcode) && !Is_Op(*mcode) && *mcode != ' ' && *mcode != '.' )		//是否是非法字符
		{
			ErrStt.lpMsg = ErrMsg[0];
			ErrStt.Cod = *mcode;
			ErrStt.Col = col;
			SetError(ErrStt);		//设置错误信息
			if (blret) blret = !blret;
		}
		mcode++;
	}

	//扫描完毕后,设置关于括号配对的错误信息
	//未完善，需修改
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
			SetError(ErrStt);		//设置错误信息
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

	mValue = 0.0;	//归0
	
	if ( Originate() )	//开始解析
	{
		//验证结果的大小
		if ( lpFirstTok->Value <= 9223372036854775808 )
		{
			//printf("\n表达式: %s \nscan pass! \n",coder);
			
			//printf("= %f \n",lpFirstTok->Value); //显示结果

			mValue = lpFirstTok->Value;			//把计算结果赋给MVALUE

			FreeError();

			mret = 1;	//Success
		}
		else
		{
			//数值过大
		}
	}
	else
	{
		GetError();
	}

	FreeTokenLink(NULL);	//释放链

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
	ErrMsg[0] =	"存在非法字符!";
	ErrMsg[1] =	"括号未配对!";
	ErrMsg[2] =	"数据长度超出限制!";
	ErrMsg[3] =	"运算符与运算符间不存在数据!";
	ErrMsg[4] = "数据与数据间存在空格,是否是误操作!";
	ErrMsg[5] = "非法的小数点!";
	ErrMsg[6] = "表达式的项不能少于3个!";
	ErrMsg[7] = "数值不能与(括号连接!";
	ErrMsg[8] = "运算符不能与)括号连接!";
	ErrMsg[9] = ")括号与(括号间无运算符!";
	ErrMsg[10] = "(括号与)括号间无项,是否是误操作!";
	ErrMsg[11] = ")括号不能数值与连接!";
	ErrMsg[12] = "(括号不能与运算符连接,正负号运算符是一个例外!";
	ErrMsg[13] = "表达式最后一项不能为运算符或(括号!";
	ErrMsg[14] = "表达式第一项不能为运算符!";
	ErrMsg[15] = "表示正负的括号内的数值超过了1个!";
	ErrMsg[16] = "除数不能为0!";
	ErrMsg[17] = "正负数的嵌套已经超出最大限制的层数!";

}

BOOL CCalc::ScanEx(char *mcode)
{
	LP_S_Token lpTokStt;		//项信息结构
	S_Err mErr;					//ERROR_MSG		
	int toknum;					//项的数目
	BOOL blret = TRUE;			//返回值

	///////
	//已修正层负数子表达式嵌套的问题
#define MAXTF	128				//最多嵌套的层数	

	int tfop[MAXTF] = {0};			//判断正负符号,并检测该括号内数值个数
	int lgkh[MAXTF] = {0};			//判断(-(x-y)) or (-((x-y)-z)) or (-((x-(-(y-e))-z))
	int idx = 0;				//数组序号

	tfop[idx] = lgkh[idx] = 0;	//第一个元素为空项 初始化为0
	///////

	if ( ( toknum = SetTokenLink(mcode) ) != -1 )
	{
		if ( toknum < 3 )		//项的数目不能少于3项
		{
			//ERROR
			mErr.lpMsg = ErrMsg[6];
			mErr.Cod = '\0';
			mErr.Col = 0;
			SetError(mErr);

			//处理错误 释放链
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
					//如果遇到右括号表示正负数的判断已结束
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//打上标记
						//返回上一层并递减记号
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

					//如果遇到右括号表示正负数的判断已结束
					//有错误
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//打上标记
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
					//需考虑正负数表示
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
							tfop[++idx] = 1;		//开始判断并打上标记
							lgkh[idx] = 0;
						}
						else if ( idx < 0 )			//数组序号小于0
						{
							idx = 1;
						}
						else						//序号大于MAXTF
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

					//如果遇到右括号表示正负数的判断已结束
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//打上标记
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
					//如果遇到右括号表示正负数的判断已结束
					if ( lgkh[idx] ) lgkh[idx]--;
					else 
					{
						tfop[idx--] = 0;	//打上标记
						if ( lgkh[idx] ) lgkh[idx]--;
					}
					break;
				}
				break;				
			}
			lpTokStt = lpTokStt->Next; 		
		}
		//检测最后一项
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
	else blret = FALSE;	//有错误
	
	if ( !blret ) FreeTokenLink(NULL);	//错误处理

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
	char *Operand = "^*/%+-";		//存放运算符
	
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
	int i = 0;	//下标检测
	int dn = 1;	//记录小数点的数量
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
			i--;		//避免使下面带*号的语句造成下标过界
		}

		if ( Is_Num(*p) )
		{
			dst[i++] = *p++;	//*-------	
		}
		else if ( *p == '.' )
		{
			if ( Is_Num(p[1]) && dn < 2 ) //小数点数量不能大于2
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
			p += GetNumber(TokStt.Tok,p);	//加上数字的长度-1
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
		else if ( *p == '.' )		//发现非法的小数点
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

BOOL CCalc::FreeTokenLink(LP_S_Token lpTok)	//参数为空删除整条链
{
	LP_S_Token p;
	
	if (!lpFirstTok) return FALSE;
	
	if ( lpTok )
	{
		//参数不为空则删除子链
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
	//有错误
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
			
			//链表操作
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;	//为待删除链打结尾符
			tp = p;		//保存废弃链副本
			p = p->Prior;

			FreeTokenLink(tp);
			RstIndex(p);

			n -= 2;
			i++;
			
		}
		p = p->Next; 
	}

	if ( n == 1 )	//少于等于3项就直接跳结尾
		return Expire(n);
	else
		return Level2(n);	//开始解析*/%
}

BOOL CCalc::Level2(int n)
{
	LP_S_Token p = lpCurrentTok , tp;
	S_Err mErr;					//ERROR_MSG
	BOOL blisop = FALSE;	//判断是否是*/%
	int m = n;

	for ( int i = 0 ; i < m ; i++ )
	{
		switch ( p->Tok[0] )
		{
		case '*':
			blisop = TRUE;
			break;
		case '/':
			//检测除数是否为0
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
			
			//链表操作
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;	//为待删除链打结尾符
			tp = p;		//保存废弃链副本
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
		return Level3(n);	//开始解析+-
}

BOOL CCalc::Level3(int n)
{
	LP_S_Token p = lpCurrentTok , tp;
	int blisop = 0;	//判断是否是+- = 1,正负 = 2 ELSE = 0
	int m = n;

	for ( int i = 0 ; i < m ; i++ )
	{
		switch ( p->Tok[0] )
		{
		case '+':
			//检测是否为正号 还是 加号
			if ( p->Prior->Type != LBRACKET ) blisop = 1;
			else blisop = 2;
			break;
		case '-':
			//检测是否为负号 还是 减号
			if ( p->Prior->Type != LBRACKET ) blisop = 1;
			else blisop = 2;
			break;
		}

		if ( blisop == 1 )	//做加减运算
		{	
			p->Prior->Value = Calcer(p->Tok[0],p->Prior->Value,p->Next->Value);    
			
			//链表操作
			tp = p->Prior->Next = p->Next->Next;
			if ( tp ) tp->Prior = p->Prior;   
			p->Next->Next = NULL;
			tp = p;		//保存废弃链副本
			p = p->Prior;

			FreeTokenLink(tp);
			RstIndex(p);

			n -= 2;
			i++;
		}
		else if ( blisop == 2 )	//做负数运算
		{
			if ( p->Tok[0] == '-'  )
				p->Value = -p->Next->Value;				
			else
				p->Value = p->Next->Value;

			////

			p->Type = p->Next->Type; 
			
			//链表操作 
			tp = p->Next;	//保存废弃链副本
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

	if ( n == 3 )	//如果剩于3项，说明子表达式解析完毕	//
	{	
		p->Value = p->Next->Value;
		p->Type = p->Next->Type; 

		//链表操作
		tp = p->Next;		//保存废弃链副本
		p->Next = tp->Next->Next;
		if ( p->Next ) p->Next->Prior = p;
		tp->Next->Next = NULL;
		
		FreeTokenLink(tp);
		RstIndex(p);
		
		Originate();		//递归
	}
	else if ( n == 1 )  //整个表达式解析完毕
	{
		//SUCESS
		return TRUE; 
	}
	else	
	{
		//出错
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

    while( *( sc = va_arg( arg_ptr, char* ) ) != '\0' )	//取得各个参数并判断是否到了EOF
    {
		::strcpy(sp , sc);

		sp += strlen(sc);
    }

    va_end( arg_ptr );

	return TRUE;
}
