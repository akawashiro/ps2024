#define BOARD_SIZE 32
#define RUNNING_TIME 10
#define ALIVE 'x'
#define DEAD '.'

void lifegame() {
  // Buffer to print without libc
  char print_buf[16];

  // Clear the screen
  print_buf[0] = '\033';
  print_buf[1] = 'c';
  print_buf[2] = '\0';
  __asm__ volatile("syscall" : : "a"(1), "D"(1), "S"(print_buf), "d"(2));

  // Welcome message
  print_buf[0] = 'L';
  print_buf[1] = 'i';
  print_buf[2] = 'f';
  print_buf[3] = 'e';
  print_buf[4] = ' ';
  print_buf[5] = 'G';
  print_buf[6] = 'a';
  print_buf[7] = 'm';
  print_buf[8] = 'e';
  print_buf[9] = '\n';
  print_buf[10] = '\0';
  __asm__ volatile("syscall" : : "a"(1), "D"(1), "S"(print_buf), "d"(9));

  // Edge of the board is always dead
  char board[2][BOARD_SIZE + 2][BOARD_SIZE + 2];

  // Initialize the board
  for (int y = 0; y < BOARD_SIZE + 2; y++) {
    for (int x = 0; x < BOARD_SIZE + 2; x++) {
      board[0][y][x] = DEAD;
      if (0 < y && y < BOARD_SIZE - 1 && 0 < x && x < BOARD_SIZE - 1 &&
          (y + x) % 7 < 2) {
        board[0][y][x] = ALIVE;
      }
    }
  }

  for (int t = 0; t < RUNNING_TIME; t++) {
    // Sleep
    for (int i = 0; i < 1 * 2e9; i++) {
    }

    // Clear the screen
    print_buf[0] = '\033';
    print_buf[1] = 'c';
    print_buf[2] = '\0';
    __asm__ volatile("syscall" : : "a"(1), "D"(1), "S"(print_buf), "d"(2));

    // Print the board
    for (int y = 1; y < BOARD_SIZE - 1; y++) {
      for (int x = 1; x < BOARD_SIZE - 1; x++) {
        print_buf[0] = board[t % 2][y][x];
        print_buf[1] = '\0';
        __asm__ volatile("syscall" : : "a"(1), "D"(1), "S"(print_buf), "d"(1));
      }
      print_buf[0] = '\n';
      print_buf[1] = '\0';
      __asm__ volatile("syscall" : : "a"(1), "D"(1), "S"(print_buf), "d"(1));
    }

    // Update the board
    for (int y = 1; y < BOARD_SIZE - 1; y++) {
      for (int x = 1; x < BOARD_SIZE - 1; x++) {
        int alive_count = 0;
        for (int ky = -1; ky <= 1; ky++) {
          for (int kx = -1; kx <= 1; kx++) {
            if (ky == 0 && kx == 0)
              continue;
            if (board[t % 2][y + ky][x + kx] == ALIVE) {
              alive_count++;
            }
          }
        }
        if (board[t % 2][y][x] == DEAD) {
          if (alive_count == 3) {
            board[(t + 1) % 2][y][x] = ALIVE;
          } else {
            board[(t + 1) % 2][y][x] = DEAD;
          }
        } else {
          if (alive_count == 2 || alive_count == 3) {
            board[(t + 1) % 2][y][x] = ALIVE;
          } else {
            board[(t + 1) % 2][y][x] = DEAD;
          }
        }
      }
    }
  }

  __asm__ volatile("mov $0x401000, %rax");
  __asm__ volatile("jmp *%rax");
}

int main() {
  lifegame();
  return 0;
}
