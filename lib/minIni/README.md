# minIni
minIni is a portable and configurable library for reading and writing ".INI" files. At 830 lines of commented source 
code
(version 1.2), minIni truly is a "mini" INI file parser, especially considering its features.

The library does not require the file I/O functions from the standard C/C++ library, but instead lets you configure 
the file I/O interface to use via macros. minIni uses limited stack space and does not use dynamic memory (malloc and 
friends) at all.

Some minor variations on standard INI files are supported too, notably minIni supports INI files that lack sections.


# Acknowledgement

minIni is derived from an earlier INI file parser (which I wrote) for desktop systems.

In turn, that earlier parser was a re-write of the code from the article "Multiplatform .INI Files" by Joseph J. Graf 
in the March 1994 issue of Dr. Dobb's Journal. In other words, minIni has its roots in the work of Joseph Graf (even 
though the code has been almost completely re-written).


# Features

minIni is a programmer's library to read and write "INI" files in embedded systems. minIni takes little resources, 
can be configured for various kinds of file I/O libraries and provides functionality for reading, writing and 
deleting keys from an INI file.

Although the main feature of minIni is that it is small and minimal, it has a few other features:

 * minIni supports reading keys that are outside a section, and it thereby supports configuration files that do not use sections (but that are otherwise compatible with INI files).
 * You may use a colon to separate key and value; the colon is equivalent to the equal sign. That is, the strings "Name: Value" and "Name=Value" have the same meaning.
 * The hash character ("#") is an alternative for the semicolon to start a comment. Trailing comments (i.e. behind a key/value pair on a line) are allowed.
 * Leading and trailing white space around key names and values is ignored.
 * When writing a value that contains a comment character (";" or "#"), that value will automatically be put between double quotes; when reading the value, these quotes are removed. When a double-quote itself appears in the setting, these characters are escaped.
 * Section and key enumeration are supported.
 * You can optionally set the line termination (for text files) that minIni will use. (This is a compile-time setting, not a run-time setting.)
 * Since writing speed is much lower than reading speed in Flash memory (SD/MMC cards, USB memory sticks), minIni minimizes "file writes" at the expense of double "file reads".
 * The memory footprint is deterministic. There is no dynamic memory allocation. 

## INI file reading paradigms

There are two approaches to reading settings from an INI file. One way is to call a function, such as 
GetProfileString() for every section and key that you need. This is especially convenient if there is a large 
INI file, but you only need a few settings from that file at any time &mdash;especially if the INI file can also 
change while your program runs. This is the approach that the Microsoft Windows API uses.

