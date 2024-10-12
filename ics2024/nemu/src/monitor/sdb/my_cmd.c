#include "my_cmd.h"

#include "common.h"
#include "debug.h"
#include "isa.h"
#include "memory/paddr.h"
#include "memory/vaddr.h"

#include <string.h>

int cmd_si(char *args)
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

int cmd_info(char *args)
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

word_t vaddr_read(vaddr_t addr, int len);
#define MAX_EXAMINE 10

int cmd_x(char *args)
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
    panic("\"N\" is limited,\"0 < N <=10\".\n");

    // Second args EXPR.
    cmd = strtok(NULL, " ");
    if (cmd == NULL)
    {
        printf("Unknown cmd \"x %lu\",Usage: x N EXPR.\n", N);
        return 0;
    }

    word_t addr_value;
    if (cmd[0] == '$' && isa_reg_str2val((cmd + 1), &addr_value))
    {
        char *_name = cmd + 1;
        if (in_pmem(addr_value))
        {
            while (N--)
            {
                char  *buf;  // asprintf buf
                // NEMU is little-ending, So 0xHigh -> Low.
                word_t value = vaddr_read(addr_value, 4);  // First 4-bytes data

                asprintf(
                    &buf, "$%s<%lX>:\t0x%08lX\t0x%08lX\n", _name, addr_value, (value), vaddr_read((addr_value + 4), 4)
                );

                size_t len = strlen(res) + strlen(buf) + 2;
                res        = realloc(res, len);
                strcat(res, buf);

                free(buf);
                addr_value += sizeof(word_t);
            }
        }
        else
        {
            printf("Out of Memory bound.\n");
        }
    }

    if (cmd[0] == '0')
    {
        // Read the physical memory
        vaddr_t addr = strtoul(cmd, NULL, 16);
        while (N--)
        {
            if (in_pmem(addr))
            {

                char  *buf;
                word_t value = vaddr_read(addr, 4);

                asprintf(&buf, "<%08lX>:\t0x%08lX\t0x%08lX\n", addr, value, vaddr_read((addr + 4), 4));

                size_t len = strlen(res) + strlen(buf) + 2;
                res        = realloc(res, len);
                strcat(res, buf);

                addr += sizeof(word_t);
                free(buf);
            }
            else
            {
                printf("Error Address.\n");
                return 0;
            }
        }
    }

    printf("%s", res);
    return 0;
}

#define MAX_EXPR_LEN 100

bool expr(char *e, word_t *result);
bool numlen_check(const char *_str);

int  cmd_p(char *args)
{
    if (args == NULL)
        return 0;

    // Must in Heap neither the `regexec` will trigger `malloc():
    char *str_expr = malloc(MAX_EXPR_LEN);

    char *sub_expr = strtok(args, " ");
    if (strlen(sub_expr) > 31)
        assert(numlen_check(sub_expr));

    strcat(str_expr, sub_expr);

    while (sub_expr && *sub_expr)
    {
        sub_expr = strtok(NULL, " ");

        if (sub_expr && *sub_expr)
        {
            str_expr = strcat(str_expr, sub_expr);
            if (strlen(sub_expr) > 31)
                assert(numlen_check(sub_expr));
        }
        Assert(strlen(str_expr) <= MAX_EXPR_LEN, "Too long expression, length <= 100.\n");
    }

    word_t value;
    if (expr(str_expr, &value))
    {
        Log("Expression \"%s\" value is: %lu", str_expr, value);
    }
    else
    {
        printf("Expression evaluate failed.\n");
    }

    return 0;
}

int cmd_w(char *args)
{
    printf("ARGS: %s\n", args);
    return 0;
}

int cmd_d(char *args)
{
    printf("ARGS: %s\n", args);
    return 0;
}