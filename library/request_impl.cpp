#include "acsetup.hpp"
#include "details/request_impl.hpp"

#include <algorithm>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

namespace xiva { namespace details {

template <typename Map>
class map_value_enumeration : public enumeration<typename Map::mapped_type> {

public:
	map_value_enumeration(typename Map::const_iterator begin, typename Map::const_iterator end);
	virtual ~map_value_enumeration();

	virtual bool empty() const;
	virtual typename enumeration<typename Map::mapped_type>::value_type next() const;

private:
	map_value_enumeration(map_value_enumeration const &);
	map_value_enumeration& operator = (map_value_enumeration const &);

private:
	typename Map::const_iterator end_;
	mutable typename Map::const_iterator begin_;
};

template <typename Map> inline bool
has(Map const &map, std::string const &name) {
	BOOST_STATIC_ASSERT((boost::is_same<std::string, typename Map::mapped_type>::value));
	return map.find(name) != map.end();
}

template <typename Map> inline std::string const&
get(Map const &map, std::string const &name) {
	BOOST_STATIC_ASSERT((boost::is_same<std::string, typename Map::mapped_type>::value));
	typename Map::const_iterator i = map.find(name);
	return (map.end() != i) ? i->second : http_constants<char>::empty_string;
}


request_impl::~request_impl() {
}

void
request_impl::swap(request_impl &other) throw () {
	std::swap(uri_, other.uri_);
	std::swap(params_, other.params_);
	std::swap(headers_, other.headers_);
	std::swap(cookies_, other.cookies_);
	std::swap(is_websocket_, other.is_websocket_);
}

bool
request_impl::has_param(std::string const &name) const {
	return has(params_, name);
}

std::string const&
request_impl::param(std::string const &name) const {
	return get(params_, name);
}

enumeration<std::string>::ptr_type
request_impl::params(std::string const &name) const {
	typedef param_map_type::const_iterator iter_type;
	std::pair<iter_type, iter_type> p = params_.equal_range(name);
	return enumeration<std::string>::ptr_type(new map_value_enumeration<param_map_type>(p.first, p.second));
}

bool
request_impl::has_header(std::string const &name) const {
	return has(headers_, name);
}

std::string const&
request_impl::header(std::string const &name) const {
	return get(headers_, name);
}

bool
request_impl::has_cookie(std::string const &name) const {
	return has(cookies_, name);
}

std::string const&
request_impl::cookie(std::string const &name) const {
	return get(cookies_, name);
}

template <typename Map> inline
map_value_enumeration<Map>::map_value_enumeration(typename Map::const_iterator begin, typename Map::const_iterator end) :
	end_(end), begin_(begin)
{
}

template <typename Map> inline
map_value_enumeration<Map>::~map_value_enumeration() {
}

template <typename Map> inline bool
map_value_enumeration<Map>::empty() const {
	return end_ == begin_;
}

template <typename Map> inline typename enumeration<typename Map::mapped_type>::value_type
map_value_enumeration<Map>::next() const {
	return (begin_++)->second;
}

}} // namespaces
