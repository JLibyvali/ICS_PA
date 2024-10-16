/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "common.h"
#include "debug.h"
#include "macro.h"

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>
#include <sys/types.h>

enum
{
    TK_NOTYPE = 256,  // space
    TK_EQ,
    TK_LPARENT,
    TK_RPARENT,
    TK_MUL,
    TK_DIV,
    TK_PLUS,
    TK_SUB,
    TK_NUM

    /* TODO: Add more token types */

};

static struct rule
{
    const char *regex;
    int         token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {"\\(", TK_LPARENT},  // (
    {"\\)", TK_RPARENT},  // )
    {" +", TK_NOTYPE},    // spaces
    {"\\*", TK_MUL},      // multiplication.
    {"/", TK_DIV},        // division.
    {"\\+", TK_PLUS},     // plus
    {"-", TK_SUB},        // subtraction.
    {"==", TK_EQ},        // equal
    {"\\b[0-9]{1,32}\\b", TK_NUM}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void           init_regex()
{
    int  i;
    char error_msg[128];
    int  ret;

    for (i = 0; i < NR_REGEX; i++)
    {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret)
        {
            regerror(ret, &re[i], error_msg, 128);
            int temp = 0;
            while (temp <= i)
            {
                regfree(&re[temp]);
                temp++;
            }
            Log("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
            panic();
        }
    }
}

typedef struct token
{
    int  type;
    char str[32];  // Max length is 9 number
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int   nr_token __attribute__((used))   = 0;

static int   length                           = ARRLEN(tokens);

static bool  make_token(char *e)
{
    int        position = 0;
    int        i;
    regmatch_t pmatch;

    nr_token     = 0;
    char num[10] = {};

    while (e[position] != '\0')
    {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++)
        {
            // Every time matches from beginning. Emmmm, not good design.
            //
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
            {
                char *substr_start = e + position;
                int   substr_len   = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position,
                    substr_len, substr_len, substr_start);

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */

                switch (rules[i].token_type)
                {
                case TK_NOTYPE:
                    {
                        break;
                    }
                case TK_LPARENT:
                    {
                        tokens[nr_token] = (Token){.type = TK_LPARENT, .str = "("};
                        nr_token++;
                        break;
                    }
                case TK_RPARENT:
                    {
                        tokens[nr_token] = (Token){.type = TK_RPARENT, .str = ")"};
                        nr_token++;
                        break;
                    }
                case TK_MUL:
                    {
                        tokens[nr_token] = (Token){.type = TK_MUL, .str = "*"};
                        nr_token++;
                        break;
                    }
                case TK_DIV:
                    {
                        tokens[nr_token] = (Token){.type = TK_DIV, .str = "/"};
                        nr_token++;
                        break;
                    }
                case TK_PLUS:
                    {
                        tokens[nr_token] = (Token){.type = TK_PLUS, .str = "+"};
                        nr_token++;
                        break;
                    }
                case TK_SUB:
                    {
                        tokens[nr_token] = (Token){.type = TK_SUB, .str = "-"};
                        nr_token++;
                        break;
                    }
                case TK_NUM:
                    {
                        snprintf(num, substr_len + 1, "%s", substr_start);
                        tokens[nr_token].type = TK_NUM;
                        strcpy(tokens[nr_token].str, num);
                        nr_token++;
                        break;
                    }
                default: break;
                }

                break;
            }
        }

        if (i == NR_REGEX)
        {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }

    return true;
}

#define MAX_NUMERIC_WIDTH 32

bool numlen_check(const char *_str)
{
    char *input = malloc(strlen(_str) * sizeof(char));
    strcpy(input, _str);  // Using copy of _str.

    char      *pattern = "\\b[0-9]{1,32}\\b";
    regex_t    regexp;
    regmatch_t pmatch;

    int        ret = regcomp(&regexp, pattern, REG_EXTENDED);
    if (ret)
    {
        char error[256];
        regerror(ret, &regexp, error, sizeof(error));
        regfree(&regexp);
        Log("The Regular Expression is fault. Error:%s.\n", error);
        panic();
    }

    while (input && *input)  // Ensure that the string will be empty.
    {
        ret = regexec(&regexp, input, 1, &pmatch, 0);
        if (ret)
        {
            char msg[256];
            regerror(ret, &regexp, msg, sizeof(msg));
            regfree(&regexp);
            printf("Regex Execute \"%s\" Finished:%s.\n", pattern, msg);
            return true;
        }

        int len = pmatch.rm_eo - pmatch.rm_so;
        if (len >= MAX_NUMERIC_WIDTH)
        {

            Log("the width of num %.*s is %d too long.\n", len, _str + pmatch.rm_so, len);
            panic();
            regfree(&regexp);
            return false;
        }

        // Delete the matched part from string.
        memmove(input + pmatch.rm_so, input + pmatch.rm_eo, strlen(input + pmatch.rm_eo) + 1);
    }
    regfree(&regexp);
    return true;
}

/**
 * @brief Check the expression is surround by parentheses.
 *
 * @param head The ordinal number of subexpression head
 * @param tail The ordinal number of subexpression tail
 */
static bool parentheses_balanced(int head, int tail)
{

    if (head < 1)
        return false;

    // ### First,
    // Must check the expression is surround by parentheses.
    if (tokens[head - 1].type != TK_LPARENT || tokens[tail - 1].type == TK_RPARENT)
        return false;

    // Check parentheses balanced.
    int    i             = 0;
    char   stack[length] = {};
    size_t top           = 0;

    // Must be even number
    size_t odd_even      = 0;

    for (i = head - 1; i < tail; i++)
    {
        if (tokens[i].type == TK_LPARENT || tokens[i].type == TK_RPARENT)
            odd_even++;
    }

    if (odd_even & 1)
        return false;

    for (i = head - 1; i < tail; i++)
    {
        if (tokens[i].type == TK_LPARENT)  // If it's left parentheses, push into stack.

            stack[top++] = tokens[i].str[0];

        if (tokens[i].type == TK_RPARENT)  // If it's right parentheses, pop out stack.
        {
            if (stack[0] == '\0')
                return false;

            if (top)
                stack[--top] = '\0';
            else
                return false;
        }
    }

    if (top == 0 && stack[top] == '\0')  // If the stack is still empty, parentheses balanced.
        return true;

    return false;
}

static void print_tokens()
{
    int i = 0;
    for (; i < length; i++)
    {
        switch (tokens[i].type)
        {
        case TK_NOTYPE: break;
        default:
            {
                if (tokens[i].str[0] == '\0')
                {
                    break;
                }
                printf("tokens[%d]:\t%s\n", i, tokens[i].str);
                break;
            }
        }
    }

    if (parentheses_balanced(1, length))
        printf("The parenteses is balanced!!\n");
    else
    {
        printf("Unbalanced parentheses.\n");
    }
}

static int get_main_operation(int head, int tail)
{
    int  main_operation = -1;

    int *index_ops      = malloc(length * sizeof(int)), i;
    int *begin          = index_ops;

    memset(index_ops, -1, length);

    /**
     * @brief How to get the main operations.
     * I. main operation is a  operation.
     * II. main operation isn't surrounded by parentheses.
     * III. the main operations is the lowest precedence.
     * IV. If exists many lowest precedence operations, the most right one is main operation.
     */

    // Extract all operations.
    for (i = head - 1; i < tail; i++)
    {
        if (tokens[i].type == TK_MUL || tokens[i].type == TK_DIV || tokens[i].type == TK_PLUS ||
            tokens[i].type == TK_SUB)
            *(index_ops++) = i;  // Store the index not the operation.
    }

    if (begin[0] == -1)
        return main_operation;

    // Calculate the main operation.
    i                     = 0;
    bool is_inparentheses = false;

    // Is surround by parentheses?
    while (begin[i] != -1)
    {
        int forward = begin[i];

        for (; forward >= 0; forward--)
        {
            if (tokens[forward].type == TK_LPARENT)
            {
                is_inparentheses = true;
            }
        }

        if (is_inparentheses)
            begin[i] = -1;  // Delete the index of that operation.

        i++;
    }

    i                = 0;
    int remained_num = 0;

    // Calculate the options with precedence.
    while (begin[i] != -1)
    {
        if (begin[i] != -1)
        {
            remained_num++;
        }

        if (remained_num)
        {
            if (tokens[begin[i]].type == TK_MUL || tokens[begin[i]].type == TK_DIV)
            {
                begin[i] = -1;
                remained_num--;
            }
            else if (tokens[begin[i]].type == TK_PLUS ||
                     tokens[begin[i]].type == TK_SUB)  // main_operation is lowest precedence operation.
            {
                if (begin[i] >= main_operation)        // the most right lowest precedence operation.
                    main_operation = begin[i];
            }
            else
            {
                Log("Error operations calculate.\n");
                assert(false);
            }
        }

        i++;
    }

    return main_operation;
}

/**
 * @brief
 *
 * @param head Ordinal number of begin at expression.
 * @param tail Ordinal number of end at expression.
 * @return The ordinal number of token.
 */
int eval(int head, int tail)
{
    if (head > tail)
    {
        return -1;
    }
    else if (head == tail)
    {
        if (tokens[head - 1].type == TK_NUM)
            
    }
    else if (parentheses_balanced(head, tail))  // If expression is surround by parentheses and parentheses balanced.
    {
        return eval(head + 1, tail + 1);        // Delete the surrounding parentheses.
    }
    else                                        // Handle another expression without parentheses surrounding.
    {

        int op_main = get_main_operation(head, tail);

        if (op_main != -1)
        {
            printf("The main expression:\n");
        }
        else
        {
            Log("Get main operation failed.\n");
            panic();
        }

        int lval      = eval(head, tail - 1);
        int rval      = eval(head + 1, tail);

        int operation = tokens[op_main].type;

        switch (operation)
        {
        case TK_MUL:
            {
                return (lval * rval);
                break;
            }
        case TK_DIV:
            {
                if(rval ==0 || lval ==0){
                    Log("Error When divide operation, Val is 0.\n");
                    panic();
                }
                return ();
            }
        case TK_PLUS:
            {
            }
        case TK_SUB:
            {
            }
        }
    }
    return 0;
}

bool expr(char *e, word_t *result)
{
    if (!make_token(e))
    {
        return false;
    }

    /* TODO: Insert codes to evaluate the expression. */
    // Evaluate.
    print_tokens();

    if (eval())
    {
    }

    return 0;
}