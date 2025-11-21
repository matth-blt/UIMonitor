#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Structure holding information about a single process.
 */
struct ProcessInfo {
    DWORD pid;           ///< Process ID.
    std::string name;    ///< Process executable name.
    double cpuUsage;     ///< CPU usage percentage for this process.
    size_t memoryUsage;  ///< Working Set Size in bytes (RAM usage).
};

/**
 * @brief Class responsible for monitoring running processes.
 * 
 * Uses Toolhelp32Snapshot to list processes and GetProcessTimes/GetProcessMemoryInfo
 * to retrieve statistics for each process.
 */
class ProcessMonitor {
public:
    /**
     * @brief Construct a new Process Monitor object.
     * Initializes system times for delta calculations.
     */
    ProcessMonitor();
    
    /**
     * @brief Updates the list of processes and their statistics.
     * 
     * Takes a snapshot of current processes, calculates CPU usage based on
     * the difference since the last update, and retrieves memory usage.
     */
    void update();
    
    /**
     * @brief Get the sorted list of processes.
     * 
     * @return const std::vector<ProcessInfo>& List of processes sorted by CPU usage.
     */
    const std::vector<ProcessInfo>& getProcesses() const;

private:
    struct ProcessTimeInfo {
        FILETIME lastKernelTime;
        FILETIME lastUserTime;
    };

    // History of process times to calculate CPU usage deltas
    // Using unordered_map for faster lookups (O(1))
    std::unordered_map<DWORD, ProcessTimeInfo> prevProcessTimes;
    
    std::vector<ProcessInfo> processList; ///< Current list of processes.
    
    // System global times for CPU calculation context
    FILETIME prevSysKernel;
    FILETIME prevSysUser;

    /**
     * @brief Helper to convert FILETIME to integer.
     * 
     * @param ft The FILETIME structure.
     * @return unsigned long long The time in 100-nanosecond intervals.
     */
    unsigned long long fileTimeToInt64(const FILETIME& ft) const;
};
