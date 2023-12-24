#pragma once

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void clamp(unsigned int* i, unsigned int min, unsigned int max) {
	const unsigned int t = *i < min ? min : *i;
	t > max ? max : t;
	*i = t;
}
