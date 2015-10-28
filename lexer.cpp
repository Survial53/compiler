#include <ctype.h>
#include <algorithm>

#include "lexer.h"

const string tokens[RES_W] = {"Begin", "End", "For", "To", "Downto", "Do", "If", "Then", "Else", "Div", "Mod", "And", "Or", "Not", "Int", "Float", "String"};

const string TOKEN_CLASS_NAME[9] = {"ttResWord", 
									"ttID", 
									"ttLogOpr",
									"ttArifOpr",
									"ttConst", 
									"ttString", 
									"ttDevider", 
									"ttAS_OP", 
									"ttUnknown"
};

//metods of CToken
CToken::CToken(void): id(LEX_UNK) {};

CToken::CToken(const CToken &other): id(other.id), text(other.text) {};

CToken::CToken(TOKENS type, const string &text): id(type), text(text) {};

//metods of CLexer
void CLexer::PushABC (int l_class, string str) {
	for (size_t i = 0; i < str.size(); i++) {
		alp[str[i]] = l_class;
	}
}

void CLexer::Init() {

		for( int i = 0; i < ALP; i++)
		{
			alp[i] = ttUnknown;
		}
		PushABC(1, "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		PushABC(2, "<>=");
		PushABC(3, "+-*/");
		PushABC(4, "0123456789");
		PushABC(5, "\"");
		PushABC(6, " ;.)(\n");
		PushABC(7, ":");

		for( int i = 0; i < RES_W; ++i)
		{
			ResWord.insert(pair<string, int>(tokens[i], i));
		}

		LexPrint.insert(pair<TOKENS, string>(EQ,     "="));
		LexPrint.insert(pair<TOKENS, string>(NEQ,   "<>"));
		LexPrint.insert(pair<TOKENS, string>(LST,    "<"));
		LexPrint.insert(pair<TOKENS, string>(MRT,    ">"));
		LexPrint.insert(pair<TOKENS, string>(LSOEQ, "<="));
		LexPrint.insert(pair<TOKENS, string>(ADD,    "+"));
		LexPrint.insert(pair<TOKENS, string>(MUL,    "*"));
		LexPrint.insert(pair<TOKENS, string>(SUB,    "-"));
		LexPrint.insert(pair<TOKENS, string>(DEL,    "/"));
		LexPrint.insert(pair<TOKENS, string>(SCOLON, ";"));
		LexPrint.insert(pair<TOKENS, string>(LBRACK, "("));
		LexPrint.insert(pair<TOKENS, string>(RBRACK, ")"));
		LexPrint.insert(pair<TOKENS, string>(PNT,    "."));
		LexPrint.insert(pair<TOKENS, string>(LEX_ASSIG, ":="));
	}

int CLexer::DefineTokenClass(ifstream &f) const
{
	return alp[f.peek()];
}

TOKENS CLexer::is_resw(string str) 
{
	RW::iterator it = ResWord.find(str);
	if (ResWord.end() != it) {
		return (TOKENS)it->second;
	} else {
		return LEX_ID;
	}
}

CToken CLexer::Skan_ID(ifstream &f)
{
	string lexeme;
	char ch;
	int cls_num;
	TOKENS tokType = LEX_ID;
	while(1) 
	{
		ch = f.get();
		cls_num = get_class_num(ch);
		if ((cls_num == ttID) || ((cls_num == ttConst) && (ch != '.'))) {
			lexeme += ch;
		} else if (ch == '$') {
			lexeme += ch;
			break;
		} else {
			f.putback(ch);
			tokType = is_resw(lexeme);
			return CToken(tokType, lexeme);
			break;
		}
	};
	return CToken(tokType, lexeme);
}


CToken CLexer::SkanString(ifstream &f)
{
	static int table[3][4] = {
	  /* "  \ any eof*/
		{1, 4, 4, 4}, /* def_st */
		{3, 2, 1, 4}, /*  st_"     */
		{1, 1, 1, 4}, /*  st_\     */
	};
	char ch = f.peek();
	TOKENS tokType = LEX_STR;
	string lexeme;
	int st = 0;
	do
	{
		ch = f.get();
		switch(ch) {
		case '"':
			st = table[st][0];
			break;
		case '\\': 
			st = table[st][1];
			break;
		case EOF:
			st = table[st][3];
			break;
		default:
			st = table[st][2];
			break;
		}

		if (st == 4) {
			tokType = LEX_UNK;
		} else {
			lexeme += ch;
		}
	} while((st != 3) && (st != 4));
	return CToken(tokType, lexeme);
}

CToken CLexer::Skan_Int_or_Float(ifstream &f)
{
	static int table[8][5] = {
	 /*num  .  e   +- any eof */
		{1, 9, 9, 9,  9}, /* 0 st_start */
		{1, 2, 8, 8,  8}, /* 1 st_num */
		{4, 8, 5, 8,  8}, /* 2 st_num. */
		{4, 9, 9, 9,  9}, /* 3 st_. */
		{4, 8, 5, 8,  8}, /* 4 st_.num */
		{7, 9, 9, 6,  9}, /* 5 st_e */
		{7, 9, 9, 9,  9}, /* 6 st_e+- */
		{7, 8, 8, 8,  8}, /* 7 st_e_num */
	};

	TOKENS tokType = LEX_INT;
	char ch;
	int cls_num;
	string lexeme;
	int st = 0;
	do
	{
		ch = f.get();
		cls_num = get_class_num(ch);
		if (((cls_num == ttConst) && (ch != '.'))) {
			st = table[st][0];
		} else if (ch == '.') {
			st = table[st][1];
		} else if (ch == 'e' || ch == 'E') {
			st = table[st][2];
		} else if (ch == '+' || ch == '-') {
			st = table[st][3];
		} else {
			st = table[st][4];
		}
		if (st == 2 || st == 3) {
			tokType = LEX_FLOAT;
			lexeme += ch;
		} else if (st == 8) {
			f.putback(ch);
		} else if (st == 9) {
			tokType = LEX_UNK;
			f.putback(ch);
		} else {
			lexeme += ch;
		}
	} while((st != 8) && (st != 9));
	return CToken(tokType, lexeme);
}

CToken CLexer::Skan_Devider(ifstream &f)
{
	char ch = f.get();
	string lexeme;
	TOKENS tokType = get_LexID(ch);
	lexeme += ch;
	return CToken(tokType, "");
}

CToken CLexer::Skan_A_OP(ifstream &f)
{
	char ch = f.get();
	string lexeme;
	TOKENS tokType = get_LexID(ch);
	lexeme += ch; 
	return CToken(tokType, "");
}

CToken CLexer::Skan_L_OP(ifstream &f)
{
	string lexeme;
	char ch = f.get();
	TOKENS tokType = get_LexID(ch);
	lexeme += ch;
	if (ch == '<')
	{
		switch(f.peek()) {
		case '>':
			lexeme += f.get();
			tokType = NEQ;
			break;
		case '=':
			lexeme += f.get();
			tokType = LSOEQ;
			break;
		}
	}
	return CToken(tokType, "");
}

CToken CLexer::Skan_Assigment(ifstream &f)
{
	string lexeme;
	char ch = f.get();
	lexeme += ch;
	TOKENS tokType = get_LexID(ch);
	if (ch == ':') {
		if (f.peek() == '=') {
			lexeme += f.get();
			tokType = LEX_ASSIG;
		} else {
			tokType = LEX_UNK;
		}
	}
	return CToken(tokType, "");
}

CToken CLexer::Skan_Unknown(ifstream &f)
{
	string lexeme;
	lexeme += f.get();
	return CToken(LEX_UNK, "");
}

void CLexer::SaveTokens(ostream &os) {
	if (os.bad()) {
		cout<<"CLexer::SaveTokens error"<<endl;
	}
	string lexeme;
	for (auto it = tokensBuffer.begin(); it != tokensBuffer.end(); ++it) {
		RW1::iterator it1 = LexPrint.find((*it).Id());
		if (((*it).Text()).compare("") == 0) {
			lexeme = it1->second;
		} else {
			lexeme = (*it).Text();
		}
		os<<"ID: "<<left<<setw(4)<<(*it).Id()<<" Lex_Class: "<<left<<setw(10)<<TOKEN_CLASS_NAME[(*it).Id()/1000]<<" Lexeme: "<<left<<lexeme/*(*it).Text()*/<<endl;
	}
}

CLexer::ptrFun CLexer::methods[HAND_SZ] = {
									 &CLexer::Skan_ID, 
									 &Skan_L_OP,
 									 &Skan_A_OP, 
									 &Skan_Int_or_Float,
									 &SkanString, 
									 &Skan_Devider, 
									 &Skan_Assigment,
									 &Skan_Unknown
};

CToken CLexer::SkanToken(ifstream &f)
{
	return (this->*methods[DefineTokenClass(f)-1])(f);
}

bool CLexer::Lex (ifstream &f)
{
	CToken token;
	char ch;
	while((ch = f.peek()) != EOF) 
	{
		CToken token  = SkanToken(f);
		if (token.Id() == LEX_UNK) {
			unknownLex.push_back(token);
		}else if ((token.Id() != SPACE) && ((token.Id()) != EOL)) {
			tokensBuffer.push_back(token);
		}
	}
	return true;
}

int CLexer::get_class_num(char ch) {
		return alp[ch];
}

TOKENS CLexer::get_LexID(char ch)
{
	return TOKENS((get_class_num(ch) * 1000) + ch);
}
