#include <iostream>
#include <string>
#include <bitset>
#include "equationparser.h"

std::string ConvertIntToBinary( int val, unsigned int digits )
{
	if( digits <= 0 )
		return "";

	std::string fullOutput = std::bitset<32>( val ).to_string();

	return fullOutput.substr( 32-digits, digits );
}

int main( int argc, char *argv[] )
{
	std::string userEq, inputStr, comparisonEq;
	std::vector<std::string> userInputs;
	bool evalResult, comparisonResult;
	std::vector<int> minterm, mintermComparison;
	std::vector<int> maxterm, maxtermComparison;

	printf( "\n  Binary Algebra Solver\n" );
	printf( "  by Timothy Volpe (c) 2017\n" );
	printf( "  v0.3\n\n" );

	comparisonEq = "";

	// Get an equation from the user
	printf( "Enter \'f\' to compare to file\n" );
	printf( "Enter an equation: " );
	std::getline( std::cin, userEq );
	if( userEq == "" ) {
		printf( "ERROR: No equation\n" );
		printf( "\nPress any key to exit..." );
		std::cin.get();
		return 5;
	}
	else if( userEq[0] == 'f' )
	{
		// Load the equation from file
		FILE *file;
		file = fopen( "equation.txt", "r" );
		if( file ) {
			char buff[512];
			fgets( buff, 512, file );
			comparisonEq = buff;
			fclose( file );
		}
		else {
			printf( "ERROR: equation.txt not found containing comparison function!\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 6;
		}

		if( comparisonEq == "" ) {
			printf( "ERROR: No comparison equation in file\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 7;
		}
		printf( " > Comparison equation: %s\n", comparisonEq.c_str() );

		printf( "Enter equation to compare: " );
		std::getline( std::cin, userEq );
		if( userEq == "" ) {
			printf( "ERROR: No equation\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 5;
		}
	}
	printf( " > Equation: %s\n", userEq.c_str() );

	// Parse the equation
	CEquationParser parser, comparisonParser;
	int parseError, comparisonError;
	parser.parse( userEq, &parseError );
	if( parseError != PARSE_ERROR_OK )
	{
		switch( parseError )
		{
		case PARSE_ERROR_INVALID_LITERAL:
			printf( "ERROR: Invalid literal in equation\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 1;
		case PARSE_ERROR_SYNTAX:
			printf( "ERROR: Syntax error in equation\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 2;
		case PARSE_ERROR_UNKNOWN:
		default:
			printf( "ERROR: Unknown parse error contact devs\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 666;
		}
	}
	else
		printf( "Equation OK!\n" );
	if( comparisonEq != "" )
	{
		comparisonParser.parse( comparisonEq, &comparisonError );
		if( comparisonError != PARSE_ERROR_OK )
		{
			switch( parseError )
			{
			case PARSE_ERROR_INVALID_LITERAL:
				printf( "ERROR: Invalid literal in comparison equation\n" );
				printf( "\nPress any key to exit..." );
				std::cin.get();
				return 1;
			case PARSE_ERROR_SYNTAX:
				printf( "ERROR: Syntax error in comparison equation\n" );
				printf( "\nPress any key to exit..." );
				std::cin.get();
				return 2;
			case PARSE_ERROR_UNKNOWN:
			default:
				printf( "ERROR: Unknown parse error in comparison equation contact devs\n" );
				printf( "\nPress any key to exit..." );
				std::cin.get();
				return 666;
			}
		}
		else
			printf( "Comparison equation OK!\n" );
	}
	// Make sure they have the same variables
	if( comparisonEq != "" ) {
		if( parser.getUniqueVariables() != comparisonParser.getUniqueVariables() ) {
			printf( "ERROR: Comparison equation and equation provided do not have the same variables!\n" );
			printf( "\nPress any key to exit..." );
			std::cin.get();
			return 9;
		}
	}
#ifdef _DEBUG
	printf( "\nClean equation: %s\n", parser.getCleanEquation().c_str() );
	printf( "Clean comparison equation: %s\n", comparisonParser.getCleanEquation().c_str() );
#endif

	// Enter the inputs
	printf( "\nEnter up to %d inputs for %d variables (hit enter twice to stop):\n V\t| ", parser.getMaxInputs(), parser.getUniqueVariableCount() );
	for( int i = 0; i < parser.getUniqueVariableCount(); i++ ) {
		printf( "%c", parser.getUniqueVariables()[i] );
	}
	printf( " (Enter \'i\' to use 0-%d as inputs)\n", parser.getMaxInputs() );
	userInputs.clear();
	while( (int)userInputs.size() < parser.getMaxInputs() )
	{
		printf( " %d\t| ", userInputs.size() );
		std::getline( std::cin, inputStr );
		if( inputStr == "" )
			break;
		else if( inputStr[0] == 'i' ) {
			// Fill in the rest with the remaining inputs incrementing
			for( unsigned int j = userInputs.size(); j < parser.getMaxInputs(); j++ ) {
				std::string input = ConvertIntToBinary( j, parser.getUniqueVariableCount() );
				printf( " %d\t| %s\n", j, input.c_str() );
				userInputs.push_back( input );
			}
			break;
		}
		else if( (int)inputStr.length() > parser.getUniqueVariableCount() ) {
			printf( "Input too long!\n" );
			continue;
		}
		else if( (int)inputStr.length() < parser.getUniqueVariableCount() ) {
			printf( "Input too short!\n" );
			continue;
		}
		else if( !CEquationParser::sanitizeInput( inputStr ) ) {
			printf( "Invalid input! Only 1s and 0s are permitted\n" );
			continue;
		}
		userInputs.push_back( inputStr );
	}

	if( userInputs.size() == 0 ) {
		printf( "No inputs provided!\n " );
		std::cin.get();
		return 5;
	}

	printf( "Evaluating equation...\n" );
	for( unsigned int i = 0; i < userInputs.size(); i++ )
	{
		parser.evaluate( userInputs[i], &evalResult, &parseError );
		switch( parseError )
		{
		case PARSE_ERROR_SYNTAX:
			printf( "ERROR: Syntax error in equation\n" );
			std::cin.get();
			return 3;
		case PARSE_ERROR_INPUT:
			printf( "ERROR: Invalid input\n" );
			std::cin.get();
			return 4;
		case PARSE_ERROR_OK:
		default:
			break;
		}
		if( comparisonEq != "" )
		{
			comparisonParser.evaluate( userInputs[i], &comparisonResult, &comparisonError );
			switch( comparisonError )
			{
			case PARSE_ERROR_SYNTAX:
				printf( "ERROR: Syntax error in comparison equation\n" );
				std::cin.get();
				return 8;
			case PARSE_ERROR_INPUT:
				printf( "ERROR: Invalid input in comparison equation\n" );
				std::cin.get();
				return 4;
			case PARSE_ERROR_OK:
			default:
				break;
			}
		}
		if( comparisonEq != "" ) {
			printf( "Result %d:\t %s (C: %s\tE: %s)\n", i, ((evalResult == comparisonResult) ? "EQUAL " : "NEQUAL"), (comparisonResult ? "TRUE" : "FALSE"), (evalResult ? "TRUE" : "FALSE") );
			if( comparisonResult )
				mintermComparison.push_back( i );
			else
				maxtermComparison.push_back( i );
		}
		else
			printf( "Result %d:\t %s\n", i, (evalResult ? "TRUE" : "FALSE") );

		if( evalResult )
			minterm.push_back( i );
		else
			maxterm.push_back( i );
	}
	// Minterms
	wprintf( L"Minterms (E): m(" );
	for( auto it = minterm.begin(); it != minterm.end(); it++ ) {
		printf( "%d", (*it) );
		if( it + 1 != minterm.end() )
			printf( ", " );
	}
	printf( ")\n" );
	if( comparisonEq != "" ) {
		wprintf( L"Minterms (C): m(" );
		for( auto it = mintermComparison.begin(); it != mintermComparison.end(); it++ ) {
			printf( "%d", (*it) );
			if( it + 1 != mintermComparison.end() )
				printf( ", " );
		}
		printf( ")\n" );
	}
	// Maxterms
	wprintf( L"Maxterms (E): M(" );
	for( auto it = maxterm.begin(); it != maxterm.end(); it++ ) {
		printf( "%d", (*it) );
		if( it + 1 != maxterm.end() )
			printf( ", " );
	}
	printf( ")\n" );
	if( comparisonEq != "" ) {
		wprintf( L"Minterms (C): M(" );
		for( auto it = maxtermComparison.begin(); it != maxtermComparison.end(); it++ ) {
			printf( "%d", (*it) );
			if( it + 1 != maxtermComparison.end() )
				printf( ", " );
		}
		printf( ")\n" );
	}

	printf( "\nPress any key to exit..." );
	std::cin.get();

	return 0;
}