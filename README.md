# Exploring The Commander X16

This repository is the result of learning how to develop assembly programs 
for the [Commander X16](https://commanderx16.com) platform.  If you are new 
to assembly programming, you should get a grasp of the 6502 CPU first.  Then 
come back here.  While the [Commander X16](https://commanderx16.com) builds 
upon the [C64](https://c64-wiki.com), knowledge of the C64 is not required.
Since the Commander X16 does use the same KERNAL, so having a C64 KERNAL 
reference would be beneficial.

## The Zakero_\*.inc Files

These are assembly include files to make programming the _Commander X16_ 
easier, or if not, at least easier to read.

### __Zakero_X16.inc__

A collection of constants and macros for interacting directly with the 
_Commander X16_ and its KERNAL.  Automatically includes Zakero_X16_Vera.inc.

### __Zakero_X16_Vera.inc__

A collection of constants and macros for interacting with the VERA chip in 
_Commander X16_.

_Why just macros and not procedures that can be linked?_  Procedures force 
parameters to be passed via registers or known memory addresses.  The 6502 
CPU only has 3 registers which would cause the caller to do some register 
juggling before a procedure could be called.  Accessing Memory can be 
sorta-fast (Zero Page) to slow (indirect memory access).  On a platform where 
every CPU cycle counts, memory access can really hurt.  Plus procedures 
completely rule out the use of constants.

Using macros allows the caller to pass constants or memory addresses but not 
registers.  This option provides for better control of how data is used and 
accessed.  The price comes with size, use the same macro twice and you used 
twice the space as a procedure.  If this is a problem, you can always put the 
macro in a procedure and call that procedure.  So you really have the best of 
both worlds using a "macro" library.

## The Makefile.\* Files

If you are at home on the command line, these Makefiles provide a simple 
build system.  Copy the `Makefile` to your directory, fill out the variables, 
and build.  Well, it's never that easy, you will also need to have the 
[cc65](https://cc65.github.io) tool-chain installed and in your path.  The 
`Makefile.X16` can also use some of the scripts in the `tool/` directory, so 
those scripts will need to be in your path as well.

## Tools

Speaking of tools, there are a collection of utilities to, again, make life 
easier.

## Examples

A collection of programs used to get familiar with the _Commander X16_.  They 
also serve as mini-tutorials for the tools.

## Bugs

The `bug_*` directories can be ignored.  When using a low-level language like 
assembly, you are bound to poke things in unexpected ways.  Usually when 
something does not work as expected, you just need to improve your 
understanding.  Sometimes, very rarely, it is an actual bug in the system.
These are examples of those bugs.
