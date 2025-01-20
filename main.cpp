#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <thread>

const int DROP_DOWN_MENU_NEW = 1;
const int MENU2 = 2;
const int START = 3;
const int STOP = 4;

DWORD interval = 100;

enum Status { disable, enable };
Status AutoClick = disable;
Status Pause = disable;

// The main window class name.
static TCHAR szWindowClass[] = _T("Clicmate");
static TCHAR szTitle[] = _T("Clicmate");// potential name: Rage Mouse - Daniel

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// This function handles what to do what input from user
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// Application menu
void AddMenu(HWND);
HMENU hMenu;

// Controls function
void AddControls(HWND);
HWND hMilliseconds;
HWND parentHwnd;
// AKA int main(){}
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    // class that sets up window with: application icon, the background color of the window, the name to display in the title bar, etc.
    WNDCLASSEX wcex;
    //filling in class
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    //Error check to make sure our class goes through RegisterClassEx()
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }
    parentHwnd = hWnd;
    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // loop gets input from user
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
    case WM_KEYDOWN://controls keyboard input
        switch (wParam) {
        case VK_F2:
            TCHAR inputMilliseconds[4];
            int addMilliseconds;
            GetWindowText(hMilliseconds, inputMilliseconds, 4);
            addMilliseconds = _ttoi(inputMilliseconds);
            interval = addMilliseconds;
            if (interval < 100) {//in case milliseconds
                interval = 100;
            }
            AutoClick = enable;
            Sleep(100);
            break;
        case VK_F3:
            AutoClick = disable;
            if (Pause) {
                Pause = disable;
            }
            break;
        }

    case WM_COMMAND:// Controls what menu items do
        switch (wParam)
        {

        case DROP_DOWN_MENU_NEW:
            break;
        case MENU2:
            break;
        case START:
            TCHAR inputMilliseconds[4];
            int addMilliseconds;
            GetWindowText(hMilliseconds, inputMilliseconds, 4);
            addMilliseconds = _ttoi(inputMilliseconds);
            interval = addMilliseconds;
            if (interval < 100) {//in case milliseconds = 0
                interval = 100;
            }
            AutoClick = enable;
            Sleep(100);
            break;

        case STOP:
            AutoClick = disable;
            if (Pause) {
                Pause = disable;
            }
            break;

        }
        break;

    case WM_CREATE:
        AddMenu(hWnd);
        AddControls(hWnd);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        if (Pause == enable) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hWnd, &cursorPos); // Convert screen coordinates to client coordinates
            HWND currentChildHwnd = ChildWindowFromPoint(parentHwnd, cursorPos);
            HWND currentParentHwnd;
            if (currentChildHwnd != parentHwnd)
            {
                currentParentHwnd = GetParent(currentChildHwnd);
            }
            else {
                currentParentHwnd = currentChildHwnd;
            }
            if (currentParentHwnd != parentHwnd) {
                AutoClick = enable;
                Pause = disable;
            }
        }
        while (AutoClick == enable && Pause == disable)
        {
                INPUT mouseInputSim[2] = {};
                ZeroMemory(mouseInputSim, sizeof(mouseInputSim));
                mouseInputSim[0].type = INPUT_MOUSE;
                mouseInputSim[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

                mouseInputSim[1].type = INPUT_MOUSE;
                mouseInputSim[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
                if (AutoClick == enable)
                {
                    POINT cursorPos;
                    GetCursorPos(&cursorPos);
                    ScreenToClient(hWnd, &cursorPos); // Convert screen coordinates to client coordinates
                    HWND currentChildHwnd = ChildWindowFromPoint(parentHwnd, cursorPos);
                    HWND currentParentHwnd;
                    if (currentChildHwnd != parentHwnd)
                    {
                        currentParentHwnd = GetParent(currentChildHwnd);
                    }
                    else {
                        currentParentHwnd = currentChildHwnd;
                    }
                    if (currentParentHwnd == parentHwnd) {
                        AutoClick = disable;
                        Pause = enable;
                    }
                    else {
                        std::thread t(SendInput, ARRAYSIZE(mouseInputSim), mouseInputSim, sizeof(INPUT));
                        t.join();
                    }
                    if (GetAsyncKeyState(VK_F3)) {
                        if (Pause) {
                            Pause = disable;
                        }
                        AutoClick = disable;
                    }
                    Sleep(interval);
                }
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}


void AddMenu(HWND hWnd)
{
    hMenu = CreateMenu();

    HMENU hDropDownMenu = CreateMenu();

    //AppendMenu(hDropDownMenu, MF_STRING, DROP_DOWN_MENU_NEW, L"new");// Part of the drop down menu 
    //AppendMenu(hDropDownMenu, MF_SEPARATOR, NULL, NULL);// a visable seperator

    //AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDropDownMenu, L"Menu1");// parameters: (menu object, flag(the type), the value it returns or the drop down menu, the name it displays)
    //AppendMenu(hMenu, MF_STRING, MENU2, L"Menu2");

    SetMenu(hWnd, hMenu);
}
void AddControls(HWND hWnd)
{   
    CreateWindowW(L"static", L"milliseconds:", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 50, 85, 20, hWnd,
        NULL, NULL, NULL);
    hMilliseconds = CreateWindowW(L"Edit", L"100", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 50, 30, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"Button", L"Start(F2)", WS_VISIBLE | WS_CHILD, 200, 204, 100, 50, hWnd, (HMENU)START,
        NULL, NULL);
    CreateWindowW(L"Button", L"Stop(F3)", WS_VISIBLE | WS_CHILD, 200, 259, 100, 50, hWnd, (HMENU)STOP,
        NULL, NULL);
}
