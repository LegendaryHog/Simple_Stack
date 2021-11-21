#include "stack.h"

#define STK_L_CAN_DIE stk_error[0].exst
#define STK_R_CAN_DIE stk_error[1].exst
#define DATA_L_CAN_DIE stk_error[2].exst
#define DATA_R_CAN_DIE stk_error[3].exst
#define STK_UFLOW stk_error[4].exst
#define STK_OFLOW stk_error[5].exst
#define SIZE_LESS_NULL stk_error[6].exst
#define CAP_LESS_STRT_CAP stk_error[7].exst
#define DATA_P_NULL stk_error[8].exst
#define LOG_P_NULL stk_error[9].exst
#define HASH_ERR stk_error[10].exst
#define DOUBLE_DTOR stk_error[11].exst
#define STACK_P_NULL stk_error[12].exst
#define ERR_NUM 13

static enum{
    UP,
    DOWN
};

typedef struct {
    bool exst;
    char er_name[VLG];
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
    {false, "!!! DOUBLE DTOR !!!"},           //(11)
    {false, "Stack pointer is null"}          //(12)
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
    NULL_PTR_EXIT
    long long hash = 0;

    for (int i = 0; i < sizeof (stack); i++)
    {
        if (!(i >= (char*)&(stk->hash) - (char*)stk && i < (char*)&(stk->canary2) - (char*)stk))
        {
            hash += (i + 1) * (*((char*)stk + i));
        }
    }

    for (long long i = 0; i < stk->capacity * sizeof (data_t) + 2 * sizeof (canary_t); i++)
    {
        hash += (i + 1) * (*((char*)stk->data - sizeof (canary_t) + i));
    }

    return hash;
}

