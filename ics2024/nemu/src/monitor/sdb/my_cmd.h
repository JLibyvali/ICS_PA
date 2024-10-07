#pragma once

#include "cpu/cpu.h"
#include "isa.h"

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
        uint64_t n = strtoul(cmd, NULL, 10);
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
    else
    {
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
    }
    return 0;
}

static int cmd_x(char *args)
{
    printf("ARGS: %s\n", args);
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