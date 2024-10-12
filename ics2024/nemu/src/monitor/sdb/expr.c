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
            panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
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
        panic("The Regular Expression is fault. Error:%s.\n", error);
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
            panic("the width of num %.*s is %d too long.\n", len, _str + pmatch.rm_so, len);
            regfree(&regexp);
            return false;
        }

        // Delete the matched part from string.
        memmove(input + pmatch.rm_so, input + pmatch.rm_eo, strlen(input + pmatch.rm_eo) + 1);
    }
    regfree(&regexp);
    return true;
}

static bool parentheses_balanced()
{
    int    len = ARRLEN(tokens), i = 0;
    char   stack[32] = {};
    size_t top       = 0;

    // Must be even number
    size_t odd_even  = 0;
    for (; i < len; i++)
    {
        if (tokens[i].type == TK_LPARENT || tokens[i].type == TK_RPARENT)
            odd_even++;
    }

    if (odd_even & 1)
        return false;

    // check parentheses balanced.
    for (i = 0; i < len; i++)
    {
        if (tokens[i].type == TK_LPARENT)

            stack[top++] = tokens[i].str[0];

        if (tokens[i].type == TK_RPARENT)
        {
            if (stack[0] == '\0')
                return false;

            if (top)
                stack[--top] = '\0';
            else
                return false;
        }
    }

    if (top == 0 && stack[top] == '\0')
        return true;

    return false;
}

static void print_tokens()
{
    int len = ARRLEN(tokens), i = 0;
    for (; i < len; i++)
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

    if (parentheses_balanced())
        printf("The parenteses is balanced!!\n");
    else
    {
        printf("Unbalanced parentheses.\n");
    }
}

bool eval() { return false; }

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