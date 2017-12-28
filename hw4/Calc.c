#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "/c/cs223/hw4/stack.h"

int main(int argc, char **argv)
{
    //check for debugflag and extra flags
    if (argc > 1)
    {
        if (argc > 2)
        {
            fprintf(stderr, "Usage: Calc [-debug]\n");
            exit(1);
        }

        if (!strcmp(argv[1], "-debug"))
        {
            debugflag = true;
        }
        else
        {
            fprintf(stderr, "Usage: Calc [-debug]\n");
            exit(1);
        }
    }

    char buffer[1025];
    while (fgets(buffer, 1025, stdin))
    {
        //Token array to hold tokenized input
        Token input[1025];
        int position = 0;

        //gets rid of newline character
        buffer[strlen(buffer) - 1] = '\0';
        printf("Input: %s\n", buffer);

        //loop to tokenize input
        char *token = strtok(buffer, " ");
        while (token != NULL)
        {
            char *check = NULL;
            double val = strtod(token, &check);
            if (!*check) // current token is a number
            {
                Token w = malloc(sizeof(struct token));
                w->type = NUM;
                w->value = val;
                char * a = malloc(strlen(token) + 1);
                strcpy(a, token);
                w->str = a;
                input[position] = w;
                position++;
            }
            else if (val == 0.0) // current token is an operator or parenthesis
            {
                Token x = malloc(sizeof(struct token));
                if(!strcmp(token, "+") || !strcmp(token, "-"))
                {
                    x->type = OP1;
                }
                else if(!strcmp(token, "*") || !strcmp(token, "/"))
                {
                    x->type = OP2;
                }
                else if(!strcmp(token, "("))
                {
                    x->type = LPAR;
                }
                else if(!strcmp(token, ")"))
                {
                    x->type = RPAR;
                }
                else // catches bad tokens
                {
                    free(x);
                    for (int ab = 0; ab < position; ab++)
                    {
                        free(input[ab]->str);
                        free(input[ab]);
                    }
                    fprintf(stderr, "Fatal Error: Bad token: %s\n", token);
                    exit(1);
                }
                x->value = 0;
                char * b = malloc(strlen(token) + 1);
                strcpy(b, token);
                x->str = b;
                input[position] = x;
                position++;
            }
            else // catches bad tokens disguised as numbers
            {
                for (int ab = 0; ab < position; ab++)
                {
                    free(input[ab]->str);
                    free(input[ab]);
                }
                fprintf(stderr, "Fatal Error. Bad token: %s\n", token);
                exit(1);
            }

            //continues the loop
            token = strtok(NULL, " ");
        }

        //start of shunting-yard algorithm

        //initialize operator stack
        stackT op;
        StackInit(&op, 1025);
        if(debugflag)
            StackPrint(&op);

        //Token output array
        Token output[1025];
        int pos = 0;

        Token temp, tempx; // temp pointers to free when popping

        for(int i = 0; i < position; i++)
        {
            if(debugflag)
            {
                printf("Token:%s: type: %d value: %.2f\n", input[i]->str, 
                input[i]->type, input[i]->value);
            }
            if(input[i]->type == NUM)
            {
                output[pos] = input[i];
                pos++;
            }
            else if (input[i]->type == OP1 || input[i]->type == OP2)
            {
                while (!StackIsEmpty(&op) && 
                (input[i]->type <= StackTop(&op)->type))
                {
                    if(StackTop(&op)->type == LPAR)
                        break;
                    output[pos] = StackPop(&op);
                    pos++;
                }
                StackPush(&op, input[i]);
            }
            else if (input[i]->type == LPAR)
            {
                StackPush(&op, input[i]);
            }
            else if (input[i]->type == RPAR)
            {
                while (!StackIsEmpty(&op) && StackTop(&op)->type != LPAR)
                {
                    output[pos] = StackPop(&op);
                    pos++;
                    if (StackIsEmpty(&op)) // mismatched parentheses
                    {
                        for (int ab = 0; ab < pos; ab++)
                        {
                            free(output[ab]->str);
                            free(output[ab]);
                        }
                        StackDestroy(&op);
                        fprintf(stderr, "Error: Mismatched paretheses\n");
                        exit(1);
                    }
                }
                if(StackIsEmpty(&op))
                {
                    free(input[i]->str);
                    free(input[i]);
                    StackDestroy(&op);
                    fprintf(stderr, "Error: Mismatched paretheses\n");
                    exit(1);
                }
                //pop off the left parenthesis without adding it to the output
                temp = StackPop(&op);
                free(temp->str);
                free(temp);
                free(input[i]->str);
                free(input[i]);
            }
            if(debugflag)
                StackPrint(&op);
        }
        //takes care of remaining operators on stack
        while (!StackIsEmpty(&op))
        {
            if(StackTop(&op)->type > 2)
            {
                for (int ab = 0; ab < pos; ab++)
                {
                    free(output[ab]->str);
                    free(output[ab]);
                }
                StackDestroy(&op);
                fprintf(stderr, "Error: Mismatched paretheses\n");
                exit(1);
            }
            output[pos] = StackPop(&op);;
            pos++;
        }
        StackDestroy(&op);
        //end of shunting-yard algorithm

        //evaluating postfix notation (final step)
        
        //initialize number stack
        stackT num;
        StackInit(&num, 1025);

        if(debugflag)
            printf("OUTPUT:\n");

        double final = output[0]->value; // double to hold final result value
        for (int p = 0; p < pos; p++)
        {
            if(debugflag)
            {
                StackPrint(&num);
                printf("Token:%s: type: %d value: %.2f\n", output[p]->str, 
                output[p]->type, output[p]->value);
            }
            if (output[p]->type == NUM) // current token is a number
            {
                Token y = malloc(sizeof(struct token));
                y->type = NUM;
                y->value = output[p]->value;
                char * c = malloc(strlen(output[p]->str) + 1);
                strcpy(c, output[p]->str);
                y->str = c;

                StackPush(&num, y);
                free(output[p]->str);
                free(output[p]);
            }
            else // current token is a operator
            {
                if(StackCount(&num) < 2)
                {
                    for (int ab = p; ab < pos; ab++)
                    {
                        free(output[ab]->str);
                        free(output[ab]);
                    }
                    StackDestroy(&num);
                    fprintf(stderr, "Fatal Error.\n");
                    exit(1);
                }
                //create new token to add back to the number stack
                Token z = malloc(sizeof(struct token));
                z->type = NUM;

                //evaluate operator
                if (!strcmp(output[p]->str, "+"))
                {
                    temp = StackPop(&num);
                    tempx = StackPop(&num);

                    z->value = temp->value + tempx->value;
                    char * s = malloc(1025);
                    sprintf(s,"%.1f", z->value);
                    z->str = s;

                    free(temp->str);
                    free(temp);
                    free(tempx->str);
                    free(tempx);

                    free(output[p]->str);
                    free(output[p]);
                }
                else if (!strcmp(output[p]->str, "-"))
                {
                    temp = StackPop(&num);
                    tempx = StackPop(&num);

                    z->value = tempx->value - temp->value;
                    char * t = malloc(1025);
                    sprintf(t,"%.1f", z->value);
                    z->str = t;

                    free(temp->str);
                    free(temp);
                    free(tempx->str);
                    free(tempx);

                    free(output[p]->str);
                    free(output[p]);
                }
                else if (!strcmp(output[p]->str, "*"))
                {
                    temp = StackPop(&num);
                    tempx = StackPop(&num);

                    z->value = temp->value * tempx->value;
                    char * u = malloc(1025);
                    sprintf(u,"%.1f", z->value);
                    z->str = u;

                    free(temp->str);
                    free(temp);
                    free(tempx->str);
                    free(tempx);

                    free(output[p]->str);
                    free(output[p]);
                }
                else if (!strcmp(output[p]->str, "/"))
                {
                    temp = StackPop(&num);
                    tempx = StackPop(&num);

                    z->value = tempx->value / temp->value;
                    char * v = malloc(1025);
                    sprintf(v,"%.1f", z->value);
                    z->str = v;

                    free(temp->str);
                    free(temp);
                    free(tempx->str);
                    free(tempx);

                    free(output[p]->str);
                    free(output[p]);
                }

                StackPush(&num, z);
                final = z->value;
            }
        }

        if(StackCount(&num) > 1)
        {
            StackDestroy(&num);
            fprintf(stderr, "Fatal Error.\n");
            exit(1);
        }
        StackDestroy(&num);

        printf("Result: %.2f\n", final);
    }
    exit(0);
}
