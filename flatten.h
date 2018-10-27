#ifndef TYPE
#   error "#define TYPE to be elaborated"
#endif

#define second(_,x) x

#define CAT_(x,y) x ## y
#define CAT(x,y) CAT_(x,y)
#define STR_(x) #x
#define STR(x) STR_(x)

#define HEADER STR(methods.TYPE.xi)

#define DEFINITION CAT(METHODS_,CAT(TYPE,_))

#include HEADER

DEFINITION(second)

