#include "acsetup.hpp"
#include "details/dynamic_loader.hpp"

#include <algorithm>
#include <boost/bind.hpp>

#include "xiva/error.hpp"
#include "xiva/settings.hpp"
#include "xiva/component_set.hpp"

#include "details/resource.hpp"

namespace xiva { namespace details {

struct handle_traits : public default_traits<lt_dlhandle> {
	void destroy(lt_dlhandle handle);
};

dynamic_loader::dynamic_loader() {
	lt_dlinit();
}

dynamic_loader::~dynamic_loader() {
	std::for_each(handles_.begin(), handles_.end(), boost::bind(&lt_dlclose, _1));
	lt_dlexit();
}

void
dynamic_loader::load_module(std::string const &name, settings const &s, component_set &cs) {

	typedef void (*register_func_type)(settings const &, component_set &);
	if (name.empty()) {
		throw error("cannot load empty module");
	}

	resource<lt_dlhandle, handle_traits> handle(lt_dlopen(name.c_str()));
	check_loaded(handle);

	register_func_type func = (register_func_type) lt_dlsym(handle.get(), "xiva_register_module");
	if (!func) {
		throw error("cannot find xiva_register_module function in %s", name.c_str());
	}

	func(s, cs);
	handles_.push_back(handle.get());
	handle.release();
}

void
dynamic_loader::check_loaded(bool condition) const {
	if (!condition) {
		throw error("%s", lt_dlerror());
	}
}

void
handle_traits::destroy(lt_dlhandle handle) {
	lt_dlclose(handle);
}

}} // namespaces
