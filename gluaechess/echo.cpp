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
        std::cout << "E(" << input << ")" << std::endl;
        if (input == "exit") break;
    }

    std::cout << "Echo: exit\n";
    return 0;
}
