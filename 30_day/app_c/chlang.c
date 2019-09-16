#include "apilib.h"

void HariMain(void)
{
    int langmode = api_getlang();
    //sjis
    static char s1[23] = {0x93, 0xfa, 0x96, 0x7b, 0x8c, 0xea, 0x83, 0x56, 0x83, 0x74, 0x83, 0x67,
                          0x4a, 0x49, 0x53, 0x83, 0x82, 0x81, 0x5b, 0x83, 0x68, 0x0a, 0x00};
    //euc
    static char s2[17] = {0xc6, 0xfc, 0xcb, 0xdc, 0xb8, 0xec, 0x45, 0x55, 0x43, 0xa5, 0xe2, 0xa1,
                          0xbc, 0xa5, 0xc9, 0x0a, 0x00};

    if (langmode == 0)
        api_putstr0("english ascii mode\n");
    if (langmode == 1)
        api_putstr0(s1);
    if (langmode == 2)
        api_putstr0(s2);
    api_end();
}
