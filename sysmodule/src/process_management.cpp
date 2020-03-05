/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "process_management.h"
#include "file_utils.h"
#include "errors.h"

void ProcessManagement::Initialize()
{
    Result rc = 0;

    rc = pmdmntInitialize();
    ASSERT_RESULT_OK(rc, "pmdmntInitialize");

    rc = pminfoInitialize();
    ASSERT_RESULT_OK(rc, "pminfoInitialize");
}

void ProcessManagement::WaitForQLaunch()
{
    Result rc = 0;
    std::uint64_t pid = 0;
    do
    {
        rc = pmdmntGetProcessId(&pid, PROCESS_MANAGEMENT_QLAUNCH_TID);
        svcSleepThread(500000000ULL);
    } while (R_FAILED(rc));
}

std::uint64_t ProcessManagement::GetCurrentApplicationId()
{
    Result rc = 0;
    std::uint64_t pid = 0;
    std::uint64_t tid = 0;
    rc = pmdmntGetApplicationProcessId(&pid);

    if (rc == 0x20f)
    {
        return PROCESS_MANAGEMENT_QLAUNCH_TID;
    }

    ASSERT_RESULT_OK(rc, "pmdmntGetApplicationProcessId");

    rc = pminfoGetProgramId(&tid, pid);

    if (rc == 0x20f)
    {
        return PROCESS_MANAGEMENT_QLAUNCH_TID;
    }

    ASSERT_RESULT_OK(rc, "pminfoGetProgramId");

    return tid;
}

void ProcessManagement::Exit()
{
    pmdmntExit();
    pminfoExit();
}
