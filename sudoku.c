#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DEBUG 0

char *level1[] = {
  "__9__7_5_",
  "_628_59_4",
  "4_39_2__7",
  "_15______",
  "____2____",
  "______64_",
  "3__1_65_9",
  "9_65_387_",
  "_7_2__4__"
};

char *level3[] = {
  "5___7____",
  "_49_3_762",
  "_1_4___8_",
  "__15_____",
  "____6____",
  "_____84__",
  "_9___3_7_",
  "728_5_64_",
  "____4___1"
};

char *level4[] = {
  "6___9____",
  "427__8_6_",
  "__9__27__",
  "____25___",
  "_6_7_1_8_",
  "___38____",
  "__62__5__",
  "_4_5__328",
  "____7___6"
};

char *level5[] = {
  "7___8_5_1",
  "___5___4_",
  "_4____9__",
  "_56__73__",
  "_1_2_5_8_",
  "__91__76_",
  "__4____5_",
  "_6___1___",
  "9_2_5___4"
};

struct cell {
  int prev;
  int next;
  int count;
  int value;
  int mask;
} cells[100];

int line_indexes[9][8] = {
  {1, 2, 3, 4, 5, 6, 7, 8},
  {0, 2, 3, 4, 5, 6, 7, 8},
  {0, 1, 3, 4, 5, 6, 7, 8},
  {0, 1, 2, 4, 5, 6, 7, 8},
  {0, 1, 2, 3, 5, 6, 7, 8},
  {0, 1, 2, 3, 4, 6, 7, 8},
  {0, 1, 2, 3, 4, 5, 7, 8},
  {0, 1, 2, 3, 4, 5, 6, 8},
  {0, 1, 2, 3, 4, 5, 6, 7}
};

int square_indexes[9][2] = {
  {1, 2},
  {0, 2},
  {0, 1},
  {4, 5},
  {3, 5},
  {3, 4},
  {7, 8},
  {6, 8},
  {6, 7}
};

int assign_count = 0;
int step = 0;

static inline int coord(int i, int j) {
  return 10 + 9 * i + j;
}

static inline int coord_i(int x) {
  return (x - 10) / 9;
}

static inline int coord_j(int x) {
  return (x - 10) % 9;
}

void print_mask(void) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      printf(" %03o", cells[coord(i, j)].mask >> 1);
    }
    printf("\n");
  }
}

void print_solution(void) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      printf(" %d", cells[coord(i, j)].value);
    }
    printf("\n");
  }
}

void loop_insert(int loop_id, int cell_id) {
  int next = cells[loop_id].next;
  cells[loop_id].next = cell_id;
  cells[cell_id].next = next;
  cells[cell_id].prev = loop_id;
  cells[next].prev = cell_id;
}

void loop_disconnect(int cell_id) {
  int prev = cells[cell_id].prev;
  int next = cells[cell_id].next;
  cells[prev].next = next;
  cells[next].prev = prev;
}

void integrity(void) {
  for (int i = 0; i < 10; i++) {
    int p = cells[i].next;
    for (int p = cells[i].next; p != i; p = cells[p].next) {
      if (cells[p].count != i) {
        printf("[%d] cell %d count %d != %d\n", step, p, cells[p].count, i);
      }
    }
  }
  for (int i = 10; i < 91; i++) {
    if (cells[i].count != __builtin_popcount(cells[i].mask)) {
      printf("[%d] cell %d count %d does not match mask %03o\n", step, i, cells[i].count, cells[i].mask >> 1);
    }
    if (cells[i].count && cells[i].value) {
      printf("[%d] cell %d has both count %d and value %d\n", step, i, cells[i].count, cells[i].value);
    }
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int n = cells[coord(i, j)].value;
      if (n) {
        for (int p = 0; p < 8; p++) {
          int c = coord(line_indexes[i][p], j);
          if (cells[c].value == n) {
            printf("[%d] cells %d,%d and %d,%d both have value %d\n", step, i, j, line_indexes[i][p], j, n);
          }
          if (cells[c].mask & (1 << n)) {
            printf("[%d] bad mask %03o at %d,%d because %d,%d is %d\n", step, cells[c].mask >> 1, line_indexes[i][p], j, i, j, n);
          }
        }
        for (int q = 0; q < 8; q++) {
          int c = coord(i, line_indexes[j][q]);
          if (cells[c].value == n) {
            printf("[%d] cells %d,%d and %d,%d both have value %d\n", step, i, j, i, line_indexes[j][q], n);
          }
          if (cells[c].mask & (1 << n)) {
            printf("[%d] bad mask %03o at %d,%d because %d,%d is %d\n", step, cells[c].mask >> 1, i, line_indexes[j][q], i, j, n);
          }
        }
      }
    }
  }
}

