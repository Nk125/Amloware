#pragma once
#include <windows.h>
#include "resource.h"
#include <string>

#ifdef UNICODE
#define mainName wWinMain
#define TYPE std::wstring
#else
#define mainName WinMain
#define TYPE std::string
#endif

#ifdef _DEBUG
class printF {
public:
    printF operator<<(std::string a) {
        OutputDebugStringA(a.data());
        return *this;
    }

    printF operator<<(std::wstring a) {
        OutputDebugStringW(a.data());
        return *this;
    }

    printF operator<<(bool a) {
        OutputDebugStringA(std::to_string(a).data());
        return *this;
    }

    printF operator<<(int a) {
        OutputDebugStringA(std::to_string(a).data());
        return *this;
    }

    printF operator<<(float a) {
        OutputDebugStringA(std::to_string(a).data());
        return *this;
    }

    printF operator<<(double a) {
        OutputDebugStringA(std::to_string(a).data());
        return *this;
    }

    printF operator<<(const char* a) {
        OutputDebugStringA(a);
        return *this;
    }

    printF operator<<(char* a) {
        OutputDebugStringA(a);
        return *this;
    }

    printF operator<<(unsigned long long a) {
        OutputDebugStringA(std::to_string(a).data());
        return *this;
    }
};

printF print; // Alias para OutputDebugString
#endif

template <typename T = HBITMAP>
T loadFromRES(int res, HINSTANCE hInst = NULL, int type = IMAGE_BITMAP, int LR = LR_DEFAULTCOLOR) {
    return static_cast<T>(LoadImage(hInst, MAKEINTRESOURCE(res), type, 0, 0, LR));
    //return LoadBitmap(hInst, MAKEINTRESOURCE(res));
}

//TYPE globalAASID(TEXT("amlosound"));

#define ID_CHANGE_WALLPAPER 5
#define ID_BOUNCING_WIN 6
#define ID_ANNOYING_SOUND 7
#define ID_BW_MOVECUR 8