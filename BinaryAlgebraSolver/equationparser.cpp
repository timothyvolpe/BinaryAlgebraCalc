#include <algorithm>
#include <ctype.h>
#include "equationparser.h"

unsigned char CEquationParser::getLiteralType( char ch )
{
	// Check if its alphabetic or numeric
	if( isalpha( ch ) )
		return LITERAL_TYPE_ALPHA;
	else if( isdigit( ch ) ) {
		if( ch == '0' || ch == '1' )
			return LITERAL_TYPE_NUMERIC;
		else
			return LITERAL_TYPE_INVALID;
	}
	// Check if its an operator
	else
	{
		switch( ch ) {
		case '+':
		case '*':
		case '^':
			return LITERAL_TYPE_OPERATOR;
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
			return LITERAL_TYPE_PAREN;
		case '\'':
			return LITERAL_TYPE_NOT;
		default:
			return LITERAL_TYPE_INVALID;
		}
	}
}

bool CEquationParser::sanitizeInput( std::string inputStr )
{
	for( auto it = inputStr.begin(); it != inputStr.end(); it++ )
	{
		switch( (*it) )
		{
		case '0':
		case '1':
			continue;
		default:
			return false;
		}
	}
	return true;
}

CEquationParser::CEquationParser() {
}
CEquationParser::~CEquationParser() {
}

bool CEquationParser::parse( std::string eq, int *pError )
{
	unsigned char literalType;

	if( pError )
		*pError = PARSE_ERROR_OK;

	// Make sure its length isnt 0
	if( eq.length() == 0 ) {
		*pError = PARSE_ERROR_LENGTH;
		return false;
	}

	// Clean up the input
	// Remove all spaces
	eq.erase( std::remove_if( eq.begin(), eq.end(), isspace ), eq.end() );
	// Make it all uppercase
	std::transform( eq.begin(), eq.end(), eq.begin(), ::toupper );
	// Check each character and make sure its valid
	for( unsigned int i = 0; i < eq.length(); i++ )
	{
		literalType = CEquationParser::getLiteralType( eq[i] );
		if( literalType == LITERAL_TYPE_INVALID ) {
			if( pError )
				*pError = PARSE_ERROR_INVALID_LITERAL;
			return false;
		}
	}

	// Now run through and insert the "assumed" operators, i.e. two adjacent literals = AND
	for( unsigned int i = 0; i < eq.length() - 1; i++ )
	{
		// Check if its a literal
		literalType = CEquationParser::getLiteralType( eq[i] );
		if( literalType == LITERAL_TYPE_ALPHA || literalType == LITERAL_TYPE_NUMERIC || literalType == LITERAL_TYPE_PAREN ) {
			// Then if the adjecent token is also a literal then we need to add an AND
			literalType = CEquationParser::getLiteralType( eq[i+1] );
			if( literalType == LITERAL_TYPE_ALPHA || literalType == LITERAL_TYPE_NUMERIC || literalType == LITERAL_TYPE_PAREN ) {
				eq.insert( i+1, 1, '*' );
				i++;
			}
			if( literalType == LITERAL_TYPE_NOT ) {
				// We have to check the NEXT character
				if( i < eq.length() - 2 ) {
					literalType = CEquationParser::getLiteralType( eq[i + 2] );
					if( literalType == LITERAL_TYPE_ALPHA || literalType == LITERAL_TYPE_NUMERIC || literalType == LITERAL_TYPE_PAREN ) {
						eq.insert( i + 2, 1, '*' );
						i += 2;
					}
				}
			}
		}
	}

	// Now we need to split the string into tokens
	m_tokens.clear();
	m_uniqueVariables.clear();
	for( unsigned int i = 0; i < eq.length(); i++ )
	{
		// Check if its a literal or an operator
		literalType = CEquationParser::getLiteralType( eq[i] );
		if( literalType == LITERAL_TYPE_ALPHA )
		{
			EquationToken token;
			token.token = eq[i];
			token.tokenType = TOKEN_TYPE_LITERAL;
			// Check if its negated
			token.negated = false;
			if( i != eq.length() - 1 ) {
				if( CEquationParser::getLiteralType( eq[i + 1] ) == LITERAL_TYPE_NOT ) {
					token.negated = true;
					i++;
				}
			}
			m_tokens.push_back( token );
			// Check if its a new unique variable
			if( std::find_if( m_uniqueVariables.begin(), m_uniqueVariables.end(), [token]( const char f )->bool { 
					return f == token.token;
				} ) == m_uniqueVariables.end() ) {
				m_uniqueVariables.push_back( token.token );
			}
		}
		if( literalType == LITERAL_TYPE_NUMERIC )
		{
			EquationToken token;
			token.token = eq[i];
			token.tokenType = TOKEN_TYPE_LITERAL;
			// Check if its negated
			token.negated = false;
			if( i != eq.length() - 1 ) {
				if( CEquationParser::getLiteralType( eq[i + 1] ) == LITERAL_TYPE_NOT ) {
					token.negated = true;
					i++;
				}
			}
			m_tokens.push_back( token );
		}
		if( literalType == LITERAL_TYPE_OPERATOR || literalType == LITERAL_TYPE_PAREN || literalType == LITERAL_TYPE_NOT )
		{
			EquationToken token;
			token.token = eq[i];
			token.negated = false;

			if( literalType == LITERAL_TYPE_NOT ) {
				// If the previous isn't a parenthesis, ignore it
				if( m_tokens.size() > 0 ) {
					if( m_tokens[m_tokens.size()-1].tokenType != TOKEN_TYPE_RIGHT_PAREN )
						continue;
				}
				else
					continue;
			}

			switch( token.token )
			{
			case '(':
			case '[':
			case '{':
				token.tokenType = TOKEN_TYPE_LEFT_PAREN;
				break;
			case ')':
			case ']':
			case '}':
				token.tokenType = TOKEN_TYPE_RIGHT_PAREN;
				break;
			case '+':
				token.tokenType = TOKEN_TYPE_OR;
				break;
			case '*':
				token.tokenType = TOKEN_TYPE_AND;
				break;
			case '^':
				token.tokenType = TOKEN_TYPE_XOR;
				break;
			case '\'':
				token.tokenType = TOKEN_TYPE_TERMNOT;
				break;
			default:
				token.tokenType = TOKEN_TYPE_LITERAL;
				break;
			}
			m_tokens.push_back( token );
		}
	}

	EquationToken endToken;
	endToken.tokenType = TOKEN_TYPE_END;
	endToken.token = ' ';
	m_tokens.push_back( endToken );

	// Organize unique vars by alphabetical
	std::sort( m_uniqueVariables.begin(), m_uniqueVariables.end() );

	m_cleanEq = eq;

	return true;
}

