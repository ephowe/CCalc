// Calc.h: interface for the CCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_)
#define AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////
//本程序对于表达式的描扫分成单独的三次.即:
//1.首轮扫描表达式是否存在非法字符.成功继续.
//2.(1)通过后,接着扫描表达的操作符与括号的排列方式是否合法.成功继续.
//3.开始计算并检测数值大小与除数是否为0.
//三步扫描互为独立,即其中一步出错后,立即中断扫描并显示ERRMSG,不再进去下去.因此之后的错误不会被检查.



//////////////////////////////////////////////////////////////////////
#define	NUMBER		1		//运算符
#define OPERANDS	2		//数字
#define LBRACKET	4		//左括号
#define RBRACKET	8		//右括号
#define VARIABLE	16		//变量		//暂时未用到

//项结构
typedef struct tag_S_Token
{
	char Tok[18];			//保存一个项
	char Type;				//项的类型
	int	 Idx;				//项的序号
	double Value;			//保存计算后的数值
	tag_S_Token *Prior;		//指向前
	tag_S_Token *Next;		//指向后
}
S_Token , *LP_S_Token;

//错误信息结构
typedef struct tag_S_Err
{
	char	*lpMsg;		//指向出错信息
	char	Cod;		//保存非法字符
	int		Col;		//出错列号
	tag_S_Err *Next;	//指向下一个错误结构
} 
S_Err , *LP_S_Err;

class CCalc  
{
private:
		
	LP_S_Token lpFirstTok , lpCurrentTok;	//项链指针
	LP_S_Err lpFirstErr , lpCurrentErr;	//错误链指针

	char *ErrMsg[20];				//存放错误信息		
	char *coder;					//指向表达式

	BOOL Is_Bra(char bra);		//是括号?
	BOOL Is_Op(char op);		//是运算符?
	BOOL Is_Num(char num);		//是数字?

	int SetTokenLink(char *mcode);			//建立项TOKEN链,建立成功 RET 项的数目,取遇错误 RET -1 ,并SETERROR
	int GetNumber(char *dst , char *src);	//取字符串中的数值,RET 数值LEN - 1

	BOOL AddToken(S_Token& tok);	//添加项
	BOOL FreeTokenLink(LP_S_Token lpTok);	//释放TOK链表

	void LoadErrorInfo();			//加载错误信息
	BOOL FreeError();				//消毁信息链释放内存
	BOOL SetError(S_Err& iErr);		//设置错误信息链

	BOOL SetValue(LP_S_Token p);			//设置项的值
	BOOL RstIndex(LP_S_Token lpTokStt);		//重设项的序号
	int GetBracket();				//取距离最近的括号内的子表达(lpCurrentTok为此子表达式的头一项)式与之长度(包括两个括号)
	double Calcer(char op , double m1 , double m2);		//计算解析后的数据

	BOOL Originate();				//起始
	BOOL Level1(int n);				//计算表达式中的次方项
	BOOL Level2(int n);				//计算表达式中的乘 除 取余
	BOOL Level3(int n);				//计算表达式中的加减
	BOOL Expire(int n);				//结止表达式

	//
	//字符串合并 ; 将任意多的字符串合并,参数以字符型变量&eof='\0'结尾,
	//首个参数为用户提供的BUF,其后的字符串都被COPY到BUF中.
	BOOL Combo_Str(char* , ...);	


public:

	CCalc();						//构造
	virtual ~CCalc();				//析构

	BOOL Scan(char* mcode);		//初步扫描表达式，出错后由GetError()取得信息
	BOOL ScanEx(char *mcode);	//深入扫描表达式，出错后由GetError()取得信息

	int DeCoder();				//解析表达式
	
	BOOL GetError();				//取得错误信息

	double mValue;				//结果
	char *mMsg[256];			//存放信息
	char mTo;					//上面MSG的序号

protected:

};

#endif // !defined(AFX_CALC_H__8B6BC266_F0C5_453C_AFF0_91497741660B__INCLUDED_)
