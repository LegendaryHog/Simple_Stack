#include "stack.h"

void ass (int expression, FILE* logfile)
{
    if (expression == 0)
    {
        fprintf (logfile, "\n do meow from str %d from function %s", __LINE__, __PRETTY_FUNCTION__ );
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//Hash
static long long Hash_Calc (stack* stk)
{
    long long hash = 0;

    for (int i = 0; i < sizeof (stack); i++)
    {
        if (!(i >= 3 * sizeof (long long) + sizeof (type_d*) + sizeof (FILE*) && i < 4 * sizeof (long long) + sizeof (type_d*) + sizeof (FILE*)))
        {
            hash += (i + 1) * (*((char*)stk + i));
        }
    }
    for (size_t i = 0; i < stk->capacity * sizeof (type_d) + 2 * sizeof (long long); i++)
    {
        hash += (i + 1) * (*((char*)stk->data + i));
    }

    return hash;
}

static int Hash_Check (stack* stk)
{
    long long hash = Hash_Calc (stk);

    if (hash != stk->hash)
    {
        for (int i = 0; i < 3; i++)
        {
            fprintf (stk->logfile, "\n !!! HASH CHECK ERROR !!! \n\n");
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

static void Hash_Up (stack* stk)
{
    stk->hash = Hash_Calc (stk);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------

void Stack_Ctor (stack* stk)
{
    stk->canary1 = 0xBE31AB;
    stk->capacity = CAPACITY_0;
    stk->size = 0;
    stk->data = (type_d*) ((char*)calloc (CAPACITY_0 * sizeof (type_d) + 2 * sizeof (long long), sizeof (char)) + sizeof (long long));
    DATACANARY1 = 0xD1CC0C; // left canary of stack
    DATACANARY2 = 0xC0CA0;  //right canary of data
    stk->logfile = fopen ("logfile.txt", "w");
    stk->canary2 = 0xBADDED;
    stk->hash = Hash_Calc (stk);
    Stack_Check (stk);
}

void Stack_Dtor (stack* stk)
{
    Stack_Check (stk);
    fclose (stk->logfile);
    free ((char*)stk->data - sizeof(long long));
}

int Stack_Push (stack* stk, type_d push)
{
    Hash_Check (stk);
    Stack_Check (stk);
    Stack_Resize_Up (stk);
    if (isfinite (push) == 0)
    {
        fprintf (stk->logfile, "Type overflow.");
        Stack_Check (stk);
        return 0;
    }

    stk->size++;
    stk->data[stk->size - 1] = push;
    Stack_Check (stk);
    Hash_Up (stk);
    return 1;
}

int Stack_Resize_Up (stack* stk)
{
    Hash_Check (stk);
    Stack_Check (stk);

    if (stk->size == stk->capacity)
    {
        stk->capacity *= 2;
        stk->data = (type_d*) ((char*)realloc ((void*)((char*)stk->data - sizeof (long long)), stk->capacity * sizeof (type_d) + 2 * sizeof(long long)) + sizeof(long long));
        if ((char*)(stk->data) - sizeof (long long)== NULL)
        {
            fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %z;\nsize = %lld\nGo and buy new laptop.\n", stk->capacity, stk->size);
            Stack_Check (stk);
            Hash_Up (stk);
            return 0;
        }
        else
        {
            DATACANARY2 = *((long long*)((char*)stk->data + stk->capacity/2 * sizeof (type_d)));
            *((long long*)((char*)stk->data + stk->capacity/2 * sizeof (type_d))) = 0;
            for (int i = 0; i < stk->capacity/2; i++)
            {
                stk->data[stk->capacity/2 + i] = 0;
            }
            Stack_Check (stk);
            Hash_Up (stk);
            return 1;
        }
    }
    else
    {
        Stack_Check (stk);
        return 0;
    }
}

int Stack_Resize_Down (stack* stk)
{
    Hash_Check (stk);
    Stack_Check (stk);

    size_t start_capacity = stk->capacity;

    if (stk->size == stk->capacity/2 - 5)
    {
        stk->capacity /= 2;
        stk->data = (type_d*) ((char*)realloc ((void*)((char*)stk->data - sizeof (long long)), stk->capacity * sizeof (type_d) + 2 * sizeof(long long)) + sizeof(long long));
        if ((char*)(stk->data) - sizeof (long long)== NULL)
        {
            fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %z;\nsize = %lld\nGo and buy new laptop.\n", stk->capacity, stk->size);
            Stack_Check (stk);
            Hash_Up (stk);
            return 0;
        }
        else
        {
            DATACANARY2 = *((long long*)((char*)stk->data + start_capacity * sizeof (type_d)));
            *((long long*)((char*)stk->data + start_capacity * sizeof (type_d))) = 0;
            Stack_Check (stk);
            Hash_Up (stk);
            return 1;
        }
    }
    else
    {
        Stack_Check (stk);
        return 0;
    }
}

type_d Stack_Pop (stack* stk)
{
    Hash_Check (stk);
    Stack_Check (stk);

    if (stk->size == 0)
    {
        fprintf (stk->logfile, "!!! STACK UNDERFLOW !!!\n");
        Stack_Dump (stk, "stack underflow,");
        return 0;
    }
    Stack_Resize_Down (stk);
    stk->size--;
    type_d pop = stk->data[stk->size];
    stk->data[stk->size] = 0;

    Hash_Up (stk);

    return pop;
}

void Stack_Dump (stack* stk, char* str)
{
    fprintf (stk->logfile, "Stack [%p]", stk);
    if (STACK_IS_BAD || *str != '\0')
    {
        fprintf (stk->logfile, "Stack is BAD. ERORRS:");

        fprintf (stk->logfile, str);

        if (stk->capacity < CAPACITY_0)
        {
            fprintf (stk->logfile, "bad capacity,");
        }
        if (stk->size < 0)
        {
            fprintf (stk->logfile, "bad size,");
        }
        if (stk->size > stk->capacity)
        {
            fprintf (stk->logfile, "stack overflow,");
        }
        if (stk == NULL || stk->data == NULL)
        {
            fprintf (stk->logfile, "Null pointer,");
        }
        if (stk->canary1 != 0xBE31AB || stk->canary2 !=  0xBADDED)
        {
            fprintf (stk->logfile, "stack canary corruption,");
        }
        if (DATACANARY1 != 0xD1CC0C || DATACANARY2 != 0xC0CA0)
        {
            fprintf (stk->logfile, "data canary corruption,");
        }
        fprintf (stk->logfile, "\n");
    }
    else
    {
        fprintf (stk->logfile, "Stack is Ok.");
        fprintf (stk->logfile, "\n");
    }
    fprintf (stk->logfile, "{\n    canary1 = %0X\n    capacity = %zd\n    size = %lld\n    ", stk->canary1, stk->capacity, stk->size);
    fprintf (stk->logfile, "data [%p]\n    {\n        ", stk->data);
    fprintf (stk->logfile, "datacanary1 = %0X\n        ", DATACANARY1);
    if (stk->capacity <= 32)
    {
        for (size_t i = 0; i < stk->capacity; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[%zd] = ", i);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");
            }
            else
            {
                fprintf (stk->logfile, " [%zd] = ", i);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");
            }
        }
    }
    else
    {
        for (size_t i = 0; i < 24; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[%zd] = ", i);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");
            }
            else
            {
                fprintf (stk->logfile, "*[%zd] = ", i);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");
            }
        }

        fprintf (stk->logfile, ".\n        .\n        .\n        ");

        for (size_t i = stk->capacity - 16; i < stk->capacity; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[%zd] = ", i, stk->data[i]);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");

            }
            else
            {
                fprintf (stk->logfile, " [%zd] = ", i, stk->data[i]);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n        ");
            }
        }

    }
    fprintf (stk->logfile, "datacanary2 = %0X\n        }\n", DATACANARY2);
    fprintf (stk->logfile, "    canary2 = %0X\n", stk->canary2);
}

int StaCkok (stack* stk)
{
    if (STACK_IS_BAD)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void Stack_Check (stack* stk)
{
    if (StaCkok (stk) == 0)
    {
        Stack_Dump (stk, "");
    }
}

