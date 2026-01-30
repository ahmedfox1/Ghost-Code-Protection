#include <iostream>
#include "InternalStructs.hpp"
#include <windows.h>


constexpr uint64_t HashString(const char* str, uint64_t value = 0xcbf29ce484222325) {
    return *str ? HashString(str + 1, (*str ^ value) * 0x100000001b3) : value;
}

uint32_t CalculateCRC32(const uint8_t* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
        uint8_t byte = data[i];
        crc = crc ^ byte;
        for (int j = 0; j < 8; ++j) {
            uint32_t mask = -(int)(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}


template <typename T>

T GetFuncAddress(uint64_t targetHash) {
    PPEB_FIX peb = (PPEB_FIX)__readgsqword(0x60);

    PPEB_LDR_DATA_FIX ldr = peb->Ldr;
    PLIST_ENTRY head = &ldr->InMemoryOrderModuleList;
    PLIST_ENTRY curr = head->Flink;

    while (curr != head) {
        PLDR_DATA_TABLE_ENTRY_FIX entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY_FIX, InMemoryOrderLinks);

        if (entry->DllBase) {
            PBYTE dllBase = reinterpret_cast<PBYTE>(entry->DllBase);
            PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(dllBase);
            PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(dllBase + dosHeader->e_lfanew);

            DWORD exportDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
            if (exportDirRVA == 0) {
                curr = curr->Flink;
                continue;
            }

            PIMAGE_EXPORT_DIRECTORY exportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(dllBase + exportDirRVA);

            PDWORD addressOfNames = reinterpret_cast<PDWORD>(dllBase + exportDir->AddressOfNames);
            PDWORD addressOfFunctions = reinterpret_cast<PDWORD>(dllBase + exportDir->AddressOfFunctions);
            PWORD addressOfNameOrdinals = reinterpret_cast<PWORD>(dllBase + exportDir->AddressOfNameOrdinals);

            for (DWORD i = 0; i < exportDir->NumberOfNames; ++i) {
                const char* funcName = reinterpret_cast<const char*>(dllBase + addressOfNames[i]);

                if (HashString(funcName) == targetHash) {
                    DWORD funcRVA = addressOfFunctions[addressOfNameOrdinals[i]];
                    return reinterpret_cast<T>(dllBase + funcRVA);
                }
            }
        }
        curr = curr->Flink;
    }
    return nullptr;
}

#pragma optimize("", off)

volatile uint32_t EXPECTED_HASH = 0;

void StartMarker() {}

void CriticalFunction() {
    std::cout << "Checking License Key..." << std::endl;
}

void EndMarker() {}

#pragma optimize("", on)

bool CheckIntegrity() {

    uint8_t* start = (uint8_t*)StartMarker;
    uint8_t* end = (uint8_t*)EndMarker;
    size_t size = end - start;

    uint32_t currentHash = CalculateCRC32(start, size);

    if (currentHash != EXPECTED_HASH) return false;
    return true;
}



bool CheckHardwareBps() {
    CONTEXT ctx = {0};

    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    HANDLE hThread = GetCurrentThread();

    if(GetThreadContext(hThread, &ctx)) {
        if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0) {

            ctx.Dr0 = 0; ctx.Dr1 = 0; ctx.Dr2 = 0; ctx.Dr3 = 0;
            ctx.Dr7 = 0;

            SetThreadContext(hThread, &ctx);
            return true;
        }
    }
    return false;
}




using namespace std;

int main()
{

    if (CheckHardwareBps()) {
      cout << "Debugger Detected!" << endl;
        cin.get();
        return -1;
    }


    if (!CheckIntegrity()) {
        cout << "Code has been patched." <<endl;
    }

    CriticalFunction();

    cin.get();
    return 0;



}