# mrubybridge
An mruby bridge that lets you load Java classes and call their methods.

## Prerequisites
* Java
* mruby 1.1

## Compilation
`gcc -I/path/to/java/library -Wl,--whole-archive -lmruby -Wl,--no-whole-archive -fPIC -shared -o libmrubystate.so MRubyState.c`
