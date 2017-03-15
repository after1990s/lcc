#pragma once
#include "c.h"
namespace lcc{
extern unsigned PERM;
extern unsigned FUNC;
void *allocate(unsigned long n, unsigned a);
void *newarray(unsigned long m, unsigned long n, unsigned a);


template <typename T>
T * NEW(T * &p, unsigned a)
{
    p = (T*)allocate(sizeof(*p), a);
    return p;
}

template  <typename T>
T * NEW0(T * &p, unsigned a)
{
    NEW(p, a);
    memset(p, 0, sizeof(*p));
    return p;
}
}