#ifndef AGRO_DAEMON_RESOURCE_REQUESTS_IMPL_HPP__
#define AGRO_DAEMON_RESOURCE_REQUESTS_IMPL_HPP__

#include "../resource_requests.hpp"

namespace Message{

template<typename Endpoint,
		typename Message>
std::string config_to_json(Endpoint const& ep,
							Message const& msg,
							CoAP::Message::Option::option const& uri_host,
							config const& cfg) noexcept
{
	rapidjson::Document doc;
	rapidjson::Value data;

	data.SetObject();

	data.AddMember("ch_config", cfg.channel_config, doc.GetAllocator());
	data.AddMember("ch_conn", cfg.channel_conn, doc.GetAllocator());

	char net_id[18], mac_ap[18];
	snprintf(net_id, 18, MACSTR, MAC2STR(cfg.net_id.addr));
	snprintf(mac_ap, 18, MACSTR, MAC2STR(cfg.mac_ap.addr));

	data.AddMember("net_id", rapidjson::Value(net_id,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("mac_ap", rapidjson::Value(mac_ap,
			doc.GetAllocator()).Move(), doc.GetAllocator());

	make_resource(doc, msg, ep, uri_host, data);

	return stringify(doc);
}

template<typename Endpoint,
		typename Message>
std::string status_to_json(Endpoint const& ep,
						Message const& msg,
						CoAP::Message::Option::option const& uri_host,
						status const& sts) noexcept
{
	rapidjson::Document doc;
	rapidjson::Value data;

	data.SetObject();

	data.AddMember("rssi", sts.rssi, doc.GetAllocator());

	make_resource(doc, msg, ep, uri_host, data);

	return stringify(doc);
}

template<typename Endpoint,
		typename Message>
std::string route_to_json(Endpoint const& ep,
						Message const& msg,
						CoAP::Message::Option::option const& uri_host,
						route const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept
{
	rapidjson::Document doc;
	rapidjson::Value data;

	data.SetObject();

	char parent[18];
	snprintf(parent, 18, MACSTR, MAC2STR(config.parent.addr));
	data.AddMember("parent", rapidjson::Value(parent,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("layer", htons(config.layer), doc.GetAllocator());

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
	data.AddMember("children", child_arr, doc.GetAllocator());

	make_resource(doc, msg, ep, uri_host, data);

	return stringify(doc);
}

template<typename Endpoint,
		typename Message>
std::string full_config_to_json(Endpoint const& ep,
						Message const& msg,
						CoAP::Message::Option::option const& uri_host,
						full_config const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept
{
	rapidjson::Document doc;
	rapidjson::Value data;

	data.SetObject();

	//Config
	data.AddMember("ch_config", config.fconfig.channel_config, doc.GetAllocator());
	data.AddMember("ch_conn", config.fconfig.channel_conn, doc.GetAllocator());

	char net_id[18], mac_ap[18];
	snprintf(net_id, 18, MACSTR, MAC2STR(config.fconfig.net_id.addr));
	snprintf(mac_ap, 18, MACSTR, MAC2STR(config.fconfig.mac_ap.addr));

	data.AddMember("net_id", rapidjson::Value(net_id,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("mac_ap", rapidjson::Value(mac_ap,
			doc.GetAllocator()).Move(), doc.GetAllocator());

	//Status
	data.AddMember("rssi", config.fstatus.rssi, doc.GetAllocator());

	//Route
	char parent[18];
	snprintf(parent, 18, MACSTR, MAC2STR(config.froute.parent.addr));
	data.AddMember("parent", rapidjson::Value(parent,
			doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("layer", htons(config.froute.layer), doc.GetAllocator());

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
	data.AddMember("children", child_arr, doc.GetAllocator());

	make_resource(doc, msg, ep, uri_host, data);

	return stringify(doc);
}

template<typename Endpoint,
		typename Message>
std::string board_config_to_json(Endpoint const& ep,
						Message const& msg,
						CoAP::Message::Option::option const& uri_host,
						board_config const& cfg,
						const char* version,
						std::size_t version_len) noexcept
{
	rapidjson::Document doc;
	rapidjson::Value data;

	data.SetObject();

	//Config
	data.AddMember("has_rtc", cfg.has_rtc, doc.GetAllocator());
	data.AddMember("has_temp_sensor", cfg.has_temp_sensor, doc.GetAllocator());

	std::string str{static_cast<const char*>(version), version_len};
	std::string::size_type pos = str.find("|");
	if(pos != std::string::npos)
	{
		data.AddMember("version_fw", rapidjson::Value(str.substr(0, pos).c_str(),
					doc.GetAllocator()).Move(), doc.GetAllocator());
		data.AddMember("version_hw", rapidjson::Value(str.substr(pos + 1).c_str(),
				doc.GetAllocator()).Move(), doc.GetAllocator());
	}
	else
	{
		data.AddMember("version_fw", rapidjson::Value(version, version_len,
					doc.GetAllocator()).Move(), doc.GetAllocator());
		data.AddMember("version_hw", rapidjson::Value(), doc.GetAllocator());
	}

	make_resource(doc, msg, ep, uri_host, data);

	return stringify(doc);
}

}//Message

#endif /* AGRO_DAEMON_RESOURCE_REQUESTS_IMPL_HPP__ */
