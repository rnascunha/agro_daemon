#ifndef AGRO_DAEMON_NOTIFY_HPP__
#define AGRO_DAEMON_NOTIFY_HPP__

#include "notify_request.hpp"
#include <string>

bool init_notify(notify_factory& factory) noexcept;
void remove_subscription() noexcept;
void add_subscription(const char* endpoint, const char* p256dh, const char* auth) noexcept;
bool notify(std::string const& payload,
		unsigned expiration = 0, unsigned ttl = 0) noexcept;
bool notify(const void* payload, size_t size,
		unsigned expiration = 0, unsigned ttl = 0) noexcept;

#endif /* AGRO_DAEMON_NOTIFY_HPP__ */
