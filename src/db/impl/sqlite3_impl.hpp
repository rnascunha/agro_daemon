#ifndef AGRO_DAEMON_DB_SQLITE3_IMPL_HPP__
#define AGRO_DAEMON_DB_SQLITE3_IMPL_HPP__

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

//template<typename ...Args>
//int sqlite3::prepare_bind_impl(statement& res,
//		int index,
//		const void* data, int size,
//		Args&& ...args) noexcept
//{
//	int rc = res.bind(++index, data, size);
//	if(rc != SQLITE_OK) return rc;
//	return prepare_bind_impl(res, index, std::forward<Args>(args)...);
//}
//
//template<typename ...Args>
//int sqlite3::prepare_bind_impl(statement& res,
//				int index,
//				const std::uint8_t* data, int size,
//				Args&& ...args) noexcept
//{
//	int rc = res.bind(++index, static_cast<const void*>(data), size);
//	if(rc != SQLITE_OK) return rc;
//	return prepare_bind_impl(res, index, std::forward<Args>(args)...);
//}

template<typename ...Args>
int sqlite3::prepare_bind(const std::string_view& sql_statement, statement& res, Args&& ...args) noexcept
{
	int rc = prepare(sql_statement, res);
	if(rc != SQLITE_OK) return rc;

	return prepare_bind_impl(res, 0, std::forward<Args>(args)...);
}

#endif /* AGRO_DAEMON_DB_SQLITE3_IMPL_HPP__ */