void init(char **data) {
  for (int i = 0; i < 10; i++) {
    cells[i].prev = i;
    cells[i].next = i;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      loop_insert(9, coord(i, j));
      cells[coord(i, j)].value = 0;
    }
  }
  for (int i = 0; i < 9; i++) {
    char *row = data[i];
    for (int j = 0; j < 9; j++) {
      char c = row[j];
      if (c != '_') {
        loop_disconnect(coord(i, j));
        cells[coord(i, j)].value = c - '0';
        cells[coord(i, j)].mask = 0;
        loop_insert(0, coord(i, j));
        assign_count++;
      }
    }
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (!cells[coord(i, j)].value) {
        int mask = 0x3fe;
        for (int p = 0; p < 8; p++) {
          mask &= ~(1 << cells[coord(line_indexes[i][p], j)].value);
        }
        for (int q = 0; q < 8; q++) {
          mask &= ~(1 << cells[coord(i, line_indexes[j][q])].value);
            }
        for (int k = 0; k < 2; k++) {
          for (int l = 0; l < 2; l++) {
            mask &= ~(1 << cells[coord(square_indexes[i][k], square_indexes[j][l])].value);
          }
        }
        cells[coord(i, j)].mask = mask;
        int count = __builtin_popcount(mask);
        cells[coord(i, j)].count = count;
        //printf("count %d\n", count);
        loop_disconnect(coord(i, j));
        loop_insert(count, coord(i, j));
      } else {
        cells[coord(i, j)].mask = 0;
        cells[coord(i, j)].count = 0;
      }
    }
  }
#if DEBUG
  integrity();
#endif
}

int selekt(void) {
  for (int i = 1; i < 10; i++) {
    if (cells[i].next != i) {
      return cells[i].next;
    }
  }
  return -1;
}

struct save {
  int count;
  int mask;
};

void adjust_mask(int i, int j, int mask, struct save *save) {
  int deadend = 0;
  int x = coord(i, j);
  if (!cells[x].value) {
    save->mask = cells[x].mask;
    save->count = cells[x].count;
    int old_mask = cells[x].mask;
    int new_mask = old_mask & mask;
    if (old_mask != new_mask) {
      //int old_count = cells[x].count;
      cells[x].count--;
      //printf("adjust mask %d,%d=%d %d>%d %d>%d\n", i, j, x, old_mask, new_mask, old_count, cells[x].count);
      loop_disconnect(x);
      loop_insert(cells[x].count, x);
      cells[x].mask = new_mask;
    }
  }
}

void deadjust_mask(int i, int j, struct save *save) {
  int x = coord(i, j);
  if (!cells[x].value && cells[x].mask != save->mask) {
    cells[x].mask = save->mask;
    cells[x].count = save->count;
    loop_disconnect(x);
    loop_insert(save->count, x);
  }
}

