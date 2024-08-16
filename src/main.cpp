#include <arm_neon.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void print_reg(uint8x16_t reg);

int main(int argc, char** argv) {

  uint8x16_t ones = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
  uint8x16_t positions = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8x16_t newlines = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

  uint8x16_t input = {20,12,42,52,12,67,36,10,30,40,20,37,72,18,28,19};

  uint8x16_t comparison = vceqq_u8(newlines, input);

  print_reg(comparison);

  uint8x16_t inverted = veorq_u8(comparison, ones);

  print_reg(inverted);

  uint8x16_t index = vorrq_u8(inverted, positions);

  print_reg(index);

  uint8_t minimum = vminvq_u8(index);

  printf("newline:%c\n", input[minimum]);
}

void print_reg(uint8x16_t reg)
{
  for (int i = 0; i < 16; ++i) {
    printf("%X ", reg[i]);
  }
  printf("\n");
}
