// Copyright 2022 Caleb Magruder

#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>    // std::string

// Redirects an std::ostream (e.g. std::cout) to a user-provided
//     filename. By default Logger redirects std::clog which can
//     be overriden at declaration. The instance's deconstructor
//     will write to the file and revert the ostream's streambuf.
//
// Warning:
//     Logger will only write to file when the instance is destroyed.
//
// Example:
//     Logger log("output.log");
//     std::clog << "Message Here";
//
//     [output.log]
//     [11:51:57] 29/01/22 : Message Here
//
// Example:
//     Logger log("output.log", std::cout);
//     std::cout << "Message Here";
//
//     [output.log]
//     [11:51:57] 29/01/22 : Message Here
//
class Logger {
 public:
    explicit Logger(std::string filename);
    Logger(std::string filename, std::ostream& ostream);

    // Calls sync() before reverting ostream redirection
    ~Logger();

    // Synchronize stream buffer without output file by adding
    // a log file prefix for each line [HH:MM:SS] DD/MM/YY
    void sync();

 private:
    // Output file stream
    std::ofstream _ofile;

    // Creates a stream buffer from ostream
    std::stringstream _ss;

    // Reference to ostream to revert the redirect in dtor
    std::ostream& _ostream;

    // Pointer to ostream's original buffer to be reverted in dtor
    std::streambuf* _origBuf;
};
