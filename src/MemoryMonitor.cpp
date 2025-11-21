#include "MemoryMonitor.hpp"
#include <iostream>

MemoryMonitor::MemoryMonitor() {
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    update();
}

void MemoryMonitor::update() {
    GlobalMemoryStatusEx(&memInfo);
}

double MemoryMonitor::getMemoryUsagePercentage() const {
    return static_cast<double>(memInfo.dwMemoryLoad);
}

size_t MemoryMonitor::getTotalMemory() const {
    return memInfo.ullTotalPhys;
}

size_t MemoryMonitor::getUsedMemory() const {
    return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
}

size_t MemoryMonitor::getFreeMemory() const {
    return memInfo.ullAvailPhys;
}
