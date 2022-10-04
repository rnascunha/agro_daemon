#include <iostream>
#include <filesystem>
#include <fstream>

#include "arguments.hpp"
#include "argh.h"
#include "version.hpp"

#include "tt/tt.hpp"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#define DEFAULT_ADDR			"0.0.0.0"
#define INVALID_PORT			0
#define DEFAULT_NUMBER_THEADS	1
#define DEFAULT_COAP_PORT		5683
#define DEFAULT_COAP_ADDR		DEFAULT_ADDR
#define DEFAULT_DBFILE			"agro.db"
#define DEFAULT_NOTIFY_PRIV_KEY	""
#define DEFAULT_TELEGRAM_BOT_TOKEN	""

static bool is_file(std::filesystem::path const& path) noexcept
{
	return (std::filesystem::exists(path) &&
				std::filesystem::is_regular_file(path));
}

void usage(const char* program) noexcept
{
	std::cout << R"(Usage:
	)" << program << R"( --help|--version|-ssl|
		[--key=<path_key_file> --cert=<path_cert_file>]
		[--dbfile=<database_file>] [--notify-key=<priv_key>]
		[--telegram-bot-token=<bot_token>]
		[--config[=<config_file>]] [--threads=<num_threads>]
		[--dev_addr=<coap_addr>] [--dev_port=<coap_port>]
		[--addr=<IP_listen>] <port>
Required:
	<port>
		Port to listen.
Required if compiled with SSL:
	-k, --key=<path_key_file>
		PEM file with the SSL private key.
	-c, --cert=<path_cert_file>
		PEM file with SSL certificate.
Options:
	-h, --help
		Prints this help message.
	-v, --version
		Prints software version.
	-s, --ssl	
		Prints 'true' if software compiled with SSL (otherwise 'false').
	-b, --dbfile=<database_file> | Default: agro.db
		Database file to use. If doesn't exists, create it.
	-n, --notify-key=<priv_key>
		Private key used to make web push notification.
	-e, --telegram-bot-token=<bot_token>
		Bot token to make notification using telegram.
	-o, --config[=<config_file>]
		If file provided, reads input configuration from file. If not
		provided, print configuration on screen (to be redirected to
		a file)
	-t, --threads=<num_threads> | Default: 1
		Number of threads to run.
	-d, --dev_addr=<coap_addr> | Default: 0.0.0.0
		Interface where devices will connect.
	-p, --dev_port=<coap_port> | Default: 5683
		Port to open CoAP service.
	-a, --addr=<IP_listen> | Default: 0.0.0.0
		Interface to listen users requests.
)";
}

