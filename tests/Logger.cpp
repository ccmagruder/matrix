// Copyright 2023 Caleb Magruder

#include <ctime>
#include <iomanip>

#include "Logger.h"

Logger::Logger(std::string filename) : Logger(filename, std::clog) {}

Logger::Logger(std::string filename, std::ostream& ostream)
    : _ostream(ostream), _ofile(filename) {
    if (!_ofile.is_open())
        std::runtime_error("Logger failed to open "+filename);

    _origBuf = _ostream.rdbuf();
    _ostream.rdbuf(_ss.rdbuf());
}

Logger::~Logger() {
    _ostream.rdbuf(_origBuf);
    sync();
    _ofile.close();
}

void Logger::sync() {
    time_t t;
    std::tm time;
    std::string str;
    while (_ss) {
        getline(_ss, str);
        t = std::time(nullptr);
        time = *std::localtime(&t);
        _ofile << std::put_time(&time, "[%H:%M:%S] %d/%m/%y : ");
        _ofile << str << std::endl;
    }
}
