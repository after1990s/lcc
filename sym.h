#pragma once
#include "c.h"
#include "alloc.h"
#include "string.h"

namespace lcc{
    //const int HASHSIZE =  256;
    //enum {CONSTANTS=1, LABELS, GLOBAL, PARAM, LOCAL};
    //typedef struct type {
    //    int op;
    //    struct type * type;
    //    int align;
    //    int size;
    //    union 
    //    {
    //        Symbol sym;
    //        struct{
    //            unsigned oldstyle:1;
    //            struct type **proto;
    //        }f;
    //    }u;
    //    xtype x;
    //}*Type;
    //typedef struct coord{
    //    char *file;
    //    unsigned x, y;
    //}Coordinate;
    //typedef union value{
    //    char sc;
    //    short ss;
    //    int i;
    //    unsigned char uc;
    //    unsigned short us;
    //    unsigned int u;
    //    float f;
    //    double d;
    //    void *p;
    //}Value;
    //typedef struct symbol {
    //    char * name;
    //    int scope;
    //    Coordinate src;
    //    struct symbol* up;
    //    List uses;
    //    int sclass;
    //    //flags
    //    unsigned generated:1;
    //    unsigned defined:1;
    //    unsigned temporary:1;
    //    //end flags
    //    Type type;

    //    float ref;
    //    union{
    //        struct {
    //            int label;
    //            Symbol equatedto;
    //        }l;//for label
    //        struct{
    //            Value v;
    //            Symbol loc;
    //        }c;//for constant
    //    }u;
    //    Xsymbol x;


    //}*Symbol;

    //struct entry{
    //    struct symbol sym;
    //    struct entry *link;
    //};
    //typedef struct table
    //{
    //    int level;
    //    struct table *previous;
    //    struct entry *buckets[HASHSIZE];
    //    Symbol all; 
    //}*Table;
    Symbol install(char* name, Table *tpp, int level, int arena);
}
