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
		case Error::value_too_small: return "Value too small";
		case Error::missing_field:	return "missing field";
		case Error::app_not_found: return "app not found";
		case Error::open_db: return "open db";
		case Error::statement_error: return "db statement error";
		case Error::internal_error: return "internal error";
		case Error::user_not_found: return "User not found";
		case Error::user_is_inactive: return "User is inactive";
		case Error::user_is_suspended: return "User is suspended";
		case Error::user_is_deleted: return "User is deleted";
		case Error::password_not_match: return "Password does not match";
		case Error::session_expired: return "Session expired";
		case Error::image_not_found: return "Image not found";
		default:
			return "(unrecognized error)";
	}
}

std::error_code make_error_code(Error e)
{
  return {static_cast<int>(e), the_Message_Err_Category};
}
