#pragma once
#include <windows.h>

/**
 * @brief Class responsible for monitoring System Memory (RAM) usage.
 * 
 * Uses Windows API (GlobalMemoryStatusEx) to retrieve memory statistics.
 */
class MemoryMonitor {
public:
    /**
     * @brief Construct a new Memory Monitor object.
     * Initializes the MEMORYSTATUSEX structure size.
     */
    MemoryMonitor();

    /**
     * @brief Updates the memory statistics.
     * Calls GlobalMemoryStatusEx to get the latest memory info.
     */
    void update();
    
    /**
     * @brief Get the Memory Usage Percentage.
     * 
     * @return double The percentage of memory in use (0 to 100).
     */
    double getMemoryUsagePercentage() const;

    /**
     * @brief Get the Total Physical Memory.
     * 
     * @return size_t Total RAM in bytes.
     */
    size_t getTotalMemory() const;

    /**
     * @brief Get the Used Physical Memory.
     * 
     * @return size_t Used RAM in bytes.
     */
    size_t getUsedMemory() const;

    /**
     * @brief Get the Free Physical Memory.
     * 
     * @return size_t Free RAM in bytes.
     */
    size_t getFreeMemory() const;

private:
    MEMORYSTATUSEX memInfo; ///< Structure holding the memory status.
};
