#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <nettle/base64.h>

/* The number of bytes read in each iteration, we do one line at a time: */
#define CHUNK_SIZE 54
/* The *maximum* size of an encoded chunk: */
#define ENCODED_SIZE BASE64_ENCODE_LENGTH(CHUNK_SIZE)

/*
 * Reads bytes from standard input and writes base64-encoded
 * on standard output.
 */
int main(int argc, char **argv){
  struct base64_encode_ctx b64_ctx;

  /* Init the context: */
  base64_encode_init(&b64_ctx);

  while(true){
    uint8_t      buffer[CHUNK_SIZE];                                   /* "buffer" will hold the bytes from disk: */
    uint8_t      result[ENCODED_SIZE + BASE64_ENCODE_FINAL_LENGTH + 1];/* "result" is the result vector: */
    unsigned int nbytes;                                               /* Number of bytes read from stdin */
    int          encoded_bytes;                                        /* total number of bytes encoded per iteration */
    nbytes        = fread(buffer, 1, CHUNK_SIZE, stdin);
    encoded_bytes = base64_encode_update(&b64_ctx, result, nbytes, buffer);

    if(nbytes < CHUNK_SIZE){
      if(ferror(stdin)){
        fprintf(stderr, "Error reading file: %s\n", strerror(errno));
        return EXIT_FAILURE;
      }
      encoded_bytes += base64_encode_final(&b64_ctx,result + encoded_bytes);

      result[encoded_bytes++] = '\n';
      if(!fwrite(result, 1, encoded_bytes, stdout) || fflush(stdout) != 0){
        fprintf(stderr, "Error writing file: %s\n", strerror(errno));
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }

    /* The result vector is written */
    result[encoded_bytes++] = '\n';
    if(!fwrite(result, 1, encoded_bytes, stdout)){
      fprintf(stderr, "Error writing file: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }
  }
}
