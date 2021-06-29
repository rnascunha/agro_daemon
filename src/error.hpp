#ifndef AGRO_MESH_ERROR_CODE_HPP__
#define AGRO_MESH_ERROR_CODE_HPP__

#include <system_error>

enum class Error{
	ill_formed = 0,
	invalid_value,
};

struct Message_Err_Category : public std::error_category{
	const char* name() const noexcept;
	std::string message(int ev) const;
};

const Message_Err_Category the_Message_Err_Category {};

std::error_code make_error_code(Error e);

#endif /* AGRO_MESH_ERROR_CODE_HPP__ */
