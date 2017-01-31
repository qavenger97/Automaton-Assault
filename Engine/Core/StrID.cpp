#include "pch.h"
#include "StrID.h"
#include <unordered_map>

namespace Hourglass
{
	namespace StrIDUtil
	{
		static std::unordered_map<StrID, std::string> strIDHashTable;

		StrID GetStrID( const char* string )
		{
			StrID strID = WSID( string );
			std::unordered_map<StrID, std::string>::iterator it = strIDHashTable.find( strID );
			if (it == strIDHashTable.end())
			{
				strIDHashTable[strID] = string;
			}

			return strID;
		}

		const char* GetStringFromStrID( StrID id )
		{
			std::unordered_map<StrID, std::string>::iterator it = strIDHashTable.find( id );
			if (it != strIDHashTable.end())
			{
				return it->second.c_str();
			}
			else
			{
				return nullptr;
			}
		}

		void CleanUpTable()
		{

		}
	}
}