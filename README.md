# 👻 Ghost Code Protection: Advanced Anti-Reverse Engineering

![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## 📜 Overview
This project demonstrates advanced **C++ software protection techniques** designed to evade static analysis tools (like **Ghidra** & **IDA Pro**) and dynamic debuggers (like **x64dbg**). The goal is to turn the binary into a "Black Box" that is difficult to analyze or tamper with.

**🎥 Watch the full tutorial on YouTube:** [LINK TO YOUR VIDEO HERE]

---

## 🛡️ Features (The 3 Modules)

### 1. The Ghost (API Hashing & Obfuscation) 👻
- **Concept:** Instead of using standard Windows APIs (e.g., `MessageBoxA`), which appear in the Import Table, we use **Runtime API Hashing**.
- **Technique:** Custom implementation of `GetProcAddress` using **PEB Walking** and **MurmurHash3**.
- **Result:** Static analysis tools show **ZERO imports**. The code appears "blind" to the OS.

### 2. The Guardian (Code Integrity Check) ⚔️
- **Concept:** The program verifies its own memory integrity at runtime.
- **Technique:** Calculates **CRC32** checksum of the `.text` section in memory.
- **Result:** If a hacker tries to patch a single byte (e.g., changing `JZ` to `JNZ`), the hash changes, and the program self-destructs or crashes intentionally.

### 3. The Hunter (Anti-Debugging) 🕸️
- **Concept:** Detects advanced debugging attempts.
- **Technique:** Scans CPU **Hardware Debug Registers (DR0 - DR7)** using `GetThreadContext`.
- **Result:** Detects "Hardware Breakpoints" often used by professionals to bypass standard anti-debug checks.

---

## 🚀 How to Build

### Prerequisites
- Visual Studio 2019/2022.
- C++17 or later standard.

### Steps
1. Clone the repository:
   ```bash
   git clone [https://github.com/AhmedSameh/Ghost-Code-Protection.git](https://github.com/AhmedSameh/Ghost-Code-Protection.git)
