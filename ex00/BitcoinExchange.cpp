
#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

static std::string trim(const std::string &str)
{
    std::string::size_type first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
        return "";

    std::string::size_type last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

BitcoinExchange::BitcoinExchange()
{
}

bool BitcoinExchange::loadDatabase(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Error: could not open database." << std::endl;
        return false;
    }

    std::getline(file, line); // Skip the header

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string date;
        std::string rate;
        double exchangeRate;

        if (!std::getline(ss, date, ',') || !std::getline(ss, rate))
            continue;

        std::stringstream rateStream(rate);

        if (!(rateStream >> exchangeRate))
            continue;

        database[date] = exchangeRate;
    }

    return true;
}

void BitcoinExchange::processInput(const std::string &filename)
{
    std::ifstream inputFile(filename.c_str());
    std::string line;

    if (!inputFile.is_open())
    {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::getline(inputFile, line); // Skip the header

    while (std::getline(inputFile, line))
    {
        std::string::size_type separator = line.find('|');

        if (separator == std::string::npos)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date = trim(line.substr(0, separator));
        std::string valueStr = trim(line.substr(separator + 1));

        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::stringstream valueStream(valueStr);
        double value;
        char extra;

        if (!(valueStream >> value) || (valueStream >> extra))
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (value < 0)
        {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }

        if (value > 1000)
        {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        if (database.empty())
        {
            std::cerr << "Error: database is empty." << std::endl;
            continue;
        }

        std::map<std::string, double>::iterator it;
        it = database.lower_bound(date);

        if (it == database.end())
        {
            --it;
        }
        else if (it->first != date)
        {
            if (it == database.begin())
            {
                std::cerr << "Error: no earlier date available for "
                          << date << std::endl;
                continue;
            }
            --it;
        }

        std::cout << date << " => " << value << " = "
                  << std::setprecision(10) << value * it->second
                  << std::endl;
    }
}
