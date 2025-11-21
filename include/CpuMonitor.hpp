#pragma once
#include <windows.h>
#include <vector>
#include <string>

/**
 * @brief Class responsible for monitoring CPU usage.
 * 
 * Uses Windows API (GetSystemTimes) to calculate the global CPU usage percentage.
 */
class CpuMonitor {
public:
    /**
     * @brief Construct a new Cpu Monitor object.
     * Initializes the previous system times to current values.
     */
    CpuMonitor();

    /**
     * @brief Updates the CPU usage statistics.
     * Calls GetSystemTimes and calculates the usage based on the difference
     * between the current and previous measurements.
     */
    void update();

    /**
     * @brief Get the calculated Global CPU Usage.
     * 
     * @return double The CPU usage percentage (0.0 to 100.0).
     */
    double getGlobalUsage() const;

private:
    FILETIME prevIdleTime;   ///< Previous system idle time.
    FILETIME prevKernelTime; ///< Previous system kernel time.
    FILETIME prevUserTime;   ///< Previous system user time.
    
    double globalUsage;      ///< Current calculated global CPU usage %.

    /**
     * @brief Helper to convert FILETIME structure to a 64-bit integer.
     * 
     * @param ft The FILETIME structure.
     * @return unsigned long long The time in 100-nanosecond intervals.
     */
    unsigned long long fileTimeToInt64(const FILETIME& ft) const;
};
