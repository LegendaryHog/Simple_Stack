#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_IS_BAD stk->capacity < CAPACITY_0 || stk->size < 0 || stk->size > stk->capacity || stk == NULL || stk->data == NULL\
 || stk->canary1 != 0xBE31AB || stk->canary2 !=  0xBADDED  || *((long*)((char*)stk->data - sizeof (long))) != 0xD1CC0C\
 || *((long*)((char*)stk->data + CAPACITY_0 * sizeof(double))) != 0xC0CA0

typedef struct {
    long canary1;
    size_t size;
    size_t capacity;
    double* data;
    FILE* logfile;
    long canary2;
} stack;

const size_t CAPACITY_0 = 8;

void Stack_Ctor (stack* stk);

void Stack_Dtor (stack* stk);

int Stack_Push (stack* stk, double push);

int Stack_Resize (stack* stk);

double Stack_Pop (stack* stk);

void Satck_Dump (stack* stk);

void ass (int expression, FILE* logfile);

void Stack_Dump (stack* stk);

int StaCkok (const stack* stk);

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
    stk->data = (double*) ((char*)calloc (CAPACITY_0 * sizeof (double) + 2 * sizeof (long), sizeof (char)) + sizeof (long));
    *((long*)((char*)stk->data - sizeof (long))) = 0xD1CC0C; // left canary of stack
    *((long*)((char*)stk->data + CAPACITY_0 * sizeof(double))) = 0xC0CA0; //right canary of data
    stk->logfile = fopen ("logfile.txt", "w");
    stk->canary2 = 0xBADDED;
}

void Stack_Dtor (stack* stk)
{
    fclose (stk->logfile);
    free ((char*)stk->data - sizeof(long));
}

int Stack_Push (stack* stk, double push)
{
    stk->size++;
    if (stk->size > stk->capacity)
    {
        if (Stack_Resize (stk) == 0)
        {

            return 0;
        }
    }
    else if (isfinite (push) == 0)
    {
        fprintf (stk->logfile, "Double type overflow.");
        return 0;
    }

    stk->data[stk->size - 1] = push;


    return 1;
}

int Stack_Resize (stack* stk)
{
    stk->capacity += CAPACITY_0;
    stk->data = (double*) (realloc ((void*)((char*)stk->data - sizeof (long)), stk->capacity * sizeof (double) + 2 * sizeof(long)) + sizeof(long));
    if ((char*)(stk->data) - sizeof (long)== NULL)
    {
        fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %zd;\nsize = %zd\nGo and buy new laptop.\n", stk->capacity, stk->size);
        return 0;
    }
    else
    {
        *((long*)((char*)stk->data + stk->capacity * sizeof (double))) = *((long*)((char*)stk->data + (stk->capacity - CAPACITY_0) * sizeof (double)));
        *((long*)((char*)stk->data + (stk->capacity - CAPACITY_0) * sizeof (double))) = 0;
        return 1;
    }
}

double Stack_Pop (stack* stk)
{
    if (stk->size == 0)
    {
        fprintf (stk->logfile, "!!! STACK UNDERFLOW!!!");
    }
    stk->size--;
    double pop = stk->data[stk->size];
    stk->data[stk->size] = 0;
    return pop;
}

void Stack_Dump (stack* stk)
{
    fprintf (stk->logfile, "Stack [%p]", stk);
    StaCkok (stk);
    fprintf (stk->logfile, "{\n    canary1 = %0X\n    capacity = %zd\n    size = %zd\n    ", stk->canary1, stk->capacity, stk->size);
    fprintf (stk->logfile, "data [%p]\n    {\n        ", stk->data);
    fprintf (stk->logfile, "datacanary1 = %0X\n        ", *((long*)((char*)stk->data - sizeof (long))));
    if (stk->capacity <= 32)
    {
        for (int i = 0; i < stk->capacity; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[i] = %lf\n        ", stk->data[i]);
            }
            else
            {
                fprintf (stk->logfile, " [i] = %lf\n        ", stk->data[i]);
            }
        }
    }
    else
    {
        for (int i = 0; i < 24; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[i] = %lf\n        ", stk->data[i]);
            }
            else
            {
                fprintf (stk->logfile, " [i] = %lf\n        ", stk->data[i]);
            }
        }

        fprintf (stk->logfile, ".\n        .\n        .\n");

        for (int i = stk->capacity - 8; i < stk->capacity; i++)
        {
            if (i < stk->size)
            {
                fprintf (stk->logfile, "*[i] = %lf\n        ", stk->data[i]);
            }
            else
            {
                fprintf (stk->logfile, " [i] = %lf\n        ", stk->data[i]);
            }
        }

    }


    fprintf (stk->logfile, "datacanary2 = %0X\n        }\n", *((long*)((char*)stk->data + stk->capacity * sizeof (double))));
    fprintf (stk->logfile, "    canary2 = %0X\n", stk->canary2);

}

int StaCkok (const stack* stk)
{
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
        if (*((long*)((char*)stk->data - sizeof (long))) != 0xD1CC0C || *((long*)((char*)stk->data + CAPACITY_0 * sizeof(double))) != 0xC0CA0)
        {
            fprintf (stk->logfile, "data canary corruption,");
        }
        fprintf (stk->logfile, "\n");

        return 0;
    }
    else
    {
        fprintf (stk->logfile, "Stack is Ok.");
        fprintf (stk->logfile, "\n");
        return 1;
    }
}






















