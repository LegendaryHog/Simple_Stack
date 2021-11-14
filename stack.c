#include "stack.h"

bool exst = false;

typedef struct {
    bool exst;
    char er_name[34];
} error;

static error stk_error[] = {
    {false, "stack left canary corruption"},  //(0)
    {false, "stack right canary corruption"}, //(1)
    {false, "data left canary corruption"},   //(2)
    {false, "data right canary corruption"},  //(3)
    {false, "stack underflow"},               //(4)
    {false, "stack overflow"},                //(5)
    {false, "size less null"},                //(6)
    {false, "capacity less start capacity"},  //(7)
    {false, "!!! DATA POINTER IS NULL !!!"},  //(8)
    {false, "logfile pointer is null"},       //(9)
    {false, "hash check error"},              //(10)
    {false, "!!! DOUBLE DTOR !!!"}            //(11)
};

void ass (int expression, FILE* logfile)
{
    if (expression == 0)
    {
        fprintf (logfile, "\n do meow from line %d from function %s", __LINE__, __PRETTY_FUNCTION__ );
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//Hash
static long long Hash_Calc (stack* stk)
{
    long long hash = 0;

    for (int i = 0; i < sizeof (stack); i++)
    {
        if (!(i >= (char*)&(stk->hash) - (char*)stk && i < (char*)&(stk->canary2) - (char*)stk))
        {
            hash += (i + 1) * (*((char*)stk + i));
        }
    }

    for (long long i = 0; i < stk->capacity * sizeof (type_d) + 2 * sizeof (canary_t); i++)
    {
        hash += (i + 1) * (*((char*)stk->data - sizeof (canary_t) + i));
    }

    return hash;
}

static int Hash_Check (stack* stk)
{
    long long hash = Hash_Calc (stk);

    if (hash != stk->hash)
    {
        stk_error[10].exst = true;
        for (int i = 0; i < 3; i++)
        {
            fprintf (stk->logfile, "\n !!! HASH CHECK ERROR !!! stk->hash = %lld hash = %lld\n\n", stk->hash, hash);
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
    exst = true;
    stk->capacity = CAPACITY_0;
    stk->size = 0;
    stk->data = (type_d*) ((char*) calloc (CAPACITY_0 * sizeof (type_d) + 2 * sizeof (canary_t), sizeof (char)) + sizeof (canary_t));
    DATACANARY1 = 0xD1CC0C; // left canary of stack
    DATACANARY2 = 0xC0CA0;  //right canary of data
    stk->logfile = fopen ("logfile.txt", "w");
    stk->canary2 = 0xBADDED;

    stk->hash = Hash_Calc (stk);
    Hash_Check (stk);
    Stack_Check (stk);
}

void Stack_Dtor (stack* stk)
{
    Stack_Check (stk);
    if (exst == false)
    {
        //printf ("\nstack does not exist\n");
        stk_error[11].exst = true;
        Stack_Check (stk);
    }
    else
    {
        exst = false;
        fclose (stk->logfile);
        free ((char*)stk->data - sizeof (canary_t));
        stk->data = POISON;
        stk = POISON;
    }
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
    Hash_Up (stk);
    Stack_Check (stk);
    return 1;
}

int Stack_Resize_Up (stack* stk)
{
    Hash_Check (stk);
    Stack_Check (stk);

    if (stk->size == stk->capacity)
    {
        stk->capacity *= 2;
        stk->data = (type_d*) ((char*)realloc ((void*)((char*)stk->data - sizeof (canary_t)), stk->capacity * sizeof (type_d) + 2 * sizeof (canary_t)) + sizeof (canary_t));
        if ((char*)(stk->data) - sizeof (canary_t) == NULL)
        {
            fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %lld;\nsize = %lld\nGo and buy new laptop.\n", stk->capacity, stk->size);
            Stack_Check (stk);
            Hash_Up (stk);
            return 0;
        }
        else
        {
            DATACANARY2 = *((canary_t*)((char*)stk->data + stk->capacity/2 * sizeof (type_d)));
            *((canary_t*)((char*)stk->data + stk->capacity/2 * sizeof (type_d))) = 0;
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
        stk->data = (type_d*) ((char*)realloc ((char*)stk->data - sizeof (canary_t), stk->capacity * sizeof (type_d) + 2 * sizeof (canary_t)) + sizeof (canary_t));
        if ((char*)(stk->data) - sizeof (canary_t) == NULL)
        {
            fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %z;\nsize = %lld\nGo and buy new laptop.\n", stk->capacity, stk->size);
            Stack_Check (stk);
            Hash_Up (stk);
            return 0;
        }
        else
        {
            DATACANARY2 = *((canary_t*)((char*)stk->data + start_capacity * sizeof (type_d)));
            *((canary_t*)((char*)stk->data + start_capacity * sizeof (type_d))) = 0;
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
        stk_error[4].exst = true;
        fprintf (stk->logfile, "!!! STACK UNDERFLOW !!!\n");
        Stack_Dump (stk);
        return 0;
    }
    Stack_Resize_Down (stk);
    stk->size--;
    type_d pop = stk->data[stk->size];
    stk->data[stk->size] = 0;

    Hash_Up (stk);

    return pop;
}

static int Error_Sum (void)
{
    int error_sum = 0;
    for (int i = 0; i < 12; i++)
    {
        error_sum += stk_error[i].exst;
        printf ("error %d: %d\n", i, stk_error[i].exst);
    }
    return error_sum;
}

int Stack_Dump (stack* stk)
{
    if (exst == false)
    {
        printf ("stack does not exist\n");
        return 0;
    }

    fprintf (stk->logfile, "Stack [%p]", stk);
    if (Error_Sum () > 0)
    {
        fprintf (stk->logfile, "Stack is BAD:\n");

        for (int i = 0; i < 12; i++)
        {
            if (stk_error[i].exst == true)
            {
                fprintf (stk->logfile, "%s;\n", stk_error[i].er_name);
            }
        }
    }
    else
    {
        fprintf (stk->logfile, "Stack is Ok.");
        fprintf (stk->logfile, "\n");
    }
    fprintf (stk->logfile, "{\n    exst = %d\n    canary1 = %0X\n    hash = %lld\n    capacity = %lld\n    size = %lld\n    ", exst, stk->canary1, stk->hash, stk->capacity, stk->size);
    fprintf (stk->logfile, "data [%p]\n    {\n        ", stk->data);
    if (stk->data > POISON);
    {
        fprintf (stk->logfile, "datacanary1 = %0X\n        ", DATACANARY1);
        if (stk->capacity <= 48)
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
        fprintf (stk->logfile, "datacanary2 = %0X\n    }\n", DATACANARY2);
    }
    fprintf (stk->logfile, "    canary2 = %0X\n}\n", stk->canary2);
    return 0;
}

int StaCkok (stack* stk)
{
    if (exst == true)
    {
        stk_error[0].exst = (stk->canary1 != 0xBE31AB);
        stk_error[1].exst = (stk->canary2 !=  0xBADDED);
        stk_error[2].exst = (DATACANARY1 != 0xD1CC0C);
        stk_error[3].exst = (DATACANARY2 != 0xC0CA0);
        stk_error[5].exst = (stk->size > stk->capacity);
        stk_error[6].exst = (stk->size < 0);
        stk_error[7].exst = (stk->capacity < CAPACITY_0);
        stk_error[8].exst = (stk->data == NULL);
        stk_error[9].exst = (stk->logfile == NULL);
    }

    if (Error_Sum () > 0)
    {
        printf ("sum of error bigger null");

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

