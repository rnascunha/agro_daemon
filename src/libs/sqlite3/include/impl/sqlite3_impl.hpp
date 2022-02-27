#ifndef AGRO_DAEMON_LIB_SQLITE3_IMPL_HPP__
#define AGRO_DAEMON_LIB_SQLITE3_IMPL_HPP__

namespace xeds{

template<typename Arg>
int sqlite3::prepare_bind_impl(statement& res, int index, Arg&& arg) noexcept
{
	return res.bind(++index, arg);
}

template<typename Arg, typename ...Args>
int sqlite3::prepare_bind_impl(statement& res, int index, Arg&& arg, Args&& ...args) noexcept
{
	int rc = res.bind(++index, arg);
	if(rc != SQLITE_OK) return rc;
	return prepare_bind_impl(res, index, std::forward<Args>(args)...);
}

template<typename ...Args>
int sqlite3::prepare_bind(const std::string_view& sql_statement, statement& res, Args&& ...args) noexcept
{
	int rc = prepare(sql_statement, res);
	if(rc != SQLITE_OK) return rc;

	return prepare_bind_impl(res, 0, std::forward<Args>(args)...);
}

template<typename Arg>
int sqlite3::statement::bind_impl(int index, Arg&& arg) noexcept
{
	return bind(++index, std::forward<Arg>(arg));
}

template<typename Arg, typename ...Args>
int sqlite3::statement::bind_impl(int index, Arg&& arg, Args&& ...args) noexcept
{
	int rc = bind(++index, arg);
	if(rc != SQLITE_OK) return rc;
	return bind_impl(index, std::forward<Args>(args)...);
}

template<typename ...Args>
int sqlite3::statement::bind_all(Args&&... args) noexcept
{
	return bind_impl(0, std::forward<Args>(args)...);
}

}//xeds

#endif /* AGRO_DAEMON_LIB_SQLITE3_IMPL_HPP__ */
