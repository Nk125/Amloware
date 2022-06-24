/*
    GUI para Debug y ejecutor principal de Amloware
*/

#include <windows.h>
#include <chrono>
#include "Global.hpp"
#include "Procedimientos.hpp"
#include "resource.h"
#include <string>
#include <thread>
#pragma comment(lib, "gdiplus")
#pragma comment(lib, "Winmm")

//LRESULT DlgResponse(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//    HINSTANCE thisMod = GetModuleHandle(NULL);
//
//    switch (msg) {
//    case WM_PAINT: {
//        PAINTSTRUCT ps;
//        HDC hdc = BeginPaint(hwnd, &ps);
//
//        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
//
//        EndPaint(hwnd, &ps);
//    }
//    break;
//
//    case WM_INITDIALOG:
//    {
//        HMENU hMenu = LoadMenu(thisMod, MAKEINTRESOURCE(IDR_BARRATOP));
//
//        if (!SetMenu(hwnd, hMenu)) {
//            print << "Error al crear el menu principal\n";
//        }
//    }
//        return TRUE;
//        break;
//
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case ID_AYUDA_ACERCADE:
//        {
//            MessageBox(hwnd, TEXT("Amloware 1.0\n\nDebug Mode"), TEXT("Acerca de..."), MB_ICONINFORMATION | MB_OK);
//        }
//        break;
//
//        case IDC_CHANGEWALL:
//        {
//            setWallpaper(thisMod);
//        }
//        break;
//
//        case IDC_CHANGECURSOR:
//        {
//            setCursor(thisMod);
//        }
//        break;
//
//        case IDC_RESTORECURSOR:
//        {
//            HCURSOR hCursor = LoadCursor(thisMod, IDI_APPLICATION);
//            SetCursor(hCursor);
//        }
//        break;
//        }
//        break;
//
//    /*case WM_CLOSE:
//        DestroyWindow(hwnd);
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;*/
//
//    default:
//        return FALSE;
//        break;
//    }
//
//    return TRUE;
//}

#ifdef _DEBUG
bool isPlayingSound = false, isEnabledMovingWin = false, moveCur = false;
HWND buttonSound, movingWin, mW_mvCur;

