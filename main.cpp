#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windowsx.h>
#include <sstream>
#include "resource.h"
#include "mouseClicks.h"

bool mouseClickMode;
bool mouseDownMode;
const int CONTROLS = 1;
const int MENU2 = 2;
const int START = 3;
const int STOP = 4;
const int MOUSE_BUTTON_SELECT = 5;
const int LEFT_BUTTON = 6;
const int RIGHT_BUTTON = 7;
const int MOUSE_CLICK = 8;
const int MOUSE_DOWN = 9;
const int MILLISECONDS_IN_A_DAY = 86400000;
const int IDT_TIMER1 = 10;
const int TIMER_ON = 11;
const int TIMER_OFF = 12;
const int ON_HOTKEY = 13;
const int OFF_HOTKEY = 14;
enum Status { disable, enable };
Status AutoClick = disable;
Status Pause = disable;
Status Timer = disable;

MouseClick userMouse;

HWND hMilliseconds;
HWND hSeconds;
HWND hMinutes;
HWND hHours;
HWND hStart;
HWND hTimerMinutes;
HWND hTimerHours;
HWND hDisableMinutes;
HWND hDisableHours;
HWND hTimerHoursLabel;
HWND hTimerMinutesLabel;
HWND hMainApplication;

HBRUSH hbrBkgnd = NULL;
// The main window class name.
static TCHAR szWindowClass[] = _T("MouseSiml");
static TCHAR szTitle[] = _T("MouseSiml");// potential name: Rage Mouse - Daniel

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

HANDLE hThread;

//handles input from user
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI clickingThreadFunc(LPVOID lpParam);

//LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
// Application menu
void AddMenu(HWND);
HMENU hMenu;

// Controls function
void AddControls(HWND);

DWORD interval;
void GetInterval(DWORD& interval);
DWORD userTime;
void GetUserTime(DWORD& interval);

