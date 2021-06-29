#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>

namespace Message{

enum class type{
	resource = 0,
	response,
	device,
	error
};

struct type_config{
	type		mtype;
	const char* name;
};

constexpr const type_config config[] = {
		{type::resource, "resource"},
		{type::response, "response"},
		{type::device, "device"},
		{type::error, "error"}
};

inline constexpr type_config const* get_config(type t) noexcept
{
	for(std::size_t i = 0; i < sizeof(config) / sizeof(type_config); i++)
	{
		if(t == config[i].mtype) return &config[i];
	}
	return nullptr;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
