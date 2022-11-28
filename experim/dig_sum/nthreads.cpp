#include <iostream>
#include <thread>

int main()
{
    std::cout << "Number of threads = "
              <<  std::thread::hardware_concurrency() << std::endl;
    std::cout << "ull = "
              <<  (unsigned long long)(-1) << std::endl;
    return 0;
}
