#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/Exception.h>

bool is_xml_output(
	const int argc,
	char* const * argv)
{
	if (1 >= argc)
	{
		return false;
	}

	if (0 != strncmp(argv[1], "xml", 3))
	{
		return false;
	}

	return true;
}

void output(
	const int argc,
	char* const * argv,
	CPPUNIT_NS::TestResultCollector& result)
{
	if (is_xml_output(argc, argv))
	{
		// Print test in a xml format.
		CPPUNIT_NS::XmlOutputter outputter(&result, std::cerr);
		outputter.write();

		return;
	}

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
	outputter.write();
}

bool need_pause(
	const int argc,
	char* const * argv)
{
	if (1 >= argc) return false;
	if (is_xml_output(argc, argv)) return false;

	return true;
}

int main(int argc, char* argv[])
{
	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
	runner.run( controller );

	// Print test in a compiler compatible format.
	output(argc, argv, result);
//	CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
//	outputter.write();

	if (need_pause(argc, argv)) getchar();

	return result.wasSuccessful() ? 0 : 1;
}