The above procedure is quite inefficient, however, when you need to retrieve quite a few settings in a row from 
the INI file &mdash;especially if the INI file is not cached in memory (which it isn't, in minIni). A different approach 
to getting settings from an INI file is to call a "parsing" function and let that function call the application 
back with the section and key names plus the associated data. XML parsing libraries often use this approach; see 
for example the Expat library.

minIni supports both approaches. For reading a single setting, use functions like ini_gets(). For the callback 
approach, implement a callback and call ini_browse(). See the minIni manual for details.


# INI file syntax

INI files are best known from Microsoft Windows, but they are also used with applications that run on other 
platforms (although their file extension is sometimes ".cfg" instead of ".ini").

INI files have a simple syntax with name/value pairs in a plain text file. The name must be unique (per section) 
and the value must fit on a single line. INI files are commonly separated into sections &mdash;in minIni, this is 
optional. A section is a name between square brackets, like "[Network]" in the example below.

```
[Network]
hostname=My Computer
address=dhcp
dns = 192.168.1.1
```

In the API and in this documentation, the "name" for a setting is denoted as the key for the setting. The key 
and the value are separated by an equal sign ("="). minIni supports the colon (":") as an alternative to the 
equal sign for the key/value delimiter.

Leading a trailing spaces around values or key names are removed. If you need to include leading and/or trailing 
spaces in a value, put the value between double quotes. The ini_gets() function (from the minIni library, see the 
minIni manual) strips off the double quotes from the returned value. Function ini_puts() adds double quotes if 
the value to write contains trailing white space (or special characters).

minIni ignores spaces around the "=" or ":" delimiters, but it does not ignore spaces between the brackets in a 
section name. In other words, it is best not to put spaces behind the opening bracket "[" or before the closing 
bracket "]" of a section name.

Comments in the INI must start with a semicolon (";") or a hash character ("#"), and run to the end of the line. 
A comment can be a line of its own, or it may follow a key/value pair (the "#" character and trailing comments 
are extensions of minIni).

For more details on the format, please see http://en.wikipedia.org/wiki/INI_file. 


# Adapting minIni to a file system

The minIni library must be configured for a platform with the help of a so- called "glue file". This glue file 
contains macros (and possibly functions) that map file reading and writing functions used by the minIni library 
to those provided by the operating system. The glue file must be called "minGlue.h".

To get you started, the minIni distribution comes with the following example glue files:

 * a glue file that maps to the standard C/C++ library (specifically the file I/O functions from the "stdio" package),
 * a glue file for Microchip's "Memory Disk Drive File System Library" (see http://www.microchip.com/),
 * a glue file for the FAT library provided with the CCS PIC compiler (see http://www.ccsinfo.com/)
 * a glue file for the EFS Library (EFSL, http://www.efsl.be/),
 * and a glue file for the FatFs and Petit-FatFs libraries (http://elm-chan.org/fsw/ff/00index_e.html). 

The minIni library does not rely on the availability of a standard C library, because embedded operating systems 
may have limited support for file I/O. Even on full operating systems, separating the file I/O from the INI format 
parsing carries advantages, because it allows you to cache the INI file and thereby enhance performance.

The glue file must specify the type that identifies a file, whether it is a handle or a pointer. For the standard 
C/C++ file I/O library, this would be:

```C
#define INI_FILETYPE        FILE*
```

If you are not using the standard C/C++ file I/O library, chances are that you need a different handle or 
"structure" to identify the storage than the ubiquitous "FILE*" type. For example, the glue file for the FatFs 
library uses the following declaration:

```C
#define INI_FILETYPE        FIL
```

The minIni functions declare variables of this INI_FILETYPE type and pass these variables to sub-functions 
(including the glue interface functions) by reference.

For "write support", another type that must be defined is for variables that hold the "current position" in a 
file. For the standard C/C++ I/O library, this is "fpos_t".

Another item that needs to be configured is the buffer size. The functions in the minIni library allocate this 
buffer on the stack, so the buffer size is directly related to the stack usage. In addition, the buffer size 
determines the maximum line length that is supported in the INI file and the maximum path name length for the 
temporary file. For example, minGlue.h could contain the definition:

```C
#define INI_BUFFERSIZE      512
```

The above macro limits the line length of the INI files supported by minIni to 512 characters.

The temporary file is only used when writing to INI files. The minIni routines copy/change the INI file to a 
temporary file and then rename that temporary file to the original file. This approach uses the least amount of 
memory. The path name of the temporary file is the same as the input file, but with the last character set to a 
tilde ("~").

Below is an example of a glue file (this is the one that maps to the C/C++ "stdio" library).

```C
#include <stdio.h>

#define INI_FILETYPE                  FILE*
#define ini_openread(filename,file)   ((*(file) = fopen((filename),"r")) != NULL)
#define ini_openwrite(filename,file)  ((*(file) = fopen((filename),"w")) != NULL)
#define ini_close(file)               (fclose(*(file)) == 0)
#define ini_read(buffer,size,file)    (fgets((buffer),(size),*(file)) != NULL)
#define ini_write(buffer,file)        (fputs((buffer),*(file)) >= 0)
#define ini_rename(source,dest)       (rename((source), (dest)) == 0)
#define ini_remove(filename)          (remove(filename) == 0)

#define INI_FILEPOS                   fpos_t
#define ini_tell(file,pos)            (fgetpos(*(file), (pos)) == 0)
#define ini_seek(file,pos)            (fsetpos(*(file), (pos)) == 0)
```

As you can see, a glue file is mostly a set of macros that wraps one function definition around another.

The glue file may contain more settings, for support of rational numbers, to explicitly set the line termination 
character(s), or to disable write support (for example). See the manual that comes with the archive for the details. 
