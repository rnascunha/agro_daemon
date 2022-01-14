#include "agro.hpp"

namespace Agro{

bool instance::read_all_reports(std::vector<Message::report>& reports,
		User::user_id id, int limit /* = 0 */) noexcept
{
	return db_.read_all_reports(reports, id, limit);
}

std::shared_ptr<std::string> instance::make_report(Message::report_type type,
				mesh_addr_t const& addr,
				std::string const& message,
				std::string const& arg,
				User::user_id id) noexcept
{
	return make_report(Message::report_commands::device, type, addr.to_string(), message, arg, id);
}

std::shared_ptr<std::string> instance::make_report(
				Message::report_commands command,
				Message::report_type type,
				std::string const& ref,
				std::string const& message,
				std::string const& arg,
				User::user_id id) noexcept
{
	db_.add_report(command, type, ref, message, arg, id);
	return std::make_shared<std::string>(Message::report_message(command, type, ref, message, arg));
}

}//Agro
