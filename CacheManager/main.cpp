#include "pch.h"
#include "CacheManager.h"

using namespace std;

int main()
{
    try {
        CacheManager cm;
        cm.Main();
    }
    catch (exception &e) {
        MessageBox(NULL, e.what(), "�g�`�v���r���[(CacheManager)", MB_OK | MB_ICONERROR);
    }
    return 0;
}
