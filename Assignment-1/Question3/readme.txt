Compile with flag -fno-stack-protector. Otherwise "*** stack smashing detected ***"  error occurs with some commands, caused by some kind of stack overflow.

gcc -o shell -fno-stack-protector shell.c
./shell