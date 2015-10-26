#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <iomanip>
using namespace std;

#define RES_W 17
#define ALP 256
#define MAS_SZ 33
#define HAND_SZ 8

enum E_TOKEN_CLASS {
	ttResWord = 0,
	ttID      = 1,
	ttLogOpr  = 2,
	ttArifOpr = 3,
	ttConst   = 4,
	ttString  = 5,
	ttDevider = 6,
	ttAS_OP   = 7,
	ttUnknown = 8
};


enum TOKENS {
	BEGIN  = 0,
	END    = 1,
	FOR    = 2,
	TO     = 3,
	DOWNTO = 4,
	DO     = 5,
	IF     = 6,
	THEN   = 7,
	ELSE   = 8,
	DIV    = 9,
	MOD    = 10,
	AND    = 11,
	OR     = 12,
	NOT    = 13,
	INT    = 14,
	FLOAT  = 15,
	STRING = 16,

	LEX_ID    = 1000,

	EQ    = 2000 + '=',
	NEQ   = 2000 + '<' + '>',
	LST   = 2000 + '<',
	MRT   = 2000 + '>',
	LSOEQ = 2000 + '<' + '=',

	MUL   = 3000 + '*',
	ADD   = 3000 + '+',
	SUB   = 3000 + '-',

	LEX_INT   = 4001,
	LEX_FLOAT = 4002,

	LEX_STR   = 5000,

	EOL     = 6000 + '\n',
	SPACE   = 6000 + ' ',
	COLON   = 6000 + ':',
	SCOLON  = 6000 + ';',
	COMMA   = 6000 + ',',
	PNT     = 6000 + '.',
	LBRACK  = 6000 + '(',
	RBRACK  = 6000 + ')',

	LEX_ASSIG = 7000 + ':'+'=',
	LEX_UNK   = 8000
};

class CToken;
class CLexer;
typedef list<CToken> TokensArray;
typedef unordered_map<string, int> RW;

class CToken {
	friend class CLexer;
private:
	TOKENS id;
	string text;
public:
	TOKENS Id(void) const {return id;}
	string Text(void) const {return text;}

	CToken (void);
	CToken (const CToken &);
	CToken (TOKENS, const string &);
};

class CLexer {
private:
	TokensArray tokensBuffer;
	TokensArray unknownLex;
	int alp[ALP];
	RW ResWord;
	typedef CToken (CLexer::*ptrFun)(ifstream &);
	static ptrFun methods[HAND_SZ];

	int DefineTokenClass(ifstream &f) const;

	TOKENS is_resw(string str);

	CToken SkanToken(ifstream &f);

	CToken Skan_ID(ifstream &f);

	CToken SkanString(ifstream &f);

	CToken Skan_Int_or_Float(ifstream &f);

	CToken Skan_Devider(ifstream &f);

	CToken Skan_A_OP(ifstream &f);

	CToken Skan_L_OP(ifstream &f);

	CToken Skan_Assigment(ifstream &f);

	CToken Skan_Unknown(ifstream &f);

	void PushABC (int l_class, string str);

public:

	const TokensArray& GetTokens(void) const {return tokensBuffer;}

	void SaveTokens(ostream &os);

	bool Lex(ifstream &f);

	void Init();

	int get_class_num(char c);

	TOKENS get_LexID(char c);
};

