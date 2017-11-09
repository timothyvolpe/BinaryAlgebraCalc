#pragma once
#include <vector>

class CKarnaughMap
{
public:
	std::vector<char> m_uniqueVariables;
	std::vector<int> m_minTerms;
	std::vector<int> m_maxTerms;

	CKarnaughMap();
	~CKarnaughMap();

	bool isValidHeader( std::string header );
};