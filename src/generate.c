// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2022 Linaro Ltd.
 * Author: Dmitry Baryshkov
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef uint32_t uint32;

#include "pm4_microcode.inl"
#include "pfp_microcode_nrt.inl"

int dump(const char *name, uint32_t *data, size_t n)
{
	FILE *f;
	int ret;
	uint32_t zero = 0;

	f = fopen(name, "w");
	if (!f) {
		fprintf(stderr, "fopen: %s: %s\n", name, strerror(errno));
		return 1;
	}

	if (fwrite(&zero, 4, 1, f) != 1) {
		fprintf(stderr, "error writing data\n");
		fclose(f);
		unlink(name);
		return 1;
	}

	ret = fwrite(data, 4, n, f);
	if (ret != n) {
		fprintf(stderr, "error writing data\n");
		fclose(f);
		unlink(name);
		return 1;
	}

	fclose(f);

	return 0;
}

int main(void)
{
	char *name;
	int ret;

	ret = asprintf(&name, "yamato_pfp_%d.fw", PFP_MICROCODE_VERSION);
	if (ret < 0)
		return 1;

	if (dump(name, aPFP_Microcode_nrt, PFP_MICROCODE_SIZE_NRT))
		return 1;

	ret = asprintf(&name, "yamato_pm4_%d.fw", PM4_MICROCODE_VERSION);
	if (ret < 0)
		return 1;

	if (dump(name, &aPM4_Microcode[0][0], PM4_MICROCODE_SIZE * 3))
		return 1;

	return 0;
}
