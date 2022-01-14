#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>
#include <cstring>

namespace Message{

template<typename Type>
struct config{
	Type		mtype;
	const char* name;
};

/**
 * Types
 */
enum class type{
	device,      /**< device */
	image,       /**< image */
	app,		/**< app */
	user,		/**< user */
	report,		/**< report */
	sensor,		/**< sensor */
	notify,		/**< notify */
};

constexpr const config<type> tconfig[] = {
	{type::device, "device"},
	{type::image, "image"},
	{type::app, "app"},
	{type::user, "user"},
	{type::report, "report"},
	{type::sensor, "sensor"},
	{type::notify, "notify"}
};

inline constexpr config<type> const* get_type_config(type t) noexcept
{
	for(std::size_t i = 0; i < sizeof(tconfig) / sizeof(tconfig[0]); i++)
	{
		if(t == tconfig[i].mtype) return &tconfig[i];
	}
	return nullptr;
}

inline constexpr config<type> const* get_type_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(tconfig) / sizeof(tconfig[0]); i++)
	{
		if(std::strcmp(t, tconfig[i].name) == 0) return &tconfig[i];
	}
	return nullptr;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
