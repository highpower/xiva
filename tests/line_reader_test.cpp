#include "acsetup.hpp"

#include <list>
#include <string>
#include <vector>

#include <boost/type.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "test_utils.hpp"
#include "details/range.hpp"
#include "details/line_reader.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>, std::list<char>,
range<char const*>, std::string> line_reader_test_types;

BOOST_AUTO_TEST_SUITE(line_reader_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(test_read_lines, Range, line_reader_test_types) {

	char const* values[] = { "123", "456", "789", "12", "345" };
	char const* end_lines[] = { "\n\r", "\r\n", "\r\n\r\n", "\n", "" };

	Range const value = as<Range>("123\n\r456\r\n789\r\n\r\n12\n345");
	typedef typename Range::const_iterator iterator_type;

	range<iterator_type> result, end_line;
	line_reader<iterator_type> reader(value.begin(), value.end());

	for (std::size_t i = 0; reader.read_line(result, end_line); ++i) {
		BOOST_CHECK_EQUAL(values[i], make_string(result));
		BOOST_CHECK_EQUAL(end_lines[i], make_string(end_line));
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_read_spaces, Range, line_reader_test_types) {


	char const* values[] = { "123\n 123\n\t123\n 123", "456", "789" };

	Range const value = as<Range>("123\n 123\n\t123\n 123\n456\n789");
	typedef typename Range::const_iterator iterator_type;

	range<iterator_type> result, end_line;
	line_reader<iterator_type> reader(value.begin(), value.end());

	for (std::size_t i = 0; reader.read_line(result, end_line); ++i) {
		BOOST_CHECK_EQUAL(values[i], make_string(result));
	}
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
