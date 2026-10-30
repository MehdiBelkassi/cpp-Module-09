#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <iomanip>

BitcoinExchange::BitcoinExchange()
{
}

static std::string trim(const std::string &str)
{
    std::string::size_type first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
        return "";

    std::string::size_type last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

static bool isValidDate(const std::string &date)
{
    // Format must be exactly YYYY-MM-DD
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for (size_t i = 0; i < date.size(); i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }

    int year;
    int month;
    int day;

    std::stringstream yearStream(date.substr(0, 4));
    std::stringstream monthStream(date.substr(5, 2));
    std::stringstream dayStream(date.substr(8, 2));

    yearStream >> year;
    monthStream >> month;
    dayStream >> day;

    if (year < 1 || month < 1 || month > 12 || day < 1)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;

    return day <= daysInMonth[month - 1];
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

    // Check the header
    if (!std::getline(file, line))
    {
        std::cerr << "Error: empty database." << std::endl;
        return false;
    }
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    if (line != "date,exchange_rate")
    {
        std::cerr << "Error: bad database header." << std::endl;
        return false;
    }

    // Read the data lines
    while (std::getline(file, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string date;
        std::string rate;
        double exchangeRate;

        if (!std::getline(ss, date, ',') || !std::getline(ss, rate))
        {
            std::cerr << "Error: bad database line => " << line << std::endl;
            return false;
        }

        if (!isValidDate(date))
        {
            std::cerr << "Error: bad date in database => " << line << std::endl;
            return false;
        }

        std::stringstream rateStream(rate);
        char extra;

        if (!(rateStream >> exchangeRate) || (rateStream >> extra) || exchangeRate < 0)
        {
            std::cerr << "Error: bad rate in database => " << line << std::endl;
            return false;
        }

        database[date] = exchangeRate;
    }

    if (database.empty())
    {
        std::cerr << "Error: database has no data." << std::endl;
        return false;
    }

    return true;
}

// bool BitcoinExchange::loadDatabase(const std::string &filename)
// {
//     std::ifstream file(filename.c_str());
//     std::string line;

//     if (!file.is_open())
//     {
//         std::cerr << "Error: could not open database." << std::endl;
//         return false;
//     }

//     std::getline(file, line);

//     while (std::getline(file, line))
//     {
        
//     }

//     return true;
// }

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
