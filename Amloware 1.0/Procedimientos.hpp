#pragma once
#include <windows.h>
#include <chrono>
#include <fstream>
#include <gdiplus.h>
#include "Global.hpp"
#include <random>
#include <thread>

/*
    Cambiar wallpaper
*/

// Copiado de la guía de microsoft sobre como convertir imágenes a otros formatos con gdi+
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    using namespace Gdiplus;
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

bool WriteBitmapFile(TYPE fileName, HBITMAP hBitmap) {
    using namespace Gdiplus;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Image* image(new Bitmap(hBitmap, NULL));

    CLSID format;

    if (GetEncoderClsid(L"image/bmp", &format) < 0) {
        format = Gdiplus::ImageFormatBMP;
    }

    Status st = image->Save(fileName.data(), &format, NULL);

    if (st != Ok) {
#ifdef _DEBUG
        print << "Fallo al escribir en el archivo: " << fileName << " Error: " << st << " " << (st == Gdiplus::InvalidParameter) << "\n";
#endif
        return false;
    }

    GdiplusShutdown(gdiplusToken);
    return true;
}

void setWallpaper(HINSTANCE thisInst) {
    TYPE tmpPath, wpaperPath;
    tmpPath.resize(MAX_PATH);

    DWORD tmpPathSize = GetTempPath(MAX_PATH, &tmpPath[0]);
    // Trimea el string hasta el primer NULL
    tmpPath.resize(tmpPathSize);

    // Cuando se usa wstring no es compatible con const char*, así que uso la macro TEXT para convertirlo a wchar_t*
    // Por cierto GetTempPath ya añade el \ al string, por lo que no hace falta en el TEXT
    wpaperPath = tmpPath + TEXT("wpAMLO.bmp");

    HBITMAP wallpaper = loadFromRES(IDB_WALLPAPER_AMLO, thisInst);

    if (!WriteBitmapFile(wpaperPath, wallpaper)) {
#ifdef _DEBUG
        print << L"Error al escribir el wallpaper\n";
#endif
        DeleteObject(wallpaper);
        return;
    }

    if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, &wpaperPath[0], SPIF_UPDATEINIFILE)) {
#ifdef _DEBUG
        print << L"Error al setear el wallpaper\n"; // No necesita forzosamente returnear
#endif
    }

    DeleteObject(wallpaper);
}

/*
    Reproducir sonido
*/

void stopAnnoyingSound() {
    //mciSendString(TYPE(TEXT("close ") + globalAASID).c_str(), NULL, 0, NULL);
    sndPlaySound(NULL, 0);
}

bool playAnnoyingSound() {
    TYPE soundPath;
    HRSRC amloRsrc;

    {
        HMODULE currentProcess = GetModuleHandle(NULL);
        amloRsrc = FindResource(currentProcess, MAKEINTRESOURCE(IDR_WAVE1), TEXT("WAVE"));
        HGLOBAL amloAnnoy = LoadResource(currentProcess, amloRsrc);
        size_t sizeRsrc = SizeofResource(currentProcess, amloRsrc);

        if (amloAnnoy == NULL) {
            return false;
        }

        std::string amloSound(static_cast<char*>(LockResource(amloAnnoy)), sizeRsrc);

        TYPE tmpPath;
        tmpPath.resize(MAX_PATH);

        DWORD tmpPathSize = GetTempPath(MAX_PATH, &tmpPath[0]);
        tmpPath.resize(tmpPathSize);

        soundPath = tmpPath + TEXT("amlo.wav");

        std::ofstream soundOut;
        soundOut.open(soundPath, std::ios::binary);

        if (soundOut.is_open()) {
            soundOut.write(amloSound.c_str(), amloSound.size());
            soundOut.close();
        }
        else {
            return false;
        }

        TYPE soundPathBuf;
        soundPathBuf.resize(MAX_PATH);

        DWORD sPBsz = GetShortPathNameW(soundPath.c_str(), &soundPathBuf[0], MAX_PATH);

        soundPathBuf.resize(sPBsz);

        soundPath = soundPathBuf;
    }

    if (!sndPlaySound(soundPath.c_str(), SND_ASYNC | SND_LOOP | SND_NODEFAULT)) {
        return false;
    }

    UnlockResource(amloRsrc);
    FreeResource(amloRsrc);

    return true;

    /*TYPE cSt = TYPE(TYPE(TEXT("open \"")) + soundPath + TEXT("\" alias \"") + globalAASID + TEXT("\""));

    print << cSt << "\n";

    mciSendString(cSt.c_str(), NULL, 0, NULL);

    cSt = TYPE(TEXT("play \"") + globalAASID + TEXT("\" wait repeat"));
    print << cSt << "\n";

    mciSendString(cSt.c_str(), NULL, 0, NULL);*/
}

