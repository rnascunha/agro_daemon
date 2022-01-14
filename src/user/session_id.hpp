#ifndef AGRO_DAEMON_USER_SESSION_ID_HPP__
#define AGRO_DAEMON_USER_SESSION_ID_HPP__

#include <string>

namespace Agro{

template<unsigned Size>
std::string generate_session_id() noexcept;

}//Agro

#include "impl/session_id_impl.hpp"

#endif /* AGRO_DAEMON_USER_SESSION_ID_HPP__ */
