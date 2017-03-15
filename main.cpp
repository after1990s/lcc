#include "c.h"
#include "alloc.h"
struct A{
    int k;
    int j;
    int m;
    int n;
};
int main(void)
{
    using namespace lcc;
    A *p = nullptr;
    NEW(p, 0);
    p = nullptr;
    p = NEW(p, 0);
}