HWND createButton(HWND pHwnd, TYPE name, int ID, int posx, int posy, int x = 200, int y = 30) {
#pragma warning(push)
#pragma warning(disable: 4312)
    return CreateWindow(TEXT("button"), name.c_str(),
        WS_VISIBLE | WS_CHILD,
        posx, posy, x, y,
        pHwnd, (HMENU)ID, NULL, NULL);
#pragma warning(pop)
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE thisMod = GetModuleHandle(NULL);

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_CREATE:
    {
        HMENU hMenu = LoadMenu(thisMod, MAKEINTRESOURCE(IDR_BARRATOP));

        if (!SetMenu(hwnd, hMenu)) {
            print << "Error al crear el menu principal\n";
        }

        createButton(hwnd, TEXT("Cambiar el fondo de pantalla"), ID_CHANGE_WALLPAPER, 20, 10);

        movingWin = createButton(hwnd, TEXT("Habilitar ventana molesta"), ID_BOUNCING_WIN, 20, 60);

        mW_mvCur = createButton(hwnd, TEXT("Habilitar movimiento de mouse"), ID_BW_MOVECUR, 15, 110, 210, 30);

        buttonSound = createButton(hwnd, TEXT("Habilitar sonido de fondo"), ID_ANNOYING_SOUND, 20, 160);

        //DialogBox(thisMod, MAKEINTRESOURCE(IDD_MAINDIALOG), hwnd, DlgResponse);
        //SetDlgItemText(hwnd, IDC_EDIT1, TEXT("Default Text"));
        //DestroyWindow(hwnd);
    }
    break;

    case WM_SETCURSOR:
    {
        //SetCursor(cursor);
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_AYUDA_ACERCADE:
        {
            MessageBox(hwnd, TEXT("Amloware 1.0\n\nModo Debug"), TEXT("Acerca de..."), MB_ICONINFORMATION | MB_OK);
        }
        break;

        case ID_AYUDA_MOVIMIENTODEMOUSE:
        {
            MessageBox(hwnd, TEXT("Mueve el mouse conforme la ventana molesta se mueva"
                ", si se clickea la opción después de que se abra la ventana molesta"
                ", esta opción se anula."), TEXT("Ayuda Movimiento de Mouse"), MB_OK);
        }
        break;

        case ID_AYUDA_VENTANAMOLESTA:
        {
            MessageBox(hwnd, TEXT("Abre una ventana con un bitmap de AMLO"
                "que se mueve aleatoriamente, para ahorrar recursos, el mouse"
                "se mueve conforme la ventana.\n\nLa ventana puede ser cerrada "
                "con ALT + F4, en la compilación Release, esta ventana no puede"
                "ser cerrada."), TEXT("Ayuda Ventana molesta"), MB_OK);
        }
        break;

        case ID_AYUDA_SONIDOMOLESTO:
        {
            MessageBox(hwnd, TEXT("Reproduce el legendario circunsaiufciasncucion de AMLO infinitamente."), TEXT("Ayuda Sonido molesto"), MB_OK);
        }
        break;

        case ID_AYUDA_CAMBIARFONDO:
        {
            MessageBox(hwnd, TEXT("Cambia el fondo de pantalla por una mañanera común de AMLO furioso."), TEXT("Ayuda Cambiar fondo"), MB_OK);
        }
        break;

        case ID_CHANGE_WALLPAPER:
        {
            setWallpaper(thisMod);
        }
        break;

        case ID_BW_MOVECUR:
        {
            if (!moveCur) {
                DestroyWindow(mW_mvCur);

                mW_mvCur = createButton(hwnd, TEXT("Apagar movimiento de mouse"), ID_BW_MOVECUR, 15, 110, 210, 30);
            }
            else {
                DestroyWindow(mW_mvCur);

                mW_mvCur = createButton(hwnd, TEXT("Habilitar movimiento de mouse"), ID_BW_MOVECUR, 15, 110, 210, 30);
            }

            moveCur = !moveCur;
        }
        break;

        case ID_BOUNCING_WIN:
        {
            if (!isEnabledMovingWin) {
                if (!startAnnoyingWindow(TEXT("ANNOYINGWIN"), moveCur)) {
                    break;
                }

                DestroyWindow(movingWin);

                movingWin = createButton(hwnd, TEXT("Inhabilitar ventana molesta"), ID_BOUNCING_WIN, 20, 60);
            }
            else {
                DestroyWindow(movingWin);

                movingWin = createButton(hwnd, TEXT("Habilitar ventana molesta"), ID_BOUNCING_WIN, 20, 60);

                destroyAnnoyingWindow(TEXT("ANNOYINGWIN"));
            }

            isEnabledMovingWin = !isEnabledMovingWin;
        }
        break;

        case ID_ANNOYING_SOUND:
        {
            if (!isPlayingSound) {
                if (!playAnnoyingSound()) {
                    break;
                }

                DestroyWindow(buttonSound);

                buttonSound = createButton(hwnd, TEXT("Inhabilitar sonido de fondo"), ID_ANNOYING_SOUND, 20, 160);
            }
            else {
                DestroyWindow(buttonSound);

                buttonSound = createButton(hwnd, TEXT("Habilitar sonido de fondo"), ID_ANNOYING_SOUND, 20, 160);

                stopAnnoyingSound();
            }

            isPlayingSound = !isPlayingSound;
        }
        break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void initWinProc(HINSTANCE thisInst) {
    TYPE className = TEXT("AMLOWARE 1_0");
    WNDCLASS wc = { }; 
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = thisInst;
    wc.lpszClassName = className.data();

    if (!RegisterClass(&wc)) {
        return;
    }

    HWND hWnd = CreateWindowEx( // Crea la ventana
        WS_EX_CLIENTEDGE,
        className.data(),
        TEXT("Amloware"),
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 260, 270,
        NULL,
        NULL,
        thisInst,
        NULL
    );

    if (hWnd == NULL) {
        return;
    }

    ShowWindow(hWnd, SW_NORMAL);

    UpdateWindow(hWnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
}
#endif

int WINAPI mainName(HINSTANCE hInst, HINSTANCE hInstPrev, PTCHAR cmdline, int cmdshow) {
    //HBITMAP smol_icon = loadFromRES(IDB_ICON_AMLO, hInst);

#ifdef _DEBUG
    // Inicia la ventana principal
    initWinProc(hInst);
#else
    setWallpaper(hInst);
    playAnnoyingSound();

    int limit = 70;
    for (int i = 1; i < limit; i++) {
        std::thread aWThr(startAnnoyingWindow, TYPE(TEXT("UNIQUE_AMLO_WIN") + std::to_wstring(i)), true);

        if (i == limit) {
            aWThr.join();
        }
        else {
            aWThr.detach();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#endif
    //DeleteObject(smol_icon);

    return 0;
}