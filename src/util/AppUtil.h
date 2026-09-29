#pragma once

#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <Psapi.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include <thread>
#include <iostream>

// For removing intellisense, defines comes from cmake.
#define PROJECT_SOURCE_DIR

namespace AppUtil
{

    namespace App{
        
        inline PROCESS_MEMORY_COUNTERS pmc;

        inline double GetAppRamUsageMB(){    
            if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
                return static_cast<double>(pmc.WorkingSetSize / (1024 * 1024));
            return 0;
        }

        inline int PhysicalCoreCount = -1;
        inline int GetPhysicalCoreCount()
        {
            if(PhysicalCoreCount != -1)
                return PhysicalCoreCount;

            DWORD length = 0;
            GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);
            if (length == 0)
                return 0;
        
            std::vector<std::byte> buffer(length);
            auto* info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
            if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &length))
                return 0;
            
        
            int cores = 0;
            DWORD offset = 0;
            while (offset < length)
            {
                auto* entry = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data() + offset);
                ++cores;
                offset += entry->Size;
            }
            PhysicalCoreCount = cores;
            return PhysicalCoreCount;
        }

        inline unsigned int GetLogicalProcessorCount(){
            return std::thread::hardware_concurrency();
        }
   
        inline int ProcessThreadCount = -1;
        inline int GetProcessThreadCount()
        {
            if(ProcessThreadCount != -1)
                return ProcessThreadCount;

            DWORD processID = GetCurrentProcessId();
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            if (snapshot == INVALID_HANDLE_VALUE)
                return 0;
            
            THREADENTRY32 entry{};
            entry.dwSize = sizeof(THREADENTRY32);
            
            int count = 0;
            if (Thread32First(snapshot, &entry))
            {
                do
                {
                    if (entry.th32OwnerProcessID == processID)
                        ++count;
                
                } while (Thread32Next(snapshot, &entry));
            }
        
            CloseHandle(snapshot);

            ProcessThreadCount = count;
            return ProcessThreadCount;
        }
    }

    namespace Path{
        
        inline std::string GetFilename(const std::string& _path)
        {
            return std::filesystem::path(_path).filename().string();
        }

        inline std::vector<std::string> GetAllFilesInDir(const char* _path)
        {
            std::vector<std::string> file_names;
            try {
                if (std::filesystem::exists(_path) && std::filesystem::is_directory(_path)) {
                    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_path)) {
                        if (entry.is_regular_file()) { 
                            file_names.push_back(entry.path().filename().string());
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
            return file_names;
        }

        inline std::string GetFilenameWithoutExtension(const std::string& _path)
        {
            return std::filesystem::path(_path).stem().string();
        }

        inline std::string project_dir(const std::string& _path)
        {
            return (std::filesystem::path(PROJECT_SOURCE_DIR) / _path).string();
        }

        inline std::string asset_dir(const std::string& _path)
        {
            return (std::filesystem::path(ASSET_DIR) / _path).string();
        }

        inline std::string shader_dir(const std::string& _path)
        {
            return (std::filesystem::path(SHADER_DIR) / _path).string();
        }

        inline std::string src_dir(const std::string& _path)
        {
            return (std::filesystem::path(SRC_DIR) / _path).string();
        }

        inline std::vector<std::string> skybox_files(const std::string& _path)
        {
            std::string path = AppUtil::Path::asset_dir(_path);
            return {
                path + "right.png",   // +X
                path + "left.png",    // -X
                path + "top.png",     // +Y
                path + "bottom.png",  // -Y
                path + "front.png",   // +Z
                path + "back.png"     // -Z
            };
        }
    }
}