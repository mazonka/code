A program for simple encoding sensitive data which has to be published on the web.
The idea is that textual file is prepared with a program (encoded),
and then published within a web page with JavaScript dynamic decoding,
so no sensitive data is sent over the Internet.

Encoding and decoding is done by swapping characters using the following process:

x(i+1)=2*x(i)  mod M
y(i+1)=2*(x(i)+y(i)) mod M

The process starts with x(1)=S1 and y(1)=S2 and lasts for 2*L steps.
The sequence of y(i) values, or more precisely the pairs of { y(i+1), y(i) }
give the indices (modulo text size) of characters to be swapped.
The numbers S1, S2, L, and M define uniquely the encryption process.

Note: pay attention to dos/unix encoding (especially IE stuffs up)