#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOKK_DEBUG

#ifdef BLOKK_DEBUG
#define BLOKK_ASSERT(Condition, Type, ErrorMessage) \
    Blokk_Assert_IMPL(Condition, Type, ErrorMessage, __FILE__, __LINE__)
#else
#define BLOKK_ASSERT(Condition, Type, ErrorMessage)
#endif


#ifdef BLOKK_DEBUG
void Blokk_Assert_IMPL(
    bool condition,
    const char *expression,
    const char *errorMessage,
    const char *note,
    const char *file,
    int line
) {
    if (!condition) {
        fprintf(stderr,
            "Assertion failed!\n"
            "-------------------\n"
            "At File: \"%s\" Line: %d\n"
            "-------------------\n"
            ">> %s\n"
            "ERROR: ^^ %s\n"
            "------------------------------\n"
            "Note:\n"
            "%s\n"
            "__________________________________________\n",
            file,
            line,
            expression,
            errorMessage,
            note
        );

        abort();
    }
}
#endif

#ifdef BLOKK_SAFETY
#define BLOKK_ReturnIfFalse(expr, returnVal) \
    if (expr) return returnVal;
#else
#define BLOKK_ReturnIfFalse(expr, returnVal)
#endif