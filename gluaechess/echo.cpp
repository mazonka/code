#include <iostream>
#include <string>

int main()
{
    std::string input;

    while (true)
    {
        // Read input from stdin
        std::getline(std::cin, input);

        // Process input and send output back to parent
        std::cout << "Echo: (" << input << ")" << std::endl;
        if (input == "bye") break;
    }

    std::cout << "Echo: exiting\n";
    return 0;
}
