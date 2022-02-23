#include "info.hpp"

namespace Agro{
namespace User{

Info::Info(){}

Info::Info(std::string const& username, std::string const& name,
		status stat, std::string const& email,
		std::string const& telegram_chat_id)
	: username_(username), name_(name),
	  status_(stat), email_(email),
	  telegram_chat_id_(telegram_chat_id){}

Info::Info(const char* username, const char* name,
		status stat, const char* email,
		const char* telegram_chat_id)
	: username_(username), name_(name ? name : ""),
	  status_(stat), email_(email ? email : ""),
	  telegram_chat_id_(telegram_chat_id ? telegram_chat_id : ""){}

void Info::set(std::string const& username, std::string const& name,
		status stat, std::string const& email,
		std::string const& telegram_chat_id) noexcept
{
	username_ = username;
	name_ = name;
	status_ = stat;
	email_ = email;
	telegram_chat_id_ = telegram_chat_id;
}

bool Info::is_valid() const noexcept
{
	return status_ != status::invalid;
}

std::string const& Info::username() const noexcept
{
	return username_;
}

std::string const& Info::name() const noexcept
{
	return name_;
}

Info::status	Info::get_status() const noexcept
{
	return status_;
}

std::string const& Info::email() const noexcept
{
	return email_;
}

std::string const& Info::telegram_chat_id() const noexcept
{
	return telegram_chat_id_;
}

Info::operator bool() const noexcept
{
	return is_valid();
}

}//User
}//Agro
