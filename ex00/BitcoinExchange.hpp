
#pragma once

#include <string>
#include <map>


class BitcoinExchange
{
private:
    std::map<std::string, double> database;

public:
    BitcoinExchange();
    bool loadDatabase(const std::string &filename);
    void processInput(const std::string &filename);
};
