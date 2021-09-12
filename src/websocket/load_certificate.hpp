#ifndef AGRO_DAEMON_WEBSOCKET_CERTIFICATE_HPP__
#define AGRO_DAEMON_WEBSOCKET_CERTIFICATE_HPP__

#include <boost/asio/ssl.hpp>
#include <string>

namespace Agro{

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string& cert_file_name,
		const std::string& key_file_name,
		boost::system::error_code& ec) noexcept;

}//Agro

#endif /* AGRO_DAEMON_WEBSOCKET_CERTIFICATE_HPP__ */