static int Hash_Check (stack* stk)
{
    NULL_PTR_EXIT
    long long hash = Hash_Calc (stk);

    if (hash != stk->hash)
    {
        HASH_ERR = true;
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

int Stack_Ctor (stack* stk)
{
    NULL_PTR_EXIT

    stk->canary1 = 0xBE31AB;
    stk->capacity = CAPACITY_0;
    stk->size = 0;
    stk->data = (data_t*) ((char*) calloc (CAPACITY_0 * sizeof (data_t) + 2 * sizeof (canary_t), sizeof (char)) + sizeof (canary_t));
    DATACANARY1 = 0xD1CC0C; // left canary of stack
    DATACANARY2 = 0xC0CA0;  //right canary of data
    stk->logfile = fopen ("logfile.txt", "w");
    stk->canary2 = 0xBADDED;

    stk->hash = Hash_Calc (stk);
    Hash_Check (stk);
    Stack_Check (stk);
    return 1;
}

void Stack_Dtor (stack* stk)
{
    Stack_Check (stk);

    NULL_PTR_EXIT
    if (stk->data == POISON)
    {
        DOUBLE_DTOR = true;
        Stack_Check (stk);
    }
    else if (stk->data == NULL)
    {
        DATA_P_NULL = true;
        Stack_Check (stk);
    }
    else
    {
        fclose (stk->logfile);
        free ((char*)stk->data - sizeof (canary_t));
        stk->data = POISON;
    }
}

int Stack_Push (stack* stk, data_t push)
{
    NULL_PTR_EXIT
    Hash_Check (stk);
    Stack_Check (stk);
    Stack_Resize(stk, UP);
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

int Stack_Resize (stack* stk, int  mode)
{
    NULL_PTR_EXIT
    Hash_Check (stk);
    Stack_Check (stk);

    long long start_capacity = stk->capacity;

    if (stk->size == stk->capacity && mode == UP || stk->size == stk->capacity/2 - 5 && stk->capacity > CAPACITY_0 && mode == DOWN)
    {
        if (mode == UP)
        {
            stk->capacity *= 2;
        }
        else
        {
            stk->capacity /= 2;
        }
        stk->data = (data_t*) ((char*)realloc ((void*)((char*)stk->data - sizeof (canary_t)), stk->capacity * sizeof (data_t) + 2 * sizeof (canary_t)) + sizeof (canary_t));
        if ((char*)(stk->data) - sizeof (canary_t) == NULL)
        {
            fprintf (stk->logfile, " !!!Stack Resize error!!! :\ncapacity = %lld;\nsize = %lld\nGo and buy new laptop.\n", stk->capacity, stk->size);
            Stack_Check (stk);
            Hash_Up (stk);
            return 0;
        }
        else
        {

            DATACANARY2 = *((canary_t*)((char*)stk->data + start_capacity * sizeof (data_t)));
            *((canary_t*)((char*)stk->data + start_capacity * sizeof (data_t))) = 0;
            if (mode == UP)
            {
                for (int i = 0; i < stk->capacity/2; i++)
                {
                    stk->data[stk->capacity/2 + i] = 0;
                }
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

data_t Stack_Pop (stack* stk)
{
    NULL_PTR_EXIT
    Hash_Check (stk);
    Stack_Check (stk);

    if (stk->size == 0)
    {
        STK_UFLOW = true;
        Stack_Check (stk);
        return 0;
    }
    Stack_Resize (stk, DOWN);
    stk->size--;
    data_t pop = stk->data[stk->size];
    stk->data[stk->size] = 0;

    Hash_Up (stk);
    Stack_Check (stk);

    return pop;
}

static int Error_Sum (void)
{
    int error_sum = 0;
    for (int i = 0; i < 12; i++)
    {
        error_sum += stk_error[i].exst;
    }
    return error_sum;
}

int Stack_Dump (stack* stk)
{
    NULL_PTR_EXIT

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
    fprintf (stk->logfile, "{\n    canary1 = %0X\n    hash = %lld\n    capacity = %lld\n    size = %lld\n    ", stk->canary1, stk->hash, stk->capacity, stk->size);
    fprintf (stk->logfile, "data [%p]\n    {\n        ", stk->data);
    if (stk->data > POISON);
    {
        fprintf (stk->logfile, "datacanary1 = %0X\n        ", DATACANARY1);
        if (stk->capacity <= LOW_CAPACITY)
        {
            for (size_t i = 0; i < stk->capacity; i++)
            {
                if (i < stk->size)
                {
                    fprintf (stk->logfile, "*[%zd] = " _FMT_ "\n        ", i, stk->data[i]);
                }
                else
                {
                    fprintf (stk->logfile, " [%zd] = " _FMT_ "\n        ", i, stk->data[i]);
                }
            }
        }
        else
        {
            for (long long i = 0; i < START_NUM; i++)
            {
                if (i < stk->size)
                {
                    fprintf (stk->logfile, "*[%zd] = " _FMT_ "\n        ", i, stk->data[i]);
                }
                else
                {
                    fprintf (stk->logfile, " [%zd] = " _FMT_ "\n        ", i, stk->data[i]);
                }
            }
            fprintf (stk->logfile, ".\n        .\n        .\n        ");

            for (long long i = END_NUM; i < stk->capacity; i++)
            {
                if (i < stk->size)
                {
                    fprintf (stk->logfile, "*[%zd] = " _FMT_ "\n        ", i, stk->data[i]);
                }
                else
                {
                    fprintf (stk->logfile, " [%zd] = " _FMT_ "\n        ", i, stk->data[i]);
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
    STACK_P_NULL = (stk == NULL);
    NULL_PTR_EXIT

    STK_L_CAN_DIE     = (stk->canary1 != 0xBE31AB);
    STK_R_CAN_DIE     = (stk->canary2 !=  0xBADDED);
    DATA_L_CAN_DIE    = (DATACANARY1 != 0xD1CC0C);
    DATA_R_CAN_DIE    = (DATACANARY2 != 0xC0CA0);
    STK_OFLOW         = (stk->size > stk->capacity);
    SIZE_LESS_NULL    = (stk->size < 0);
    CAP_LESS_STRT_CAP = (stk->capacity < CAPACITY_0);
    DATA_P_NULL       = (stk->data == NULL);
    LOG_P_NULL        = (stk->logfile == NULL);

    if (Error_Sum () > 0)
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