/*
    Ventana molesta
*/

HWND annoyingWindow;
HBITMAP bigy_icon;
BITMAP amloInfo;
bool annoyingWinDestroyed = false, moveWithWin = false;

void moveAnnoyingWin(HWND hWnd) {
    std::random_device rd;
    std::mt19937 mt(rd());

    int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
    int screenY = GetSystemMetrics(SM_CYFULLSCREEN);
    std::uniform_int_distribution<> Xrand(0, screenX);
    std::uniform_int_distribution<> Yrand(0, screenY);

    while (!annoyingWinDestroyed) {
        int x, y;
        x = Xrand(mt);
        y = Yrand(mt);

        if (moveWithWin) {
            SetCursorPos(x, y);
        }

        SetWindowPos(hWnd, HWND_TOPMOST, x, y, amloInfo.bmWidth, amloInfo.bmHeight, SWP_SHOWWINDOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

#ifdef _DEBUG
    print << "Annoying window movements thread finished\n";
#endif
}

LRESULT CALLBACK annoyingWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc, hdcMem;
        HGDIOBJ oldBitmap;

        hdc = BeginPaint(hwnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, bigy_icon);

        BitBlt(hdc, 0, 0, amloInfo.bmWidth, amloInfo.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
        DeleteDC(hdc);
        DeleteObject(oldBitmap);
        break;
    }
    break;

    case WM_CREATE:
        annoyingWinDestroyed = false;
        std::thread(moveAnnoyingWin, hwnd).detach();
        break;
    case WM_CLOSE:
#ifndef _DEBUG
        break;
#endif
    case WM_DESTROY:
        annoyingWinDestroyed = true;
        DeleteObject(bigy_icon);
        DestroyWindow(hwnd);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void destroyAnnoyingWindow(TYPE myID = TEXT("ANNOYINGWIN")) {
    DestroyWindow(annoyingWindow);
    UnregisterClass(myID.c_str(), GetModuleHandle(NULL));
}

void handleAnnoyingWin(HWND hWnd) {
    MSG Msg;
    
    while (GetMessage(&Msg, hWnd, 0, 0) > 0 && !annoyingWinDestroyed) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

#ifdef _DEBUG
    print << "Annoying window handle finished\n";
#endif
}

bool startAnnoyingWindow(TYPE myID = TEXT("ANNOYINGWIN"), bool moveCurWithWin = false) {
    HINSTANCE thisInst = GetModuleHandle(NULL);
    
    TYPE className = myID;
    WNDCLASS wc = { };

    wc.lpfnWndProc = annoyingWindowProc;
    wc.hInstance = thisInst;
    wc.lpszClassName = className.data();

    if (!RegisterClass(&wc)) {
        return false;
    }

    annoyingWinDestroyed = true;

    bigy_icon = loadFromRES(IDB_POINTING_AMLO, GetModuleHandle(NULL));
    GetObject(bigy_icon, sizeof(amloInfo), &amloInfo);

    moveWithWin = moveCurWithWin;

    annoyingWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        className.data(),
        TEXT("¡AMLO!"),
        (WS_OVERLAPPED | WS_CAPTION),
        CW_USEDEFAULT, CW_USEDEFAULT, amloInfo.bmWidth, amloInfo.bmHeight,
        NULL,
        NULL,
        thisInst,
        NULL
    );

    if (annoyingWindow == NULL) {
        return false;
    }

    ShowWindow(annoyingWindow, SW_NORMAL);

    UpdateWindow(annoyingWindow);

#ifdef _DEBUG
    std::thread(handleAnnoyingWin, annoyingWindow).detach();
#else
    handleAnnoyingWin(annoyingWindow);
#endif
    
    return true;
}

