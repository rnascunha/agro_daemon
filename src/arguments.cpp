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
#define DEFAULT_NOTIFY_PRIV_KEY	"priv.pem"

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
			rapidjson::Value(args.db_file.data(), args.db_file.size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	doc.AddMember("notify_key",
				rapidjson::Value(args.notify_priv_key.data(), args.notify_priv_key.size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	doc.AddMember("num_threads", args.num_threads, doc.GetAllocator());
	doc.AddMember("dev_addr",
			rapidjson::Value(args.coap_addr.data(), args.coap_addr.size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	doc.AddMember("dev_port", args.coap_port, doc.GetAllocator());
	doc.AddMember("addr",
				rapidjson::Value(args.addr.data(), args.addr.size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
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
		tt::error(stderr, "ERROR! Configuration file %s not found!", file.c_str());
//		std::cerr << "ERROR! Configuration file " << file << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::ifstream f{file};
	if(!f)
	{
		tt::error(stderr, "ERROR! Error trying to open %s file!", file.c_str());
//		std::cerr << "ERROR! Error trying to open " << file << " file!\n";
		exit(EXIT_FAILURE);
	}

	char data[4096];
	auto size = f.readsome(data, 4096);
	data[size] = '\0';
	f.close();

	rapidjson::Document doc;
	if (doc.Parse(data).HasParseError() || !doc.IsObject())
	{
		tt::error(stderr, "ERROR! Error parsing configuration file %s.", file.c_str());
//		std::cerr << "ERROR! Error parsing configuration file " << file << ".\n";
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
			<< "|num threads: " << args.num_threads << "\n"
			<< "|dev addr: " << args.coap_addr << ":" << args.coap_port << "\n"
			<< "|addr: " << args.addr << ":" << args.port
			<< "\n---------------------------\n";
}

void read_parameters(int, char** argv, arguments& args) noexcept
{
	argh::parser cmdl;

	cmdl.add_params({"-b", "--dbfile"});
	cmdl.add_params({"-n", "--notify-key"});
	cmdl.add_params({"-o", "--config"});
	cmdl.add_params({"-t", "--threads"});
	cmdl.add_params({"-d", "--dev_addr"});
	cmdl.add_params({"-p", "--dev_port"});
	cmdl.add_params({"-a", "--addr"});

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
	 * Checking port
	 */
	if(cmdl.pos_args().size() >= 2 && !(cmdl(1) >> args.port))
	{
		tt::error(stderr, "ERROR! Listening port not provided!");
//		std::cerr << "ERROR! Listening port not provided!\n";
		exit(EXIT_FAILURE);
	}

	if(!(args.port > 1 && args.port <= 65535) || args.port == INVALID_PORT)
	{
		tt::error(stderr, "ERROR! Invalid listening port. [%d]", args.port);
//		std::cerr << "ERROR! Invalid listening port [" << args.port << "]\n";
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
		tt::error(stderr, "ERROR! -t/--threads: argument option must be a positive interger (>= 1).");
//		std::cerr << "ERROR! -t/--threads: argument option must be a positive interger (>= 1)\n";
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
		tt::error(stderr, "ERROR! Invalid device port provided! [%d]", args.port);
//		std::cerr << "ERROR! Invalid device port provided!\n";
		exit(EXIT_FAILURE);
	}

#if USE_SSL == 1
	if(!cmdl({"-k", "--key"}) || !cmdl({"-c", "--cert"}))
	{
		tt::error(stderr, "ERROR! SSL support require key(-k)/certificate(-c) files.");
//		std::cerr << "ERROR! SSL support require key(-k)/certificate(-c) files.\n";
		exit(EXIT_FAILURE);
	}
	cmdl({"-k", "--key"}) >> args.key;
	cmdl({"-c", "--cert"}) >> args.cert;
#else /* USE_SSL == 1 */
	if(cmdl({"-k", "--key"}))
	{
		tt::warning(stderr, "WARNING! No SSL support. Ignoring -k/--key option.");
//		std::cerr << "WARNING! No SSL support. Ignoring -k/--key option\n";
	}

	if(cmdl({"-c", "--cert"}))
	{
		tt::warning(stderr, "WARNING! No SSL support. Ignoring -c/--cert option.");
//		std::cerr << "WARNING! No SSL support. Ignoring -c/--cert option\n";
	}
#endif /* USE_SSL == 1 */

	/**
	 * Checking files
	 */
	if(!is_file(args.notify_priv_key))
	{
		tt::error(stderr, "Notify private key file '%s' not found!", args.notify_priv_key.c_str());
		tt::warning(stderr, "Notifying will be disabled.");
//		std::cerr << "Notify private key file '" << args.notify_priv_key << "' not found! Notifying will be disabled\n";
	}

#if USE_SSL == 1
	if(!is_file(args.key))
	{
		tt::error(stderr, "ERROR! PEM key file '%s' not found!", args.key.c_str());
//		std::cerr << "ERROR! PEM key file '" << args.key << "' not found!\n"
		exit(EXIT_FAILURE);
	}

	if(!is_file(args.cert))
	{
		tt::error(stderr, "ERROR! Certified file '%s' not found!", args.cert.c_str());
//		std::cerr << "ERROR! Certified file '" << args.cert << "' not found!\n";
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
//		std::cerr << "Database file '" << args.db_file << "' will be created.\n";
	}

	tt::enable_if<tt::type::debug>([&]{
		tt::color<tt::type::debug>();
		print_args(args);
		tt::rs();
	});
}
