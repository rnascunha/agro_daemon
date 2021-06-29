#include "error.hpp"

const char* Message_Err_Category::name() const noexcept
{
	return "daemon";
}

std::string Message_Err_Category::message(int ev) const
{
	switch(static_cast<Error>(ev))
	{
		case Error::ill_formed: return "Ill formed message";
		case Error::invalid_value: return "Invalid value";
		default:
			return "(unrecognized error)";
	}
}

std::error_code make_error_code(Error e)
{
  return {static_cast<int>(e), the_Message_Err_Category};
}
