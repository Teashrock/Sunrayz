#ifdef _WIN32
#include <windows.h>
#endif

void AllocError(void* ptr, const char* text)
{
    if (ptr == NULL) {
        #ifdef _WIN32
        MessageBox(NULL, (LPCTSTR)text, (LPCTSTR)"Allocation error!", MB_OK | MB_ICONERROR);
        #endif
    }
}