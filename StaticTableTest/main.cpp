#include "..\StaticTable\Table.h"


#include <windows.h>
#include <chrono>
#include <iomanip>
#include <sstream>

int colNum = 10, rowNum = 30, clientWidth, clientHeight, scrollWidth = 0, scrollPos = 0, clickPosX = 0, clickPosY = 0;
HWND hWnd;
Table* table = nullptr;
bool doubleLeftClicked = false;
bool doubleRightClicked = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void updateClientSizeParams();
std::wstring getFormattedTime(const std::chrono::system_clock::time_point& time);
std::chrono::system_clock::time_point clickTime;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const wchar_t TableClass[] = L"Table";
    int x = 0;


    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TableClass;
    wc.style = CS_DBLCLKS;
    RegisterClass(&wc);

    hWnd = CreateWindowEx(0, TableClass, L"Table", WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        0, 0, screenWidth - 40, screenHeight,
        NULL, NULL, hInstance, NULL);

    updateClientSizeParams();

    table = new Table(hWnd, clientWidth - scrollWidth, clientHeight, rowNum, colNum);

    ShowWindow(hWnd, SW_SHOWMAXIMIZED);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clearRect;
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
    {
        break;
    }
    case WM_SIZE: {
        updateClientSizeParams();
        InvalidateRect(hWnd, NULL, true);
    }
    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &clearRect);
        FillRect(hdc, &clearRect, (HBRUSH)(COLOR_WINDOW + 1));
        table->updateTableParams(clientWidth - scrollWidth, clientHeight);
        SetScrollRange(hWnd, SB_VERT, 0, table->rowsLeft(), true);
        table->drawTable(hdc, scrollPos);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_COMMAND: {
        break;
    }
    case WM_VSCROLL: {
        int oldPos = scrollPos;
        switch (LOWORD(wParam)) {
        case SB_LINEUP:
            if (--scrollPos < 0) scrollPos = 0;
            break;
        case SB_LINEDOWN:
            if (++scrollPos > table->rowsLeft())scrollPos = table->rowsLeft();
            break;
        }
        if (oldPos != scrollPos)
        {
            SetScrollPos(hWnd, SB_VERT, scrollPos, true);
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_LBUTTONDOWN: {
        clickTime = std::chrono::system_clock::now();
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        table->startClickTimer();
        break;
    }
    case WM_LBUTTONDBLCLK: {
        clickTime = std::chrono::system_clock::now();
        table->stopClickTimer();
        doubleLeftClicked = true;
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        table->handleDblLeftClick(clickPosX, clickPosY, getFormattedTime(clickTime));
        doubleLeftClicked = false;
        break;
    }
    case WM_RBUTTONDOWN: {
        clickTime = std::chrono::system_clock::now();
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        table->startRClickTimer();
        break;
    }
    case WM_RBUTTONDBLCLK: {
        clickTime = std::chrono::system_clock::now();
        table->stopRClickTimer();
        doubleRightClicked = true;
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        table->handleDblRightClick(clickPosX, clickPosY, getFormattedTime(clickTime));
        doubleRightClicked = false;
        break;
    }

    case WM_TIMER: {
        if (wParam == table->CLICK_TIMER_ID) {
            table->stopClickTimer();
            if (!doubleLeftClicked) {
                table->handleLeftClick(clickPosX, clickPosY, getFormattedTime(clickTime));
            }
        }
        if (wParam == table->RCLICK_TIMER_ID) {
            table->stopRClickTimer();
            if (!doubleRightClicked) {
                table->handleRightClick(clickPosX, clickPosY, getFormattedTime(clickTime));
            }
        }
        break;
    }
    case WM_KEYDOWN: {
        if (GetKeyState(VK_CONTROL) & 0x8000 &&
            GetKeyState(VK_SHIFT) & 0x8000 &&
            wParam == 'T') {
            table->changeCellType();
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

void updateClientSizeParams() {
    RECT rect;
    GetClientRect(hWnd, &rect);
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;
}

std::wstring getFormattedTime(const std::chrono::system_clock::time_point& time) {
    std::time_t t = std::chrono::system_clock::to_time_t(time);
    std::tm localTime;
    localtime_s(&localTime, &t);

    std::wostringstream oss;
    oss.imbue(std::locale(""));
    oss << std::put_time(&localTime, L"%Y-%m-%d %H:%M:%S");
    return oss.str();
}

