/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_ESCAPE_H_
#define MDEA_ESCAPE_H_

#include "escape.h"

#include <stdlib.h>

#include "error.h"

wchar_t *mdea_escape(const wchar_t *str, wchar_t **error)
{
	size_t alloc = 0;
	size_t size = 0;
	wchar_t *buf = NULL;
	for (;;) {
		if (alloc == size) {
			alloc = alloc ? 2 * alloc : 2;
			buf = realloc(buf, alloc * sizeof(*buf));
		}
		if (*str != '\\') {
			buf[size] = *str;
			if (*str == 0)
				break;
			++str;
		} else {
			++str;
			if (*str == 0) {
				free(buf);
				mdea_error(error, L"Unexpected end of string");
				return NULL;
			}
			if (*str == '0') {
				buf[size] = 0;
				++str;
			} else if (*str == '\'') {
				buf[size] = '\'';
				++str;
			} else if (*str == '"') {
				buf[size] = '"';
				++str;
			} else if (*str == '\\') {
				buf[size] = '\\';
				++str;
			} else if (*str == 'n') {
				buf[size] = '\n';
				++str;
			} else if (*str == 'r') {
				buf[size] = '\r';
				++str;
			} else if (*str == 'v') {
				buf[size] = '\v';
				++str;
			} else if (*str == 't') {
				buf[size] = '\t';
				++str;
			} else if (*str == 'b') {
				buf[size] = '\b';
				++str;
			} else if (*str == 'f') {
				buf[size] = '\f';
				++str;
			} else if (*str == 'x') {
				++str;
				int tmp;
				if (swscanf(str, L"%02X%n", &buf[size], &tmp) < 1 || tmp != 2) {
					free(buf);
					mdea_error(error, L"Expected \\xXX");
					return NULL;
				}
				str += 2;
			} else if (*str == 'u') {
				++str;
				if (*str == '{') {
					int tmp;
					if (swscanf(str, L"{%X}%n", &buf[size], &tmp) < 1) {
						free(buf);
						mdea_error(error, L"Expected \\u{X} ... \\u{XXXXXX}");
						return NULL;
					}
					str += tmp;
				} else {
					int tmp;
					if (swscanf(str, L"%04X%n", &buf[size], &tmp) < 1 || tmp != 4) {
						free(buf);
						mdea_error(error, L"Expected \\uXXXX");
						return NULL;
					}
					str += 4;
				}
			} else {
				free(buf);
				mdea_error(error, L"Unknown escape sequence: \\%lc", *str);
				return NULL;
			}
		}
		++size;
	}
	return buf;
}

#endif  // MDEA_ESCAPE_H_
