#ifndef AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__
#define AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__

#include "../ota.hpp"
#include "rapidjson/document.h"
#include "../../ota/types.hpp"
#include <ostream>
#include <fstream>
#include <iomanip>

namespace Message {

static std::string sha256_string(const uint8_t *data)
{
	std::ostringstream hash;
    for (int i = 0; i < 32; ++i)
    {
    	hash << std::hex
    				<< std::setfill('0')
    				<< std::setw(2)
    				<< (int)data[i];
    }
    return hash.str();
}

template<typename Allocator>
rapidjson::Value& make_image(std::string const& name,
		std::size_t size,
		esp_app_desc_t const& desc,
		rapidjson::Value& data,
		Allocator& alloc) noexcept
{
	data.SetObject();

	data.AddMember("name", rapidjson::Value(name.c_str(), alloc).Move(), alloc);
	data.AddMember("size", size, alloc);
	data.AddMember("version", rapidjson::Value(desc.version, alloc), alloc);
	data.AddMember("hash", rapidjson::Value(sha256_string(desc.app_elf_sha256).c_str(), alloc).Move(), alloc);
	data.AddMember("project", rapidjson::Value(desc.project_name, alloc).Move(), alloc);
	data.AddMember("date", rapidjson::Value(desc.date, alloc).Move(), alloc);
	data.AddMember("time", rapidjson::Value(desc.time, alloc).Move(), alloc);
	data.AddMember("magic_number", desc.magic_word, alloc);
	data.AddMember("secure_version", desc.secure_version, alloc);

	return data;
}

template<typename Allocator>
rapidjson::Value& make_image_list(std::filesystem::path const& path,
		rapidjson::Value& data,
		Allocator& alloc) noexcept
{
	data.SetArray();
	for(auto const& f : std::filesystem::directory_iterator(path))
	{
		if(!std::filesystem::is_regular_file(f)) continue;

		std::size_t size = std::filesystem::file_size(f);
		if(size < (sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)))
			continue;

		std::ifstream ifs{f.path(), std::ios::binary};
		if(!ifs)
		{
			continue;
		}

		esp_app_desc_t image_description;
		ifs.seekg(sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t), ifs.beg);
		ifs.read((char*)&image_description, sizeof(esp_app_desc_t));
		ifs.close();

		std::string name = f.path();
		name = name.substr(name.find_last_of('/') + 1);

		rapidjson::Value image;
		data.PushBack(make_image(name, size, image_description, image, alloc), alloc);
	}

	return data;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__ */
