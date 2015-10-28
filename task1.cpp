#include <lexer.h>

int main()
{
	CLexer Lexer;
	Lexer.Init();
	ifstream inputFile;
	inputFile.open("prog.txt");
	if (!inputFile.is_open()) {
		cout << "Error opening file" << endl;
		return 0;
	}
	Lexer.Lex(inputFile);
	Lexer.SaveTokens(cout);
	inputFile.close();
	return 0;
}