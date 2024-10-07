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

// refer to
// http://www.patorjk.com/software/taag/#p=display&f=Big&t=Type%20Something%20

/*
   _ ____   ___    __    __  __                         _
  (_)___ \ / _ \  / /   |  \/  |                       | |
   _  __) | (_) |/ /_   | \  / | __ _ _ __  _   _  __ _| |
  | ||__ < > _ <| '_ \  | |\/| |/ _` | '_ \| | | |/ _` | |
  | |___) | (_) | (_) | | |  | | (_| | | | | |_| | (_| | |
  |_|____/ \___/ \___/  |_|  |_|\__,_|_| |_|\__,_|\__,_|_|

*/

unsigned char isa_logo[] = {
    0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x5f,
    0x20, 0x20, 0x20, 0x20, 0x5f, 0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x0a,
    0x28, 0x5f, 0x29, 0x5f, 0x5f, 0x5f, 0x20, 0x5c, 0x20, 0x2f, 0x20, 0x5f, 0x20, 0x5c, 0x20, 0x20, 0x2f, 0x20, 0x2f,
    0x20, 0x20, 0x20, 0x7c, 0x20, 0x20, 0x5c, 0x2f, 0x20, 0x20, 0x7c, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x20, 0x7c, 0x0a,
    0x20, 0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x29, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x29, 0x20, 0x7c, 0x2f, 0x20, 0x2f, 0x5f,
    0x20, 0x20, 0x20, 0x7c, 0x20, 0x5c, 0x20, 0x20, 0x2f, 0x20, 0x7c, 0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20,
    0x5f, 0x5f, 0x20, 0x20, 0x5f, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x7c, 0x20, 0x7c, 0x0a,
    0x7c, 0x20, 0x7c, 0x7c, 0x5f, 0x5f, 0x20, 0x3c, 0x20, 0x3e, 0x20, 0x5f, 0x20, 0x3c, 0x7c, 0x20, 0x27, 0x5f, 0x20,
    0x5c, 0x20, 0x20, 0x7c, 0x20, 0x7c, 0x5c, 0x2f, 0x7c, 0x20, 0x7c, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20, 0x27,
    0x5f, 0x20, 0x5c, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20, 0x7c, 0x0a,
    0x7c, 0x20, 0x7c, 0x5f, 0x5f, 0x5f, 0x29, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x29, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x29,
    0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x7c, 0x20, 0x7c, 0x20, 0x7c,
    0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x5f, 0x7c, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x0a,
    0x7c, 0x5f, 0x7c, 0x5f, 0x5f, 0x5f, 0x5f, 0x2f, 0x20, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f, 0x20, 0x5c, 0x5f, 0x5f, 0x5f,
    0x2f, 0x20, 0x20, 0x7c, 0x5f, 0x7c, 0x20, 0x20, 0x7c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x5f, 0x7c,
    0x20, 0x7c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x5f, 0x7c, 0x0a,
    0x00
};
