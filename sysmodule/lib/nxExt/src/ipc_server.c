/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/ipc_server.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

Result ipcServerInit(IpcServer* server, const char* name, u32 max_sessions)
{
    if(max_sessions < 1 || max_sessions > (MAX_WAIT_OBJECTS - 1))
    {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    strncpy(server->name, name, sizeof(server->name) - 1);
    server->max = max_sessions + 1;
    server->count = 0;

    Result rc = smRegisterService(&server->handles[0], server->name, false, max_sessions);
    if(R_SUCCEEDED(rc))
    {
        server->count = 1;
    }
    return rc;
}

Result ipcServerExit(IpcServer* server)
{
    for(u32 i = 0; i < server->count; i++)
    {
        svcCloseHandle(server->handles[i]);
    }
    server->count = 0;
    return smUnregisterService(server->name);
}

static Result _ipcServerAddSession(IpcServer* server, Handle session)
{
    if(server->count >= server->max)
    {
        return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
    }

    server->handles[server->count] = session;
    server->count++;
    return 0;
}

static Result _ipcServerDeleteSession(IpcServer* server, u32 index)
{
    if(!index || index >= server->count)
    {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    for(u32 j = index; j < (server->count - 1); j++)
    {
        server->handles[j] = server->handles[j + 1];
    }
    server->count--;
    return 0;
}

static Result _ipcServerParseRequest(IpcServerRequest* r)
{
    memset(r, 0, sizeof(IpcServerRequest));
    Result rc = ipcParse(&r->command);
    if(R_FAILED(rc))
    {
        return rc;
    }

    r->data.cmdId = 0;
    r->data.size = 0;
    r->data.ptr =  NULL;

    if(r->command.CommandType == IpcCommandType_Request)
    {
        IpcServerRawHeader *header = r->command.Raw;
        if(r->command.RawSize < sizeof(IpcServerRawHeader) || header->magic != SFCI_MAGIC)
        {
            return MAKERESULT(Module_Libnx, LibnxError_BadInput);
        }

        r->data.cmdId = header->cmdId;
        if(r->command.RawSize > sizeof(IpcServerRawHeader))
        {
            r->data.size = r->command.RawSize - sizeof(IpcServerRawHeader);
            r->data.ptr = ((u8*)r->command.Raw) + sizeof(IpcServerRawHeader);
        }
    }

    return 0;
}

static void _ipcServerPrepareResponse(Result rc, void* data, size_t dataSize)
{
    IpcCommand c;
    ipcInitialize(&c);
    IpcServerRawHeader* rawHeader = ipcPrepareHeader(&c, sizeof(IpcServerRawHeader) + dataSize);
    rawHeader->magic = SFCO_MAGIC;
    rawHeader->result = rc;
    if(R_SUCCEEDED(rc))
    {
        memcpy(((u8*)rawHeader) + sizeof(IpcServerRawHeader), data, dataSize);
    }
}

static Result _ipcServerProcessNewSession(IpcServer* server)
{
    Handle session;
    Result rc = svcAcceptSession(&session, server->handles[0]);
    if(R_SUCCEEDED(rc) && R_FAILED(rc = _ipcServerAddSession(server, session)))
    {
        svcCloseHandle(session);
    }
    return rc;
}

static Result _ipcServerProcessSession(IpcServer* server, IpcServerRequestHandler handler, void* userdata, u32 handleIndex)
{
    s32 unusedIndex;
    IpcServerRequest r;
    IpcCommand c;
    size_t dataSize = 0;
    u8 data[IPC_SERVER_EXT_RESPONSE_MAX_DATA_SIZE];
    bool close = false;

    ipcInitialize(&c);
    ipcPrepareHeader(&c, 0);

    Result rc = svcReplyAndReceive(&unusedIndex, &server->handles[handleIndex], 1, 0, UINT64_MAX);
    if(R_SUCCEEDED(rc))
    {
        rc = _ipcServerParseRequest(&r);
    }

    if(R_SUCCEEDED(rc))
    {
        switch(r.command.CommandType)
        {
            case IpcCommandType_Request:
                _ipcServerPrepareResponse(
                    handler(userdata, &r, data, &dataSize),
                    data,
                    dataSize
                );
                break;
            case IpcCommandType_Close:
                _ipcServerPrepareResponse(0, NULL, 0);
                close = true;
                break;
            default:
                _ipcServerPrepareResponse(MAKERESULT(11, 403), NULL, 0);
                close = true;
                break;
        }

        rc = svcReplyAndReceive(&unusedIndex, &server->handles[handleIndex], 0, server->handles[handleIndex], 0);
        if(rc == KERNELRESULT(TimedOut))
        {
            rc = 0;
        }
    }

    if(R_FAILED(rc) || close)
    {
        _ipcServerDeleteSession(server, handleIndex);
    }

    return rc;
}

Result ipcServerProcess(IpcServer* server, IpcServerRequestHandler handler, void* userdata)
{
    s32 handleIndex = -1;
    Result rc = svcWaitSynchronization(&handleIndex, server->handles, server->count, UINT64_MAX);

    if(R_SUCCEEDED(rc) && (handleIndex < 0 || handleIndex >= server->count))
    {
        rc = MAKERESULT(Module_Libnx, LibnxError_NotFound);
    }

    if(R_SUCCEEDED(rc))
    {
        if(handleIndex)
        {
            rc = _ipcServerProcessSession(server, handler, userdata, handleIndex);
        }
        else
        {
            rc = _ipcServerProcessNewSession(server);
        }
    }

    return rc;
}

