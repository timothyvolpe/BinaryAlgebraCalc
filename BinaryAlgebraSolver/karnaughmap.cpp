#include "karnaughmap.h"
#include <algorithm>
#include <ctype.h>
#include <map>
#include "util.h"

CKarnaughMap::CKarnaughMap() {
	useGrayCode = true;
}
CKarnaughMap::~CKarnaughMap() {
}

int CKarnaughMap::isValidHeader( std::string header )
{
	int dimension1, dimension2;
	std::vector<char> usedVariables;

	if( header == "" )
		return HEADER_ERROR_EMPTY;

	// Make it all uppercase
	std::transform( header.begin(), header.end(), header.begin(), ::toupper );

	// Confirm each variable in the string
	for( auto it = header.begin(); it != header.end(); it++ )
	{
		// Try to find it in the unique variables, should be found
		if( std::find( m_uniqueVariables.begin(), m_uniqueVariables.end(), (*it) ) == m_uniqueVariables.end() )
			return HEADER_ERROR_INVALID;

		// Make sure we havent already used it!
		if( std::find( usedVariables.begin(), usedVariables.end(), (*it) ) != usedVariables.end() )
			return HEADER_ERROR_REPEAT;
		// Make sure none of them have already been used!
		if( std::find( m_columnVars.begin(), m_columnVars.end(), (*it) ) != m_columnVars.end() )
			return HEADER_ERROR_REPEAT;
		if( std::find( m_rowVars.begin(), m_rowVars.end(), (*it) ) != m_rowVars.end() )
			return HEADER_ERROR_REPEAT;

		usedVariables.push_back( (*it) );
	}

	// Check to make sure it fits the dimensions
	dimension1 = (int)m_uniqueVariables.size() / 2;
	dimension2 = (int)ceil( m_uniqueVariables.size() / 2.0 );
	if( m_columnVars.size() == dimension1 || m_rowVars.size() == dimension1 ) {
		if( header.length() != dimension2 )
			return HEADER_ERROR_DIMMISMATCH;
	}
	if( m_columnVars.size() == dimension2 || m_rowVars.size() == dimension2 ) {
		if( header.length() != dimension1 )
			return HEADER_ERROR_DIMMISMATCH;
	}
	else {
		if( header.length() != dimension1 && header.length() != dimension2 )
			return HEADER_ERROR_DIMMISMATCH;
	}

	return HEADER_OK;
}

bool CKarnaughMap::setColumnVars( std::string columnVars )
{
	if( this->isValidHeader( columnVars ) != HEADER_OK )
		return false;

	// Make it all uppercase
	std::transform( columnVars.begin(), columnVars.end(), columnVars.begin(), ::toupper );

	m_columnVars = std::vector<char>( columnVars.begin(), columnVars.end() );

	return true;
}
bool CKarnaughMap::setRowVars( std::string rowVars )
{
	if( this->isValidHeader( rowVars ) != HEADER_OK )
		return false;

	// Make it all uppercase
	std::transform( rowVars.begin(), rowVars.end(), rowVars.begin(), ::toupper );

	m_rowVars = std::vector<char>( rowVars.begin(), rowVars.end() );

	return true;
}

int CKarnaughMap::getBoxTermNumber( std::string columnVal, std::string rowVal )
{
	std::map<char, bool> inputVariables;
	std::string binaryString;

	// Associate each variable with its value
	for( unsigned int i = 0; i < m_columnVars.size(); i++ ) {
		if( columnVal[i] == '0' )
			inputVariables.insert( std::pair<char, bool>( m_columnVars[i], false ) );
		else
			inputVariables.insert( std::pair<char, bool>( m_columnVars[i], true ) );
	}
	for( unsigned int i = 0; i < m_rowVars.size(); i++ ) {
		if( rowVal[i] == '0' )
			inputVariables.insert( std::pair<char, bool>( m_rowVars[i], false ) );
		else
			inputVariables.insert( std::pair<char, bool>( m_rowVars[i], true ) );
	}

	// Construct binary string in correct order
	binaryString = "";
	for( auto it = inputVariables.begin(); it != inputVariables.end(); it++ ) {
		if( !(*it).second )
			binaryString += '0';
		else
			binaryString += '1';
	}

	return ConvertBinaryToInt( binaryString );
}

void CKarnaughMap::print()
{
	std::string offset, horizontal, boxPadLeft, boxPadRight;
	std::vector<std::string> columnVals, rowVals;
	printf( "\n" );

	// Generate structure elements
	offset.assign( m_rowVars.size()+1, ' ' );
	horizontal.assign( 3+(int)pow( 2, m_columnVars.size() )*(m_columnVars.size() +3), '-' );
	boxPadLeft.assign( ((m_columnVars.size() + 2) / 2)-1, ' ' );
	boxPadRight.assign( (int)ceil((m_columnVars.size() + 2) / 2), ' ' );

	// Use Gray code?
	if( this->useGrayCode ) {
		columnVals = GenerateGrayCode( m_columnVars.size() );
		rowVals = GenerateGrayCode( m_rowVars.size() );
	}
	else
	{
		// Generate the column and row values
		columnVals.reserve( (int)pow( 2, m_columnVars.size() ) );
		for( int i = 0; i < (int)pow( 2, m_columnVars.size() ); i++ )
		columnVals.push_back( ConvertIntToBinary( i, m_columnVars.size() ) );
		rowVals.reserve( (int)pow( 2, m_rowVars.size() ) );
		for( int i = 0; i < (int)pow( 2, m_rowVars.size() ); i++ )
		rowVals.push_back( ConvertIntToBinary( i, m_rowVars.size() ) );
	}

	

	// Print column header
	printf( "\t%s", offset.c_str() );
	printf( "%s\n", std::string( m_columnVars.begin(), m_columnVars.end() ).c_str() );

	// Print column values
	printf( "\t%s", offset.c_str() );
	printf( "%s |", std::string( m_rowVars.size(), ' ' ).c_str() );
	for( unsigned int i = 0; i < columnVals.size(); i++ ) {
		printf( " %s |", columnVals[i].c_str() );
	}
	printf( "\n" );
	printf( "\t%s %s\n", std::string( m_rowVars.begin(), m_rowVars.end() ).c_str(), horizontal.c_str() );

	// Print rows
	for( unsigned int i = 0; i < rowVals.size(); i++ ) {
		printf( "\t%s%s |", offset.c_str(), rowVals[i].c_str() );
		// Print values
		for( unsigned int j = 0; j < columnVals.size(); j++ )
		{
			int boxTerm;

			// Get the box term number
			boxTerm = this->getBoxTermNumber( columnVals[j], rowVals[i] );
			// See if its a donkey or  minterm
			if( std::find( m_donkeyTerms.begin(), m_donkeyTerms.end(), boxTerm ) != m_donkeyTerms.end() )
				printf( "%sX%s|", boxPadLeft.c_str(), boxPadRight.c_str() );
			else if( std::find( m_minTerms.begin(), m_minTerms.end(), boxTerm ) != m_minTerms.end() )
				printf( "%s1%s|", boxPadLeft.c_str(), boxPadRight.c_str() );
			else
				printf( "%s0%s|", boxPadLeft.c_str(), boxPadRight.c_str() );
		}
		printf( "\n" );
	}

	printf( "\n" );
}