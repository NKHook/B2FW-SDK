#include <shlobj_core.h>
#include <stdio.h>
#include <string>
#include <Windows.h>

static HMODULE WinHttp;
static bool(__stdcall* oWinHttpGetIEProxyConfigForCurrentUser)(void*);

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

extern "C" __declspec(dllexport) bool __stdcall WinHttpGetIEProxyConfigForCurrentUser(void* proxyConFig) {
    int result = oWinHttpGetIEProxyConfigForCurrentUser(proxyConFig);
    return result;
}

void initialize(HINSTANCE hinstDLL) {// allocate console window; find original winhttp dll file; find and load it;
// store handle to dll file; find original handle version
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONIN$", "r", stdin);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    //Find original wininet.dll
    char sys32Path[MAX_PATH];

    memset(sys32Path, 0, MAX_PATH);
 
#if INTPTR_MAX == INT64_MAX
    SHGetFolderPathA(nullptr, CSIDL_SYSTEM, nullptr, SHGFP_TYPE_CURRENT, sys32Path);
#elif INTPTR_MAX == INT32_MAX
    SHGetFolderPathA(nullptr, CSIDL_SYSTEMX86, nullptr, SHGFP_TYPE_CURRENT, sys32Path);
#endif

    std::string sys32Str(sys32Path);
    std::string winhttpPath = sys32Str + "\\Winhttp.dll";

    WinHttp = LoadLibraryA(winhttpPath.c_str());
    if (WinHttp == NULL) {
        MessageBoxA(0, "Failed to find Winhttp.dll in System32", "Proxy Error", MB_OK);
        exit(1);
    }
    else {
        printf("Loaded original wininet\n");

    }

    oWinHttpGetIEProxyConfigForCurrentUser = (bool(__stdcall*)(void*))GetProcAddress(WinHttp, "WinHttpGetIEProxyConfigForCurrentUser");
    if (oWinHttpGetIEProxyConfigForCurrentUser == NULL) {
        MessageBoxA(0, "Failed to find WinHttpGetIEProxyConfigForCurrentUser", "Proxy Error", MB_OK);
        exit(1);
    }
}  


// Implement dll main
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        // Write code here
        initialize(hinstDLL);
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break; // do not do cleanup if process termination scenario
        }

        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

//WinHttpGetIEProxyConfigForCurrentUser

/*
1. Write the function
2. Tell linker don't change function name
3. Original function
*/

