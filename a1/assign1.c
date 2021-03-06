#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall
 *-Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith
 *-Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter
 *-Wshadow -Wuninitialized -Wold-style-definition -c assign1.c
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign1.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 *
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

// selection sort because its fast for small arrays of ints
void intselectsort(int *a,int n)
{
  for(int i=0; i<n-1; i++) {
    int max = i; //sort in descending order so we use max instead of min

    for(int j=i+1; j < n; j++) {
      if(a[j] > a[max]) {
        max = j;
      }
    }

    if(max != i) {
      //swap two ints
      int temp = a[i];
      a[i] = a[max];
      a[max] = temp;
    }
  }

  return;
}

unsigned long byte_sort(unsigned long arg) {
  int bytes [8];
  for(int i=0; i < 8; i++) {
    bytes[i] = (arg >> (8*i)) & 0xff;
  }

  intselectsort(bytes,8); //sort the bytes (descending)

  unsigned long output = 0;
  for(int i=0; i < 8; i++) {
    output = output << 8;
    output += bytes[i];
  }

  return output;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of
 * 16 4-bit numbers, and returns an unsigned long integer containing
 * the same nibbles, sorted numerically, with smaller-valued nibbles
 * towards the "small end" of the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 *
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort(unsigned long arg) {
  int nibbles[16];
  for(int i=0; i < 16; i++) {
    nibbles[i] = (arg >> (4*i)) & 0xf;
  }

  intselectsort(nibbles,16); //sort the nibbles (descending)

  unsigned long output = 0;
  for(int i=0; i < 16; i++) {
    output = output << 4;
    output += nibbles[i];
  }

  return output;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

void eltfree(struct elt *node) {
  if(node == NULL) { // in case we try to clear an empty linked list
    return;
  }

  if(node->link != NULL) {
    eltfree(node->link); // recursively free
  }

  free(node);
}

struct elt *name_list(void) {
  const char *name = "jeffrey";
  int n = sizeof(name) / sizeof(name[0]);

  struct elt *prev = NULL;

  for(int i=n-1; i>=0; i--) {
    struct elt *new = malloc(sizeof(struct elt));

    // if malloc fails
    if(new == NULL) {
      eltfree(prev); // free existing nodes
      return NULL;
    }

    // initialize values of new elt
    new->val = name[i];
    new->link = prev;

    prev = new;
  }

  return prev;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert(enum format_t mode, unsigned long value){
  int *output;
  int n;

  // TODO: cleanup redundant code
  if(mode == OCT) {
    n = 22;
    output = malloc(sizeof(int)*n);

    for(int i=0;i<n;i++) {
      output[i] = (value >> (i*3)) & 7;
    }

  } else if(mode == BIN) {
    n = 64;
    output = malloc(sizeof(int)*n);

    for(int i=0;i<n;i++) {
      output[i] = value >> (i) & 1;
    }

  } else if(mode == HEX) {
    n = 16;
    output = malloc(sizeof(int)*n);

    for(int i=0;i<n;i++) {
      output[i] = (value >> (i*4)) & 0xF;
    }

  } else {
    return;
  }

  for(int i = n-1; i >= 0; i--) {
    int a = output[i];
    if(a>9){
      a += 55; // to get a-f for hex
    } else {
      a += 48; // 0-9
    }

    putc(a,stdout);
  }
  putc(10,stdout); // new line

  free(output);
  return;
}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 *
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me(void){
  int fd = open("me.txt", O_RDWR | O_APPEND | O_CREAT, 0777);
  write(fd,"--------------:::-----------:+---------:+ddhso++///ydddhyssyyhhhyhhddyooooshds+/\n...-----------:::-..--------:/------:/oshdddddddhhyhdddhyyyyyhhhyyhddyooooshdo+/\n...-----------::-.....------//---:/shhddddmmmmmmmdddddhhyyyyhdhyyydddyoooosddo+:\n...------------:-.......----+/-:ohddmmmmmmmmmmmmmmmdddddhhhhddhhhhhddsooossdh+/:\n...-----------::.........---++sdmmmmmmmmmdddddddddddddddddddddhhhhhddsooosydy+/:\n......--------::.........--/ydmmmmmdhysoo++++++++oossyhmmmmmmddhhhddhsoossydy+/:\n......--------:-.........:shdmmdho+/:::::::///////+++oosydmmmmmdhhhdhsoosshds+/:\n.....---------:-.......-/ydmmdy/----:::::::://////++++++ooydmmmmdddhhysssshdo+::\n......--------:-......-+ydmmh+-....------:::://////++++++ooshdmmmdddhsssssddo+::\n.....---------:......-+shmmh+:-....------:::::://////+++++ooshdmmmmdhysssydho+:-\n......--------:.....-+shmmh+:-......-------::::://////+++++ooshmmmmddhsssydyo/:-\n......--------:....-+sydmds:....``..----::::::://+ooossoo+++ooydmmmmdhyyyydso/:-\n.......------::...:osydmmh+-..``..--:::::::::/+oyyyssssssssoooshmmmmmdhyyhdso/:-\n........-----:-..-osyhdmmy/--:/+oo+//:::::///++o+++/////+++oooosdmmmmmdhyhds+/--\n........-----/..-+syhdmmds//+++++++++//:::///++//::://+++++++++ohmmmmmmdhhhs+:--\n.........---:/.-+osyddmmds:-...-:://///:-:://++//++oossssoo++++osdmNmmmdddhs+:--\n........----:/-+osyhddmmd+-----:/++++/:-..://+++syyhhddhhhhso+++oymNNmmmddhs/::-\n........----://osyhdyo/+s+::/osyhhdhs/-...://////++ooooooo+++++++odmNmmmddys/:--\n..........--/+ossyhdy/-:/+/sssssssso+-....:/+///:::////////////++ohmmNmmmdys+:--\n.........-:/++osyhhdh:.-///:://+++/:-.--..-:/++//:------::://///++hmmmmmmdhs+:--\n........-:/++oosyhdds:-://:.......--.--.`.-:/+oo+/:-----:::////++oydmdmmmmds+:--\n........://+oooshhdy/::://:.......--:----:://++++o/::-:::::////++oyddddmmmdso:--\n.......-//+ooooshhy:-:////-....---://-/+++++oys++oo+/::://////+++oyhddmmmmhs+:--\n.......-//+osooyhy:-:://+/------::////oyyooooosooo++////////++++ooyhddmmmmho+:--\n.......-++osso+ys:---::::::::::////:--/+++++++++++++++//++++++++ooyhhdmmmmyo+---\n.......-/ooss+:-.....`..-:////+++++/:---::///+++++++++o+++++++++ooyhhmmmmdyo+---\n........:oo+:..........-::///+++++++o+//+oooosssssooosso++++++++osdhmNNmmdso+---\n........:+++:-............--:/++++ooo++yhhyyyyyyhyyyyso+++++++++ohNmNNmmmdo+/---\n......-/+++++/:............-://+osyso+oso++oosssssoooo+++++++++osmNNNNmmmd++/---\n...-:/+oooosso+:.........-://+++++ooo+++oooooooo+++++++++++++oosdNNNNmmmmh+//---\n///+oooosssssso+/-.....--::/++++++++ooooossssoooo+++++++++ooooymNNNNNmmmmh//:--.\n++ooooooooooooooo+:---------:/++ooooooooosssoooo+++++++++ooosdNNNNNNNmmmmh//::-.\nooooooooooooooooooo+::------://+++oosoo++++++++++++++oooosshmNNNNNNNNNmmmds/::-.\nooooo+oooooooosyhhys+/:::::///+++++oossoooooooooo+oooossyhdNNNNNNNNNNNNmmdho/:--\noooooooosssyyyyyyyysso+/:::///++ooo+oyhhyyysssssssssyyhhhdmNNNNNNNNNNNmmmddy//:-\nossssssyyyyyyyyssyyyyys+:---://++++oohmmmddhhhhhhhhhhhhhhdNNNNNNNNNNNNNmmmmds+::\nsyyyyyyyyyyysyyyyhhyhhhy+::::/++++ooosdmmddhhhhyyyyyyyyhhmNNNNNNNNNNNNNNNNmmmmhs\nyhyyyyhhyyyssyyyhhhhhdddhs++++ooooosydmmddhhyyyyyyyyyyyhdmNNNNNNNNNNNNNNNNNNNmmm\nyyyyyyyyyyyyhhhhhddmmmdmddyssshdddmmmmmdddhhyyyyyyyyyyyhdNNNNNNNNNNNNNNNNNNNNNNN\nhhhhhhhhhhdddhdddddmdmmmmmddmNNNNNNNNNmdddhhyyyyyyyyyhhdmNNNNNNNNNNNNNNNNNNNNNNN\ndddhdddddddddddmmmmdmmmmmmmmmdmmNNNMMNNNmddhhyyyhhhhddmNNNNMMMNNNNNNNNNNNNNNNNNN\nmdddmmmmmmmmmmmmmmmmmmmmNmmNNNNNNNNNMNNMMMNNNmmmNNNNNNNNMMMMMMMMMNNNNNNNNNNNNNNN\nmmmmmmmmmmmmmmmmmmmmmmNNNNNNMMMNNNNNMMMMMMMMMMMMMMNNNNMMMMMMMMMMNNNNNNNNNNNNNNNN\ndmmmmmmmmmmmmmmmmmmmmmNNNNNMMMMNNNNNNNMMMMMMMMMNMNNNNNNMMMMMMNNNNNNNNNNNNNNNNNNN",3564);
  close(fd);
}
