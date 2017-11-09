#pragma once
#include <string>
#include <vector>

std::string ConvertIntToBinary( int val, unsigned int digits );
int ConvertBinaryToInt( std::string binary );

std::vector<std::string> GenerateGrayCode( int bits );

std::vector<int> ParseDonkeys( std::string donkeys );