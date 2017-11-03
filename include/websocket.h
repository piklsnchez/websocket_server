#ifndef __WEBSOCKET_H
#define __WEBSOCKET_H
#include "socket.h"
#include "hash_map.h"
void start(struct socket*);
struct hash_map* get_headers(struct socket*);
void send_message(struct socket*, char*);
void do_websocket(struct socket*);
void do_upgrade(struct socket*, struct hash_map*);
char* toBase64(uint8_t*);
uint8_t* sha1(char*);
void printBinary(uint8_t*);
char* trimLeadingSpaces(char*);
#endif
