#ifndef AGRO_DAEMON_WEBSOCKET_TYPES_HPP__
#define AGRO_DAEMON_WEBSOCKET_TYPES_HPP__

#include "share.hpp"
#include "listener.hpp"
#include "websocket.hpp"

namespace Agro{

#if USE_SSL == 0
using websocket = Websocket<false>;
#else /* USE_SSL == 0 */
using websocket = Websocket<true>;
#endif /* USE_SSL == 0 */

using websocket_ptr = std::shared_ptr<websocket>;

using listener = Listener<websocket>;

using share = share<websocket>;
using share_ptr = std::shared_ptr<share>;

}//Agro

#endif /* AGRO_DAEMON_WEBSOCKET_TYPES_HPP__ */
