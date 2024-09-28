#include <windows.h>
#include <iostream>

const char* PIPE_NAME = R"(\\.\pipe\MyDuplexPipe)";

int main() {
    HANDLE hPipe;
    char buffer[128];
    DWORD bytesRead, bytesWritten;

    // Create a named pipe
    hPipe = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,                  // Number of instances
        128,               // Output buffer size
        128,               // Input buffer size
        0,                 // Client time-out
        NULL               // Default security attributes
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Waiting for client connection..." << std::endl;

    // Wait for a client to connect
    if (!ConnectNamedPipe(hPipe, NULL)) {
        std::cerr << "ConnectNamedPipe failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    while (true) {
        // Read from the pipe
        if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
            std::cerr << "ReadFile failed. Error: " << GetLastError() << std::endl;
            break;
        }
        buffer[bytesRead] = '\0'; // Null-terminate the string
        std::cout << "Received from client: " << buffer << std::endl;

        // Send a response back to the client
        std::string response = "Echo: " + std::string(buffer);
        if (!WriteFile(hPipe, response.c_str(), response.size(), &bytesWritten, NULL)) {
            std::cerr << "WriteFile failed. Error: " << GetLastError() << std::endl;
            break;
        }
    }

    // Cleanup
    CloseHandle(hPipe);
    return 0;
}
