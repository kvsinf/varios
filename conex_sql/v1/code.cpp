#include <windows.h>
#include <objbase.h>
#include <oledb.h>
#include <msdasc.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "oledb.lib")
#pragma comment(lib, "msdasc.lib")

HWND hEdit;
HWND hButton;

// Version: 1
// Autor  : KVS
// Fecha  : 29-04-2026
//--------------------------------------
// strstr para wchar (sin CRT)
//--------------------------------------
const wchar_t* MiWcsStr(const wchar_t* str, const wchar_t* sub)
{
    if (!*sub) return str;

    for (; *str; str++)
    {
        const wchar_t* h = str;
        const wchar_t* n = sub;

        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }

        if (!*n)
            return str;
    }

    return 0;
}

//--------------------------------------
// Agregar Provider (ahora SQLOLEDB)
//--------------------------------------
void AsegurarProvider(const wchar_t* input, wchar_t* output)
{
    if (MiWcsStr(input, L"Provider="))
    {
        lstrcpyW(output, input);
        return;
    }

    lstrcpyW(output, L"Provider=SQLOLEDB;");
    lstrcatW(output, input);
}

//--------------------------------------
// Test conexión SQL
//--------------------------------------
void TestConexion(HWND hwnd)
{
    wchar_t* buffer = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, 2048 * sizeof(wchar_t));
    wchar_t* connStr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, 4096 * sizeof(wchar_t));

    if (!buffer || !connStr)
    {
        MessageBoxW(hwnd, L"Error de memoria", L"Error", MB_OK);
        return;
    }

    GetWindowTextW(hEdit, buffer, 2048);

    if (lstrlenW(buffer) == 0)
    {
        MessageBoxW(hwnd, L"Ingrese cadena de conexión", L"Atención", MB_OK);
        goto cleanup;
    }

    AsegurarProvider(buffer, connStr);

    IDataInitialize* pIDataInitialize = 0;
    IDBInitialize* pIDBInitialize = 0;

    HRESULT hr = CoCreateInstance(
        CLSID_MSDAINITIALIZE,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IDataInitialize,
        (void**)&pIDataInitialize
    );

    if (FAILED(hr))
    {
        MessageBoxW(hwnd, L"Error creando IDataInitialize", L"Error", MB_OK);
        goto cleanup;
    }

    hr = pIDataInitialize->GetDataSource(
        0,
        CLSCTX_INPROC_SERVER,
        connStr,
        IID_IDBInitialize,
        (IUnknown**)&pIDBInitialize
    );

    if (FAILED(hr))
    {
        MessageBoxW(hwnd, L"Cadena inválida o Provider no disponible", L"Error", MB_OK);
        pIDataInitialize->Release();
        goto cleanup;
    }

    hr = pIDBInitialize->Initialize();

    if (SUCCEEDED(hr))
    {
        MessageBoxW(hwnd, L"Conexión exitosa", L"OK", MB_OK);
        pIDBInitialize->Uninitialize();
    }
    else
    {
        MessageBoxW(hwnd, L"No se pudo conectar", L"Error", MB_OK);
    }

    pIDBInitialize->Release();
    pIDataInitialize->Release();

cleanup:
    if (buffer) HeapFree(GetProcessHeap(), 0, buffer);
    if (connStr) HeapFree(GetProcessHeap(), 0, connStr);
}

//--------------------------------------
// Window Procedure
//--------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        hEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL,
            10, 10, 460, 200,
            hWnd, 0, 0, 0
        );

        hButton = CreateWindowW(
            L"BUTTON",
            L"Test conexión SQL",
            WS_CHILD | WS_VISIBLE,
            10, 220, 200, 30,
            hWnd, (HMENU)1, 0, 0
        );
    }
    break;

    case WM_SIZE:
    {
        int w = LOWORD(lParam);
        int h = HIWORD(lParam);

        MoveWindow(hEdit, 10, 10, w - 20, h - 70, TRUE);
        MoveWindow(hButton, 10, h - 50, 200, 30, TRUE);
    }
    break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            TestConexion(hWnd);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
	
	}

	


    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

//--------------------------------------
// Entry point SIN CRT
//--------------------------------------
void WINAPI WinMainCRTStartup()
{
    HINSTANCE hInstance = GetModuleHandleW(0);

    CoInitialize(0);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"TestSQL";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //  CLAVE

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        L"TestSQL",
        L"Test SQL OLEDB (SQLOLEDB)",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        100, 100, 600, 350,
        0, 0, hInstance, 0
    );

    ShowWindow(hWnd, SW_SHOW);

    MSG msg;
    while (GetMessageW(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoUninitialize();

    ExitProcess(0);
}
