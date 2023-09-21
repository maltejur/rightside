#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define BUFF_WIDTH 60
#define BUFF_HEIGHT 120

#define WAS_NEWLINE 1

char empty_row[BUFF_WIDTH + 2];

void print_at(unsigned x, unsigned y, char *str, unsigned strlen) {
  printf("\033[s\033[%u;%uH%.*s\033[u", x, y, strlen, str);
}

void clear() {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  for (unsigned row = 0; row < w.ws_row; row++) {
    print_at(row + 1, w.ws_col - BUFF_WIDTH - 1, empty_row, BUFF_WIDTH + 2);
  }
}

void sigint() {
  clear();
  exit(0);
}

int main(int argc, char *argv[]) {
  memset(empty_row, ' ', BUFF_WIDTH + 2);
  signal(SIGINT, sigint);
  clear();
  struct winsize w;
  char c;
  unsigned x, y;
  char buff[BUFF_HEIGHT][BUFF_WIDTH];
  memset(buff, ' ', BUFF_WIDTH * BUFF_HEIGHT);
  unsigned buff_offset = 0;
  unsigned flags = WAS_NEWLINE;

  while (1) {
    c = getchar();
    if (c == EOF)
      break;
    ioctl(0, TIOCGWINSZ, &w);
    if (!(flags & WAS_NEWLINE))
      x += 1;
    if (c == '\n')
      flags |= WAS_NEWLINE;
    else
      flags &= ~WAS_NEWLINE;
    if (x > BUFF_WIDTH || c == '\n') {
      x = 0;
      y += 1;
      if (y > BUFF_HEIGHT) {
        buff_offset += 1;
        memset(buff[(y + buff_offset) % BUFF_HEIGHT], ' ', BUFF_WIDTH);
      }
    }
    if (c != '\n')
      buff[y % BUFF_HEIGHT][x] = c;
    for (unsigned row = 0; row < w.ws_row; row++) {
      int y_offset = buff_offset;
      if (y > w.ws_row) {
        y_offset += (y % BUFF_HEIGHT) - w.ws_row;
      }
      print_at(row + 1, w.ws_col - BUFF_WIDTH - 1,
               buff[(row + y_offset) % BUFF_HEIGHT], BUFF_WIDTH);
    }
  }
  clear();
  return 0;
}
