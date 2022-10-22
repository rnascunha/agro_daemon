#include "time_helper.hpp"

long time_epoch_seconds() noexcept
{
	return static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(
		        std::chrono::system_clock::now().time_since_epoch()).count());
}

long time_epoch_miliseconds() noexcept
{
	return static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>(
			        std::chrono::system_clock::now().time_since_epoch()).count());
}
