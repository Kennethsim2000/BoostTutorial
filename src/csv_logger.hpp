#pragma once
#include <fstream>
#include <mutex>
#include "types.hpp"

class CSVLogger
{
    std::mutex mtx;
    std::ofstream ofs_; // output file stream used for writing data to files
public:
    CSVLogger(const std::string &filename)
    {
        ofs_.open(filename, std::ios::out | std::ios::app);
        // std::ios::out is opening a file for writing
        // std::ios::app is ensuring all output operations happen at the end of the file(append)
        if (!ofs_.is_open())
        {
            throw std::runtime_error("Unable to open log file: " + filename);
        }
        ofs_.seekp(0, std::ios::end); // used to set the position of the write pointer in an output stream
        // offset 0, position is relative to the end of the file
        if (ofs_.tellp() == 0)
        { // tellP is used to obtain the position of write pointer
            // this means if the output file stream is empty
            ofs_ << "timestamp_ms, buy_order, sell_order, price, qty\n";
        }
    }
    void log_trade(const Trade &t)
    {
        std::lock_guard<std::mutex> g(mtx);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.ts.time_since_epoch()).count();
        ofs_ << ms << "," << t.buy_order << "," << t.sell_order << "," << t.price << "," << t.qty << "\n";
        ofs_.flush();
    }
};