EquationToken CEquationParser::evalPeek() {
	return (*m_evaluationIterator);
}
EquationToken CEquationParser::evalGet() {
	//printf( "Advancing from %c to", (*m_evaluationIterator).token );
	//printf( " %c\n", (*(m_evaluationIterator+1)).token );
	return (*m_evaluationIterator++);
}

bool CEquationParser::evalLiteral()
{
	if( this->evalPeek().negated )
		return !(m_variables[this->evalGet().token]);
	return (m_variables[this->evalGet().token]);
}
bool CEquationParser::evalFactor()
{
	if( this->evalPeek().tokenType == TOKEN_TYPE_LITERAL )
		return this->evalLiteral();
	else if( this->evalPeek().tokenType == TOKEN_TYPE_LEFT_PAREN ) {
		this->evalGet();
		bool result = this->evalExpression();
		this->evalGet(); // Syntax error possible here!
		if( this->evalPeek().tokenType == TOKEN_TYPE_TERMNOT ) {
			this->evalGet();
			result = !result;
		}
		return result;
	}
	return true;
}
bool CEquationParser::evalTerm()
{
	bool result = this->evalFactor();
	if( this->evalPeek().tokenType == TOKEN_TYPE_END )
		return result;
	while( this->evalPeek().tokenType == TOKEN_TYPE_AND ) {
		if( this->evalGet().tokenType == TOKEN_TYPE_AND ) {
			bool partOfBool = this->evalFactor();
			result = (result && partOfBool);
		}
	}
	return result;
}
bool CEquationParser::evalExpression()
{
	bool result = this->evalTerm();
	if( this->evalPeek().tokenType == TOKEN_TYPE_END )
		return result;
	while( this->evalPeek().tokenType == TOKEN_TYPE_OR || this->evalPeek().tokenType == TOKEN_TYPE_XOR )
	{
		if( this->evalPeek().tokenType == TOKEN_TYPE_OR ) {
			this->evalGet();
			bool partOfBool = this->evalTerm();
			result = (result || partOfBool);
		}
		else if( this->evalPeek().tokenType == TOKEN_TYPE_XOR ) {
			this->evalGet();
			bool partOfBool = this->evalTerm();
			result = result ^ partOfBool;
		}
	}
	return result;
}

bool CEquationParser::evaluate( std::string input, bool *pResult, int *pError )
{
	if( pError )
		*pError = PARSE_ERROR_OK;

	// Associate the variables with their input
	if( input.length() != m_uniqueVariables.size() ) {
		if( pError )
			*pError = PARSE_ERROR_INPUT;
		return false;
	}
	m_variables.clear();
	for( unsigned int i = 0; i < m_uniqueVariables.size(); i++ )
		m_variables.insert( std::pair<char, bool>( m_uniqueVariables[i], (input[i] == '0' ? false : true) ) );
	m_variables.insert( std::pair<char, bool>( '0', false ) );
	m_variables.insert( std::pair<char, bool>( '1', true ) );

	// Begin recursive descent
	m_evaluationIterator = m_tokens.begin();
	bool result = this->evalExpression();
	*pResult = result;

	return true;
}