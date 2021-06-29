#ifndef AGRO_DAEMON_RESOURCE_INIT_HPP__
#define AGRO_DAEMON_RESOURCE_INIT_HPP__

#include <vector>
#include "../device/list.hpp"

namespace Resource{

void init(engine&, Device_List&, std::vector<engine::resource_node>&) noexcept;

void put_route_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept;
void put_status_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept;
void put_board_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept;
void put_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept;
void put_full_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept;

}//Resource

#endif /* AGRO_DAEMON_RESOURCE_INIT_HPP__ */
