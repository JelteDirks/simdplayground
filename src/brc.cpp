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
  long i = 0;

  auto start = std::chrono::high_resolution_clock::now();

  for (; i < filesize; ++i) {
    if (mappedfile[i] == '\n') {
      ++nr_newlines;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken by loop: " << elapsed.count() << " seconds" << std::endl;

  close(fd);
  munmap(mappedfile, filesize);

  std::cout << "newlines: " << nr_newlines << std::endl;
}
