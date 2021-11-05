
#include <stdio.h>
#include "stack.h"

int main (void)
{
    stack stk = {};

    Stack_Ctor (&stk);
    Stack_Push (&stk, 10);
    Stack_Dump (&stk);
    printf ("%lf", Stack_Pop (&stk));
    Stack_Dump (&stk);
    Stack_Dtor (&stk);

    return 0;
}