void StopClicker();
void StartClicker(HWND);
int ParseToInt(char* str);
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
        450, 400,
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
    hMainApplication = hWnd;
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

    case WM_COMMAND:// Controls what menu items do
        switch (wParam)
        {

        case MENU2:
            break;
        case TIMER_ON:
            Timer = enable;
            //gets rid of the greyed-out input boxes
            if (hDisableHours) {
                DestroyWindow(hDisableHours);
            }
            if (hDisableMinutes) {
                DestroyWindow(hDisableMinutes);
            }
            //replaces greyed-out windows for user to interacte with
            hTimerHours = CreateWindowW(L"Edit", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 125, 175, 20, 20, hWnd,
                NULL, NULL, NULL);
            hTimerMinutes = CreateWindowW(L"Edit", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 200, 175, 20, 20, hWnd,
                NULL, NULL, NULL);
            Button_Enable(hTimerHoursLabel , true);
            Button_Enable(hTimerMinutesLabel, true);
            break;
        case TIMER_OFF:
            //gets rid of user interactive windows for timer
            if (hTimerHours) {
                DestroyWindow(hTimerHours);
            }
            if (hTimerMinutes) {
                DestroyWindow(hTimerMinutes);
            }
            //recreates the greyed-out input boxes
            hDisableHours = CreateWindowW(L"static", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 125, 175, 20, 20, hWnd,
                NULL, NULL, NULL);
            hDisableMinutes = CreateWindowW(L"static", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 200, 175, 20, 20, hWnd,
                NULL, NULL, NULL);
            Button_Enable(hDisableHours, false);
            Button_Enable(hDisableMinutes, false);
            Button_Enable(hTimerHoursLabel, false);
            Button_Enable(hTimerMinutesLabel, false);
            Timer = disable;
            break;
        case LEFT_BUTTON:
            userMouse.SetLeftClick();
            break;
        case RIGHT_BUTTON:
            userMouse.SetRightClick();
		case MOUSE_CLICK:
            userMouse.SetMouseClick();
            break;
		case MOUSE_DOWN:
			userMouse.SetMouseDown();
			break;
        case START:
            StartClicker(hWnd);
            break;
        case STOP:
            StopClicker();
            break;
        case CONTROLS:
            
            break;
        }
        break;
    case WM_HOTKEY:
        switch(wParam)
        {
            case ON_HOTKEY:
                StartClicker(hWnd);
                break;
            case OFF_HOTKEY:
                StopClicker();
        }
        break;
    case WM_TIMER:
        StopClicker();
        break;
    case WM_CREATE:
        //AddMenu(hWnd);
        AddControls(hWnd);
        hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255)); // Create a white brush
        break;
    case WM_CTLCOLORSTATIC:
        //the lines below make the background of the static text boxes white for the purpose of fitting in with the application background
        hdc = (HDC)wParam;
        SetBkColor(hdc, RGB(255, 255, 255)); // Set the background color to white
        return (INT_PTR)hbrBkgnd;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        
        // Create a grey brush
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        //recatangle around the click interval 
        Rectangle(hdc, 40, 40 , 380, 80);

        Rectangle(hdc, 40, 165, 235, 205);

        SelectObject(hdc, hOldPen);

        // Clean up
        DeleteObject(hPen);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        if (hbrBkgnd) {
            DeleteObject(hbrBkgnd); // Clean up the brush
        }
        if(hThread)
        {
            CloseHandle(hThread); //close clicking thread if it exist
        }
        PostQuitMessage(0);
        break;
    default:
        if (Pause == enable) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hWnd, &cursorPos); // Convert screen coordinates to client coordinates
            HWND currentChildHwnd = ChildWindowFromPoint(hMainApplication, cursorPos);
            HWND currentParentHwnd;
            if (currentChildHwnd != hMainApplication)
            {
                currentParentHwnd = GetParent(currentChildHwnd);
            }
            else {
                currentParentHwnd = currentChildHwnd;
            }
            if (currentParentHwnd != hMainApplication) {
                Pause = disable;
                break;
            }
        }
        if(AutoClick == enable && Pause == disable)
        {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hWnd, &cursorPos); // Convert screen coordinates to client coordinates
            HWND currentChildHwnd = ChildWindowFromPoint(hMainApplication, cursorPos);
            HWND currentParentHwnd;
            if (currentChildHwnd != hMainApplication)
            {
                currentParentHwnd = GetParent(currentChildHwnd);
            }
            else {
                currentParentHwnd = currentChildHwnd;
            }
            if (currentParentHwnd == hMainApplication) {
                Pause = enable;
                break;
            }
        }
        if (hThread && AutoClick == disable) {
            CloseHandle(hThread);
            hThread = NULL;
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//AddMenu function adds the menu bar at top of the application
void AddMenu(HWND hWnd)
{
    hMenu = CreateMenu();

    HMENU hDropDownMenu = CreateMenu();

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDropDownMenu, "edit");// parameters: (menu object, flag(the type), the value it returns or the drop down menu, the name it displays)
    AppendMenu(hDropDownMenu, MF_STRING, CONTROLS, "controls");// Part of the drop down menu 
    AppendMenu(hDropDownMenu, MF_SEPARATOR, NULL, NULL);// a visable seperator

    AppendMenu(hMenu, MF_STRING, MENU2, "help");

    SetMenu(hWnd, hMenu);
}
//adds the UI and hotkeys to the app
void AddControls(HWND hWnd)
{
    //displays the interval
    CreateWindowW(L"static", L"Interval", WS_VISIBLE | WS_CHILD | SS_LEFT, 50, 30, 50, 15, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"milliseconds:", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 50, 85, 20, hWnd,
        NULL, NULL, NULL);
    hMilliseconds = CreateWindowW(L"Edit", L"100", WS_VISIBLE | WS_CHILD | WS_BORDER, 145, 50, 30, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"static", L"seconds:", WS_VISIBLE | WS_CHILD | SS_CENTER, 185, 50, 60, 20, hWnd,
        NULL, NULL, NULL);
    hSeconds = CreateWindowW(L"Edit", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 252, 50, 20, 20, hWnd,
        NULL, NULL, NULL);
    //SetWindowSubclass(hMilliseconds, EditSubclassProc, 0, 0);
    CreateWindowW(L"static", L"minutes:", WS_VISIBLE | WS_CHILD | SS_CENTER, 280, 50, 60, 20, hWnd,
        NULL, NULL, NULL);
    hMinutes = CreateWindowW(L"Edit", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 348, 50, 20, 20, hWnd,
        NULL, NULL, NULL);

    //displays the timer
    CreateWindowW(L"static", L"Timer:", WS_VISIBLE | WS_CHILD | SS_CENTER, 45, 155, 50, 20, hWnd,
        NULL, NULL, NULL);
    CreateWindowW(L"Button", L"on", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 100, 155, 40, 20, hWnd,
        (HMENU)TIMER_ON, NULL, NULL);
    HWND hTimerOff = CreateWindowW(L"Button", L"off", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 145, 155, 40, 20, hWnd,
        (HMENU)TIMER_OFF, NULL, NULL);
    hTimerMinutesLabel = CreateWindowW(L"static", L"minutes:", WS_VISIBLE | WS_CHILD | SS_CENTER, 60, 175, 60, 15, hWnd,
        NULL, NULL, NULL);
    hDisableMinutes = CreateWindowW(L"static", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 125, 175, 20, 20, hWnd,
        NULL, NULL, NULL);
    hTimerHoursLabel = CreateWindowW(L"static", L"hours:", WS_VISIBLE | WS_CHILD | SS_CENTER, 150, 175, 45, 20, hWnd,
        NULL, NULL, NULL);
    hDisableHours = CreateWindowW(L"static", L"- -", WS_VISIBLE | WS_CHILD | WS_BORDER, 200, 175, 20, 20, hWnd,
        NULL, NULL, NULL);
    //the timer is automatically off when the app starts so the buttons are disable in the code below
    Button_Enable(hDisableHours, false);
    Button_Enable(hDisableMinutes, false);
    Button_Enable(hTimerHoursLabel, false);
    Button_Enable(hTimerMinutesLabel, false);
    SendMessage(hTimerOff, BM_SETCHECK, BST_CHECKED, 0);
    Timer = disable;

    //displays the mouse button settings
    CreateWindowW(L"Button", L"Mouse Button", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 40, 80, 110, 75, hWnd,
        NULL, NULL, NULL);
    HWND hLeftButton = CreateWindowW(L"Button", L"left-click", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 50, 100, 80, 20, hWnd,
        (HMENU)LEFT_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"right-click", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 50, 125, 80, 20, hWnd,
        (HMENU)LEFT_BUTTON, NULL, NULL);
    SendMessage(hLeftButton, BM_SETCHECK, BST_CHECKED, 0);

    //displays the mouse output settings
    CreateWindowW(L"Button", L"Mouse Output", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 260, 80, 120, 75, hWnd,
        NULL, NULL, NULL);
    HWND hMouseClick = CreateWindowW(L"Button", L"mouse-click", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 270, 100, 95, 20, hWnd,
        (HMENU)MOUSE_CLICK, NULL, NULL);
    CreateWindowW(L"Button", L"mouse-down", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 270, 125, 100, 20, hWnd,
        (HMENU)MOUSE_DOWN, NULL, NULL);
    SendMessage(hMouseClick, BM_SETCHECK, BST_CHECKED, 0);

    //displays start and stop buttons
    hStart = CreateWindowW(L"Button", L"Start(F2)", WS_VISIBLE | WS_CHILD | BS_FLAT , 40, 210, 170, 50, hWnd, (HMENU)START,
        NULL, NULL);
    CreateWindowW(L"Button", L"Stop(F3)", WS_VISIBLE | WS_CHILD | BS_FLAT, 215, 210, 170, 50, hWnd, (HMENU)STOP,
        NULL, NULL); 
    
    
    //sets hotkeys
    RegisterHotKey(hWnd, ON_HOTKEY, 0, VK_F2); 
    RegisterHotKey(hWnd, OFF_HOTKEY, 0, VK_F3);
}
//this operates on a different thread so it dosen't interfere with the message loop while its running
DWORD WINAPI clickingThreadFunc(LPVOID lpParam) {
    while (AutoClick == enable)
    {
        if(Pause == disable)
        {
            if (mouseClickMode) {
                userMouse.Click();
                Sleep(interval);
            }
            if (mouseDownMode) {
                userMouse.MouseDown();
                Sleep(1000);
            }
        }
    }

    return 0;
}
//gets interval from the click interval
void GetInterval(DWORD& interval) {
    interval = 0;

    char userInput[4];
    int addMilliseconds;
    GetWindowText(hMilliseconds, userInput, 4);
    addMilliseconds = _ttoi(userInput);
    interval += addMilliseconds;

    int addSeconds;
    GetWindowText(hSeconds, userInput, 4);
    userInput[3] = '/0';
    addSeconds = ParseToInt(userInput);
    interval += addSeconds * 1000;

    int addMinutes;
    GetWindowText(hMinutes, userInput, 4);
    addMinutes = ParseToInt(userInput);
    interval += addMinutes * 60000;

   
    if (interval < 1) {//in case milliseconds = 0
        interval = 1;
    }
}
//gets the interval from the timer
void GetUserTime(DWORD& interval) {
    interval = 0;

    TCHAR userInput[4];

    int addMinutes;
    GetWindowText(hTimerMinutes, userInput, 3);
    addMinutes = _ttoi(userInput);
    interval += addMinutes * 60000;

    int addHours;
    GetWindowText(hTimerHours, userInput, 3);
    addHours = _ttoi(userInput);
    interval += addHours * 3600000;
    if (interval > MILLISECONDS_IN_A_DAY) {
        interval = MILLISECONDS_IN_A_DAY;
    }
}
//closes resources to stop the autoclicking
void StopClicker() {
    AutoClick = disable;
    if (Pause) {
        Pause = disable;
    }
    if (Timer == enable) {
        KillTimer(hMainApplication, IDT_TIMER1);
    }
    Button_Enable(hStart, true);
    CloseHandle(hThread);

}
//gets settings set by the user and ignitiates the auto-clicker
void StartClicker(HWND hWnd) {
    GetInterval(interval);
    mouseClickMode = userMouse.IsClickModeOn();
    mouseDownMode = userMouse.IsDownModeOn();
    AutoClick = enable;
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
    if (Timer == enable)
    {
        GetUserTime(userTime);
        if (userTime > 0)
        {
            SetTimer(hWnd, IDT_TIMER1, userTime, NULL);
        }
    }
    Button_Enable(hStart, false);
    Sleep(100);
}
int ParseToInt(char* str){
    if(str != '\0') {
        if(isdigit(*str)) {
            return ParseToInt(++str) + atoi(str);
        }
        return ParseToInt(++str);
    }
    return 0;
}
    
    
   
//int ParseToInt(char* str) {
//    std::ostringstream parse;
//    char *userTime;
//    for (int i = 0; str; i++) {
//        if (isdigit(str[i])) {
//             parse << str[i];
//             usertime << 
//        }
//    }
//    return userTime ? atoi(userTime) : 0;
//
//}
//LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
//{
//    switch (message)
//    {
//    case WM_SETFOCUS:
//        SetWindowText(hWnd, "");
//        break;
//    }
//    return DefSubclassProc(hWnd, message, wParam, lParam);
//}
