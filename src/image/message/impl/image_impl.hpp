#ifndef AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__
#define AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__

#include <fstream>
#include "rapidjson/document.h"

#include "../../image.hpp"

namespace Agro{
namespace Message {

template<typename Allocator>
rapidjson::Value& make_image(std::string const& name,
		std::size_t size,
		esp_app_desc_t const& desc,
		Image const& image,
		rapidjson::Value& data,
		Allocator& alloc) noexcept
{
	data.SetObject();

	data.AddMember("id", image.id, alloc);
	data.AddMember("name",
			rapidjson::Value(name.data(), static_cast<rapidjson::SizeType>(name.size()), alloc).Move(),
			alloc);
	data.AddMember("size", size, alloc);
	data.AddMember("version", rapidjson::Value(desc.version, alloc).Move(), alloc);
	data.AddMember("hash", rapidjson::Value(sha256_string(desc.app_elf_sha256).c_str(), alloc).Move(), alloc);
	data.AddMember("project", rapidjson::Value(desc.project_name, alloc).Move(), alloc);
	data.AddMember("date", rapidjson::Value(desc.date, alloc).Move(), alloc);
	data.AddMember("time", rapidjson::Value(desc.time, alloc).Move(), alloc);
	data.AddMember("magic_number", desc.magic_word, alloc);
	data.AddMember("secure_version", desc.secure_version, alloc);
	data.AddMember("idf_version", rapidjson::Value(desc.idf_ver, alloc).Move(), alloc);
	data.AddMember("description",
			rapidjson::Value(image.description.data(), static_cast<rapidjson::SizeType>(image.description.size()), alloc).Move(),
			alloc);
	data.AddMember("uploader", image.uploader, alloc);
	data.AddMember("date_upload", image.time, alloc);

	return data;
}

template<typename Allocator>
rapidjson::Value& make_image_list(std::filesystem::path const& path,
		instance& instance,
		rapidjson::Value& data,
		Allocator& alloc) noexcept
{
	data.SetArray();
	for(auto const& f : std::filesystem::directory_iterator(path))
	{
		esp_app_desc_t image_description;
		if(!Agro::get_image_description(f.path(), image_description)) continue;
		std::size_t size = std::filesystem::file_size(f);

		std::string name = f.path().string();
		name = name.substr(name.find_last_of('/') + 1);

		Image imagedb;
		if(!instance.read_image(name, imagedb))
		{
			tt::error("Error reading image '%s' info from database", name.c_str());
			continue;
		}

		rapidjson::Value data_image;
		data.PushBack(make_image(name, size, image_description, imagedb, data_image, alloc), alloc);
	}

	return data;
}

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_OTA_IMPL_HPP__ */
