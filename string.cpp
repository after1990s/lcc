#include "string.h"

namespace lcc
{

    char * string(char *str)
    {
        char *s = nullptr;
        for (s = str; *s; s++)
        {
            ;
        }
        return stringn(str, s-str);
    }
    char * stringn(char *str, int len)
    {
        assert(false);//page 22.
    }
    char * stringd(int n)
    {
        char str[25] = {0};
        char *s = str + sizeof(str);
        unsigned m = 0;
        if (n == INT_MAX)
        {
            m = (unsigned) INT_MAX + 1;
        }
        else if (n < 0)
        {
            m = -n;
        }
        else
        {
            m = n;
        }
        do{
            *--s = m % 10 + '0';
        } while ((m /= 10) != 0);
        if (n < 0)
        {
            *--s = '-';

        }
        return stringn(s, str + sizeof(str)-s);
    }

}