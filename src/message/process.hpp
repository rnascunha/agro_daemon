#ifndef AGRO_MESH_MESSAGE_PROCESS_HPP__
#define AGRO_MESH_MESSAGE_PROCESS_HPP__

#include "rapidjson/document.h"
#include "../instance/agro.hpp"
#include "../user/user.hpp"
#include "../websocket/types.hpp"
#include <string>

//#include "types.hpp"
//#include <map>

namespace Agro{
namespace Message{

void process(std::string&& str,
		websocket_ptr,
		instance&,
		User::Logged&) noexcept;

void process_report(rapidjson::Document const&,
		websocket_ptr ws,
		instance& instance,
		User::Logged& user) noexcept;

void process_image(rapidjson::Document const&, websocket_ptr, instance&) noexcept;
void process_app(rapidjson::Document const&, Agro::websocket_ptr, instance&) noexcept;

//class Process{
//	public:
//		using callback = void(*)(rapidjson::Document const&,
//									websocket_ptr,
//									instance&,
//									User::Logged&) noexcept;
//
//		Process& install(::Message::type, callback) noexcept;
//		bool exec(::Message::type,
//				rapidjson::Document const&,
//				websocket_ptr,
//				instance&,
//				User::Logged&) noexcept;
//	private:
//		std::unordered_map<::Message::type, callback> list_cb_;
//};


}//Message
}//Agro

#endif /* AGRO_MESH_MESSAGE_PROCESS_HPP__ */
