#if _MSC_VER
#include <SDKDDKVer.h>
#endif /* _MSC_VER */

#include "client.hpp"

namespace SMTP{

std::string make_mail_account(account const& account) noexcept
{
	std::ostringstream os;
	if(account.name.empty())
	{
		os << account.email;
	}
	else
	{
		os << account.name << " <" << account.email << ">";
	}

	return os.str();
}

std::string make_mail_message(
		account const& from,
		std::vector<account> const& tos,
		std::string const& subject,
		std::string const& body) noexcept
{
	std::ostringstream os;

	os << "FROM: " << make_mail_account(from) << "\r\n";

	os << "TO: ";
	bool flag = true;
	for(auto const& to : tos)
	{
		os << make_mail_account(to);
		if(flag && tos.size() > 1)
		{
			os << ", ";
			flag = false;
		}
	}
	os << "\r\n";

	if(!subject.empty())
	{
		os << "SUBJECT: " << subject << "\r\n";
	}

	if(!body.empty())
	{
		os << "\r\n" << body;
	}

	return os.str();
}

void make_mail_data(
		data& data,
		account const& from,
		std::vector<account> const& tos,
		std::string const& subject,
		std::string const& body) noexcept
{
	data.from_email = from.email;
	for(auto const& to : tos)
	{
		data.to_email.push_back(to.email);
	}
	data.message = make_mail_message(from, tos, subject, body);
}

}//SMTP
