#ifndef AGRO_MESH_NOTIFY_MESSAGE_HPP__
#define AGRO_MESH_NOTIFY_MESSAGE_HPP__

#include <string>

namespace Message{

std::string make_public_key(std::string_view const& public_key) noexcept;

}//Message

#endif /* AGRO_MESH_NOTIFY_MESSAGE_HPP__ */
