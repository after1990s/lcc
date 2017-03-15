#include "type.h"

namespace lcc{
static struct typeentry{
    struct type type;
    struct typeentry *link;
} *typetable[128];

Type chartype;
Type doubletype;
Type floattype;
Type inttype;
Type longdouble;
Type longtype;
Type shorttype;
Type signedchar;
Type unsignedchar;
Type unsignedlong;
Type unsignedshort;
Type unsignedtype;
Type voidptype;
Type voidtype;

Symbol pointersym;

extern int level;
extern int Aflag;//warming non-ANSI usage.
Type type(int op, Type ty, int size, int align, void *sym)
{
    unsigned h = (op ^ (unsigned(ty) >> 3)) & (NELEMS(typetable) - 1);
    struct typeentry *tn = nullptr;
    if (op != FUNCTION && (op != ARRAY || size > 0))
    {
        for (tn = typetable[h]; tn; tn = tn->link)
        {
            if (tn->type.op == op && tn->type.type == ty &&
                tn->type.size ==  size && tn->type.align == align &&
                tn->type.u.sym == sym)
                return &tn->type;
        }
    }
    NEW(tn, PERM);
    tn->type.op = op;
    tn->type.type = ty;
    tn->type.size = size;
    tn->type.align = align;
    tn->type.u.sym = (Symbol)sym;
    memset(&tn->type.x, 0, sizeof(tn->type.x));
    tn->link = typetable[h];
    typetable[h] = tn;
    return &tn->type;
}

void typeinit()
{
#define xx(v, name, op, metrics){\
    Symbol p = install(string(name), &types, GLOBAL, PERM);\
    v = type(op, 0, IR->metrics.size, IR->metrics.align, p);\
    p->type = v; p->addressed = IR->metrics.outofline;}

    xx(chartype, "char", IR->unsigned_char ? UNSIGNED : INT, charmetric);
    xx(doubletype, "double", FLOAT, doublemetric);
    xx(floattype, "float", FLOAT, floatmetric);
    xx(inttype, "int", INT, intmetric);
    xx(longdouble, "long double", FLOAT, longdoublemetric);
    xx(longtype, "long int", INT, longmetric);
    xx(longlong, "long long int", INT, longlongmetric);
    xx(shorttype, "short", INT, shortmetric);
    xx(signedchar, "signed char", INT, charmetric);
    xx(unsignedchar, "unsigned char", UNSIGNED, charmetric);
    xx(unsignedlong, "unsigned long", UNSIGNED, longmetric);
    xx(unsignedshort, "unsigned short", UNSIGNED, shortmetric);
    xx(unsignedtype, "unsigned int", UNSIGNED, intmetric);
    xx(unsignedlonglong, "unsigned long long", UNSIGNED, longlongmetric);

#undef xx
    Symbol p = nullptr;
    
    p = install(string("void"), &types, GLOBAL, PERM);
    voidtype = type(VOID, NULL, 0,0,p);
    p->type = voidtype;

    pointersym = install(string("T*"), &types, GLOBAL, PERM);
    pointersym->addressed = IR->ptrmetric.outofline;
    voidptype = ptr(voidtype);

}

static int maxlevel;
void rmtypes(int lev)
{
    if (maxlevel >= lev)
    {
        maxlevel = 0;
        for (int i = 0; i < NELEMS(typetable); i++)
        {
            //remove types with u.sym->scope >= lev
            struct typeentry *tn = nullptr;
            struct typeentry **tq = &typetable[i];
            while ((tn = *tq) != NULL)
            {
                if (tn->type.op == FUNCTION)
                {
                    tq = &tn->link;
                }
                else if (tn->type.u.sym && tn->type.u.sym->scope >= lev)
                {
                    *tq = tn->link;
                }
                else
                {
                    //recompute maxlevel
                    if (tn->type.u.sym && tn->type.u.sym->scope > maxlevel)
                        maxlevel = tn->type.u.sym->scope;
                    tq = &tn->link;
                }
            }
        }
    }
}

Type ptr(Type ty)
{
    return type(POINTER, ty, IR->ptrmetric.size, IR->ptrmetric.align, pointersym);
}

