#include "stack.h"

int main (void)
{
    stack stk = {0};
    printf ("%zd", sizeof (bool));

    Stack_Ctor (&stk);
    Stack_Check (&stk);
    Stack_Dump (&stk);
    for (int i = 0; i < 2000; i++)
    {
       Stack_Push (&stk, 10);
    }
    Stack_Dump (&stk);
    Stack_Pop (&stk);
    Stack_Dump (&stk);
    //Stack_Dtor (&stk);
    //Stack_Dtor (&stk);

    return 0;
}

