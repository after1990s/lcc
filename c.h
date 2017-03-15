#pragma once

#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <stdio.h>
//#define NEW(p,a) ((p) = allocate(sizeof *(p), (a)))
//#define NEW0(p,a) memset(NEW((p),(a)), 0, sizeof *(p))
#define NELEMS(a) ((int)(sizeof(a) / sizeof((a)[0])))
#define isqual(t) ((t)->op >= CONST)
#define unqual(t) (isqual(t) ? (t)->type : t)
#define isvolatile(t) ((t)->op == VOLATILE \
                        || (t)->op == CONST+VOLATILE\
                        ||(t)->op == CONST)
#define isconst(t)    ((t)->op == CONST \
                        || (t)->op == CONST+VOLATILE)
#define isarray(t)    (unqual(t)->op == ARRAY)
#define isstruct(t)  (unqual(t)->op == STRUCT\
                     ||unqual(t)->op==UNION)
#define isunion(t)    (unqual(t)->op == UNION)
#define isfunc(t)     (unqual(t)->op == FUNCTION)
#define isptr(t)      (unqual(t)->op == POINTER)
#define ischar(t)     ((t)->size == 1 && isint(t))
#define isint(t)      (unqual(t)->op == INT \
                    || unqual(t)->op == UNSIGNED)
#define isfloat(t)    (unqual(t)->op == FLOAT)
#define isarith(t)    (unqual(t)->op <= UNSIGNED)
#define isunsigned(t) (unqual(t)->op == UNSIGNED)
#define isscalar(t)   (unqual(t)->op <= POINTER \
                    || unqual(t)->op == ENUM)
#define isenum(t)     (unqual(t)->op == ENUM)
#define fieldsize(p) (p)->bitsize
#define fieldright(p) ((p)->lsb -1)
#define fieldleft(p)  (8 *(p)->type->size - fieldsize(p) - fieldright(p))
#define fieldmask(p) (~(~(unsigned)0 << fieldsize(p)))
#define error printf
#define warning printf
namespace lcc
{
    typedef struct symbol *Symbol;

    typedef struct coord {
        char *file;
        unsigned x, y;
    } Coordinate;
    typedef struct table *Table;

    typedef union value {
        long i;
        unsigned long u;
        long double d;
        void *p;
        void(*g)(void);
    } Value;
    typedef struct tree *Tree;

   

    typedef struct field *Field;
    struct field {
        char *name;
        Type type;
        int offset;
        short bitsize;
        short lsb;
        Field link;
    };
    typedef struct {
        unsigned printed : 1;
        unsigned marked;
        unsigned short typeno;
        void *xt;
    } Xtype;
    struct entry{
        struct symbol sym;
        struct entry *link;
    };
    struct table {
        int level;
        Table previous;
        struct entry *buckets[256];
        Symbol all;
    };
    typedef struct table *Table;
    struct symbol {
        char *name;
        int scope;
        Coordinate src;
        Symbol up;
        List uses;
        int sclass;
        unsigned structarg : 1;

        unsigned addressed : 1;
        unsigned computed : 1;
        unsigned temporary : 1;
        unsigned generated : 1;
        unsigned defined : 1;
        Type type;
        float ref;
        union {
            struct {
                int label;
                Symbol equatedto;
            } l;
            struct {
                unsigned cfields : 1;
                unsigned vfields : 1;
                Table ftab;		/* omit */
                Field flist;
            } s;
            int value;
            Symbol *idlist;
            struct {
                Value min, max;
            } limits;
            struct {
                Value v;
                Symbol loc;
            } c;
            struct {
                Coordinate pt;
                int label;
                int ncalls;
                Symbol *callee;
            } f;
            int seg;
            Symbol alias;
            struct {
                Node cse;
                int replace;
                Symbol next;
            } t;
        } u;
        Xsymbol x;
    };
    struct type{
        int op;
        Type type;//对于function类型，这个值为返回类型
        int align;
        int size;
        union{
            Symbol sym;
            struct {
                unsigned oldstyle:1;
                Type *proto;
            }f;
        }u;
        Xtype x;
    };
    typedef struct type *Type;
    enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL };
    enum { CHAR=1, LONG, ARRAY,FUNCTION, INT, ENUM,
           STRUCT, CONST, UNSIGNED, FLOAT, UNION, VOLATILE,
           SHORT, DOUBLE, POINTER, VOID};

    typedef struct metrics{
        unsigned char size;
        unsigned char align;
        unsigned char outofline;
    }Metrics;
    typedef struct interface{
        Metrics charmetric;
        Metrics shortmetric;
        Metrics intmetric;
        Metrics floatmetric;
        Metrics doublemetric;
        Metrics ptrmetric;
        Metrics structmentric;

        //interface functions
        void(*stabblock)(int, int, Symbol*);
        void(*stabend) (Coordinate*, Symbol, Coordinate**, Symbol*, Symbol*);
        void(*stabfend)(Symbol, int);
        void(*stabline)(Coordinate*);
        void(*stabsym)(Symbol);
        void(*stabtype)(Symbol);
        
        Xinterface x;
    }Interface;

    typedef struct node* Node;
    struct node{
        short op;
        short count;
        Symbol syms[3];
        Node kids[2];
        Node link;
        Xnode x;
    };

    enum {
        F=FLOAT,
        D=DOUBLE,
        C=CHAR,
        S=SHORT,
        I=INT,
        U=UNSIGNED,
        P=POINTER,
        V=VOID,
        B=STRUCT
    };
    //node operators
    enum{
        CNST = 1<<4,
        CNSTC = CNST + C,
        CNSTD = CNST + D,
        CNSTF = CNST + F,
        CNSTI = CNST + I,
        CNSTP = CNST + P,
        CNSTS = CNST + S,
        CNSTU = CNST + U,
        ARG = 2 << 4,
        ARGB = ARG + B,
        ARGD = ARG + D,
        ARGF = ARG + F,
        ARGI = ARG + I,
        ARGP = ARG + P
    };

    extern Table constants;
    extern Table externals;
    extern Table globals;
    extern Table identifiers;
    extern Table labels;
    extern Table types;
    extern Type chartype;
    extern Type doubletype;
    extern Type floattype;
    extern Type inttype;
    extern Type longdouble;
    extern Type longtype;
    extern Type shorttype;
    extern Type signedchar;
    extern Type unsignedchar;
    extern Type unsignedlong;
    extern Type unsignedshort;
    extern Type unsignedtype;
    extern Type voidptype;
    extern Type voidtype;

};