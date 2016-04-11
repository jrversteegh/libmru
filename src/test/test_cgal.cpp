#include <iostream>
#include <sstream>

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
typedef K::Point_3 Point;
typedef K::Aff_transformation_3 Transformation;

class CGALTest: public CppUnit::TestFixture {
  void testVectorArithmetic() {
    Vector v1(1,2,3);
    Vector v2(2,2,3);
    Vector v3 = v1 + v2;
    CPPUNIT_ASSERT_EQUAL(Vector(3, 4, 6), v3);
  }
  void testOutput() {
    Vector v1(1.1,2,3);
    ostringstream os;
    os << v1;
    CPPUNIT_ASSERT_EQUAL(string("1.1 2 3"), os.str());
  }
  void testTransform() {
    Point p1(1, 1, 1);
    Transformation tranformation(
        2, 0, 0, 3,
        0, 3, 0, 4,
        0, 0, 4, 5);
    Point p2 = tranformation(p1);
    CPPUNIT_ASSERT_EQUAL(Point(5, 7, 9), p2);
  }
public:
  CPPUNIT_TEST_SUITE(CGALTest);
  CPPUNIT_TEST(testVectorArithmetic);
  CPPUNIT_TEST(testOutput);
  CPPUNIT_TEST(testTransform);
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

