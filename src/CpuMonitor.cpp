#include "CpuMonitor.hpp"
#include <iostream>

CpuMonitor::CpuMonitor() : globalUsage(0.0) {
    GetSystemTimes(&prevIdleTime, &prevKernelTime, &prevUserTime);
}

unsigned long long CpuMonitor::fileTimeToInt64(const FILETIME& ft) const {
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

void CpuMonitor::update() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return;
    }

    unsigned long long idle = fileTimeToInt64(idleTime) - fileTimeToInt64(prevIdleTime);
    unsigned long long kernel = fileTimeToInt64(kernelTime) - fileTimeToInt64(prevKernelTime);
    unsigned long long user = fileTimeToInt64(userTime) - fileTimeToInt64(prevUserTime);

    // KernelTime includes IdleTime in some definitions, but GetSystemTimes docs say:
    // "The amount of time that the system has spent in Kernel mode (including all threads in all processes, on all processors)."
    // "The amount of time the system has been idle."
    // Usually: Total = Kernel + User. 
    // But Kernel includes Idle? 
    // MSDN says: "The sum of the kernel and user times is the total amount of time..."
    // However, usually KernelTime returned by GetSystemTimes INCLUDES IdleTime.
    // Let's verify the standard formula for GetSystemTimes.
    // Formula: 
    // Total = (Kernel - OldKernel) + (User - OldUser)
    // Idle = (Idle - OldIdle)
    // Usage = (Total - Idle) / Total * 100.0

    unsigned long long totalSystem = kernel + user;
    
    if (totalSystem > 0) {
        globalUsage = (double)(totalSystem - idle) / (double)totalSystem * 100.0;
    } else {
        globalUsage = 0.0;
    }

    prevIdleTime = idleTime;
    prevKernelTime = kernelTime;
    prevUserTime = userTime;
}

double CpuMonitor::getGlobalUsage() const {
    return globalUsage;
}
