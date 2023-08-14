#pragma once


// Standard lib
#include <chrono>


namespace Timer {
  inline double GetElapsedTime() {
    static std::chrono::high_resolution_clock::time_point timeOld= std::chrono::high_resolution_clock::now();
    static std::chrono::high_resolution_clock::time_point timeNew= std::chrono::high_resolution_clock::now();
    timeNew= std::chrono::high_resolution_clock::now();
    double elapsed= std::chrono::duration<double>(timeNew - timeOld).count();
    timeOld= std::chrono::high_resolution_clock::now();
    return elapsed;
  }
}  // namespace Timer
