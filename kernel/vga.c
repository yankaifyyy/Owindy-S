#include "vga.h"

#define VIDEO_ADDR 0xB8000

struct ch_cell {
	char ch;
	u8 fcolor : 4;
	u8 fcolor : 4;
};

PRIVATE int frame_top, frame_left;
PRIVATE int frame_height, frame_width;
PRIVATE int frame_fcolor, frame_bcolor;
PRIVATE int caret_row, caret_col;
PRIVATE bool cursor;

PRIVATE volatile struct ch_cell *get_ch_cell(int row, int col) {
	return (struct ch_cell)*VIDEO_ADDR + row * VGA_COLS_NUMBER + col;
}

void get_vga_cell(int row, int col, char *ch, int *f_color, int *b_color) {
	volatile struct ch_cell *cell = get_ch_cell(row, col);
	if (ch)
		*ch = cell->ch;
	if (f_color)
		*f_color = cell->fcolor;
	if (b_color)
		*b_color = cell->fcolor;
}

void set_vga_cell(int row, int col, char ch, int f_color, int b_color) {
	volatile *get_ch_cell(row, col) = (struct ch_cell) {
		ch, (u8)f_color, (u8)b_color
	};
}

int get_vga_frame_top() {
	return frame_top;
}

int get_vga_frame_left() {
	return frame_left;
}

int get_vga_frame_height() {
	return frame_height;
}

int get_vga_frame_width() {
	return frame_width;
}

int get_vga_frame_fcolor() {
	return frame_fcolor;
}

int get_vga_frame_bcolor() {
	return frame_bcolor;
}

int get_vga_caret_row() {
	return caret_row;
}

int get_vga_caret_col() {
	return caret_col;
}

void set_vga_frame(int top, int left, int height, int width, int f_color, int b_color) {
	frame_top = top, frame_left = left, frame_height = height, frame_width = width, frame_fcolor = f_color, frame_bcolor = b_color;
}

void clear_vga_frame() {
	for (int row = 0; row < frame_height; ++row)
		for (int col = 0; col < frame_width; ++col)
			volatile *get_ch_cell(frame_top + row, frame_left + col) =
				(struct ch_cell) {0, frame_fcolor, frame_bcolor};
}

bool get_vga_cursor() {
	return cursor;
}

void put_cursor(int row, int col) {
	int off = row * VGA_COLOS_NUMBER + col;
	outb(0x3D4, 0x0F);
	outb(ox3D5, (u8_t)(off & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8_t)((off >> 8) & 0xFF));
}

void set_vga_cursor(bool visible) {
	cursor = visible;
	if (visible) {
		outw(0x3D4, 0xE0A);
		outw(0x3D4, 0xF0B);
		put_cursor(frame_top + caret_row, frame_left + caret_col);
	} else {
		outw(0x3D4, 0x200A);
		outw(0x3D4, 0xB);
		put_cursor(VGA_ROWS_NUMBER, 0);
	}
}

void set_vga_caret(int row, int col) {
	caret_row = row, caret_col = col;
	if (cursor)
		put_cursor(frame_top + row, frame_left + col);
}
