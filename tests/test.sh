#!/usr/bin/env bash

set -euo pipefail

MINI_LS="./my_ls"

if [[ ! -x "$MINI_LS" ]]; then
    echo "Error: '$MINI_LS' not found or is not executable"
    exit 2
fi

TMP_DIR="out"
mkdir "$TMP_DIR"
trap 'rm -rf "$TMP_DIR"' EXIT

MINI_OUT="$TMP_DIR/mini.out"
LS_OUT="$TMP_DIR/ls.out"

run_in_pty() {
    local output_file=$1
    shift

	script -q -c "$*" /dev/null \
	| sed '1d;$d' \
	> "$output_file"

	#sed -i 's/\t/        /g' $output_file
}

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

passed=0
failed=0
test_case() {
    local test_name=$1
    shift

    echo -e "${GREEN}[ RUN      ] $test_name"

    run_in_pty "$MINI_OUT" "$MINI_LS $*"
    run_in_pty "$LS_OUT" "ls $*"

    expand -t 8 "$MINI_OUT" > "$MINI_OUT.tmp"
    mv "$MINI_OUT.tmp" "$MINI_OUT"

    expand -t 8 "$LS_OUT" > "$LS_OUT.tmp"
    mv "$LS_OUT.tmp" "$LS_OUT"    
	
    if cmp -s "$MINI_OUT" "$LS_OUT"; then
        echo "[       OK ] $test_name"
        passed=$((passed + 1))
		echo -e "${GREEN}[       OK ] $test_name"
    else
        echo -e "${RED}[  FAILED  ] $test_name"
        failed=$((failed + 1))

        echo
        diff -u "$LS_OUT" "$MINI_OUT" || true
        echo
    fi
}

echo "[==========] Running tests."

# Basic
test_case "MiniLsTest.Default" .
test_case "MiniLsTest.Root" /

# Format options
test_case "MiniLsTest.LongFormat" -l /
test_case "MiniLsTest.OnePerLineFormat" -1 /
test_case "MiniLsTest.CommaFormat" -m /
test_case "MiniLsTest.HorizontalFormat" -x /
test_case "MiniLsTest.ColumnsFormat" -C /

# Hidden files
test_case "MiniLsTest.ShowAll" -a /
test_case "MiniLsTest.ShowAlmostAll" -A /

# Recursion
test_case "MiniLsTest.Recursive" -R .

# Directory handling
test_case "MiniLsTest.DirectoryItself" -d /
test_case "MiniLsTest.DirectoryItselfLong" -ld /

# Sorting
test_case "MiniLsTest.SortBySize" -S /
test_case "MiniLsTest.SortByTime" -t /
test_case "MiniLsTest.SortReversed" -r /
test_case "MiniLsTest.NoSort" -U /
test_case "MiniLsTest.UnsortedShowAll" -f /

# Metadata display
test_case "MiniLsTest.ShowInode" -i /
test_case "MiniLsTest.NumericIds" -n /
test_case "MiniLsTest.NoOwner" -g /
test_case "MiniLsTest.NoGroup" -o /
test_case "MiniLsTest.ShowBlockSizes" -s /

# File type indicators
test_case "MiniLsTest.ClassifyAll" -F /
test_case "MiniLsTest.ClassifyDirectories" -p /

# Time selection
test_case "MiniLsTest.AccessTime" -u -l /
test_case "MiniLsTest.ChangeTime" -c -l /

# Symbolic links
test_case "MiniLsTest.FollowSymlinks" -L /

# Combined options
test_case "MiniLsTest.LongRecursiveAll" -laR ./test_dir
test_case "MiniLsTest.InodeLongAll" -ila /
test_case "MiniLsTest.SortTimeReverse" -tr ./test_dir
test_case "MiniLsTest.SortSizeReverse" -Sr ./test_dir
test_case "MiniLsTest.LongWithIndicators" -lF ./test_dir
test_case "MiniLsTest.BlocksHumanAll" -sah ./test_dir

# Multiple arguments
test_case "MiniLsTest.MultipleFiles" /etc/passwd /etc/hosts
test_case "MiniLsTest.MultipleDirs" /tmp /var
test_case "MiniLsTest.MixedFilesAndDirs" /etc/passwd /tmp

# Non-existing files
test_case "MiniLsTest.NonExistent" does_not_exist
test_case "MiniLsTest.MixedExistingAndMissing" /tmp does_not_exist

# Symlinks
test_case "MiniLsTest.SymlinkLong" -l test_dir/s1
test_case "MiniLsTest.SymlinkFollow" -lL test_dir/s2
test_case "MiniLsTest.BrokenSymlink" -l test_dir/s1

# Empty dirs
mkdir -p empty_dir
test_case "MiniLsTest.EmptyDirectory" empty_dir
test_case "MiniLsTest.EmptyDirectoryLong" -l empty_dir
rm -rf empty_dir

# Directory with no access
test_case "MiniLsTest.PermissionDeniedRecursive" -R test_dir/d

# Conflicting flags
test_case "MiniLsTest.LongAndNumeric" -ln /
test_case "MiniLsTest.AllAndDirectory" -ad test_dir
test_case "MiniLsTest.SortTimeSize" -tS test_dir

# Special files
test_case "MiniLsTest.DevNull" -l /dev/null
test_case "MiniLsTest.DevZero" -l /dev/zero

echo "[==========] Test run complete."

if [[ $failed -eq 0 ]]; then
    echo -e "${GREEN}[  PASSED  ] $passed test(s).${NC}"
else
    echo -e "${GREEN}[  PASSED  ] $passed test(s).${NC}"
    echo -e "${RED}[  FAILED  ] $failed test(s).${NC}"
    exit 1
fi
