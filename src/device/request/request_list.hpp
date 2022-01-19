#ifndef AGRO_DAEMON_DEVICE_REQUEST_LIST_HPP__
#define AGRO_DAEMON_DEVICE_REQUEST_LIST_HPP__

namespace Agro{
namespace Device{
namespace Request{

enum class request_type{
	custom,
	ac_load1_on,
	ac_load1_off,
	ac_load2_on,
	ac_load2_off,
	ac_load3_on,
	ac_load3_off,
	reset,
	uptime,
	reset_reason,
	get_rtc,
	update_rtc,
	get_fuse,
	update_fuse,
	waive_root,
	sensors,
	route,
	board,
	config,
	full_config,
	get_ota,
	update_ota,
	send_job,
	get_job,
	delete_job,
	get_app,
	send_app,
	run_app,
	delete_app
};

}//Request
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_REQUEST_LIST_HPP__ */
