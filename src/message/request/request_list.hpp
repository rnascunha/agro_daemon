#ifndef AGRO_DAEMON_MESSAGE_REQUEST_LIST_HPP__
#define AGRO_DAEMON_MESSAGE_REQUEST_LIST_HPP__

namespace Message{

enum class requests{
	custom,
	ac_load1_on,
	ac_load1_off,
	ac_load2_on,
	ac_load2_off,
	ac_load3_on,
	ac_load3_off,
	reset,
	uptime,
	get_rtc,
	update_rtc,
	get_fuse,
	update_fuse,
	sensor,
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

}//Message

#endif /* AGRO_DAEMON_MESSAGE_REQUEST_LIST_HPP__ */
