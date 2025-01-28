#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "resource.h"
#include "mouseClicks.h"
const int DROP_DOWN_MENU_NEW = 1;
const int MENU2 = 2;
const int START = 3;
const int STOP = 4;
const int MOUSE_BUTTON_SELECT = 5;
const int LEFT_BUTTON = 6;
const int RIGHT_BUTTON = 7;
const int MILLISECONDS_IN_A_DAY = 86400000;
enum Status { disable, enable };
Status AutoClick = disable;
Status Pause = disable;

MouseClick userMouse;

HBRUSH hbrBkgnd = NULL;
// The main window class name.
static TCHAR szWindowClass[] = _T("Clicmate");
static TCHAR szTitle[] = _T("MouseSiml");// potential name: Rage Mouse - Daniel

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

HANDLE hThread;

// This function handles what to do what input from user
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI clickingThreadFunc(LPVOID lpParam);

// Application menu
void AddMenu(HWND);
HMENU hMenu;

// Controls function
void AddControls(HWND);
HWND hMilliseconds;
HWND hSeconds;
HWND hMinutes;
HWND hHours;
HWND parentHwnd;
DWORD interval;
void GetInterval(DWORD& interval);
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
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
        case LEFT_BUTTON:
            userMouse.SetLeftClick();
            break;
        case RIGHT_BUTTON:
            userMouse.SetRightClick();
            break;
        case START:
            GetInterval(interval);
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
        hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255)); // Create a white brush
        break;
    case WM_CTLCOLORSTATIC:
        hdc = (HDC)wParam;
        SetBkColor(hdc, RGB(255, 255, 255)); // Set the background color to white
        return (INT_PTR)hbrBkgnd;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        MoveToEx(hdc, 50, 40, NULL);
        LineTo(hdc, 410, 40);
        MoveToEx(hdc, 50, 75, NULL);
        LineTo(hdc, 410, 75);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        if (hbrBkgnd) {
            DeleteObject(hbrBkgnd); // Clean up the brush
        }
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
                if (!hThread)
                {
                    hThread = CreateThread(
                        NULL,                   // default security attributes
                        0,                      // use default stack size  
                        clickingThreadFunc,       // thread function name
                        NULL,                   // argument to thread function 
                        0,                      // use default creation flags 
                        NULL);
                }
                DWORD threadStatus = WaitForSingleObject(hThread, 0);
                if (threadStatus == WAIT_OBJECT_0) {
                    CloseHandle(hThread);
                    hThread = NULL;
                }
            }
            if (GetAsyncKeyState(VK_F3)) {
                if (Pause) {
                    Pause = disable;
                }
                AutoClick = disable;
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
    CreateWindowW(L"static", L"Interval", WS_VISIBLE | WS_CHILD | SS_LEFT, 50, 20, 60, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"milliseconds:", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 50, 85, 20, hWnd,
        NULL, NULL, NULL);
    hMilliseconds = CreateWindowW(L"Edit", L"100", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 50, 30, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"seconds:", WS_VISIBLE | WS_CHILD | SS_CENTER, 170, 50, 60, 20, hWnd,
        NULL, NULL, NULL);
    hSeconds = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | WS_BORDER, 235, 50, 20, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"minutes:", WS_VISIBLE | WS_CHILD | SS_CENTER, 255, 50, 60, 20, hWnd,
        NULL, NULL, NULL);
    hMinutes = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | WS_BORDER, 320, 50, 20, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"hours:", WS_VISIBLE | WS_CHILD | SS_CENTER, 340, 50, 45, 20, hWnd,
        NULL, NULL, NULL);
    hHours = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | WS_BORDER, 390, 50, 20, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"Button", L"Mouse Button", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 50, 80, 110, 75, hWnd, 
        NULL, NULL, NULL);
    HWND hLeftButton = CreateWindowW(L"Button", L"left-click", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,60, 100, 80, 20, hWnd,
        (HMENU)LEFT_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"right-click", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 60, 125, 80, 20, hWnd,
        (HMENU)LEFT_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"Start(F2)", WS_VISIBLE | WS_CHILD, 75, 260, 175, 50, hWnd, (HMENU)START,
        NULL, NULL);
    CreateWindowW(L"Button", L"Stop(F3)", WS_VISIBLE | WS_CHILD, 250, 260, 175, 50, hWnd, (HMENU)STOP,
        NULL, NULL);

    SendMessage(hLeftButton, BM_SETCHECK, BST_CHECKED, 0);
}
DWORD WINAPI clickingThreadFunc(LPVOID lpParam) {
    
    userMouse.Click();

    Sleep(interval);

    return 0;
}
void GetInterval(DWORD& interval) {
    interval = 0;

    TCHAR userInput[4];
    int addMilliseconds;
    GetWindowText(hMilliseconds, userInput, 4);
    addMilliseconds = _ttoi(userInput);
    interval += addMilliseconds;

    int addSeconds;
    GetWindowText(hSeconds, userInput, 3);
    addSeconds = _ttoi(userInput);
    interval += addSeconds * 1000;

    int addMinutes;
    GetWindowText(hMinutes, userInput, 3);
    addMinutes = _ttoi(userInput);
    interval += addMinutes * 60000;

    int addHours;
    GetWindowText(hHours, userInput, 3);
    addHours = _ttoi(userInput);
    interval += addHours * 3600000;
    if (interval > MILLISECONDS_IN_A_DAY) {
        interval = MILLISECONDS_IN_A_DAY;
    }
    if (interval < 100) {//in case milliseconds = 0
        interval = 100;
    }
}