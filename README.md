# mrubybridge [![Build Status](https://travis-ci.org/dragostis/mrubybridge.svg?branch=master)](https://travis-ci.org/dragostis/mrubybridge)
An mruby bridge that lets you load Java classes and call their methods.

## Prerequisites
* Java
* mruby 1.1

```ruby
# add this to your build_config.rb then run make
conf.cc do |cc|
  cc.flags << '-fPIC'
end
```

## Compilation
```
xxd -i java_class.rb JavaClass.h
gcc -I/path/to/java/library -Wl,--whole-archive -lmruby -Wl,--no-whole-archive -fPIC -shared -o libmrubystate.so MRubyState.c
```