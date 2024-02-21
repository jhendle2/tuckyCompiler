# TuckyCompiler

A tokenizer, AST, and QBE-transpiler written in C.

## Table of Contents
* [Install and Building](#install-and-building)
    * [Git](#git)
    * [QBE](#qbe)
* [Usage](#usage)
* [License](#license)

# Install and Building
## Git
First, clone the repository.
Then, build with your favorite system.
```bash
git  clone https://github.com/jhendle2/tuckyCompiler.git
cd   tuckyCompiler
make build
# make install (TODO: Implement this)
```
## QBE
[QBE's Repository](https://c9x.me/compile/code.html)

Clone QBE and follow its `README.md` to install
```bash
git clone git://c9x.me/qbe.git
cd qbe-VERSION
sudo make install # `sudo` may be unnecesary
qbe -v # test if the installation worked
```

# Examples
Source Code
```c
/* hello_world.c */
int main() {
    printf("Hello, World!\n");
    return 0;
}
```

# Usage
How to compile and run the `hello_world.c` example from above
```bash
./tucky -f hello_world.c -o hello_world
qbe -o hello_world.s hello_world.ssa
cc  -o hello_world hello_world.s
./hello_world

> Hello, World!
```

# License
Written by: Jonah Hendler (jhendler17@gmail.com)

[GNU GENERAL PUBLIC LICENSE](LICENSE)