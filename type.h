#pragma once
#include "c.h"
#include "alloc.h"
#include "string.h"
#include "sym.h"
namespace lcc{
    void error(char *s, ...);
    void outtype(Type ty);
}
