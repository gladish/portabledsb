# portabledsb
A portable c++ version of the AllJoyn DSB

Building:

Right now we're only building on OSX/Linux.

1. export ALLJOYN_INSTALL_DIR=[path to your build of alljouyn core]
2. make [options]

[options]
* DEBUG -- debug build (-g) otherwise -O2 is used
* V     -- verbose output (skip if you want cleaner output)


The AllSeen Alliance has a Device System Bridge (DSB), which allows non-allseen devices to
communicate on an AllJoyn bus. There's a good document describing the software here:

  https://git.allseenalliance.org/cgit/dsb.git/plain/Docs/AllJoyn%20-%20Device%20System%20Bridge%20-%20Whitepaper%20v1.0.pdf

The code was contributed by Microsoft and is writting using c++/cx (WinRT). This project's
goal is to re-create that software in std c++ so that it can be run on Windows, Linux, and
Mac. 

The WinRT version is here:

  https://git.allseenalliance.org/cgit/dsb.git/
