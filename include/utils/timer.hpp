#pragma once

#include <chrono>
#include <iostream>

// A basic scoped timer to measure the duration of a block of code
class Timer {
public:
    inline static bool enabled{false}; // Flag to enable/disable profiling

    Timer(const std::string& label = "Timer")
        : m_label{label}, m_is_running(true), m_start_time(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        stop(); // Ensure the timer stops when the object goes out of scope
    }

    // Stop the timer and print the time elapsed
    void stop() {
        if (enabled && m_is_running) {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time);
            std::cout << "[" << m_label << "] Time elapsed: " << duration.count() << " ms" << std::endl;
            m_is_running = false;
        }
    }

private:
    std::string m_label{};
    bool m_is_running{true}; // Flag to check if the timer is running
    std::chrono::high_resolution_clock::time_point m_start_time;
};