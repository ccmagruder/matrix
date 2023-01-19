// Copyright 2023 Caleb Magruder

#pragma once

#include <string>  // std::string

#include "gtest/gtest.h"
#include "Logger.h"

class TestWithLogging : public testing::Test {
 public:
    TestWithLogging();
    ~TestWithLogging();
    // void SetUp() override;
    // void TearDown() override;
 private:
    Logger* log;
    std::string _testname;
    std::string _fixturename;
};
