#include <windows.h>
#include <iostream>
#include <string>

const char* PIPE_NAME = R"(\\.\pipe\MyDuplexPipe)";

int main() {
    HANDLE hPipe;
    char buffer[128];
    DWORD bytesRead, bytesWritten;

    // Connect to the named pipe
    hPipe = CreateFileA(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,              // No sharing
        NULL,           // Default security attributes
        OPEN_EXISTING,  // Opens existing pipe
        0,              // Default attributes
        NULL            // No template file
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    while (true) {
        // Get user input
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;

        // Send the input to the server
        if (!WriteFile(hPipe, input.c_str(), input.size(), &bytesWritten, NULL)) {
            std::cerr << "WriteFile failed. Error: " << GetLastError() << std::endl;
            break;
        }

        // Read the response from the server
        if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            std::cerr << "ReadFile failed. Error: " << GetLastError() << std::endl;
            break;
        }
        buffer[bytesRead] = '\0'; // Null-terminate the string
        std::cout << "Received from server: " << buffer << std::endl;
    }

    // Cleanup
    CloseHandle(hPipe);
    return 0;
}
