#include "process_request.hpp"
#include "coap_engine.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

#include "message/make.hpp"
#include <string>
#include "coap-te-debug.hpp"
#include <iostream>
#include "websocket.hpp"

/**
 * Request callback (signature defined at transaction)
 */
static void request_cb(void const* trans, CoAP::Message::message const* response, void*) noexcept
{
	std::cout << "Request callback called...\n";

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	std::cout << "Status: " << static_cast<int>(t->status()) << "\n";
	if(response)
	{
		std::cout << "Response received!\n\n";
		CoAP::Debug::print_message_string(*response);

		rapidjson::Document doc;
		Message::make_response(doc, t->request(), *response, t->endpoint(), t->status());

		std::string resp_str{Message::stringify(doc)};
		std::cout << resp_str << "\n";

		Websocket<false>::write_all(resp_str);
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		std::cout << "Response NOT received\n";

		rapidjson::Document doc;
		Message::make_response(doc, t->request(), t->endpoint(), t->status());

		std::string resp_str{Message::stringify(doc)};
		std::cout << resp_str << "\n";

		Websocket<false>::write_all(resp_str);
	}
}

void process_request(engine& coap_engine, std::string&& data) noexcept
{
	rapidjson::Document d;

	if (d.Parse(data.c_str()).HasParseError() || !d.IsObject())
	{
		std::cerr << "Error parsing receiving message...\n";
		//Not valid message;
		return;
	}

	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	d.Accept(writer);

	std::cout << "Received: " << sb.GetString() << "\n";

	CoAP::Error ec;
	engine::endpoint ep{d["endpoint"]["addr"].GetString(),
						static_cast<std::uint16_t>(d["endpoint"]["port"].GetUint()), ec};
	if(ec)
	{
		std::cerr << "Endpoint parse error[" << ec.value() << "/" << ec.message() << "]\n";
		return;
	}

	std::vector<CoAP::Message::Option::node> list;
	list.emplace_back(CoAP::Message::Option::code::uri_host, d["device"].GetString());

	rapidjson::Value& resource = d["resource"];
	for (auto& v : resource.GetArray())
	{
		std::cout << "v: " << v.GetString() << "\n";
		list.emplace_back(CoAP::Message::Option::code::uri_path, v.GetString());
	}

	engine::request req{ep};
	req.header(CoAP::Message::type::confirmable, CoAP::Message::code::get);

	for(auto& op : list)
	{
		req.add_option(op);
	}

	if(d.HasMember("payload") && !d["payload"].IsNull())
	{
		req.payload(d["payload"].GetString());
	}

	req.callback(request_cb);

	coap_engine.send(req, ec);
	if(ec)
	{
		std::cerr << "Send error[" << ec.value() << "/" << ec.message() << "]\n";
	}
}
