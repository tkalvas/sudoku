#! /usr/bin/env python3

import sys
import time
import types

level1 = ["__9__7_5_",
          "_628_59_4",
          "4_39_2__7",
          "_15______",
          "____2____",
          "______64_",
          "3__1_65_9",
          "9_65_387_",
          "_7_2__4__"]

level3 = ["5___7____",
          "_49_3_762",
          "_1_4___8_",
          "__15_____",
          "____6____",
          "_____84__",
          "_9___3_7_",
          "728_5_64_",
          "____4___1"]

level4 = ["6___9____",
          "427__8_6_",
          "__9__27__",
          "____25___",
          "_6_7_1_8_",
          "___38____",
          "__62__5__",
          "_4_5__328",
          "____7___6"]

level5 = ["7___8_5_1",
          "___5___4_",
          "_4____9__",
          "_56__73__",
          "_1_2_5_8_",
          "__91__76_",
          "__4____5_",
          "_6___1___",
          "9_2_5___4"]

stats = types.SimpleNamespace()
stats.steps = 0
stats.branches = 0


grid = 81 * [0]

def print_grid():
    for i in range(0, 9):
        row = ''
        for j in range(0, 9):
            n = grid[coord(i, j)]
            row += ' ' + str(n)
        print(row)

def coord(i, j):
    return 9 * i + j

def assign(i, j, n):
    grid[coord(i, j)] = n

def init(data):
    for i, row in enumerate(data):
        for j, col in enumerate(row):
            if col != '_':
                assign(i, j, int(col))

def possible():
    poss = 81 * [None]
    for i in range(0, 9):
        for j in range(0, 9):
            n = grid[coord(i, j)]
            if n:
                s = set()
            else:
                s = set([1, 2, 3, 4, 5, 6, 7, 8, 9])
                for p in range(0, 9):
                    s.discard(grid[coord(p, j)])
                for q in range(0, 9):
                    s.discard(grid[coord(i, q)])
                k = i - i % 3
                l = j - j % 3
                for m in range(k, k + 3):
                    for n in range(l, l + 3):
                        s.discard(grid[coord(m, n)])
            poss[coord(i, j)] = s
    return poss

class DoneError(Exception):
    pass

def select(active_branches):
    poss = possible()
    alt = 10 * [None]
    for i in range(0, 10):
        alt[i] = []
    for i in range(0, 9):
        for j in range(0, 9):
            s = poss[coord(i, j)]
            alt[len(s)].append((i, j, s))
    if len(alt[0]) == 81:
        not_done = False
        for i in range(0, 81):
            if not grid[i]:
                not_done = True
        if not not_done:
            print(active_branches)
            raise DoneError()
        return None
    for i in range(1, 10):
        if alt[i]:
            return alt[i][0]

def solve(active_branches):
    selection = select(active_branches)
    if selection:
        i, j, s = selection
        if len(s) > 1:
            active_branches = active_branches[:] + [[i, j, None, list(s)]]
            print(len(active_branches) * ' ', i, j, list(s))
            stats.branches += 1
        for n in s:
            stats.steps += 1
            grid[coord(i, j)] = n
            if len(s) > 1:
                active_branches[-1][2] = n
            solve(active_branches)
        grid[coord(i, j)] = 0

def main():
    start = time.monotonic_ns()
    init(level5)
    try:
        solve([])
    except DoneError:
        print(stats)
        print_grid()
    stop = time.monotonic_ns()
    print('time elapsed', (stop - start) / 1000000.0, 'ms')

if __name__ == '__main__':
    main()

