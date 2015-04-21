#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <CGAL/Vector_3.h>


class CGALTest: public CppUnit::TestFixture {
  void testDummy() {
  }
public:
  CPPUNIT_TEST_SUITE(CGALTest);
  CPPUNIT_TEST(testDummy);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(CGALTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

