
#include "RPN.hpp"
#include <stack>
#include <iostream>
#include <sstream>

void RPN::EvaluateContent(std::string content)
{
    std::stack<int> numbers;
    std::istringstream stream(content);
    std::string token;

    while (stream >> token)
    {
        if (token.length() == 1
            && token[0] >= '0' && token[0] <= '9')
        {
            numbers.push(token[0] - '0');
        }
        else if (token.length() == 1
            && (token[0] == '+' || token[0] == '-'
            || token[0] == '*' || token[0] == '/'))
        {
            if (numbers.size() < 2)
            {
                std::cerr << "Error" << std::endl;
                return;
            }

            int right = numbers.top();
            numbers.pop();

            int left = numbers.top();
            numbers.pop();

            if (token[0] == '+')
                numbers.push(left + right);
            else if (token[0] == '-')
                numbers.push(left - right);
            else if (token[0] == '*')
                numbers.push(left * right);
            else if (token[0] == '/')
            {
                if (right == 0)
                {
                    std::cerr << "Error" << std::endl;
                    return;
                }
                numbers.push(left / right);
            }
        }
        else
        {
            std::cerr << "Error" << std::endl;
            return;
        }
    }

    if (numbers.size() != 1)
    {
        std::cerr << "Error" << std::endl;
        return;
    }

    std::cout << numbers.top() << std::endl;
}
