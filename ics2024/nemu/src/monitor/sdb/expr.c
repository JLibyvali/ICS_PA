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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>

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
    {"\\b[0-9]{1,9}\\b", TK_NUM}
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
        if (ret != 0)
        {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
        }
    }
}

typedef struct token
{
    int  type;
    char str[10];  // Max length is 9 number
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
                        tokens[nr_token] = (Token){.type = TK_LPARENT, .str = ""};
                        nr_token++;
                        break;
                    }
                case TK_RPARENT:
                    {
                        tokens[nr_token] = (Token){.type = TK_RPARENT, .str = ""};
                        nr_token++;
                        break;
                    }
                case TK_MUL:
                    {
                        tokens[nr_token] = (Token){.type = TK_MUL, .str = ""};
                        nr_token++;
                        break;
                    }
                case TK_DIV:
                    {
                        tokens[nr_token] = (Token){.type = TK_DIV, .str = ""};
                        nr_token++;
                        break;
                    }
                case TK_PLUS:
                    {
                        tokens[nr_token] = (Token){.type = TK_PLUS, .str = ""};
                        nr_token++;
                        break;
                    }
                case TK_SUB:
                    {
                        tokens[nr_token] = (Token){.type = TK_SUB, .str = ""};
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

bool expr(char *e, word_t *result)
{
    if (!make_token(e))
    {
        return false;
    }

    /* TODO: Insert codes to evaluate the expression. */
    // TODO();

    return 0;
}

bool numlen_check(const char *_str)
{
    bool       res     = false;

    char      *pattern = "\\b[0-9]{1,9}\\b";
    regex_t    regexp;
    regmatch_t pmatch;

    int        ret = regcomp(&regexp, pattern, REG_EXTENDED);
    if (ret)
    {
        char error[256];
        regerror(ret, &regexp, error, sizeof(error));
        panic("Regular expression is fault:%s.\n", error);
    }

    ret = regexec(&regexp, _str, 1, &pmatch, 0);
    if (ret)
    {
        char error[256];
        regerror(ret, &regexp, error, sizeof(error));
        panic("Regular expression Execute failed:%s.\n", error);
    }

    int len = pmatch.rm_eo - pmatch.rm_so;
}
