/****************************************************************************/
/* include                                                                  */
/****************************************************************************/
#include "framework.h"
#include "WinAPI_Sample.h"

/****************************************************************************/
/* define                                                                   */
/****************************************************************************/
#define MAX_LOADSTRING  100
#define MAX_WIDTH       640     // ウインドウ幅
#define MAX_HEIGHT      480     // ウインドウ高さ

/****************************************************************************/
/* global variable                                                          */
/****************************************************************************/
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

/****************************************************************************/
/* prototype                                                                */
/****************************************************************************/
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HDC                 funcCreateMemDC(HDC, LONG, LONG, DWORD);
BOOL                funcDeleteMemDC(HDC);
BOOL                funcGetSizeMemDC(HDC, LONG*, LONG*);

/****************************************************************************/
/* WinMain                                                                  */
/****************************************************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MSG msg;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPISAMPLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPISAMPLE));

    // メイン メッセージ ループ:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

/****************************************************************************/
/* MyRegisterClass                                                          */
/****************************************************************************/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPISAMPLE));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPISAMPLE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

/****************************************************************************/
/* InitInstance                                                             */
/****************************************************************************/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;   // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowW(
        szWindowClass,         // クラス名
        TEXT("Sample"),        // ウインドウ名
        WS_OVERLAPPEDWINDOW,   // ウインドウスタイル
        CW_USEDEFAULT,         // ウインドウのX座標
        CW_USEDEFAULT,         // ウインドウのY座標
        MAX_WIDTH,             // ウインドウ幅
        MAX_HEIGHT,            // ウインドウ高さ
        NULL,                  // 親ウインドウハンドル
        NULL,                  // メニューハンドル
        hInstance,             // インスタンスハンドル
        NULL);                 // ウインドウ作成データ

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);  // ウインドウの表示状態を設定
    UpdateWindow(hWnd);          // ウインドウ更新

    return TRUE;
}

/****************************************************************************/
/* WndProc                                                                  */
/****************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    static HDC hMemDC;          // オフスクリーン
    static HBITMAP hBitmap;     // ビットマップ
    PAINTSTRUCT ps;
    int wmId;
    LONG sx, sy;

    switch (message) {
    case WM_CREATE:
        hDC = GetDC(hWnd);
        hMemDC = funcCreateMemDC(hDC, MAX_WIDTH, MAX_HEIGHT, WHITENESS);
        ReleaseDC(hWnd, hDC);
        break;

    case WM_COMMAND:
    {
        wmId = LOWORD(wParam);

        // 選択されたメニューの解析:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
        // 表画面へ転送
        hDC = BeginPaint(hWnd, &ps);
        funcGetSizeMemDC(hMemDC, &sx, &sy);
        BitBlt(hDC, 0, 0, MAX_WIDTH, MAX_HEIGHT, hMemDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;

    case WM_CLOSE:
        funcDeleteMemDC(hMemDC);
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

/****************************************************************************/
/* About                                                                    */
/****************************************************************************/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

/****************************************************************************/
/* funcCreateMemDC                                                          */
/****************************************************************************/
HDC funcCreateMemDC(HDC hDC, LONG sx, LONG sy, DWORD dwRop)
{
    HDC     hMemDC;
    HBITMAP hBitmap;

    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateCompatibleBitmap(hDC, sx, sy);
    SelectObject(hMemDC, hBitmap);
    SelectObject(hMemDC, GetStockObject(DC_PEN));
    SelectObject(hMemDC, GetStockObject(DC_BRUSH));
    DeleteObject(hBitmap);

    if (dwRop != 0) {
        PatBlt(hMemDC, 0, 0, sx, sy, dwRop);
    }
    return hMemDC;
}

/****************************************************************************/
/* funcDeleteMemDC                                                          */
/****************************************************************************/
BOOL funcDeleteMemDC(HDC hMemDC)
{
    return DeleteDC(hMemDC);
}

/****************************************************************************/
/* funcGetSizeMemDC                                                         */
/****************************************************************************/
BOOL funcGetSizeMemDC(HDC hMemDC, LONG* sx, LONG* sy)
{
    HGDIOBJ     hBitmap;
    BITMAP      BmpInfo;

    if ((hBitmap = GetCurrentObject(hMemDC, OBJ_BITMAP)) != NULL) {
        GetObject(hBitmap, sizeof(BITMAP), &BmpInfo);
        *sx = BmpInfo.bmWidth;
        *sy = BmpInfo.bmHeight;
        return TRUE;
    }
    return FALSE;
}