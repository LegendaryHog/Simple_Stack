#ifndef STACK_INCLUDED
#define STACK_INCLUDED

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define POISON 1000-7

#define type_d double
#define _FMT_ "%lf"
#define canary_t long long

#define DATACANARY1 *((canary_t*)((char*)stk->data - sizeof (canary_t)))
#define DATACANARY2 *((canary_t*)((char*)stk->data + stk->capacity * sizeof(type_d)))

#define CAPACITY_0 8

typedef struct {
    long long canary1;
    long long size;
    long long capacity;
    type_d* data;
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

void Stack_Ctor (stack* stk);

void Stack_Dtor (stack* stk);

int Stack_Push (stack* stk, type_d push);

int Stack_Resize (stack* stk, int mode);

type_d Stack_Pop (stack* stk);

void ass (int expression, FILE* logfile);

int Stack_Dump (stack* stk);

int StaCkok (stack* stk);

void Stack_Check (stack* stk);

#endif

