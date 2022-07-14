/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: theflysong
 *
 * logging.h
 *
 * 日志
 *
 */



#pragma once

#include <tayhuang/defs.h>
#include <stdarg.h>

PUBLIC void init_serial(void);
PUBLIC void write_serial_char(char ch);
PUBLIC void write_serial_str(const char *str);

enum {
    INFO = 0,
    WARN,
    ERROR,
    FATAL,
    TIPS,
    ATTENTION
};

PUBLIC void __log(const char *name, const char *type, const char *msg);
PUBLIC void _log(const char *name, const int type, const char *fmt, va_list args);
PUBLIC void log(const char *name, const char *type, const char *fmt, ...);
PUBLIC void linfo(const char *name, const char *fmt, ...);
PUBLIC void lwarn(const char *name, const char *fmt, ...);
PUBLIC void lerror(const char *name, const char *fmt, ...);
PUBLIC void lfatal(const char *name, const char *fmt, ...);
PUBLIC void ltips(const char *name, const char *fmt, ...);
PUBLIC void lattention(const char *name, const char *fmt, ...);