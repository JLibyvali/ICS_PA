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

#include "local-include/reg.h"

#include "common.h"
#include "isa.h"
#include "memory/paddr.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *regs[] = {"$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void        isa_reg_display()
{

    int    i;
    char  *res  = strdup("");

    word_t temp = -1;

    printf("ALL Registers infomation.\n");

    for (i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++)
    {
        char *buf;
        if (isa_reg_str2val(regs[i], &temp))
            asprintf(&buf, "%s\t0X%lX\n", regs[i], temp);
        else
            asprintf(&buf, "%s\tErr\tErr\n", regs[i]);

        size_t len = strlen(res) + strlen(buf) + 1;
        res        = realloc(res, len);
        strcat(res, buf);

        free(buf);
    }

    printf("%s", res);
    free(res);
}

bool isa_reg_str2val(const char *name, word_t *value)
{

    bool res = false;

    if (name == NULL)
    {
        return false;
    }

    int regs_len = MUXDEF(CONFIG_RVE, 16, 32);

    while (regs_len--)
    {
        if (!strcmp(name, reg_name(regs_len)))
        {
            res    = true;
            *value = gpr(regs_len);
            return res;
        }
        else
        {
            continue;
        }
    }

    return false;
}