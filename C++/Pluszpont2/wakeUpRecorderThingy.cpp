#include <iostream>
#include <string>

int main()
{
    struct week
    {
        std::string day;
        std::string time;
    };
    
    week wakeUpTimes[7];

    for (int i = 0; i < 7; i++)
    {
        std::cout << "Enter day " << i + 1 << ": ";
        std::cin >> wakeUpTimes[i].day;
        std::cout << "Enter wake-up time for " << wakeUpTimes[i].day << ": ";
        std::cin >> wakeUpTimes[i].time;
    }
    
    
    for (const auto& wakeUpTime : wakeUpTimes)
    {
        std::cout << wakeUpTime.day << ": " << wakeUpTime.time << std::endl;
    }

    return 0;
}
 