static void make_config_file(arguments const& args) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();

	doc.AddMember("dbfile",
			rapidjson::Value(args.db_file.data(), static_cast<int>(args.db_file.size()), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	doc.AddMember("notify_key",
				rapidjson::Value(args.notify_priv_key.data(), static_cast<int>(args.notify_priv_key.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	doc.AddMember("telegram_bot_token",
				rapidjson::Value(args.telegram_bot_token.data(), static_cast<int>(args.telegram_bot_token.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	doc.AddMember("num_threads", args.num_threads, doc.GetAllocator());
	doc.AddMember("dev_addr",
			rapidjson::Value(args.coap_addr.data(), static_cast<int>(args.coap_addr.size()), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	doc.AddMember("dev_port", args.coap_port, doc.GetAllocator());
	doc.AddMember("addr",
				rapidjson::Value(args.addr.data(), static_cast<int>(args.addr.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());

#if USE_SSL == 1
	doc.AddMember("key",
				rapidjson::Value(args.key.data(), args.key.size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	doc.AddMember("cert",
					rapidjson::Value(args.cert.data(), args.cert.size(), doc.GetAllocator()).Move(),
					doc.GetAllocator());
#endif /* USE_SSL == 1 */

	doc.AddMember("port", args.port, doc.GetAllocator());

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	std::cout << buffer.GetString() << std::endl;
}

static void read_config_file(std::filesystem::path const& file, arguments& args) noexcept
{
	if(!is_file(file))
	{
		tt::error(stderr, "Configuration file '%s' not found!", file.c_str());
		exit(EXIT_FAILURE);
	}

	std::ifstream f{file};
	if(!f)
	{
		tt::error(stderr, "Error trying to open '%s' file!", file.c_str());
		exit(EXIT_FAILURE);
	}

	char data[4096];
	auto size = f.readsome(data, 4096);
	data[size] = '\0';
	f.close();

	rapidjson::Document doc;
	if (doc.Parse(data).HasParseError() || !doc.IsObject())
	{
		tt::error(stderr, "Error parsing configuration file '%s'.", file.c_str());
		exit(EXIT_FAILURE);
	}

	if(doc.HasMember("dbfile") && doc["dbfile"].IsString())
	{
		args.db_file = doc["dbfile"].GetString();
	}

	if(doc.HasMember("notify_key") && doc["notify_key"].IsString())
	{
		args.notify_priv_key = doc["notify_key"].GetString();
	}

	if(doc.HasMember("telegram_bot_token") && doc["telegram_bot_token"].IsString())
	{
		args.telegram_bot_token = doc["telegram_bot_token"].GetString();
	}

	if(doc.HasMember("num_threads") && doc["num_threads"].IsInt())
	{
		args.num_threads = doc["num_threads"].GetInt();
	}

	if(doc.HasMember("dev_addr") && doc["dev_addr"].IsString())
	{
		args.coap_addr = doc["dev_addr"].GetString();
	}

	if(doc.HasMember("dev_port") && doc["dev_port"].IsInt())
	{
		args.coap_port = doc["dev_port"].GetInt();
	}

	if(doc.HasMember("addr") && doc["addr"].IsString())
	{
		args.addr = doc["addr"].GetString();
	}

#if USE_SSL == 1
	if(doc.HasMember("key") && doc["key"].IsString())
	{
		args.key = doc["key"].GetString();
	}

	if(doc.HasMember("cert") && doc["cert"].IsString())
	{
		args.cert = doc["cert"].GetString();
	}
#endif /* USE_SSL == 1 */

	if(doc.HasMember("port") && doc["port"].IsInt())
	{
		args.port = doc["port"].GetInt();
	}
}

static void print_args(arguments const& args) noexcept
{
	std::cout <<
R"(---------------------------
|        Arguments        |
---------------------------
)"
			<< "|dbfile: " << args.db_file << "\n"
			<< "|notify_key: " << args.notify_priv_key << "\n"
			<< "|telegram_bot_token: " << args.telegram_bot_token << "\n"
			<< "|num threads: " << args.num_threads << "\n"
			<< "|dev addr: " << args.coap_addr << ":" << args.coap_port << "\n"
			<< "|addr: " << args.addr << ":" << args.port << "\n"
#if USE_SSL == 1
			<< "|key file: " << args.key << "\n"
			<< "|cert file: " << args.cert << "\n"
#endif /* USE_SSL == 1 */
			<< "---------------------------\n";
}

void read_parameters(int, char** argv, arguments& args) noexcept
{
	argh::parser cmdl;

	cmdl.add_params({"-b", "--dbfile"});
	cmdl.add_params({"-n", "--notify-key"});
	cmdl.add_params({"-e", "--telegram-bot-token"});
	cmdl.add_params({"-o", "--config"});
	cmdl.add_params({"-t", "--threads"});
	cmdl.add_params({"-d", "--dev_addr"});
	cmdl.add_params({"-p", "--dev_port"});
	cmdl.add_params({"-a", "--addr"});
	cmdl.add_params({"-k", "--key"});
	cmdl.add_params({"-c", "--cert"});

	cmdl.parse(argv);

	if(cmdl[{"-h", "--help"}])
	{
		usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	if(cmdl[{"-v", "--version"}])
	{
		std::cout << AGRO_DAEMON_NAME << " " << AGRO_DAEMON_GIT_SHORT_COMMIT <<
				" " << AGRO_DAEMON_GIT_BRANCH << "\n"
				<< __DATE__ << " " << __TIME__ << "\n";
		exit(EXIT_SUCCESS);
	}

	if(cmdl[{"-s", "-ssl"}])
	{
		std::cout << (USE_SSL ? "true" : "false") << "\n";
		exit(EXIT_SUCCESS);
	}

	args.addr = DEFAULT_ADDR;
	args.port = INVALID_PORT;
	args.coap_addr = DEFAULT_COAP_ADDR;
	args.coap_port = DEFAULT_COAP_PORT;
	args.db_file = DEFAULT_DBFILE;
	args.notify_priv_key = DEFAULT_NOTIFY_PRIV_KEY;
	args.telegram_bot_token = DEFAULT_TELEGRAM_BOT_TOKEN;
	args.num_threads = DEFAULT_NUMBER_THEADS;

	std::string file_config;
	if(cmdl({"-o", "--config"}) >> file_config)
		read_config_file(file_config, args);

	/**
	 * Database file
	 */
	cmdl({"-b","--dbfile"}, DEFAULT_DBFILE) >> args.db_file;

	/**
	 * Notify private key
	 */
	cmdl({"-n","--notify-key"}, DEFAULT_NOTIFY_PRIV_KEY) >> args.notify_priv_key;

	/**
	 * Telegram bot token
	 */
	cmdl({"-e","--telegram-bot-token"}, DEFAULT_TELEGRAM_BOT_TOKEN) >> args.telegram_bot_token;

	/**
	 * Checking port
	 */
	if(cmdl.pos_args().size() < 2 && args.port == INVALID_PORT)
	{
		tt::error(stderr, "Listening port not provided!");
		exit(EXIT_FAILURE);
	}

	cmdl(1) >> args.port;

	if(!(args.port > 1 && args.port <= 65535) || args.port == INVALID_PORT)
	{
		tt::error(stderr, "Invalid listening port. [%d]", args.port);
		exit(EXIT_FAILURE);
	}

	/**
	 * Address
	 */
	cmdl({"-a", "--addr"}, DEFAULT_ADDR) >> args.addr;

	/**
	 * Number of threads
	 */
	if(!(cmdl({"-t", "--threads"}, DEFAULT_NUMBER_THEADS) >> args.num_threads)
			|| args.num_threads < 1)
	{
		tt::error(stderr, "-t/--threads: argument option must be a positive interger (>= 1).");
		exit(EXIT_FAILURE);
	}

	/**
	 * Device addr
	 */
	cmdl({"-d", "--dev_addr"}, DEFAULT_COAP_ADDR) >> args.coap_addr;

	/**
	 * Device port
	 */
	if(!(cmdl({"-p", "--dev_port"}, DEFAULT_COAP_PORT) >> args.coap_port)
			|| !(args.coap_port > 1 && args.coap_port <= 65535))
	{
		tt::error(stderr, "Invalid device port provided! [%d]", args.port);
		exit(EXIT_FAILURE);
	}

#if USE_SSL == 1
	if(!cmdl({"-k", "--key"}) && args.key.empty())
	{
		tt::error(stderr, "SSL support requires key(-k) files.");
		exit(EXIT_FAILURE);
	}
	if(cmdl({"-k", "--key"}))
		cmdl({"-k", "--key"}) >> args.key;

	if(!cmdl({"-c", "--cert"}) && args.cert.empty())
	{
		tt::error(stderr, "SSL support requires certificate(-c) files.");
		exit(EXIT_FAILURE);
	}
	if(cmdl({"-c", "--cert"}))
		cmdl({"-c", "--cert"}) >> args.cert;
#else /* USE_SSL == 1 */
	if(cmdl({"-k", "--key"}))
	{
		tt::warning(stderr, "No SSL support. Ignoring -k/--key option.");
	}

	if(cmdl({"-c", "--cert"}))
	{
		tt::warning(stderr, "No SSL support. Ignoring -c/--cert option.");
	}
#endif /* USE_SSL == 1 */

	/**
	 * Checking files
	 */
#if USE_SSL == 1
	if(!is_file(args.key))
	{
		tt::error(stderr, "PEM key file '%s' not found!", args.key.c_str());
		exit(EXIT_FAILURE);
	}

	if(!is_file(args.cert))
	{
		tt::error(stderr, "Certified file '%s' not found!", args.cert.c_str());
		exit(EXIT_FAILURE);
	}
#endif /* USE_SSL == 1 */

	/**
	 * Output config
	 */
	if(cmdl[{"-o", "--config"}])
	{
		make_config_file(args);
		exit(EXIT_SUCCESS);
	}

	if(!is_file(args.db_file))
	{
		tt::status("Database file '%s' will be created.", args.db_file.c_str());
	}

	tt::enable_if<tt::type::debug>([&]{
		tt::color<tt::type::debug>();
		print_args(args);
		tt::rs();
	});
}
