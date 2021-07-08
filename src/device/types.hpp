#ifndef AGRO_MESH_DEVICE_TYPES_HPP__
#define AGRO_MESH_DEVICE_TYPES_HPP__

#include <cstdint>
#include <vector>
#include "../resources/types.hpp"
#include "../error.hpp"
#include <system_error>

struct mesh_addr_t{
	static constexpr const unsigned str_size = 18;

	std::uint8_t addr[6];    /**< mac address */

	mesh_addr_t();
	mesh_addr_t(mesh_addr_t const&);
	mesh_addr_t(std::uint8_t const*);
	mesh_addr_t(const char*, unsigned, std::error_code&);

    bool set(const char*, unsigned) noexcept;

    bool operator==(mesh_addr_t const& rhs) const noexcept;
	bool operator!=(mesh_addr_t const& rhs) const noexcept;
	/**
	 * Needed to be used as a key to a map
	 */
	bool operator<(mesh_addr_t const& rhs) const noexcept;

	mesh_addr_t& operator=(mesh_addr_t const&) noexcept;
	mesh_addr_t& operator=(Resource::mesh_addr_t const&) noexcept;

	char* to_string(char*, unsigned = 18) const noexcept;
	std::string to_string() const noexcept;
};

using value_time = std::uint32_t;

value_time get_time() noexcept;

template<typename Number>
struct Value{
	Value() = default;
	Value(value_time t, Number n);

	value_time		time = 0;
	Number			value = 0;

	void update(Number) noexcept;
	void update(value_time, Number) noexcept;
};

template<typename Number,
		unsigned Max = 20>
class Value_List{
	public:
		using value = Value<std::remove_reference_t<Number>>;

		void add(value_time, Number) noexcept;
		void add(Number) noexcept;
		std::size_t size() const noexcept;

		/**
		 * It will add only if a change of the value happened
		 */
		bool add_change(value_time, Number) noexcept;
		bool add_change(Number) noexcept;

		typedef typename std::vector<value>::iterator iterator;
		iterator begin();
		iterator end();

		typedef typename std::vector<value>::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		value const& operator[](unsigned) const noexcept;
	private:
		std::vector<value> list_;
};

#include "impl/types_impl.hpp"

#endif /* AGRO_MESH_DEVICE_TYPES_HPP__ */