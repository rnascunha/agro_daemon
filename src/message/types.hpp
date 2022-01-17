#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>
#include "../helper/utility.hpp"

namespace Message{

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

inline constexpr auto get_type_config(const char* name) noexcept
{
    return ::get_config(name, tconfig);
}

inline constexpr auto get_config(type mtype) noexcept
{
    return ::get_config(mtype, tconfig);
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
