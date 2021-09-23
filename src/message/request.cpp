#include <iostream>

#include "process.hpp"
#include "types.hpp"
#include "request/types.hpp"
#include "coap-te-debug.hpp"
#include "make.hpp"
#include "../websocket/types.hpp"
#include "../resources/process.hpp"
#include "info.hpp"

namespace Message{

static CoAP::Message::code string_to_method(const char* method_str)
{
	std::string s{method_str};
	if(s == "get") return CoAP::Message::code::get;
	if(s == "post") return CoAP::Message::code::post;
	if(s == "put") return CoAP::Message::code::put;
	if(s == "del") return CoAP::Message::code::cdelete;

	return CoAP::Message::code::bad_request;
}

/**
 * Request callback (signature defined at transaction)
 */
static void request_cb(void const* trans,
		CoAP::Message::message const* response,
		void* request,
		Agro::websocket_ptr ws,
		Agro::instance& instance) noexcept
{
	std::cout << "Request callback called...\n";

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	std::cout << "Status: " << static_cast<int>(t->status()) << "\n";
	if(response)
	{
		std::cout << "Response received!\n\n";
		CoAP::Debug::print_message_string(*response);

		auto req = static_cast<Message::requests>(reinterpret_cast<std::uintptr_t>(request));
		engine::endpoint const ep = t->endpoint();
		Message::request_config const* config = Message::get_requests_config(req);
		if(!config) return;

		CoAP::Message::Option::option op;
		CoAP::Message::Option::get_option(*response, op, CoAP::Message::Option::code::uri_host);

		std::error_code ec;
		::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
		if(ec) return;

		if(config->response)
		{
			config->response(ep,
					host,
					req,
					t->request(),
					*response,
					t->status(),
					instance,
					ws);
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		std::cout << "Response NOT received\n";

		rapidjson::Document doc;
		Message::make_response(doc, t->request(), t->endpoint(), t->status());

		std::string resp_str = Message::stringify(doc);
		std::cout << resp_str << "\n";

		ws->write_all(resp_str);
	}
}

static void process_custom_request(rapidjson::Document const& d,
							Message::request_message& msg,
							std::error_code& ec) noexcept
{
	if(!d.HasMember("method") || !d["method"].IsString())
	{
		ec = make_error_code(Error::missing_field);
		return;
	}

	msg.method = string_to_method(d["method"].GetString());
	if(msg.method == CoAP::Message::code::bad_request)
	{
		ec = make_error_code(Error::invalid_value);
		return;
	}

	if(d.HasMember("resource") && d["resource"].IsArray())
	{
		rapidjson::Value const& resource = d["resource"];
		for (auto& v : resource.GetArray())
		{
			msg.options.emplace_back(CoAP::Message::Option::code::uri_path, v.GetString());
		}
	}

	if(d.HasMember("query") && d["query"].IsArray())
	{
		rapidjson::Value const& query = d["query"];
		for (auto& v: query.GetArray())
		{
			msg.options.emplace_back(CoAP::Message::Option::code::uri_query, v.GetString());
		}
	}

	if(d.HasMember("payload") && !d["payload"].IsNull())
	{
		msg.payload = [](rapidjson::Document const& d, void* buf, std::size_t size, std::error_code&){
			const char* payload = d["payload"].GetString();
			std::size_t s = std::strlen(payload);

			std::memcpy(buf, payload, s);
			return s;
		};
	}
}

static void send_request(
		const char* mac,
		engine::endpoint const& ep,
		Message::requests request_type,
		Message::request_message const& msg,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		rapidjson::Document const& doc) noexcept
{
	engine::request req{ep};
	req.header(msg.mtype, msg.method);

	CoAP::Message::Option::node host{CoAP::Message::Option::code::uri_host, mac};

	std::vector<CoAP::Message::Option::node> v;
	for(auto const& op : msg.options)
	{
		v.emplace_back(op);
	}

	req.add_option(host);
	for(auto& op : v)
	{
		req.add_option(op);
	}

	std::uint8_t buffer[engine::packet_size];
	std::size_t size = 0;
	if(msg.payload)
	{
		std::error_code ec;
		size = msg.payload(doc, buffer, engine::packet_size, ec);
		if(ec) return;
		if(size > 0)
		{
			req.payload(buffer, size);
		}
	}

	req.callback(std::bind(request_cb,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					ws,
					std::ref(instance)),
			reinterpret_cast<int*>(request_type));

	CoAP::Error ec;
	instance.coap_engine().send(req, ec);
	if(ec)
	{
		std::cerr << "Send Request error[" << ec.value() << "/" << ec.message() << "]\n";
	}
}

void process_request(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged&) noexcept
{
	if(!doc.HasMember("request") || !doc["request"].IsString())
	{
		std::cerr << "Not valid request\n";
		return;
	}

	Message::request_config const* config = Message::get_requests_config(doc["request"].GetString());
	std::cout << "Request search: " << doc["request"].GetString() << "\n";
	if(!config)
	{
		std::cerr << "Request config not found\n";
		return;
	}

	if(!doc.HasMember("device") || !doc["device"].IsString())
	{
		std::cerr << "Request device not found\n";
		return;
	}
	Agro::Device::Device const* dev = instance.device_list()[doc["device"].GetString()];
	if(!dev)
	{
		std::cerr << "Device not found\n";
		return;
	}

	Message::request_message const* req = config->message;
	Message::request_message custom_req;
	if(config->mtype == Message::requests::custom)
	{
		std::error_code ec;
		process_custom_request(doc, custom_req, ec);
		if(ec)
		{
			std::cerr << "Error processing custom request\n";
			return;
		}
		req = &custom_req;
	}
	std::cout << "Req: " << static_cast<int>(config->mtype) << " " << config->name << "\n";

	send_request(doc["device"].GetString(),
			dev->get_endpoint(),
			config->mtype,
			*req,
			ws,
			instance,
			doc);
}

}//Message
