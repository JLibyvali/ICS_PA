#pragma once

#include "common.h"
#include "cpu/cpu.h"
#include "isa.h"
#include "memory/paddr.h"
#include "memory/vaddr.h"

#include <stdio.h>
#include <string.h>

// ######################################
//  Definition of SDB command function
// ######################################
static int cmd_si(char *args)
{
    char *cmd        = strtok(args, " ");
    bool  print_inst = false;

    if (cmd == NULL)
    {
        cpu_exec(1);
    }
    else
    {
        // get the step N.
        size_t n   = strtoul(cmd, NULL, 10);
        print_inst = (n <= MAX_INST_TO_PRINT);

        // check any extra arguments.
        char *temp = strtok(NULL, " ");
        if (temp == NULL)
        {
            if (print_inst)
            {
                cpu_exec(n);
            }
            else
            {
                printf("Too much instructions, NO Print.\n");
                cpu_exec(n);
            }
        }
        else
        {
            printf("Usage: \"step N\", Too much arguments:\"");
            while (temp)
            {
                printf("%s ", temp);
                temp = strtok(NULL, " ");
            }
            printf("\" \n");
        }
    }

    return 0;
}

static int cmd_info(char *args)
{
    char *cmd = strtok(args, " ");
    if (cmd == NULL)
    {
        printf("Usage: \"info SUBCMD[r/w]\".\n");
        return 0;
    }

    if (!strcmp("r", cmd))
    {
        isa_reg_display();
    }
    else if (!strcmp("w", cmd))
    {
        printf("Still not implemented.\n");
    }
    else
    {
        printf("Unknown arguments \"%s\",\tUsage: \"info SUBCMD[r/w]\".\n", cmd);
        return 0;
    }
    return 0;
}

#define MAX_EXAMINE 10

static int cmd_x(char *args)
{
    char *cmd = strtok(args, " ");
    char *res = strdup("");

    // First args N.
    if (cmd == NULL)
    {
        printf("Unknown cmd \"x \",Usage: x N EXPR.\n");
        return 0;
    }

    size_t N = strtoul(cmd, NULL, 10);
    // Ensure the right N
    assert(N > 0 && N <= MAX_EXAMINE);

    // Second args EXPR.
    cmd = strtok(NULL, " ");
    if (cmd == NULL)
    {
        printf("Unknown cmd \"x %lu\",Usage: x N EXPR.\n", N);
        return 0;
    }

    if (cmd[0] == '$')
    {
        char *_name = cmd + 1;

        while (N--)
        {
            word_t addr;
            char  *buf;
            assert(isa_reg_str2val(_name, &addr));
            if (addr)
            {
                asprintf(&buf, "$%s:\t0x%lx\n", _name, addr);

                size_t len = strlen(res) + strlen(buf) + 2;
                res        = realloc(res, len);
                strcat(res, buf);

                free(buf);
                addr += sizeof(word_t);
            }
        }
    }

    if (cmd[0] == '0')
    {
        word_t addr = strtoul(cmd, NULL, 16);
        while (N--)
        {

            if (in_pmem(addr))
            {
                char  *buf;
                word_t value = vaddr_read(addr, 1);
                asprintf(&buf, "0x%lX:\t0x%lX\n", addr, value);

                size_t len = strlen(res) + strlen(buf) + 2;
                res        = realloc(res, len);
                strcat(res, buf);

                addr += sizeof(word_t);
                free(buf);
            }
            else
            {
                printf("Error Addr\n.");
            }
        }
    }

    printf("%s", res);
    return 0;
}

static int cmd_p(char *args)
{
    printf("ARGS: %s\n", args);
    return 0;
}

static int cmd_w(char *args)
{
    printf("ARGS: %s\n", args);
    return 0;
}

static int cmd_d(char *args)
{
    printf("ARGS: %s\n", args);
    return 0;
}