#include <stdio.h>
#include "assign1.c"

//byte_sort
void bytesort1() {
  printf("Test: bytesort1\n");

  unsigned long output = byte_sort(0x0403deadbeef0201);
  unsigned long expected = 0xefdebead04030201;

  if(output == expected) {
    printf("TEST PASSED\n\n");
  } else {
    printf("TEST FAILED\n\n");
  }
}

void bytesort2() {
  printf("Test: bytesort2\n");

  unsigned long output = byte_sort(0x0000000000000000);
  unsigned long expected = 0x0000000000000000;

  if(output == expected) {
    printf("TEST PASSED\n\n");
  } else {
    printf("TEST FAILED\n\n");
  }
}

//nibblesort
void nibblesort1() {
  printf("Test: nibblesort1\n");

  unsigned long output = nibble_sort(0x12345678abcd9999);
  unsigned long expected = 0xdcba999987654321;

  if(output == expected) {
    printf("TEST PASSED\n\n");
  } else {
    printf("TEST FAILED\n\n");
  }
}

void nibblesort2() {
  printf("Test: nibblesort2\n");

  unsigned long output = nibble_sort(0x0403deadbeef0201);
  unsigned long expected = 0xfeeeddba43210000;

  if(output == expected) {
    printf("TEST PASSED\n\n");
  } else {
    printf("TEST FAILED\n\n");
  }
}

int main(int argc, const char* argv[]) {
  printf("BEGIN TESTS\n\n");

  bytesort1();
  bytesort2();

  nibblesort1();
  nibblesort2();

  struct elt *elt1 = name_list();

  struct elt *walk = elt1;
  while(walk != NULL){
    printf("%c",walk->val);
    walk = walk->link;
  }
  eltfree(elt1);

  printf("\nConvert: ");

  printf("\n: 570017771\n ");
  convert(OCT, 98574329);
  printf("\nConvert: 101111000000001111111111001\n ");
  convert(BIN, 98574329);
  printf("\nConvert: 5E01FF9\n");
  convert(HEX, 98574329);

  printf("END TESTS\n");

  draw_me();
  return 0;
}
