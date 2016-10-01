# socket-programming-and-multi-threading
This program reads two matrices from a .csv file and multiplies them.
There are two programs: C++ Program as a Client and JAVA Program as a Server.
Initially, Client reads both matrices.
Then a thread in C++ sends a row of Matrix A and complete Matrix B to java over TCP/IP and Server receives it in a thread.
Then for each column in matrix B a thread is started in Server which calculates one row of ans Matrix and returs the same to the Client as response.
The same thing is done for all rows of matrix A.

This works well in Linux Platform.

Commands to run C++ Program: g++ project.cpp -lpthread
                            ./a.out
Commands to run Java Program: javac Server.java
                              java Server
