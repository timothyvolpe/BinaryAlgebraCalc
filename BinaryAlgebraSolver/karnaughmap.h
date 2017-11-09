#pragma once
#include <vector>

enum
{
	HEADER_OK,
	HEADER_ERROR_EMPTY,
	HEADER_ERROR_INVALID,
	HEADER_ERROR_REPEAT,
	HEADER_ERROR_DIMMISMATCH
};

enum
{
	CONTROL_SIGNALS_NONE,
	CONTROL_SIGNALS_COLUMN,
	CONTROL_SIGNALS_ROW
};

class CKarnaughMap
{
private:
	std::vector<char> m_columnVars;
	std::vector<char> m_rowVars;

	int getBoxTermNumber( std::string rowVal, std::string columnVal );
public:
	std::vector<char> m_uniqueVariables;
	std::vector<int> m_minTerms;
	std::vector<int> m_maxTerms;
	std::vector<int> m_donkeyTerms;
	int m_controlSignals;

	CKarnaughMap();
	~CKarnaughMap();

	int isValidHeader( std::string header );

	bool setColumnVars( std::string columnVars);
	bool setRowVars( std::string rowVars );

	void print();
};