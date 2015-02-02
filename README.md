# mrubybridge
An mruby bridge that lets you load Java classes and call their methods.

# Prerequisites
* Java
* mruby 1.1

# Compilation
`gcc -I/path/to/java/library -lmruby -shared -o libmrubystate.so MRubyState.c`
