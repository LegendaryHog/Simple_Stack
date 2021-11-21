#ifndef STACK_INCLUDED
#define STACK_INCLUDED

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define POISON 1000-7

#define data_t double
#define _FMT_ "%lf"
#define canary_t long long

#define LOW_CAPACITY 48
#define START_NUM 24
#define END_NUM stk->capacity - 16
#define VLG 34
#define NULL_PTR_EXIT if (stk == NULL){STACK_P_NULL = true; return 0;}
#define GOD_LOVE_TRIPLE 3

#define DATACANARY1 *((canary_t*)((char*)stk->data - sizeof (canary_t)))
#define DATACANARY2 *((canary_t*)((char*)stk->data + stk->capacity * sizeof(data_t)))

#define CAPACITY_0 8

typedef struct {
    long long canary1;
    long long size;
    long long capacity;
    data_t* data;
    FILE* logfile;
    long long hash;
    long long canary2;
} stack;

//--------------------------------------------
//Hash

/*static long long Hash_Calc (stack* stk);
static void Hash_Up (stack* stk);
static int Hash_Check (stack* stk);*/

//--------------------------------------------

int Stack_Ctor (stack* stk);

void Stack_Dtor (stack* stk);

int Stack_Push (stack* stk, data_t push);

int Stack_Resize (stack* stk, int mode);

data_t Stack_Pop (stack* stk);

void ass (int expression, FILE* logfile);

int Stack_Dump (stack* stk);

int StaCkok (stack* stk);

void Stack_Check (stack* stk);

#endif
