gcc server.c -o s -lws2_32 -Include sm3.c sm4.c -lgmp 
gcc client.c -o c -lws2_32 -Include sm3.c sm4.c -lgmp
