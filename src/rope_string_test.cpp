#include <cppunit/extensions/HelperMacros.h>

#include "rope_string.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// rope_string_test
////////////////////////////////////////////////////////////////////////////////
class rope_string_test : public CPPUNIT_NS::TestFixture
{ 
	CPPUNIT_TEST_SUITE(rope_string_test);

	//--------------------------------------------------------------------------

	// = Basic

	CPPUNIT_TEST(Size_OK);
	CPPUNIT_TEST(Linearized_Basic);
	CPPUNIT_TEST(Substitute_string);
	CPPUNIT_TEST(Substitute_ropestring);
	CPPUNIT_TEST(Substitute_ropestring_myself);

	// = Concat

	CPPUNIT_TEST(Concat_basic_basic);
	CPPUNIT_TEST(Concat_basic_concat);

	//--------------------------------------------------------------------------

	CPPUNIT_TEST_SUITE_END();

private:
	//--------------------------------------------------------------------------
	// テストデータ
	//--------------------------------------------------------------------------

	typedef
	rope_string< char >
	rope;

	rope*	p_;

public:

	void setUp()
	{
		p_ = new rope;
	}

	void
	tearDown()
	{
		delete p_;
	}

	//--------------------------------------------------------------------------
	// Test
	//--------------------------------------------------------------------------

	void
	Size_OK()
	{
		const char str[] = "RopeTest";
		delete p_;

		p_ = new rope(str);

		CPPUNIT_ASSERT_EQUAL(strlen(str), p_->size());
	}

	void
	Linearized_Basic()
	{
		const char str[] = "RopeTest";
		delete p_;

		p_ = new rope(str);

		CPPUNIT_ASSERT_EQUAL(string(str), p_->linearized());
	}

	void
	Substitute_string()
	{
		string str("RopeTest");

		*p_ = str;

		CPPUNIT_ASSERT_EQUAL(str.size(), p_->size());
		CPPUNIT_ASSERT_EQUAL(str, p_->linearized());
	}

	void
	Substitute_ropestring()
	{
		rope rhs("RopeTest2");

		*p_ = rhs;

		CPPUNIT_ASSERT_EQUAL(rhs.size(), p_->size());
		CPPUNIT_ASSERT_EQUAL(rhs.linearized(), p_->linearized());
	}

	void
	Substitute_ropestring_myself()
	{
		rope rhs("RopeTest2");
		*p_ = rhs;

		*p_ = *p_;

		CPPUNIT_ASSERT_EQUAL(rhs.size(), p_->size());
		CPPUNIT_ASSERT_EQUAL(rhs.linearized(), p_->linearized());
	}

	void
	Concat_basic_basic()
	{
		const string lhs_string = "Rope Lhs,";
		const string rhs_string = "Rope Rhs";

		*p_ = lhs_string;
		rope rhs(rhs_string);

		rope r = p_->concat(rhs);

		const string expected = lhs_string + rhs_string;
		CPPUNIT_ASSERT_EQUAL(expected.size(), r.size());
		CPPUNIT_ASSERT_EQUAL(expected, r.linearized());

		CPPUNIT_ASSERT_EQUAL(expected.size(), p_->size());
		CPPUNIT_ASSERT_EQUAL(expected, p_->linearized());
	}

	void
	Concat_basic_concat()
	{
		const string strings[] =
		{
			"1st",
			"2nd",
			"3rd",
		};

		*p_ = strings[0];
		rope rhs(strings[1]);
		rhs.concat(rope(strings[2]));

		const rope r = p_->concat(rhs);

		const string expected = strings[0] + strings[1] + strings[2];
		CPPUNIT_ASSERT_EQUAL(expected.size(), r.size());
		CPPUNIT_ASSERT_EQUAL(expected, r.linearized());

		CPPUNIT_ASSERT_EQUAL(expected.size(), p_->size());
		CPPUNIT_ASSERT_EQUAL(expected, p_->linearized());

	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(rope_string_test);

////////////////////////////////////////////////////////////////////////////////
//	CPPUNIT_ASSERT_EQUAL(expected, actual);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, DELTA);
////////////////////////////////////////////////////////////////////////////////
