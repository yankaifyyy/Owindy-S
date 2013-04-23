#ifndef _OWINDYS_UTIL_H_
#define _OWINDYS_UTIL_H_

#include "type.h"

static inline u8_t inb(u16_t port) {
	u8_t val;
	__asm__ __volatile__ (
			"inb %%dx, %%ax" : "=a"(val) : "d"(port)
			);
	return val;
}

static inline u16_t inw(u16_t port) {
	u16_t val;
	__asm__ __volatile__ (
			"inw %%dx, %%ax" : "=a"(val) : "d"(port)
			);
	return val;
}

static inline void outb(u16_t port, u8_t val) {
	__asm__ __volatile__ (
			"outb %%al, %%dx" : : "a"(val), "d"(port)
			);
}

static inline void outw(u16_t port, u16_t val) {
	__asm__ __volatile__ (
			"outw %%ax, %%dx" : : "a"(val), "d"(port)
			);
}

static inline u64_t rdmsr(u32_t msr) {
	u32_t low, high;
	__asm__ __volatile__ (
			"rdmsr" : "=a"(low), "=d"(high) : "c"(msr)
			);
	return (((u64_t)high << 32) | low);
}

static inline void wrmsr(u32_t msr, u64_t val) {
	u32_t low = (u32_t)val, high = (u32_t)(val >> 32);
	__asm__ __volatile__ (
			"wrmsr" : : "a"(low), "d"(high), "c"(msr)
		);
}

char *strcat(char *dst, const char *src);
char *strcpy(char *dst, const char *src);
size_t strlen(const char *str);
char *strrev(char *str);

void *memset(void *ptr, int value, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
int memcmp(const void *buf1, const void *buf2, size_t size);
void *memmem(const void *buf1, size_t size1, const void *buf2, size_t size2);

char *ultoa(unsigned long val, char *buf, int radix);

int kputchar (char ch);
int kprintf (const char *fmt, ...);

#endif // UTIL_H
