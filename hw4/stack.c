#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "/c/cs223/hw4/stack.h"

void StackInit(stackT *stackP, int maxSize)
{
    stackP->maxSize = maxSize;
    stackP->top = -1;
    stackP->count = 0;
    stackP->contents = calloc(maxSize, sizeof(Token));
    assert(stackP->contents);
}

void StackDestroy(stackT *stackP)
{
    for (int i = 0; i <= stackP->top; i++)
    {
        free(stackP->contents[i]->str);
        free(stackP->contents[i]);
    }
    free(stackP->contents);
}

bool StackIsEmpty(stackT *stackP)
{
    if (stackP->count == 0)
        return true;
    return false;
}

bool StackIsFull(stackT *stackP)
{
    if (stackP->count == stackP->maxSize)
        return true;
    return false;
}

void StackPush(stackT *stackP, Token element)
{
    assert(element);
    if(StackIsFull(stackP))
    {
        StackDestroy(stackP);
        fprintf(stderr, "Fatal Error.\n");
        exit(1);
    }
    stackP->count++;
    stackP->top++;
    stackP->contents[stackP->top] = element;
}

Token StackPop(stackT *stackP)
{
    if(StackIsEmpty(stackP))
    {
        StackDestroy(stackP);
        fprintf(stderr, "Fatal Error.\n");
        exit(1);
    }
    Token pop = stackP->contents[stackP->top];
    stackP->count--;
    stackP->top--;
    return pop;
}

int StackCount(stackT *stackP)
{
    return stackP->count;
}

void StackPrint(stackT *stackP)
{
    printf("Stack: size: %d :", stackP->count);
    for (int i = stackP->top; i >= 0; i--)
    {
        printf("[%d %s %.1f] ", stackP->contents[i]->type, 
        stackP->contents[i]->str, stackP->contents[i]->value);
    }
    printf("\n");
}

Token StackTop(stackT *stackP)
{
    if(StackIsEmpty(stackP))
    {
        fprintf(stderr, "The stack is empty!\n");
        exit(1);
    }
    return stackP->contents[stackP->top];
}
