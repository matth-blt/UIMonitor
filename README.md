# 🖥️ UIMonitor - TUI System Monitor

[![Français](https://img.shields.io/badge/lang-Français-blue.svg)](README_FR.md)

A lightweight, high-performance Terminal User Interface (TUI) System Resource Monitor for Windows, written in C++.
Inspired by tools like `htop` and `btop`, UIMonitor provides real-time insights into your system's performance directly from your console.

## 📋 Features

- ✅ **Real-time CPU Monitoring** - Global CPU utilization percentage.
- ✅ **Memory Usage Tracking** - RAM consumption (Used/Total) with visual gauge.
- ✅ **Process Manager** - Live list of running processes sorted by CPU usage.
- ✅ **High Performance** - Low overhead using native Windows APIs.
- ✅ **Modern TUI** - Beautiful interface powered by FTXUI.

## 🚀 Installation / Build

### Prerequisites
- **Windows 10/11**
- **Visual Studio 2022** (with C++ Desktop Development workload)
- **CMake 3.10+**
- **Git**

### Compilation

1.  **Clone the repository**:
    ```powershell
    git clone https://github.com/matth-blt/UIMonitor.git
    cd UIMonitor
    ```

2.  **Initialize vcpkg**:
    ```powershell
    ./vcpkg/bootstrap-vcpkg.bat
    ```

3.  **Configure the project**:
    ```powershell
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
    ```

4.  **Build**:
    ```powershell
    cmake --build build --config Release
    ```

5.  **Run**:
    ```powershell
    ./build/Release/UIMonitor.exe
    ```

## 📦 Project Structure

```
UIMonitor/
├── include/
│   ├── CpuMonitor.hpp
│   ├── MemoryMonitor.hpp
│   └── ProcessMonitor.hpp
├── src/
│   ├── main.cpp
│   ├── CpuMonitor.cpp
│   ├── MemoryMonitor.cpp
│   └── ProcessMonitor.cpp
├── vcpkg/
├── CMakeLists.txt
├── vcpkg.json
└── LICENSE
```

## 🎨 Detailed Features

### 1️⃣ CPU Monitoring
Calculates global CPU usage using the native Windows API `GetSystemTimes`.
- **Precision**: Calculates the delta between kernel/user times and idle times.
- **Efficiency**: Runs on a separate thread to avoid blocking the UI.

### 2️⃣ Memory Monitoring
Retrieves system memory statistics using `GlobalMemoryStatusEx`.
- **Visuals**: Displays a gauge showing used vs. total physical memory.
- **Stats**: Shows exact percentage and GB usage.

### 3️⃣ Process Manager
Maintains a live list of active processes using `CreateToolhelp32Snapshot`.
- **Sorting**: Automatically sorts the top 20 processes by CPU usage.
- **Optimization**: Uses `std::unordered_map` for O(1) process lookup and delta calculation.
- **Details**: Shows PID, Process Name, CPU %, and Memory Working Set.

### 4️⃣ Terminal UI (FTXUI)
Built with **FTXUI** (Functional Terminal User Interface).
- **Double Buffering**: Flicker-free rendering.
- **Responsive**: Adapts to terminal size.
- **Interactive**: Clean and modern layout.

## 🙏 Acknowledgements

- **[FTXUI](https://github.com/ArthurSonzogni/FTXUI)** by Arthur Sonzogni
- **[vcpkg](https://github.com/microsoft/vcpkg)** by Microsoft
- **Windows API** documentation
