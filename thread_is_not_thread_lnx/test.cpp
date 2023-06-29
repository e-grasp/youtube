#include <thread>

int main()
{
    std::thread t1 {[](){
        while(1);
    }};

    std::thread t2 {[](){
        while(1);
    }};

    t1.join();
    t2.join();

    return 0;
}