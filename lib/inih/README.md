# inih
[![Build Status](https://travis-ci.org/jtilly/inih.svg?branch=master)](https://travis-ci.org/jtilly/inih)

This is a header only C++ version of [inih](https://github.com/benhoyt/inih).

**inih (INI Not Invented Here)** is a simple [.INI file](http://en.wikipedia.org/wiki/INI_file) parser written in C. It's only a couple of pages of code, and it was designed to be _small and simple_, so it's good for embedded systems. It's also more or less compatible with Python's [ConfigParser](http://docs.python.org/library/configparser.html) style of .INI files, including RFC 822-style multi-line syntax and `name: value` entries.

## Usage

All you need to do is to include `INIReader.h`. Consider the following example (`INIReaderTest.cpp`):

```cpp
#include <iostream>
#include "INIReader.h"

int main() {

    INIReader reader("test.ini");

    if (reader.ParseError() != 0) {
        std::cout << "Can't load 'test.ini'\n";
        return 1;
    }
    std::cout << "Config loaded from 'test.ini': version="
              << reader.GetInteger("protocol", "version", -1) << ", name="
              << reader.Get("user", "name", "UNKNOWN") << ", email="
              << reader.Get("user", "email", "UNKNOWN") << ", pi="
              << reader.GetReal("user", "pi", -1) << ", active="
              << reader.GetBoolean("user", "active", true) << "\n";
    return 0;

}
```

To compile and run:

```sh
g++ INIReaderTest.cpp -o INIReaderTest.out
./INIReaderTest.out
# Config loaded from 'test.ini': version=6, name=Bob Smith, email=bob@smith.com, pi=3.14159, active=1
```
