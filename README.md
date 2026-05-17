# my_ls

A reimplementation of the Unix `ls` command in C, supporting multiple flags and directory listing behaviors.

---

## Description

`my_ls` is a custom implementation of the Unix `ls` utility written in C.

It is designed for practicing system programming concepts such as:
- file system traversal
- directory handling
- sorting algorithms
- working with Unix system calls

The goal is to replicate core functionality of `ls` while keeping the implementation clean and educational.

---

## Features

- Basic file and directory listing
- Multiple output formats:
  - long format (`-l`)
  - single column (`-1`)
  - comma-separated (`-m`)
  - horizontal and column layout (`-x`, `-C`)
- Hidden files support:
  - `-a`, `-A`
- Sorting options:
  - by name
  - by size (`-S`)
  - by time (`-t`, `-u`, `-c`)
  - reverse order (`-r`)
- Recursive listing (`-R`)
- File type indicators (`-F`, `-p`)
- Metadata display:
  - inode (`-i`)
  - numeric UID/GID (`-n`)
  - no owner/group (`-g`, `-o`)
  - block sizes (`-s`)
- Symbolic link handling (`-L`)
- Multiple arguments support
- Error handling for missing files and directories

---

## Notes

- Human-readable mode (`-h`) is implemented, but its rounding behavior may differ from GNU `ls`.
  In some edge cases, size formatting may not exactly match GNU coreutils behavior.

---

## Build

```bash
make
```

This produces the my_ls binary.

## Usage

```bash
./mini_ls [options] [files...]
```

Example

```bash
./mini_ls -la /
```
![](/screenshots/my_ls_demonstration_2.png)

## Tests

The project includes an automated test suite comparing output with system ls.

Run tests:

```bash
cd tests
./test.sh
```

![](/screenshots/my_ls_test_4.png) 

## Memory Check (Valgrind)

```bash
valgrind --leak-check=full ./mini_ls -la /
```

![](/screenshots/valgrind_memory_leak_test.png)

