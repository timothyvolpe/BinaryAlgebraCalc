#pragma once
#include <string>
#include <vector>
#include <map>

enum
{
	PARSE_ERROR_UNKNOWN,
	PARSE_ERROR_OK,
	PARSE_ERROR_INVALID_LITERAL,
	PARSE_ERROR_SYNTAX,
	PARSE_ERROR_LENGTH,
	PARSE_ERROR_INPUT
};

enum : unsigned char
{
	LITERAL_TYPE_INVALID,
	LITERAL_TYPE_ALPHA,
	LITERAL_TYPE_NUMERIC,
	LITERAL_TYPE_OPERATOR,
	LITERAL_TYPE_PAREN,
	LITERAL_TYPE_NOT
};

enum :unsigned char
{
	TOKEN_TYPE_END,
	TOKEN_TYPE_LITERAL,
	TOKEN_TYPE_AND,
	TOKEN_TYPE_OR,
	TOKEN_TYPE_XOR,
	TOKEN_TYPE_LEFT_PAREN,
	TOKEN_TYPE_RIGHT_PAREN,
	TOKEN_TYPE_TERMNOT
};

struct EquationToken
{
	char token;
	unsigned char tokenType;
	bool negated;

	inline bool operator==( const EquationToken& rhs ) {
		return (this->token == rhs.token);
	}
};

class CEquationParser
{
private:
	std::string m_cleanEq;
	std::vector<EquationToken> m_tokens;
	std::vector<char> m_uniqueVariables;

	std::map<char, bool> m_variables;
	std::vector<EquationToken>::iterator m_evaluationIterator;

	EquationToken evalPeek();
	EquationToken evalGet();

	bool evalLiteral();
	bool evalFactor();
	bool evalTerm();
	bool evalExpression();
public:
	static unsigned char getLiteralType( char ch );
	static bool sanitizeInput( std::string inputStr );

	CEquationParser();
	~CEquationParser();

	bool parse( std::string eq, int *pError );
	bool evaluate( std::string input, bool *pResult, int *pError );

	inline const std::string& getCleanEquation() { return m_cleanEq; }
	inline const std::vector<char>& getUniqueVariables() { return m_uniqueVariables; }
	inline int getUniqueVariableCount() { return m_uniqueVariables.size(); }
	inline int getMaxInputs() { return (int)pow( 2, m_uniqueVariables.size() ); }
};
