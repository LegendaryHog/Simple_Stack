#include "stack.h"

int main (void)
{
    stack stk = {0};

    Stack_Ctor (&stk);

    for (int i = 0; i < 32; i++)
    {
        Stack_Push (&stk, 10);
    }

    Stack_Dump (&stk);

    for (int i = 0; i < 22; i++)
    {
        Stack_Pop (&stk);
    }

    Stack_Dump (&stk);
    Stack_Dtor (&stk);

    return 0;
}

