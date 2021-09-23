#include "db.hpp"
#include <string>
#include <system_error>

namespace Agro{

DB::DB(std::string const& db_name, std::error_code& ec)
	: db_(db_name.c_str(), ec)
{
	if(ec) return;
	clear_all_sessions();
}

}//Agro
