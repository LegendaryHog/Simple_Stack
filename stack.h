#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define _FMT_ "%lf"
#define type_t double

#define STACK_IS_BAD stk->capacity < CAPACITY_0 || stk->size < 0 || stk->size > stk->capacity || stk == NULL || stk->data == NULL\
 || stk->canary1 != 0xBE31AB || stk->canary2 !=  0xBADDED  || DATACANARY1 != 0xD1CC0C\
 || DATACANARY2 != 0xC0CA0
#define DATACANARY1 *((long long*)((char*)stk->data - sizeof(long long)))
#define DATACANARY2 *((long long*)((char*)stk->data + stk->capacity * sizeof(type_t)))
#define HASH_ADD sizeof (long long) + 2 * sizeof (size_t) + sizeof (type_t*) + sizeof (FILE*)
#define HASH_UP stk->hash = Hash_Culc (stk)



typedef struct {
    long long canary1;
    size_t size;
    size_t capacity;
    type_t* data;
    FILE* logfile;
    long long hash;
    long long canary2;
} stack;

const size_t CAPACITY_0 = 8;

long long Hash_Culc (stack* stk);

int Hash_Check (stack* stk);

void Stack_Ctor (stack* stk);

void Stack_Dtor (stack* stk);

int Stack_Push (stack* stk, type_t push);

int Stack_Resize (stack* stk);

type_t Stack_Pop (stack* stk);

void Satck_Dump (stack* stk);

void ass (int expression, FILE* logfile);

void Stack_Dump (stack* stk);

int StaCkok (stack* stk);

void Stack_Check (stack* stk);

void ass (int expression, FILE* logfile)
{
    if (expression == 0)
    {
        fprintf (logfile, "\n do meow from str %d from function %s", __LINE__, __PRETTY_FUNCTION__ );

    }
}

void Stack_Ctor (stack* stk)
{
    stk->canary1 = 0xBE31AB;
    stk->capacity = CAPACITY_0;
    stk->size = 0;
    stk->data = (type_t*) ((char*)calloc (CAPACITY_0 * sizeof (type_t) + 2 * sizeof (long long), sizeof (char)) + sizeof (long long));
    DATACANARY1 = 0xD1CC0C; // left canary of stack
    DATACANARY2 = 0xC0CA0;  //right canary of data
    stk->logfile = fopen ("logfile.txt", "w");
    stk->canary2 = 0xBADDED;
    stk->hash = Hash_Culc (stk);
    Stack_Check;
}

void Stack_Dtor (stack* stk)
{
    Stack_Check;
    fclose (stk->logfile);
    free ((char*)stk->data - sizeof(long long));
}

int Stack_Push (stack* stk, type_t push)
{
    Hash_Check (stk);
    Stack_Check (stk);
    if (stk->size >= stk->capacity)
    {
        if (Stack_Resize (stk) == 0)
        {
            Stack_Check;
            return 0;
        }
    }
    else if (isfinite (push) == 0)
    {
        fprintf (stk->logfile, "Double type overflow.");
        Stack_Check;
        return 0;
    }

    stk->size++;
    stk->data[stk->size - 1] = push;
    Stack_Check (stk);
    HASH_UP;
    return 1;
}

int Stack_Resize (stack* stk)
{
    Hash_Check (stk);
    Stack_Check;
    stk->capacity += CAPACITY_0;
    stk->data = (type_t*) ((char*)realloc ((void*)((char*)stk->data - sizeof (long long)), stk->capacity * sizeof (type_t) + 2 * sizeof(long long)) + sizeof(long long));
    if ((char*)(stk->data) - sizeof (long long)== NULL)
    {
        fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %zd;\nsize = %zd\nGo and buy new laptop.\n", stk->capacity, stk->size);
        Stack_Check;
        HASH_UP;
        return 0;
    }
    else
    {
        DATACANARY2 = *((long long*)((char*)stk->data + (stk->capacity - CAPACITY_0) * sizeof (type_t)));
        *((long long*)((char*)stk->data + (stk->capacity - CAPACITY_0) * sizeof (type_t))) = 0;
        for (int i = 0; i < CAPACITY_0; i++)
        {
            stk->data[stk->capacity - CAPACITY_0 + i] = 0;
        }
        Stack_Check;
        HASH_UP;
        return 1;
    }
}

type_t Stack_Pop (stack* stk)
{
    Hash_Check (stk);
    Stack_Check;
    if (stk->size == 0)
    {
        fprintf (stk->logfile, "!!! STACK UNDERFLOW !!!\n");
        fprintf (stk->logfile, "Stack_Pop pop 0 and don't change size");
        return 0;
    }
    stk->size--;
    type_t pop = stk->data[stk->size];
    stk->data[stk->size] = 0;
    Stack_Check;
    HASH_UP;
    return pop;
}

void Stack_Dump (stack* stk)
{
    fprintf (stk->logfile, "Stack [%p]", stk);
    if (STACK_IS_BAD)
    {
        fprintf (stk->logfile, "Stack is BAD. ERORRS:");

        if (stk->capacity < CAPACITY_0)
        {
            fprintf (stk->logfile, "Bad capacity,");
        }
        if (stk->size < 0)
        {
            fprintf (stk->logfile, "Bad size,");
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
    fprintf (stk->logfile, "{\n    canary1 = %0X\n    capacity = %zd\n    size = %zd\n    ", stk->canary1, stk->capacity, stk->size);
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

        fprintf (stk->logfile, ".\n        .\n        .\n");

        for (size_t i = stk->capacity - 16; i < stk->capacity; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "        *[%zd] = ", i, stk->data[i]);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n");

            }
            else
            {
                fprintf (stk->logfile, "         [%zd] = ", i, stk->data[i]);
                fprintf (stk->logfile, _FMT_, stk->data[i]);
                fprintf (stk->logfile, "\n");
            }
        }

    }


    fprintf (stk->logfile, "        datacanary2 = %0X\n        }\n", DATACANARY2);
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
        Stack_Dump (stk);
    }
}

long long Hash_Culc (stack* stk)
{
    long long hash = 0;

    for (int i = 0; i < sizeof (stack); i++)
    {
        if (!(i >= HASH_ADD && i < HASH_ADD + sizeof (long long)))
        {
            hash += (i + 1) * (*((char*)stk + i));
        }
    }
    for (size_t i = 0; i < stk->capacity * sizeof (type_t) + 2 * sizeof (long long); i++)
    {
        hash += (i + 1) * (*((char*)stk->data + i));
    }

    return hash;
}

int Hash_Check (stack* stk)
{
    long long hash = Hash_Culc (stk);

    if (hash != stk->hash)
    {
        fprintf (stk->logfile, "\n !!! HASH CHECK ERROR !!! \n");
        return 0;
    }
    else
    {
        return 1;
    }
}






















