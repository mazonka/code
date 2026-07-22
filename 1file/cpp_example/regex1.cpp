#include <iostream>
#include <regex>
#include <string>

int main()
{
    std::regex date_pattern(R"(\d{2}/\d{2}/\d{4})");
    std::string input = "The date is (31/12/2024).";
    std::smatch match;

    if (std::regex_search(input, match, date_pattern))
    {
        // Extract and handle the matched date
        for (const auto & s : match)
        {
            std::cout << s.str() << std::endl;
        }
    }
    else
    {
        std::cout << "Date not found." << std::endl;
    }

    return 0;
}
