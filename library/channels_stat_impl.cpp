#include "acsetup.hpp"
#include "details/channels_stat_impl.hpp"

#include <cassert>
#include <list>

#include "xiva/channel_id.hpp"

namespace xiva { namespace details {

class list_value_enumeration : public enumeration<std::string> {

public:
	list_value_enumeration();
	virtual ~list_value_enumeration();

	virtual bool empty() const;
	virtual enumeration<std::string>::value_type next() const;

	void push_back(std::string const &value);

private:
	list_value_enumeration(list_value_enumeration const &);
	list_value_enumeration& operator = (list_value_enumeration const &);

private:
	mutable std::list<std::string> lst_;
};

list_value_enumeration::list_value_enumeration()
{
}

list_value_enumeration::~list_value_enumeration() {
}

bool
list_value_enumeration::empty() const {
	return lst_.empty();
}

enumeration<std::string>::value_type
list_value_enumeration::next() const {
	assert(!lst_.empty());

	std::string value = lst_.front();
	lst_.pop_front();
	return value;
}

inline void
list_value_enumeration::push_back(std::string const &value) {
	lst_.push_back(value);
}


channels_stat_impl::channels_stat_impl()
{
}

channels_stat_impl::~channels_stat_impl() {
}

void
channels_stat_impl::inc(channel_id const &ch_id) {

	if (!ch_id.name().empty()) {
		boost::mutex::scoped_lock lock(mutex_);

		std::map<std::string, channel_stat>::iterator it = data_.find(ch_id.name());
		if (data_.end() == it) {
			channel_stat cs;
			cs[ch_id.key()] = 1;
			data_[ch_id.name()] = cs;
		}
		else {
			channel_stat &cs = it->second;
			channel_stat::iterator i = cs.find(ch_id.key());
			if (cs.end() == i) {
				cs[ch_id.key()] = 1;
			}
			else {
				unsigned int &counter = i->second;
				counter++;
			}
		}
	}
}

void
channels_stat_impl::dec(channel_id const &ch_id) {

	if (!ch_id.name().empty()) {
		boost::mutex::scoped_lock lock(mutex_);

		std::map<std::string, channel_stat>::iterator it = data_.find(ch_id.name());
		assert(data_.end() != it);
		channel_stat &cs = it->second;
		channel_stat::iterator i = cs.find(ch_id.key());
		assert(cs.end() != i);
		unsigned int &counter = i->second;
		assert(counter);
		if (!--counter) {
			cs.erase(i);
			if (cs.empty()) {
				data_.erase(it);
			}
		}
	}
}

enumeration<std::string>::ptr_type
channels_stat_impl::load_names() const {

	boost::intrusive_ptr<list_value_enumeration> result(new list_value_enumeration());
	boost::mutex::scoped_lock lock(mutex_);

	for (std::map<std::string, channel_stat>::const_iterator it = data_.begin(), end = data_.end(); it != end; ++it) {
		result->push_back(it->first);
	}
	return result;
}

enumeration<std::string>::ptr_type
channels_stat_impl::load_keys(std::string const &ch_name) const {

	boost::intrusive_ptr<list_value_enumeration> result(new list_value_enumeration());
	if (!ch_name.empty()) {
		boost::mutex::scoped_lock lock(mutex_);

		std::map<std::string, channel_stat>::const_iterator it = data_.find(ch_name);
		if (data_.end() != it) {
			channel_stat const &cs = it->second;
			assert(!cs.empty());
			for (channel_stat::const_iterator i = cs.begin(), end = cs.end(); i != end; ++i) {
				result->push_back(i->first);
			}
		}
	}
	return result;
}

} } // namespaces
