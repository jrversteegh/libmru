#include "boost_copy_file.h"

namespace test_utils {

void copy_file(const boost::filesystem::path& src, const boost::filesystem::path& tgt) {
  boost::filesystem::copy_file(src, tgt);
}

}
