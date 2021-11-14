#ifndef STACK_INCLUDED
#define STACK_INCLUDED

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define type_d double
#define _FMT_ "%lf"

#define DATACANARY1 *((long long*)((char*)stk->data - sizeof(long long)))
#define DATACANARY2 *((long long*)((char*)stk->data + stk->capacity * sizeof(type_d)))
#define CAPACITY_0 8

enum {
    NO,
    YES
};

typedef struct {
    long long canary1;
    bool exst;
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

int Stack_Resize_Up (stack* stk);

int Stack_Resize_Down (stack* stk);

type_d Stack_Pop (stack* stk);

void ass (int expression, FILE* logfile);

int Stack_Dump (stack* stk);

int StaCkok (stack* stk);

void Stack_Check (stack* stk);

#endif

