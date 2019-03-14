/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2017, Jakob Kaivo <jkk@ung.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define _XOPEN_SOURCE 700
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int settime(const char *t, const char *tz)
{
	fprintf(stderr, "date: Setting clock not yet supported\n"); /* TODO */
	return t == tz;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	char *tz = getenv("TZ");
	char *format = "+%a %b %e %H:%M:%S %Z %Y";
	const char conversions[] = "aAbBcCdDeEhHIjmMnOprStTuUVwWxXyYZ%";
	const char Econversions[] = "cCxXyY";
	const char Oconversions[] = "deHImMSuUVwWy";

	int c;
	while ((c = getopt(argc, argv, "u")) != -1) {
		switch (c) {
		case 'u':
			tz = "UTC0";
			break;

		default:
			return 1;
		}
	}

	if (tz == NULL) {
		tz = "UTC0";
	}

	if (optind == argc - 1) {
		format = argv[optind];
	} else if (optind < argc - 1) {
		fprintf(stderr, "date: Too many operands\n");
		return 1;
	}

	if (format[0] != '+') {
		return settime(format, tz);
	}

	format++;	/* Skip over the leading + */
	for (char *verify = format; *verify; verify++) {
		if (*verify != '%') {
			continue;
		}

		if (!strchr(conversions, verify[1])) {
			fprintf(stderr, "date: Unrecognized conversion specification '%%%c'\n", verify[1]);
			return 1;
		}

		if (verify[1] == 'O' && !strchr(Oconversions, verify[2])) {
			fprintf(stderr, "date: Unrecognized conversion specification '%%O%c'\n", verify[2]);
			return 1;
		}

		if (verify[1] == 'E' && !strchr(Econversions, verify[2])) {
			fprintf(stderr, "date: Unrecognized conversion specification '%%E%c'\n", verify[2]);
			return 1;
		}
	}
	
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	size_t bufsize = 0;
	char *output = NULL;

	do {
		bufsize += BUFSIZ;
		output = realloc(output, bufsize);
		if (output == NULL) {
			fprintf(stderr, "date: Not enough memory to convert date\n");
			return 1;
		}
	} while (strftime(output, bufsize, format, tm) == 0);

	puts(output);

	return 0;
}
