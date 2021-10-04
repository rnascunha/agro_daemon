#ifndef AGRO_DAEMON_AGRO_IMPL_HPP__
#define AGRO_DAEMON_AGRO_IMPL_HPP__

#include "../agro.hpp"

namespace Agro{

template<unsigned TimeoutMs>
void instance::run(int num_threads, CoAP::Error& ecp) noexcept
{
	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc_, SIGINT, SIGTERM);
	bool keep_running = true;
	signals.async_wait(
		[this, &keep_running](boost::system::error_code const&, int){
			ioc_.stop();
			coap_engine_.get_connection().close();
			keep_running = false;
		});


	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(num_threads - 1);
	for(auto i = num_threads - 1; i > 0; --i)
		v.emplace_back(
		[this]
		{
			ioc_.run();
		});

	while(keep_running)
	{
		coap_engine_.run<TimeoutMs>(ecp);
		ioc_.run_for(std::chrono::milliseconds(TimeoutMs));
	}

	// Block until all the threads exit
	for(auto& t : v)
		t.join();
}

}//Agro

#endif /* AGRO_DAEMON_AGRO_IMPL_HPP__ */
