#include "websocket.h"
#include "socket.h"
#include "linked_list.h"
#include <nettle/base64.h>
#include <nettle/sha1.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const char MAGIC[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const char SEP[]   = ":\0";
const char REPLY_FORMAT[] = "HTTP/1.1 101 Switching Protocols\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Accept: %s\r\n\r\n";

int main(int argc, char* argv[]){
  printf("ENTER main\n");
  struct socket* sock = socket_new();
  int s = sock->sBind(sock);
  if(s < 0){
    fprintf(stderr, "Couldn't bind\n");
    return -1;
  }
  s = sock->sListen(sock);
  if(s < 0){
    fprintf(stderr, "Couldn't listen\n");
    return -1;
  }
  struct sockaddr_in clientAddress;
  while(true){
    //wait for someone to connect
    int fd = sock->sAccept(sock, &clientAddress);
    struct socket* clientSock = socket_new1(fd);
    if(clientSock->fd > -1){
      start(clientSock);
    } else {
      fprintf(stderr, "Couldn't connect\n");
      return -1;
    }
  }
  printf("EXIT main\n");
}

void start(struct socket* sock){
  printf("ENTER start %s\n", sock->toString(sock));
  char* req = sock->sReadLine(sock); //first line is request string GET
  printf("request: %s\n", req);
  if(strlen(req) >= 3 && req[0] == 'G' && req[1] == 'E' && req[2] == 'T'){
    struct hash_map* headers = get_headers(sock);
    do_upgrade(sock, headers);
    do_websocket(sock);
    headers->delete(headers);
  } else {
    fprintf(stderr, "Bad request\n");    
  }
  printf("EXIT start\n");
}

/**
 * read all headers from socket
 * caller must delete map
 */
struct hash_map* get_headers(struct socket* sock){
  printf("ENTER get_headers %s\n", sock->toString(sock));
  struct hash_map* headers = hash_map_new();
  for(char* h = sock->sReadLine(sock); strlen(h) > 0; h = sock->sReadLine(sock)){
    char* key   = strtok_r(h, SEP, &h);
    char* value = trimLeadingSpaces(strtok_r(h, SEP, &h));
    headers->put(headers, key, value);
  }
  printf("EXIT get_headers %s\n", headers->toString(headers));
  return headers;
}

void do_upgrade(struct socket* sock, struct hash_map* headers){
  printf("ENTER do_upgrade %s %s\n", sock->toString(sock), headers->toString(headers));
  char str[255];
  strcpy(str, headers->get(headers, "Sec-WebSocket-Key"));
  strcat(str, MAGIC);
  char result[255];
  uint8_t* sha1_str = sha1(str);
  char* base64_str  = toBase64(sha1_str);
  sprintf(result, REPLY_FORMAT, base64_str);
  printf("result: %s\n", result);
  sock->sWrite(sock, result);

  free(sha1_str);
  free(base64_str);
  printf("EXIT do_upgrade\n");
}

void do_websocket(struct socket* sock){
  printf("ENTER do_websocket %s\n", sock->toString(sock));
  while(true){
    sock->sReadByte(sock);
    uint8_t b = sock->sReadByte(sock);
    if(b < 128){//FIN
      break;
    }
    int     payload_len = b ^ 128;
    uint8_t payload[payload_len];
    int     mask_len    = 4;
    uint8_t mask[mask_len];
    //pull in mask
    for(int i = 0; i < mask_len; i++){
      mask[i] = sock->sReadByte(sock);
    }
    //pull in message
    for(int i = 0; i < payload_len; i++){
      payload[i] = sock->sReadByte(sock);
    }
    char result[payload_len+1];//null terminate
    for(int i = 0; i < payload_len; i++){
      result[i] = payload[i] ^ mask[i % 4];
    }
    result[payload_len] = '\0';
    send_message(sock, result);
  }
  printf("EXIT do_websocket\n");
}

/* bits 9 - 15 are message length if less than 126 bit 8 should be 0 */
void send_message(struct socket* sock, char* msg){
  printf("ENTER send_message %s\n", msg);
  int len = strlen(msg);
    uint8_t b[3] = {(uint8_t)129};
    int b_len = 2;
  if(len < 126){
    b[1] = (uint8_t)len;
  } else {
    b[1] = (uint8_t)127;
    b[2] = (uint8_t)len;
    b_len = 3;
  }
  sock->sWriteBinary(sock, b, b_len);
  sock->sWrite(sock, msg);
  printf("EXIT send_message\n");
}

/**
 * caller must free
 */
char* toBase64(uint8_t* b){
  printf("ENTER toBase64 "); printBinary(b); putchar('\n');
  struct base64_encode_ctx ctx;
  base64_encode_init(&ctx);
  int len           = strlen((char*)b);
  uint8_t* result   = (uint8_t*)calloc(BASE64_ENCODE_LENGTH(len) + BASE64_ENCODE_FINAL_LENGTH + 1, sizeof(uint8_t));
  int encoded_bytes = base64_encode_update(&ctx, result, len, b);
  encoded_bytes    += base64_encode_final(&ctx, result + encoded_bytes);
  printf("EXIT toBase64 %s\n", (char*)result);
  return (char*)result;
}

/**
 * caller must free
 */
uint8_t* sha1(char* data){
  printf("ENTER sha1 %s\n", data);
  struct sha1_ctx ctx;
  uint8_t* digest = (uint8_t*)calloc(SHA1_DIGEST_SIZE, sizeof(uint8_t));
  sha1_init(&ctx);
  sha1_update(&ctx, strlen(data), (uint8_t*)data);
  sha1_digest(&ctx, SHA1_DIGEST_SIZE, digest);
  printf("EXIT sha1 "); printBinary(digest); putchar('\n');
  return digest;
}

void printBinary(uint8_t* data){
  int len = strlen((char*)data);
  for(int i = 0; i < len; i++){
    printf("%02x", data[i]);
  }
}

char* trimLeadingSpaces(char* str){
  while('\0' != str[0] && ' ' == str[0]){
    str++;
  }
  return str;
}
