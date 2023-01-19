// Copyright 2023 Caleb Magruder

#include <filesystem>

#include "TestWithLogging.h"

TestWithLogging::TestWithLogging() {
    _testname = testing::UnitTest::GetInstance()
        ->current_test_info()->name();
    // in parameterized tests testname is followed by /i
    // which is invalid because _testname is not a directory
    
    // _testname[_testname.find('/')] = '.';

    // comes in the form of e.g. tDisplay/tDisplay
    _fixturename = testing::UnitTest::GetInstance()
        ->current_test_info()->test_suite_name();
    // discard everything after and including '/'
    _fixturename = _fixturename.substr(0, _fixturename.find("/"));

    // store original path
    std::string original_path = std::filesystem::current_path();
    // create log/(_fixturename) directory
    std::filesystem::create_directory("log");
    // change directory to log/ to create fixturename subdirectory
    std::filesystem::current_path(original_path + "/log");
    std::filesystem::create_directory(_fixturename);
    std::string logfilename = _fixturename + "/" + _testname + ".log";
    log = new Logger(logfilename);
    // restore original path
    std::filesystem::current_path(original_path);

    std::clog << _fixturename
        + "(" + _testname + ")TestWithLogging::TestWithLogging()\n";
}

TestWithLogging::~TestWithLogging() {
    std::clog << _fixturename
        + "(" + _testname + ")TestWithLogging::~TestWithLogging()\n";
    delete log;
}
