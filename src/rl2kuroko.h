#include <kuroko/kuroko.h>
#include <kuroko/util.h>
#include <kuroko/value.h>
#include <kuroko/vm.h>
#include <raylib.h>

/*KRK_FUNC(rlGetScreenWidth, {
    FUNCTION_TAKES_EXACTLY(0);
    return AS_INTEGER(GetScreenWidth());
})*/

static KrkValue rlGetScreenWidth(int argc, KrkValue argv[], int hasKw) {
    if (argc != 0) return krk_runtimeError(vm.exceptions->argumentError, "rlGetScreenWidth() expects exactly 0 argument, %d given", argc);
 
    return INTEGER_VAL(AS_INTEGER(GetScreenWidth()));
}

void KurokoAssemble()
{
    krk_defineNative(&vm.builtins->fields, "GetScreenWidth", rlGetScreenWidth);
}