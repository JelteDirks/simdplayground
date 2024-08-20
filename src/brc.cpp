#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <arm_neon.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <unistd.h>

void print_reg(uint8x16_t reg);

int main() {
  uint8x16_t ones = vdupq_n_u8(1);
  uint8x16_t newlines = vdupq_n_u8(10);

  int fd = open("../data/measurements10M.txt", O_RDONLY);

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
  long i = 0;

  auto start = std::chrono::high_resolution_clock::now();

  for (; i + 128 < filesize; i+=128) {
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+16])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+32])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+48])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+64])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+80])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+96])), ones));
    nr_newlines += vaddvq_u8(vandq_u8(vceqq_u8(newlines, vld1q_u8(&mappedfile[i+112])), ones));
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