void assign(int x, int n, struct save *saves) {
  //printf("assign %d %d\n", x, cells[x].count);
  loop_disconnect(x);
#if DEBUG
  if (cells[x].value) {
    printf("cell %d already has a value (mask %03o, count %d)\n", x, cells[x].mask >> 1, cells[x].count);
  }
#endif
  cells[x].value = n;
  cells[x].mask = 0;
  cells[x].count = 0;
  loop_insert(0, x);
  int i = coord_i(x);
  int j = coord_j(x);
#if DEBUG
  if (step == 49 || step == 50) printf("[%d] (%d,%d)=%d\n", step, i, j, n);
#endif
  //printf("assign %d,%d %d\n", i, j, n);
  int mask = ~(1 << n);
  for (int p = 0; p < 8; p++) {
    adjust_mask(line_indexes[i][p], j, mask, &saves[p]);
  }
  for (int q = 0; q < 8; q++) {
    adjust_mask(i, line_indexes[j][q], mask, &saves[8 + q]);
  }
  int s = 16;
  for (int k = 0; k < 2; k++) {
    for (int l = 0; l < 2; l++) {
      adjust_mask(square_indexes[i][k], square_indexes[j][l], mask, &saves[s++]);
    }
  }
  assign_count++;
  step++;
#if DEBUG
  integrity();
#endif
}

void deassign(int x, int n, struct save *saves, int mask, int count) {
  //printf("deassign %d\n", x);
  loop_disconnect(x);
#if DEBUG
  if (!cells[x].value) {
    printf("cell %d already zero\n", x);
  }
#endif
  cells[x].value = 0;
  loop_insert(count, x);
  int i = coord_i(x);
  int j = coord_j(x);
#if DEBUG
  if (step == 49) printf("[%d] (%d,%d)=%d (mask %03o, count %d)\n", step, i, j, 0, mask >> 1, count);
#endif
  for (int p = 0; p < 8; p++) {
    deadjust_mask(line_indexes[i][p], j, &saves[p]);
  }
  for (int q = 0; q < 8; q++) {
    deadjust_mask(i, line_indexes[j][q], &saves[8 + q]);
  }
  int s = 16;
  for (int k = 0; k < 2; k++) {
    for (int l = 0; l < 2; l++) {
      deadjust_mask(square_indexes[i][k], square_indexes[j][l], &saves[s++]);
    }
  }
  cells[x].count = count;
  cells[x].mask = mask;
  assign_count--;
}

int solve(void) {
#if DEBUG
  if (step == 49 || step == 50) {
    print_solution();
    print_mask();
  }
#endif
  //printf("solve %d\n", assign_count);
  if (assign_count == 81)
    return 1;
  int selection = selekt();
  if (selection > 0) {
    struct save saves[20];
    int mask = cells[selection].mask;
    int count = cells[selection].count;
    for (int n = 1; n < 10; n++) {
      if (mask & (1 << n)) {
        assign(selection, n, saves);
        if (solve())
          return 1;
        deassign(selection, n, saves, mask, count);
      }
    }
  }
  return 0;
}

char *data[9];

void input(void) {
  char buf[2];
  int line = 0;
  int column = 0;
  for (int i = 0; i < 9; i++) {
    data[i] = malloc(9);
    for (int j = 0; j < 9; j++) {
      data[i][j] = '_';
    }
  }
  while (1) {
    buf[1] = 0;
    int len = read(0, buf, 2);
    if (!len) return;
    if (isdigit(buf[0]))
      data[line][column] = buf[0];
    else if (isdigit(buf[1]))
      data[line][column] = buf[1];
    column++;
    if (buf[0] == '\n' || buf[1] == '\n') {
      line++;
      column = 0;
    }
  }
}

// incorrect solution in 5-1.sud

int main(int argc, char *argv[]) {
  struct timespec start, stop;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  input();
  init(&data[0]);
  //print_solution();
  //init(level3);
  if (!solve()) {
    printf("no solution\n");
  }
  clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
  //print_mask();
  print_solution();
  stop.tv_sec -= start.tv_sec;
  if (stop.tv_nsec < start.tv_nsec) {
    stop.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
    stop.tv_sec--;
  } else {
    stop.tv_nsec -= start.tv_nsec;
  }
  printf("time elapsed %ld.%09ld s\n", stop.tv_sec, stop.tv_nsec);
}
