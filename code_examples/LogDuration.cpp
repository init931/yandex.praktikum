#include <chrono>
#include <iostream>
#include <thread>

using std::string_literals::operator""s;
using std::chrono_literals::operator""s;

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(std::string operation_name) :
        op_name_(operation_name) {
    }

    ~LogDuration() {
        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr << op_name_ << ": "s << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    const Clock::time_point start_time_ = Clock::now();
    std::string op_name_;
};

int main() {
    std::cout << "Ожидание 5s..."s << std::endl;

    {
        LogDuration sleep_guard("sleep"s);
        // операция - ожидание 5 секунд
        std::this_thread::sleep_for(5s);
    }

    std::cout << "Ожидание завершено"s << std::endl;
    return 0;
}
