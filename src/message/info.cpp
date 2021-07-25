#include "info.hpp"
#include "types.hpp"

namespace Message{

static constexpr const config<info> info_config[] = {
	{info::error, "error"},
	{info::warning, "warning"},
	{info::success, "success"},
	{info::info, "info"},
};

static constexpr const config<info_category> info_category_config[] = {
	{info_category::general, "general"},
	{info_category::image, "image"},
	{info_category::app, "app"},
	{info_category::device, "device"},
};

static constexpr config<info> const* get_config(info t) noexcept
{
	for(std::size_t i = 0; i < sizeof(info_config) / sizeof(info_config[0]); i++)
	{
		if(t == info_config[i].mtype) return &info_config[i];
	}
	return nullptr;
}

static constexpr config<info_category> const* get_config(info_category t) noexcept
{
	for(std::size_t i = 0; i < sizeof(info_category_config) / sizeof(info_category_config[0]); i++)
	{
		if(t == info_category_config[i].mtype) return &info_category_config[i];
	}
	return nullptr;
}

//static constexpr config<info> const* get_info_config(const char* t) noexcept
//{
//	for(std::size_t i = 0; i < sizeof(info_config) / sizeof(info_config[0]); i++)
//	{
//		if(std::strcmp(t, info_config[i].name) == 0) return &info_config[i];
//	}
//	return nullptr;
//}

static void add_info(rapidjson::Document& doc, info inf) noexcept
{
	config<info> const* conf = get_config(inf);
	if(!conf) return;

	doc.AddMember("info", rapidjson::Value(conf->name, doc.GetAllocator()).Move(), doc.GetAllocator());
}

static void add_category(rapidjson::Document& doc, info_category category) noexcept
{
	config<info_category> const* conf = get_config(category);
	if(!conf) return;

	doc.AddMember("category", rapidjson::Value(conf->name, doc.GetAllocator()).Move(), doc.GetAllocator());
}

void make_info(rapidjson::Document& doc, info inf, info_category category, const char* message, const char* data/* = nullptr */) noexcept
{
	doc.SetObject();
	add_type(doc, type::info);
	add_info(doc, inf);
	add_category(doc, category);
	doc.AddMember("message", rapidjson::Value(message, doc.GetAllocator()).Move(), doc.GetAllocator());
	if(data)
	{
		doc.AddMember("data", rapidjson::Value(data, doc.GetAllocator()).Move(), doc.GetAllocator());
	}
}

void make_info(rapidjson::Document& doc, info inf, const char* message, const char* data/* = nullptr */) noexcept
{
	make_info(doc, inf, info_category::general, message, data);
}

void make_info(rapidjson::Document& doc, info inf, mesh_addr_t const& addr, const char* message, const char* data/* = nullptr */) noexcept
{
	make_info(doc, inf, info_category::device, message, data);
	add_device(doc, addr);
}

void make_info(rapidjson::Document& doc, info inf, CoAP::Message::Option::option const& addr,
		const char* message, const char* data/* = nullptr */) noexcept
{
	make_info(doc, inf, info_category::device, message, data);
	add_device(doc, addr);
}

std::string make_info(info inf, info_category category, const char* message, const char* data/* = nullptr */) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, category, message, data);

	return stringify(doc);
}

std::string make_info(info inf, const char* message, const char* data/* = nullptr */) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, message, data);

	return stringify(doc);
}

std::string make_info(info inf, mesh_addr_t const& addr, const char* message, const char* data/* = nullptr */) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, addr, message, data);

	return stringify(doc);
}

std::string make_info(info inf, CoAP::Message::Option::option const& addr, const char* message, const char* data/* = nullptr */) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, addr, message, data);

	return stringify(doc);
}

}//Message
