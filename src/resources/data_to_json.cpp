#include "data_to_json.hpp"

#include <cstdio>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

static void add_ep_op(rapidjson::Document& doc,
		const char* resource,
		engine::endpoint const& ep,
		CoAP::Message::Option::node const& op) noexcept
{
	rapidjson::Value epj;

	epj.SetObject();

	doc.AddMember("resource", rapidjson::Value(resource,
			doc.GetAllocator()).Move(), doc.GetAllocator());

	char buffer[50];
	ep.address(buffer, 50);
	epj.AddMember("addr", rapidjson::Value(buffer,
				doc.GetAllocator()).Move(), doc.GetAllocator());
	epj.AddMember("port", ep.port(), doc.GetAllocator());

	doc.AddMember("endpoint", epj, doc.GetAllocator());

	char mac[18];
	std::snprintf(mac, 18, "%.*s", op.value.length, static_cast<const char*>(op.value.value));
	doc.AddMember("device", rapidjson::Value(mac,
			doc.GetAllocator()).Move(), doc.GetAllocator());
}

std::string config_to_json(engine::endpoint const& ep,
							CoAP::Message::Option::node const& op,
							config const& cfg) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();

	add_ep_op(doc, "config", ep, op);

	doc.AddMember("ch_config", cfg.channel_config, doc.GetAllocator());
	doc.AddMember("ch_conn", cfg.channel_conn, doc.GetAllocator());

	char net_id[18], mac_ap[18];
	snprintf(net_id, 18, MACSTR, MAC2STR(cfg.net_id.addr));
	snprintf(mac_ap, 18, MACSTR, MAC2STR(cfg.mac_ap.addr));

	doc.AddMember("net_id", rapidjson::Value(net_id,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	doc.AddMember("mac_ap", rapidjson::Value(mac_ap,
			doc.GetAllocator()).Move(), doc.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);

	return std::string{sb.GetString()};
}

std::string status_to_json(engine::endpoint const& ep,
						CoAP::Message::Option::node const& op,
						status const& cfg) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();

	add_ep_op(doc, "status", ep, op);

	doc.AddMember("rssi", cfg.rssi, doc.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);

	return std::string{sb.GetString()};
}

std::string route_to_json(engine::endpoint const& ep,
						CoAP::Message::Option::node const& op,
						route const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();

	add_ep_op(doc, "route", ep, op);

	char parent[18];
	snprintf(parent, 18, MACSTR, MAC2STR(config.parent.addr));
	doc.AddMember("parent", rapidjson::Value(parent,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	doc.AddMember("layer", htons(config.layer), doc.GetAllocator());

	rapidjson::Value child_arr;
	child_arr.SetArray();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		char child_buf[18];
		std::snprintf(child_buf, 18, "" MACSTR "", MAC2STR(children));
		children += 6;

		child_arr.PushBack(rapidjson::Value(child_buf,
				doc.GetAllocator()).Move(), doc.GetAllocator());
	}
	doc.AddMember("children", child_arr, doc.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);

	return std::string{sb.GetString()};
}

std::string full_config_to_json(engine::endpoint const& ep,
						CoAP::Message::Option::node const& op,
						full_config const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();

	add_ep_op(doc, "full_config", ep, op);

	//Config
	doc.AddMember("ch_config", config.fconfig.channel_config, doc.GetAllocator());
	doc.AddMember("ch_conn", config.fconfig.channel_conn, doc.GetAllocator());

	char net_id[18], mac_ap[18];
	snprintf(net_id, 18, MACSTR, MAC2STR(config.fconfig.net_id.addr));
	snprintf(mac_ap, 18, MACSTR, MAC2STR(config.fconfig.mac_ap.addr));

	doc.AddMember("net_id", rapidjson::Value(net_id,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	doc.AddMember("mac_ap", rapidjson::Value(mac_ap,
			doc.GetAllocator()).Move(), doc.GetAllocator());

	//Status
	doc.AddMember("rssi", config.fstatus.rssi, doc.GetAllocator());

	//Route
	char parent[18];
	snprintf(parent, 18, MACSTR, MAC2STR(config.froute.parent.addr));
	doc.AddMember("parent", rapidjson::Value(parent,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	doc.AddMember("layer", htons(config.froute.layer), doc.GetAllocator());

	rapidjson::Value child_arr;
	child_arr.SetArray();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		char child_buf[18];
		std::snprintf(child_buf, 18, "" MACSTR "", MAC2STR(children));
		children += 6;

		child_arr.PushBack(rapidjson::Value(child_buf,
				doc.GetAllocator()).Move(), doc.GetAllocator());
	}
	doc.AddMember("children", child_arr, doc.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);

	return std::string{sb.GetString()};
}