Type defrf(Type ty)
{
    if (isptr(ty)){
        ty = ty->type;
    }
    else
    {
        printf("type error: %s\n", "pointer expected");
        exit(0);
    }
    return isenum(ty) ? unqual(ty)->type : ty;
}

Type array(Type ty, int n, int a)
{
    if (isfunc(ty)){
        printf("illeage arry of type: %s, %s", __FILE__, __LINE__);
        exit(0);
    }
    if (level > GLOBAL && isarray(ty) && ty->size == 0)
    {
        error("missing array size");

    }
    if (ty->size==0)
    {
        if (unqual(ty) == voidtype)
        {
        error("illeegal type array: %s, %s", __FILE__, __LINE__);
        }
        else if (Aflag >= 2)
        {
            warning("");
        }
    }
    else if (n > INT_MAX / ty->size)
    {
        error("size of 'array behind max size: %s, %s", __FILE__, __LINE__);
        n= 1;
    }
    else
    {
        ;
    }
    return type(ARRAY, ty, n*ty->size, a? a : ty->align, NULL);
}

//convert array to pointer.
Type atop(Type ty)
{
    if (isarray(ty))
    {
        return ptr(ty->type);
    }
    error("array expected");
    return ptr(ty);

}

Type qual(int op, Type ty)
{
    if (isarray(ty))
    {
        ty = type(ARRAY, qual(op, ty->type), ty->size, ty->align, NULL);
    }
    else if (isfunc(ty))
    {
        warning("qualified function type ignored\n");
    }
    else if (isconst(ty) && op == CONST
    ||       isvolatile(ty) && op == VOLATILE)
    {
        error("illegal type %s %s", __FILE__, __LINE__);
    }
    else
    {
        if (isqual(ty)){
            op += ty->op;
            ty = ty->type;
        }
        ty = type(op, ty, ty->size, ty->align, NULL);
    }
    return ty;
}

Type func(Type ty, Type *proto, int style)
{
    if (ty && isarray(ty) || isfunc(ty))
    {
        error("illegal return type %s %s", __FILE__, __LINE__);
    }
    ty = type(FUNCTION, ty, 0, 0, NULL);
    ty->u.f.oldstyle = style;
    ty->u.f.proto = proto;
    return ty;
}

Type freturn(Type ty)
{
    if (isfunc(ty))
    {
        return ty->type;
    }
    error("function excepted %s %s", __FILE__, __LINE__);
    return inttype;
}

//可变参数的最后一个参数为void，
//判断最后一个参数类型来判断是否是可变参数的函数
int variadic(Type ty)
{
    if (isfunc(ty) && ty->u.f.proto)
    {
        int i = 0;
        for (i = 0; ty->u.f.proto[i]; i++)
        {
            ;
        }
        return (i>1 && ty->u.f.proto[i-1] == voidtype);
    }
    return 0;
}

Type newstruct(int op, char *tag)
{
    Symbol p=nullptr;
    if (*tag==0)
        tag = stringd(genlabel(1));
    else
    {//check for redefinition
        if ((p = lookup(tag, types)) != NULL && 
            (p->scope == level || 
            p->scope == PARAM &&
            level==PARAM+1))
            return p->type;
        else
        {
            error("redefinition of %s previously defined at %w\n", p->name, &p->src);
        }
    }
    p = install(tag, &types, level, PERM);
    p->type = type(op, NULL, 0, 0, p);
    if (p->scope > maxlevel)
        maxlevel = p->scope;
    p->src = src;
    return p->type;
}

