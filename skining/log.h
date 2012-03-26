/**
 * ログ出力
 */
#pragma once

#define LOG_ENABLE
#define LOG_E_ENABLE
#define LOG_W_ENABLE
#define LOG_I_ENABLE
#define LOG_D_ENABLE
#define LOG_V_ENABLE

#ifdef LOG_ENABLE
#include <stdio.h>
void log_fprint(FILE* file, const char* func, const char* filename, int line, const char* format, ...);
#endif // LOG_ENABLE

#if defined(LOG_ENABLE) && defined(LOG_E_ENABLE)
#define Log_e(fmt, ...)	\
	log_fprint(stderr, "E/" ## __FUNCTION__, __FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define Log_e(fmt, ...) (void)0
#endif // defined(LOG_ENABLE) && defined(LOG_E_ENABLE)

#if defined(LOG_ENABLE) && defined(LOG_W_ENABLE)
#define Log_w(fmt, ...)	\
	log_fprint(stdout, "W/" ## __FUNCTION__, __FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define Log_w(fmt, ...) (void)0
#endif // defined(LOG_ENABLE) && defined(LOG_W_ENABLE)

#if defined(LOG_ENABLE) && defined(LOG_I_ENABLE)
#define Log_i(fmt, ...)	\
	log_fprint(stdout, "I/" ## __FUNCTION__, __FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define Log_i(fmt, ...) (void)0
#endif // defined(LOG_ENABLE) && defined(LOG_I_ENABLE)

#if defined(LOG_ENABLE) && defined(LOG_D_ENABLE)
#define Log_d(fmt, ...)	\
	log_fprint(stdout, "D/" ## __FUNCTION__, __FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define Log_d(fmt, ...) (void)0
#endif // defined(LOG_ENABLE) && defined(LOG_D_ENABLE)

#if defined(LOG_ENABLE) && defined(LOG_V_ENABLE)
#define Log_v(fmt, ...)	\
	log_fprint(stdout, "V/" ## __FUNCTION__, __FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define Log_v(fmt, ...) (void)0
#endif // defined(LOG_ENABLE) && defined(LOG_V_ENABLE)