#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <CGAL/Vector_3.h>
#include <CGAL/Simple_cartesian.h>

using namespace std;
using namespace CGAL;

typedef Simple_cartesian<float> K;
typedef K::Vector_3 Vector;

class CGALTest: public CppUnit::TestFixture {
  void testVectorArithmetic() {
    Vector v1(1,2,3);
    Vector v2(2,2,3);
    Vector v3 = v1 + v2;
    CPPUNIT_ASSERT_EQUAL(Vector(3, 4, 6), v3);
  }
public:
  CPPUNIT_TEST_SUITE(CGALTest);
  CPPUNIT_TEST(testVectorArithmetic);
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

