#include <boost/asio.hpp>

#include "arguments.hpp"
#include "tt/tt.hpp"
#include "user/user.hpp"
#include "instance/agro.hpp"
#include "notify/libs/smtp/client.hpp"

#if USE_SSL == 1
#include "websocket/load_certificate.hpp"
#endif /* USE_SSL == 1 */

static constexpr const std::string_view subscriber{"mailto:email@email.com"};

static pusha::key get_notify_key(std::filesystem::path const& path, bool& enable, Agro::DB& db) noexcept;
static std::string get_telegram_bot_token(std::string const& telegram_bot_token, bool& enable, Agro::DB& db) noexcept;
static bool get_smtp_server_info(SMTP::server& server, bool& enable, Agro::DB& db) noexcept;

int main(int argc, char** argv)
{
	/**
	 * Reading parameters
	 */
	arguments args;
	read_parameters(argc, argv, args);

	/**
	 * Websocket endpoint
	 */
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(args.addr, ec);
	if(ec)
	{
		tt::error("Invalid address! [%s]", args.addr.c_str());
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(args.port);
	boost::asio::ip::tcp::endpoint epl{address, port};

	/**
	 * Devices endpoint
	 */
	CoAP::Error ecp;
	udp_conn::endpoint ep{args.coap_addr.c_str(), static_cast<unsigned short>(args.coap_port), ecp};
	if(ecp)
	{
		tt::error("Invalid device endpoint! [%s:%d]", args.coap_addr.c_str(), args.coap_port);
		return EXIT_FAILURE;
	}

	/**
	 * Initiating agro instance
	 */
	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	/**
	 * Open database
	 */
	std::error_code ecb;
	Agro::DB db{args.db_file, ecb};
	if(ecb)
	{
		tt::error("Error opening DB! [%s]", args.db_file.c_str());
		return EXIT_FAILURE;
	}

	/**
	 * Instantiating notify factory
	 */
	Agro::Notify::Factory noti_fac;
	bool enable;

	/**
	 * Adding push notification
	 */
	pusha::key notify_priv_key = get_notify_key(args.notify_priv_key, enable, db);
	tt::status("Adding 'push notification' notify support");
	auto n1 = std::make_unique<Agro::Notify::push>(ioc, std::move(notify_priv_key), subscriber);
	n1->enable(enable);
	noti_fac.add("push", std::move(n1));

	/**
	 * Adding telegram bot notification
	 */
	std::string notify_telegram_token = get_telegram_bot_token(args.telegram_bot_token, enable, db);
	tt::status("Adding 'telegram bot' notify support");
	auto n2 = std::make_unique<Agro::Notify::telegram>(ioc, notify_telegram_token);
	n2->enable(enable);
	noti_fac.add("telegram", std::move(n2));

	/**
	 * Adding mail support
	 */
	SMTP::server mail_server;
	get_smtp_server_info(mail_server, enable, db);
	tt::status("Adding 'mail' notify support");
	auto n3 = std::make_unique<Agro::Notify::mail>(ioc, mail_server, "Agro Telemetry");
	n3->enable(enable);
	noti_fac.add("mail", std::move(n3));

	Agro::instance instance{ioc,
		std::move(db),
		std::move(noti_fac),
		ep, epl,
#if USE_SSL == 1
		args.key, args.cert,
#endif /* USE_SSL == 1 */
		ecb};
	if(ecb)
	{
		return EXIT_FAILURE;
	}

	/**
	 * Running
	 */
	CoAP::Error ecc;
	instance.run<50>(args.num_threads, ecc);

	if(ecc && ecc != CoAP::errc::socket_receive)
	{
		tt::error("Error running! [%d/%s]", ecp.value(), ecp.message());
	}

	return EXIT_SUCCESS;
}

static pusha::key get_notify_key(std::filesystem::path const& path, bool& enable, Agro::DB& db) noexcept
{
	pusha::key key;

	if(!path.empty())
	{
		if(!std::filesystem::is_regular_file(path))
		{
			tt::error("notify private key: '%s' is not a valid file.", path.c_str());
			goto import_db;
		}
		else if(!key.import(path))
		{
			tt::error("notify private key: Failed to import from '%s' file.", path.c_str());
			goto import_db;
		}
		else {
			tt::debug("notify private key: imported from '%s' file.", path.c_str());
			enable = true;
			goto end;
		}
	}
	else
	{
import_db:
		std::string keyb64 = db.notify_private_key(enable);
		if(!keyb64.empty())
		{
			if(!key.import(keyb64))
			{
				tt::error("notifiy private key: import from 'DB' fail.");
			}
			else
			{
				tt::debug("notify private key: key imported from DB.");
			}
		}
	}

end:
	return key;
}

static std::string get_telegram_bot_token(std::string const& telegram_bot_token, bool& enable, Agro::DB& db) noexcept
{
	enable = false;
	if(!telegram_bot_token.empty()) return telegram_bot_token;

	return db.notify_telegram_bot_token(enable);
}

static bool get_smtp_server_info(SMTP::server& server, bool& enable, Agro::DB& db) noexcept
{
	if(db.notify_mail_server_info(server, enable) == SQLITE_ROW)
	{
		return mail_factory::is_valid(server);
	}

	return false;
}
