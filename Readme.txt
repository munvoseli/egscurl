For egs.cpp:

$ g++ egs.cpp -lcurl ; ./a.out
also
$ ./a.out 50 https://www.egscomics.com/comic/balance-012
or something

results are stored in egsout.txt, just concatenated, no option or plan for anything fancy yet

comic starts at 4 instead of 1 because that's what HarJIT's aggregate index data says the ookii id is for the first comic



For egsv2.cpp:

$ g++ egsv2.cpp -lcurl ; ./a.out list2.txt

egsv2 can handle continuing an egsv2 list or starting a new one.  ID and URL are not configured by the command line in the current state of the program, but are only continued from the file.

egsv2 by default just uses ordinal numbers for the comics instead of offset ordinal numbers.
