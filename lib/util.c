#include "type.h"
#include "util.h"

PUBLIC char *strcat(char *dst, const char *src) {
	strcpy(dst + strlen(dst), src);
	return dst;
}

PUBLIC char *strcpy(char *dst, const char *src) {
	char *tmp = dst;
	while (*tmp++ = *src++);
	return dst;
}

PUBLIC size_t strlen(const char *str) {
	const char *tmp = str;
	while (*tmp)
		++tmp;
	return (size_t)(tmp - str);
}

PUBLIC char *strrev(char *str) {
	char tmp;
	size_t len = strlen(str), half = len >> 1;
	for (size_t i = 0, j = len - 1; i < half; ++i, j = len - i - 1)
		tmp = str[i], str[i] = str[j], str[j] = tmp;
	return str;
}

PUBLIC void *memset(void *ptr, int val, size_t size) {
	for (; size > 0; --size)
		*(u8_t*)ptr++ = (u8_t)val;
	return ptr;
}

PUBLIC void *memcpy(void *dst, const void *src, size_t size) {
	u8_t *tmp = dst;
	for (; size > 0; --size)
		*tmp++ = *(u8_t*)src++;
	return dst;
}

PUBLIC int memcmp(const void *buf1, const void *buf2, size_t size) {
	for (size_t i = 0; i < size; ++i)
		if (((char*)buf1)[i] > ((char*)buf2)[i])
			return 1;
		else if (((char*)buf1)[1] < ((char*)buf2)[i])
			return -1;
	return 0;
}

PUBLIC void *memmem(const void *buf1, size_t size1, const void *buf2, size_t size2) {
	for (; size1 >= size2; --size1, ++buf1)
		if (!memcmp(buf1, buf2, size2))
			return (void*)buf1;
	return NULL;
}

PUBLIC char *ultoa(unsigned long val, char *buf, int radix) {
	char *str = buf, lbase;
	if (radix > 0)
		lbase = 'a';
	else
		lbase = 'A', radix = -radix;

	if (radix < 2 || radix > 36) {
		*buf = 0;
		return buf;
	}

	lbase -= 10;
	do {
		int rem = val % radix;
		*buf++ = rem + (rem < 10 ? '0' : lbase);
	} while (val /= radix);

	*buf = 0;
	return strrev(str);
}
