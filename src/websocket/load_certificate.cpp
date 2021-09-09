#include "load_certificate.hpp"

namespace Agro{

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string cert_file_name,
		const std::string key_file_name,
		boost::system::error_code& ec) noexcept
{
	ctx.set_password_callback([](std::size_t,
            boost::asio::ssl::context_base::password_purpose)
        {
            return "test";
        }, ec);
	if(ec) return;

    ctx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use,
		ec);
    if(ec) return;

	ctx.use_certificate_chain_file(cert_file_name, ec);
	if(ec) return;

	ctx.use_private_key_file(key_file_name, boost::asio::ssl::context::pem, ec);
	if(ec) return;

//	ctx.use_tmp_dh_file("dh512.pem", ec);
//	if(ec) return;
}

}//Agro
