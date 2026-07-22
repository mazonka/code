#include <iostream>
#include <regex>
#include <string>

int main()
{
    std::regex amount_pattern(R"([\d]{1,3}(?:,\d{1,3})?\.[\d]{2})");
    // Using std::regex with the std::regex_constants::icase
    // flag for case-insensitive matching
    std::smatch match;

    std::string input = "The price is $93.19, ($1,000.99) on sale.";

    if (std::regex_search(input, match, amount_pattern))
    {
        // Process each match found in 'match'
        for (const auto & s : match)
        {
            std::cout << s.str() << std::endl;
        }
    }
    else
    {
        std::cout << "No amount with dollar signs and cents found." << std::endl;
    }

    return 0;
}