#ifndef BOOST_COPY_FILE_INCLUDED
#define BOOST_COPY_FILE_INCLUDED

#include <boost/filesystem.hpp>

namespace test_utils {

extern void copy_file(const boost::filesystem::path& src, const boost::filesystem::path& tgt);

}

#endif
