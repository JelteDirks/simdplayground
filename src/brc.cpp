#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <arm_neon.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 1024 * 16

void print_reg(uint8x16_t reg);

uint8x16_t positions = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint8x16_t newlines = vdupq_n_u8(10);
uint8x16_t semicolons = vdupq_n_u8(59);

int main() {

  int fd = open("../data/measurements100M.txt", O_RDONLY);

  if (fd == -1) {
    std::cerr << "error opening file\n";
    return 1;
  }

  struct stat filestats;
  if (fstat(fd, &filestats) == -1) {
    std::cerr << "error statting file\n";
    return 1;
  }

  size_t filesize = filestats.st_size;
  uint8_t* mappedfile = (uint8_t *) mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0);

  long nr_newlines = 0;
  uint8x16_t input;
  int i = 0;
  size_t last = 0;

  while (i+16 < filesize) {
    input = vld1q_u8(&mappedfile[i]);
    uint8x16_t compare = vceqq_u8(newlines, input);

    if (vaddvq_u8(compare)) {
      const uint8_t minimum = vminvq_u8(vorrq_u8(vmvnq_u8(compare), positions));
      nr_newlines += 1;
      i += minimum + 1;
      last = i - 1;
    } else {
      i += 16;
    }
  }

  for (; i < filesize; ++i) {
    if (mappedfile[i] == '\n') {
      nr_newlines += 1;
    }
  }

  close(fd);
  munmap(mappedfile, filesize);

  std::cout << "newlines: " << nr_newlines << std::endl;
}
