#include <arm_neon.h>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void print_reg(uint8x16_t reg);

uint8x16_t ones = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
uint8x16_t positions = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint8x16_t newlines = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

int main(int argc, char** argv) {

  if (argc < 2) {
    fprintf(stderr, "not enough arguments\n");
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "error opening file\n");
    return 1;
  }

  long nr_newlines = 0;
  uint8x16_t input;
  uint8_t read_buf[1024];
  ssize_t bytes_read;
  int i;

  while ((bytes_read = read(fd, read_buf, 1024)) > 0) {

    int vector_ops = bytes_read / 16;
    i = 0;

    while (vector_ops--) {
      input = vld1q_u8(&read_buf[i]);
      uint8x16_t compare = vceqq_u8(newlines, input);

      if (vaddvq_u8(compare)) {
        uint8_t minimum = vminvq_u8(vorrq_u8(veorq_u8(compare, ones), positions));
        nr_newlines += 1;
        read_buf[i + minimum] = '\0';
        i -= 16;
        vector_ops++;
      }
      i += 16;
    }
  }

  for (; i < bytes_read; ++i) {
    if (read_buf[i] == '\n') {
      nr_newlines += 1;
      printf("%c\n", read_buf[i]);
    }
  }

  close(fd);

  printf("newlines: %ld\n", nr_newlines);
}

void print_reg(uint8x16_t reg)
{
  for (int i = 0; i < 16; ++i) {
    printf("%X ", reg[i]);
  }
  printf("\n");
}
