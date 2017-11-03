#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <nettle/sha1.h>
#include <nettle/base64.h>

#define BUF_SIZE 1000
static void display_hex(int, uint8_t*);

int main(int argc, char* argv[]){
  struct sha1_ctx ctx;
  uint8_t* digest = (uint8_t*)calloc(SHA1_DIGEST_SIZE, sizeof(char));
  sha1_init(&ctx);
  sha1_update(&ctx, strlen(argv[1]), (uint8_t*)argv[1]);
  sha1_digest(&ctx, SHA1_DIGEST_SIZE, digest);
  
  struct base64_encode_ctx b64_ctx;
  base64_encode_init(&b64_ctx);
  int len             = strlen((char*)digest);
  uint8_t* b64_result = (uint8_t*)calloc(BASE64_ENCODE_LENGTH(len) + BASE64_ENCODE_FINAL_LENGTH + 1, sizeof(uint8_t));
  int encoded_bytes   = base64_encode_update(&b64_ctx, b64_result, len, digest);
  encoded_bytes      += base64_encode_final(&b64_ctx,  b64_result + encoded_bytes);
  puts((char*)b64_result);

  free(digest);
  free(b64_result);
  return 0;
}

static void display_hex(int length, uint8_t* data){
  for(int i = 0; i < length; i++){
    printf("%02x", data[i]);
  }
  puts("\n");
}
