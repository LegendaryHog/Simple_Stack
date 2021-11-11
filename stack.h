#ifndef STACK_INCLUDED
#define STACK_INCLUDED

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
#define CAPACITY_0 8

typedef struct {
    long long canary1;
    long long size;
    long long capacity;
    type_t* data;
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

int Stack_Push (stack* stk, type_t push);

int Stack_Resize_Up (stack* stk);

int Stack_Resize_Down (stack* stk);

type_t Stack_Pop (stack* stk);

void Satck_Dump (stack* stk);

void ass (int expression, FILE* logfile);

void Stack_Dump (stack* stk);

int StaCkok (stack* stk);

void Stack_Check (stack* stk);

#endif

