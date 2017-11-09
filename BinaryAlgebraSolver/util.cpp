#include "util.h"
#include <bitset>
#include <algorithm>

std::string ConvertIntToBinary( int val, unsigned int digits )
{
	if( digits <= 0 )
		return "";

	std::string fullOutput = std::bitset<32>( val ).to_string();

	return fullOutput.substr( 32 - digits, digits );
}

int ConvertBinaryToInt( std::string binary )
{
	int decimal;

	decimal = 0;
	for( int i = binary.size()-1; i >= 0; i-- ) {
		if( binary[i] == '1' )
			decimal += (int)pow( 2, binary.size()-1-i );
	}

	return decimal;
}

std::vector<int> ParseDonkeys( std::string donkeys )
{
	std::vector<int> donkeyTerms;

	// Sanitize the input (remove spaces)
	donkeys.erase( std::remove_if( donkeys.begin(), donkeys.end(), isspace ), donkeys.end() );

	// Check each token
	size_t pos = 0;
	std::string token;
	while( (pos = donkeys.find( "," )) != std::string::npos )
	{
		token = donkeys.substr( 0, pos );
		// Convert to integer
		donkeyTerms.push_back( strtol( token.c_str(), 0, 10 ) );
		donkeys.erase( 0, pos + 1 );
	}
	// Convert last token if it exists
	if( donkeys != "" )
		donkeyTerms.push_back( strtol( donkeys.c_str(), 0, 10 ) );

	return donkeyTerms;
}

/*
From: http://www.geeksforgeeks.org/given-a-number-n-generate-bit-patterns-from-0-to-2n-1-so-that-successive-patterns-differ-by-one-bit/
*/
// This function generates all n bit Gray codes and prints the
// generated codes
std::vector<std::string> GenerateGrayCode( int bits )
{
	// 'arr' will store all generated codes
	std::vector<std::string> arr;

	// base case
	if( bits <= 0 )
		return arr;

	// start with one-bit pattern
	arr.push_back( "0" );
	arr.push_back( "1" );

	// Every iteration of this loop generates 2*i codes from previously
	// generated i codes.
	int i, j;
	for( i = 2; i < (1 << bits); i = i << 1 )
	{
		// Enter the prviously generated codes again in arr[] in reverse
		// order. Nor arr[] has double number of codes.
		for( j = i - 1; j >= 0; j-- )
			arr.push_back( arr[j] );

		// append 0 to the first half
		for( j = 0; j < i; j++ )
			arr[j] = "0" + arr[j];

		// append 1 to the second half
		for( j = i; j < 2 * i; j++ )
			arr[j] = "1" + arr[j];
	}

	return arr;
}