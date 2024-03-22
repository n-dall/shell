#include <windows.h>
#include <stdio.h>
#include <windef.h>

#define ID_EDIT 1001
#define ID_BUTTON 1002
#define ID_OUTPUT 1003

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd;
    MSG Msg;
    WNDCLASSEX WndClsEx;

    // Step 1: Registering the Window Class
    WndClsEx.cbSize = sizeof(WNDCLASSEX);
    WndClsEx.style = CS_HREDRAW | CS_VREDRAW;
    WndClsEx.lpfnWndProc = WndProc;
    WndClsEx.cbClsExtra = 0;
    WndClsEx.cbWndExtra = 0;
    WndClsEx.hInstance = hInstance;
    WndClsEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClsEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WndClsEx.lpszMenuName = NULL;
    WndClsEx.lpszClassName = "Shell";
    WndClsEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&WndClsEx)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, "Shell", "Shell", WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndInput, hwndOutput;
    static HANDLE hOutputReadTmp, hOutputRead, hOutputWrite;
    static HANDLE hInputWriteTmp, hInputRead, hInputWrite;
    static HANDLE hErrorWrite;
    static HANDLE hThread;
    static CHAR buffer[256];
    static DWORD dwRead;
    DWORD dwWritten;
    SECURITY_ATTRIBUTES sa;
    struct tagRECT rectClient = {.bottom = 0, .left = 0, .right = 0, .top = 0};


    switch (msg) {
        case WM_CREATE:
            GetClientRect(hwnd, &rectClient);

            // Create input textbox
            hwndInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     10, rectClient.bottom - 70, rectClient.right - 100, 25,
                                     hwnd, (HMENU)ID_EDIT, NULL, NULL);

            // Create "Run" button
            CreateWindow("BUTTON", "Run", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         rectClient.right - 80, rectClient.bottom - 70, 70, 25,
                         hwnd, (HMENU)ID_BUTTON, NULL, NULL);

            // Create output textbox
            hwndOutput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
                                      10, 10, rectClient.right - 20, rectClient.bottom - 100,
                                      hwnd, (HMENU)ID_OUTPUT, NULL, NULL);
            break;

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == ID_BUTTON) {
                GetWindowText(hwndInput, buffer, sizeof(buffer));
                char formatBuffer[258] = "> ";
                strcat(formatBuffer, buffer);

                SendMessage(hwndOutput, EM_REPLACESEL, 0, (LPARAM)formatBuffer);
                SendMessage(hwndOutput, EM_REPLACESEL, 0, (LPARAM)"\r\n");

                // Execute command
                FILE *fp;
                char result[40];
                strcat(buffer, " 2>&1");

                fp = popen(buffer, "r");

                while (fgets(result, sizeof(result), fp) != NULL) {
                    SendMessage(hwndOutput, EM_REPLACESEL, 0, (LPARAM)result);
                    SendMessage(hwndOutput, EM_REPLACESEL, 0, (LPARAM)"\r\n");
                }

                pclose(fp);
            }
            break;

        case WM_SIZE:
            // Resize the child windows when the parent window is resized
            MoveWindow(hwndInput, 10, HIWORD(lParam) - 70, LOWORD(lParam) - 100, 25, TRUE);
            MoveWindow(hwndOutput, 10, 10, LOWORD(lParam) - 20, HIWORD(lParam) - 100, TRUE);
            MoveWindow(GetDlgItem(hwnd, ID_BUTTON), LOWORD(lParam) - 80, HIWORD(lParam) - 70, 70, 25, TRUE);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}