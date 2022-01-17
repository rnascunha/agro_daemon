#ifndef AGRO_DAEMON_MESSAGE_REPORT_HPP__
#define AGRO_DAEMON_MESSAGE_REPORT_HPP__

#include <string>
//#include "types.hpp"
#include "../device/types.hpp"
#include "../helper/utility.hpp"

#include "rapidjson/document.h"

namespace Agro{
namespace Message{

enum class report_commands{
	list = 0,
	device,
	image,
	app,
	notify
};

enum class report_type{
	error = 0,
	warning = 1,
	success = 2,
	info = 3
};

static const constexpr int report_all_user = -1;

struct report{
	report(report_commands,
			report_type,
			std::string const&,
			std::string const&,
			std::string const&,
			long);

	report_commands commands;
	report_type	type;
	std::string reference;
	std::string message;
	std::string arg;
	long		time;
};


constexpr const config<report_commands> report_config[] = {
	{report_commands::list, "list"},
	{report_commands::device, "device"},
	{report_commands::image, "image"},
	{report_commands::app, "app"},
	{report_commands::notify, "notify"},
};

inline constexpr auto get_report_config(const char* name) noexcept
{
    return ::get_config(name, report_config);
}

inline constexpr auto get_config(report_commands mtype) noexcept
{
    return ::get_config(mtype, report_config);
}

std::string report_message(report_commands command,
		report_type type,
		std::string const& reference,
		std::string const& message,
		std::string const& arg /* = "" */) noexcept;

std::string report_message(report_type,
		mesh_addr_t const&,
		std::string const& /* message */,
		std::string const& = ""/* arg */) noexcept;

std::string report_message(std::vector<report> const&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_REPORT_HPP__ */
