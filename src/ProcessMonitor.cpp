#include "ProcessMonitor.hpp"
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <algorithm>
#include <unordered_set>

// Helper to convert WCHAR (Unicode) to std::string (UTF-8)
// Necessary because Windows API uses WCHAR when UNICODE is defined
std::string WCharToString(const WCHAR* wstr) {
    if (!wstr) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_needed <= 0) return "";
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, NULL, NULL);
    // Remove null terminator
    if (!strTo.empty() && strTo.back() == '\0') strTo.pop_back();
    return strTo;
}

ProcessMonitor::ProcessMonitor() {
    // Initialize system times to avoid huge spikes on first update
    GetSystemTimes(NULL, &prevSysKernel, &prevSysUser);
}

unsigned long long ProcessMonitor::fileTimeToInt64(const FILETIME& ft) const {
    // Combine high and low parts of FILETIME
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

const std::vector<ProcessInfo>& ProcessMonitor::getProcesses() const {
    return processList;
}

void ProcessMonitor::update() {
    std::vector<ProcessInfo> newProcessList;
    std::unordered_set<DWORD> currentPids;
    
    // 1. Get current System Times (Global CPU usage context)
    FILETIME sysIdle, sysKernel, sysUser;
    if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser)) {
        return; // Failed to get system times
    }

    unsigned long long sysKernelDiff = fileTimeToInt64(sysKernel) - fileTimeToInt64(prevSysKernel);
    unsigned long long sysUserDiff = fileTimeToInt64(sysUser) - fileTimeToInt64(prevSysUser);
    unsigned long long sysTotalDiff = sysKernelDiff + sysUserDiff;

    // Update previous system times for next iteration
    prevSysKernel = sysKernel;
    prevSysUser = sysUser;

    // 2. Snapshot processes
    // TH32CS_SNAPPROCESS captures all processes in the system
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    do {
        ProcessInfo pInfo;
        pInfo.pid = pe32.th32ProcessID;
        
        // Fix: Convert WCHAR string to std::string (UTF-8)
        #ifdef UNICODE
            pInfo.name = WCharToString(pe32.szExeFile);
        #else
            pInfo.name = pe32.szExeFile;
        #endif

        pInfo.cpuUsage = 0.0;
        pInfo.memoryUsage = 0;
        
        currentPids.insert(pInfo.pid);

        // Open process to get stats
        // PROCESS_QUERY_INFORMATION: Required for GetProcessTimes
        // PROCESS_VM_READ: Required for GetProcessMemoryInfo
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        
        if (hProcess) {
            // Memory Usage (Working Set)
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                pInfo.memoryUsage = pmc.WorkingSetSize;
            }

            // CPU Usage
            FILETIME ftCreation, ftExit, ftKernel, ftUser;
            if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                // Check if we have history for this PID to calculate delta
                if (prevProcessTimes.count(pInfo.pid)) {
                    ProcessTimeInfo& prev = prevProcessTimes[pInfo.pid];
                    
                    unsigned long long procKernelDiff = fileTimeToInt64(ftKernel) - fileTimeToInt64(prev.lastKernelTime);
                    unsigned long long procUserDiff = fileTimeToInt64(ftUser) - fileTimeToInt64(prev.lastUserTime);
                    unsigned long long procTotalDiff = procKernelDiff + procUserDiff;

                    // Calculate CPU % for this process relative to total system time
                    if (sysTotalDiff > 0) {
                        pInfo.cpuUsage = (double)procTotalDiff / (double)sysTotalDiff * 100.0;
                    }
                }

                // Update history for this PID
                prevProcessTimes[pInfo.pid] = {ftKernel, ftUser};
            }

            CloseHandle(hProcess);
        } 
        // Note: Access Denied is common for system processes (PID 0, 4, etc.)

        newProcessList.push_back(pInfo);

    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);

    // Optimization: Clean up old processes from map to prevent memory leak
    // We iterate over the map and remove PIDs that were not seen in this snapshot
    for (auto it = prevProcessTimes.begin(); it != prevProcessTimes.end(); ) {
        if (currentPids.find(it->first) == currentPids.end()) {
            it = prevProcessTimes.erase(it);
        } else {
            ++it;
        }
    }
    
    // Sort by CPU usage descending to show most active processes first
    std::sort(newProcessList.begin(), newProcessList.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.cpuUsage > b.cpuUsage;
    });

    processList = newProcessList;
}
