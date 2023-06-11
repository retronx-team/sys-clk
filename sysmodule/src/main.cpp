/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <cstdlib>
#include <cstring>
#include <malloc.h>

#include <switch.h>

#include "errors.h"
#include "file_utils.h"
#include "board.h"
#include "process_management.h"
#include "clock_manager.h"
#include "ipc_service.h"

#define INNER_HEAP_SIZE 0x30000

extern "C"
{
    extern std::uint32_t __start__;

    std::uint32_t __nx_applet_type = AppletType_None;
    TimeServiceType __nx_time_service_type = TimeServiceType_System;
    std::uint32_t __nx_fs_num_sessions = 1;

    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void)
    {
        void* addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        /* Newlib Heap Management */
        extern char* fake_heap_start;
        extern char* fake_heap_end;

        fake_heap_start = (char*)addr;
        fake_heap_end = (char*)addr + size;
    }

    void __appInit(void)
    {
        if (R_FAILED(smInitialize()))
        {
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
        }

        Result rc = setsysInitialize();
        if (R_SUCCEEDED(rc))
        {
            SetSysFirmwareVersion fw;
            rc = setsysGetFirmwareVersion(&fw);
            if (R_SUCCEEDED(rc))
                hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
            setsysExit();
        }
    }

    void __appExit(void)
    {
        smExit();
    }
}

int main(int argc, char** argv)
{
    Result rc = FileUtils::Initialize();
    if (R_FAILED(rc))
    {
        fatalThrow(rc);
        return 1;
    }

    try
    {
        Board::Initialize();
        ProcessManagement::Initialize();

        ProcessManagement::WaitForQLaunch();

        ClockManager* clockMgr = new ClockManager();
        IpcService* ipcSrv = new IpcService(clockMgr);

        FileUtils::LogLine("Ready");

        clockMgr->SetRunning(true);
        clockMgr->GetConfig()->SetEnabled(true);
        ipcSrv->SetRunning(true);

        while (clockMgr->Running())
        {
            clockMgr->Tick();
            clockMgr->WaitForNextTick();
        }

        ipcSrv->SetRunning(false);
        delete ipcSrv;
        delete clockMgr;
        ProcessManagement::Exit();
        Board::Exit();
    }
    catch (const std::exception &ex)
    {
        FileUtils::LogLine("[!] %s", ex.what());
    }
    catch (...)
    {
        std::exception_ptr p = std::current_exception();
        FileUtils::LogLine("[!?] %s", p ? p.__cxa_exception_type()->name() : "...");
    }

    FileUtils::LogLine("Exit");
    svcSleepThread(1000000ULL);
    FileUtils::Exit();
    return 0;
}
