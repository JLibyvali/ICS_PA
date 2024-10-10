#ifndef __MY_CMD__H
#define __MY_CMD__H

#include "common.h"
#include "cpu/cpu.h"

// ######################################
//  Declaration of SDB command function
// ######################################
/**
 * @brief Single step execute.
 *
 * @param args si N(default 1)
 * @return int
 */
int cmd_si(char *args);

/**
 * @brief Show registers information.
 *
 * @param args
 * @return int
 */
int cmd_info(char *args);

/**
 * @brief Examine the memory data.
 *
 * @param args
 * @return int
 */
int cmd_x(char *args);

/**
 * @brief Print the result of expression.
 *
 * @param args
 * @return int
 */
int cmd_p(char *args);

/**
 * @brief Set the watch-point.
 *
 * @param args
 * @return int
 */
int cmd_w(char *args);

/**
 * @brief Delete the watch-point.
 *
 * @param args
 * @return int
 */
int cmd_d(char *args);

#endif