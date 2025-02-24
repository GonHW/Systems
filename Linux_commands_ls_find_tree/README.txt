FALL 2022
Project files: ls.c find.c tree.c
Compile: make all

Execute each file
ls.c:   ./ls 
ls.c:   ./ls -l
find.c: ./find name_of_file
tree.c: ./tree
tree.c: ./tree ..

Name: Hench Wu		(NetID: hhw14) 


ls with the -l command-line option

If the user invokes ls -l (lowercase “l” for “long”), you should print a “long format” with
extra information about each file.

-rw-rw-r-- bob users 1562 Sep 29 12:00 find.c
-rw-rw-r-- bob users 1024 Sep 29 12:05 ls.c
-rw-rw-r-- bob users 176 Sep 28 11:27 Makefile
-rw-rw-r-- bob users 2044 Sep 27 18:23 tree.c

The first column contains a 10-character permissions string.
• the first character is ‘-’ for files, ‘d’ for directories
• the next three are read, write, and execute permissions for the user
• the next three are read, write, and execute permissions for the group
• the next three are read, write, and execute permissions for others

Permissions should be denoted by a ‘r’, ‘w’, or ‘x’ if present, or ‘-’ otherwise.
The second and third columns show the file owner’s user name and group name (or user
ID / group ID if a name can’t be found).
The next column shows the file size in bytes.
Next is the file’s modification time (mtime) formatted as in the example above.
Finally the filename is listed.
You may find strftime, getpwuid, and getgrgid useful.


find
The find command takes a pattern as a command-line argument and recursively searches
through directories to find a filename matching that pattern. It should print a relative path
starting with “./” for every file/directory that matches.
For example, running ./find ls.c from within the hw2 directory should print ./ls.c.
If run from the parent directory, the output would be ./hw2/ls.c.

There may be multiple matches. If we run ./find .c from within the hw2 directory,
we should see:

./find.c
./ls.c
./tree.c

The output does not need to be sorted. If nothing matches, it shouldn’t print anything.
Pattern matching should be case sensitive.


tree
The tree command prints all files/directories contained in the current directory as a tree.
If we run ./tree from the cs214 directory, we should see:
.-
hw1
  - factor.c
  - grep.c
  - monster.c
  - sort.c
  - uniq.c
- hw2
  - find.c
  - ls.c
  - Makefile
  - tree.c

The first line of output is always “.” to denote the current directory. tree recurses
into directories to print all files/directories within them. For each subdirectory, two spaces
of indentation are added. For example, if we had a file hw1/test/src/foo.c, tree would
print:
.
-hw1
  - test
    - src
      - foo.c
Files/directories within each subdirectory should be sorted with the same case-insensitive
lexicographic order as ls.
