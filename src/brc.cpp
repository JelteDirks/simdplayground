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

uint8x16_t newlines = vdupq_n_u8(10);
uint8x16_t ones = vdupq_n_u8(1);

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
  int i = 0;

  auto start = std::chrono::high_resolution_clock::now();

  for (; i + 16 < filesize; i+=16) {
    uint8x16_t input = vld1q_u8(&mappedfile[i]);
    //print_reg(input);
    uint8x16_t compare = vceqq_u8(newlines, input);
    //print_reg(compare);
    uint8x16_t masked_ones = vandq_u8(compare, ones);
    //print_reg(masked_ones);
    nr_newlines += vaddvq_u8(masked_ones);
    //printf("%d\n", vaddvq_u8(masked_ones));
  }

  for (; i < filesize; ++i) {
    if (mappedfile[i] == '\n') {
      nr_newlines += 1;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken by loop: " << elapsed.count() << " seconds" << std::endl;

  std::cout << "newlines: " << nr_newlines << std::endl;

  close(fd);
  munmap(mappedfile, filesize);
}

void print_reg(uint8x16_t reg)
{
  for (int i = 0 ; i < 16; i++) {
    printf("%x\t", reg[i]);
  }
  printf("\n");
}
