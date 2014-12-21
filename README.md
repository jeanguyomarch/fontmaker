fontmaker
=========

This command-line utility has been originally developed for the SmartWrist project. More info at: http://phabricator.jeanguyomarch.com/diffusion/PFE/.

What it does
------------

Fontmaker takes as inputs:
- a TTF file;
- the size font to extract;
- a fontmaker file which indicates what characters must be conteverted to bitmaps.

And it outputs:
- a C source file containing the bitmap font, various information about it and how to extract it (optimized);
- a C header file containing the prototypes of the provided API.

For more info, run `fontmaker --help`.

Requirements
------------

Fontmaker relies on:
- `cmake` as a build system;
- `freetype2` to extract font properties and generate bitmaps from TTF files;
- `flex` to parse the fontmaker charmap files;
- `gperf` to generate perfect hash functions for bitmap decoding.


Usage
-----

Let's assume:
- `$OUT_C` the C source file to be produced;
- `$OUT_H` the C header file to be produced;
- `$TTF` the TTF file to be used as a basis;
- `$PT` the font size (in pts);
- `$MAP` the fontmaker charmap file.

```
fontmaker --gc "$OUT_C" --gh "$OUT_H" --font-file="$TTF" --map-file="$MAP" --font-size="$PT"
```


Fontmaker Charmap File Format
-----------------------------

Rules:
- `##` starts a comment line;
- whitespace ` ` is always built-in;
- any other character must be separated by a space, tabulation, newline, etc.

Example:
```
## Digits
0 1 2 3 4
5       6       7 8 9

## Some ASCII characters
N e V E R
G o n N A
g i v
y O U
P

## Multi-bytes characters
É Ç é À ∑
```

Have a look at the example in `tests/charmap.fm`.


Build
-----

Fontmaker uses Cmake because it aims at being as portable as possible.

- `mkdir -p build && cd build`
- `cmake ..`
- `make`


Author
------

Jean Guyomarc'h <jean.guyomarch@gmail.com>


License
-------

The MIT License (MIT)

Copyright (c) 2014 Jean Guyomarc'h

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