Field newfield(char *name, Type ty, Type fty)
{
    Field p, *q = &ty->u.sym->u.s.flist;
    if (name == NULL)
    {
        name = stringd(genlabel(1));
    }
    for (p = *q; p;q = &p->link, p = *q)
    {
        if (p->name == name)
        {
            error("duplicate field name %s in %t\n",name ,ty);
        }
    }
    NEW0(p, PERM);
    *q = p;
    p->name = name;
    p->type = ty;
    return p;

}
int eqtype(Type ty1, Type ty2, int ret)
{
    if (ty1==ty2)
        return 1;
    if (ty1->op != ty2->op)
        return 0;
    switch (ty1->op)
    {
    case CHAR: case SHORT: case UNSIGNED:case INT:
    case ENUM:case UNION: case STRUCT: case DOUBLE:
        return 0;
    case POINTER:
    case VOLATILE: case VOLATILE+CONST:
    case CONST:
            return eqtype(ty1->type, ty2->type, 1);
    case ARRAY:
        if (eqtype(ty1->type, ty2->type, 1)) {
            if (ty1->size == ty2->size)
            return 1;
            if (ty1->size==0 || ty2->size==0)
            return ret;
        }
        return 0;
    case FUNCTION:
        if (eqtype(ty1->type, ty2->type, 1)){
            Type *p1 = ty1->u.f.proto, *p2 = ty2->u.f.proto;
            if (p1==p2)
                return 1;
            if (p1 && p2)
            {
                //check for compatible prototypes
                for (; *p1 && *p2; p1++, p2++)
                {
                    if (eqtype(unqual(*p1), unqual(*p2), 1) == 0)
                        return 0;
                }
                if (*p1==NULL && *p2==NULL) //参数数量相同
                {
                    return 1; 
                }

            }
            else
            {
            //????
                if (variadic(p1 ? ty1 :ty2))
                    return 0;
                if (p1==NULL)
                    p1 = p2;
                for (; *p1; p1++)
                {
                    Type ty = unqual(*p1);
                    if (promote(ty) != ty || ty == floattype)
                    {
                        return 0;
                    }
                    return 1;
                }
            }
        }
        //
    default:
        return ret;

    }
    return ret;
}
Type promote(Type ty)
{
    ty = unqual(ty);
    if (isunsigned(ty) || ty == longtype)
        return ty;
    else if (isint(ty) || isenum(ty))
        return inttype;
    else
        return ty;
}

Type compose(Type ty1, Type ty2)
{
    if (ty1==ty2)
        return ty1;
    switch (ty1->op)
    {
        case POINTER:
            return ptr(compose(ty1->type, ty2->type));
        case CONST+VOLATILE:
            return qual(CONST, qual(VOLATILE, compose(ty1->type, ty2->type)));
        case  CONST: case VOLATILE:
            return qual(ty1->op, compose(ty1->type, ty2->type));
        case ARRAY:
        //
        case FUNCTION:
        //
    }
}

int ttob(Type ty){
    switch (ty->op)
    {
    case CONST: case VOLATILE: case CONST+VOLATILE:
        return ttob(ty->type);
    case CHAR: case SHORT: case INT: case UNSIGNED:
    case VOID: case FLOAT: case DOUBLE: 
        return ty->op;
    case POINTER: case FUNCTION:
        return POINTER;
    case ARRAY: case STRUCT:case UNION:
        return STRUCT;
    case ENUM:
        return INT;
    default:
        break;
    }
    return ;//not done page 55
}
Type btot(int op)
{
    switch (optype(op))
    {
        case F: return floattype;
        
    }
}

//ex4.3. page 57
int hasproto(Type ty)
{
    Type t1 = ty;
    for (t1 = ty; t1; t1 = t1->type)
    {
        if (t1->op == FUNC)
        {
            if (!t1->u.f.proto)
            {
                return 0;
            }
        }
    }
    return 1;
}
void error(char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    char argz[5] = {0};
    for (; s!=NULL; s++)
    {
        memset(argz, 0, sizeof(argz));
        if (*s == '%')
        {
            argz[0] = *s;
            s++;
            if (s==NULL)
                return;
            switch (*s){
                case 't':
                    outtype((Type)va_arg(ap, Type));
                    break;
                case 'w':
                    break;
                default:
                    printf(argz, va_arg(ap, char*));
                       
            }
        }
    }
}
void outtype(Type ty)
{
    if (ty != NULL)
    {
        printf("type op: %d, type size: %d\n", ty->op, ty->size);
        if (ty->type != NULL)
        {
            printf("next type:");
            outtype(ty->type);
        }
        printf("\n\n");
    }
}
Field fieldref(char *name, Type ty)
{
    if (ty->op == STRUCT)
    {
        for (Field f = ty->u.sym->u.s.flist; f!=NULL; f = f->link)
        {
            if (f->name == name)
            {
                return f;
            }
        }
    }
    return NULL;
}
}