/*
================================================================================
  File:		slow.c
--------------------------------------------------------------------------------

  This program implements a "pipe", i.e. it reads data from its standard input
  device (stdin) and outputs data to its standard output device (stdout).

  The sole purpose of this program is to simulate the "slow input" of an old
  terminal device connected to a serial line at a particular speed (baud rate).
  This can be used to "play" ancient "VT100 Videos" (aka animations) at about
  the same speed they would have been seen at a terminal in the 1980's.

  The default is about 9600 baud which outputs about 960 bytes per second,
  assuming 9600 bits per second with 1 start bit, 8 data bits, and 1 stop bit,
  (a total bit count of 10) and thus 9600 / 10 = 960 bytes per second.

  The only commandline argument is the number of bytes per second to write to
  the output device. The default is 960, the minimum is 10 bytes per second.

  Note: this program is character set agnostic, i.e. it can handle UTF-8
  character encoding transparantly by sending multiple bytes in sequence.

  The program sends two escape sequences before and after the file contents,
  respectively:
    - <esc>[H<esc>[J       # home (pos. 0,0) and clear screen
    - <esc>[24;0H          # position (24, 0)

  The best experience for any VT100 "video" is very likely if you set your
  terminal emulation (xterm) to 24 or 25 lines and 80 columns, depending on
  the file's contents. You may need to enable some "special features" if
  possible in your terminal program:
   - line drawing characters
   - double width characters
   - double height characters
   - visible bell (YMMV)

  Examples:

    $ cat xmas.txt | slow
    $ slow 480 < xmas.txt

  These examples will output 'xmas.txt' at the approximated speed of
  a 9600 or 4800 baud terminal, respectively.

  There are several "DEC VT100 animations", for instance:
    - https://vt100.net/dec/animation/xmas3.txt  (recommended)
    - https://vt100.net/dec/animation/
    - http://www.textfiles.com/artscene/vt100/

  Known issues:

    - This version requires the Posix function usleep to work, i.e. it does not
      yet work on Windows (TBD soon).

    - The calculation of the "delay" does not take into account that reading from
      the input device and writing to the output device needs some time. Therefore
      you may want to set a higher "speed" than calculated, e.g. 1100 rather than
      960 for a 9600 baud terminal simulation. This depends on your system speed
      and terminal emulation (e.g. xterm).

    - No build script included, build with: 'gcc -o slow slow.c'

  Written by: Albrecht Schlosser, Dec 2022

  License: free to use for everybody w/o restrictions. Enjoy!

================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage() {
  fprintf(stderr, "usage: slow [speed] < input.txt\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int c;
  int delay;        /* delay in microseconds */
  int bps = 960;    /* bytes per second */
  char esc = 0x1b;  /* <esc> character */

  if (argc > 2)
    usage();

  if (argc > 1) {
    bps = atoi(argv[1]);
    if (bps < 10)
      bps = 10;
  }

  delay = 1000 * 1000 / bps;

  setvbuf(stdout, NULL, _IONBF, 0); /* set stdout unbuffered */

  /* home and clear screen */
  fprintf(stdout, "%c[H%c[J", esc, esc);

  /* read input and "echo" it to the output stream with delay */
  while ((c = fgetc(stdin)) != EOF) {
    fputc(c, stdout);
    if (delay > 4)
      usleep(delay);
  }

  /* position to line 24 */
  fprintf(stdout, "%c[24;0H", esc);

  return 0;
}
