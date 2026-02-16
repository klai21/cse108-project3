#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <curl/curl.h>
#include <math.h>
#include <string.h>

#define BLOCK_SIZE 1024
#define BUCKET_SIZE 4
#define HEIGHT 9
#define NUM_BLOCKS 1024
unsigned char key[32];
FILE *stash;
FILE *pos;


int encrypt_block(unsigned char *plaintext, unsigned char *key, unsigned char *ciphertext, EVP_CIPHER_CTX *ctx) {
    int c_len = 0;
    int len = 0;
    unsigned char iv[16];
    int iv_len = 16;
    RAND_bytes(iv, 16);
    // char* ciphertext = malloc(sizeof(char) * (BLOCK_SIZE + 28));
    memcpy(ciphertext, iv, 16);
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext+iv_len, &c_len, plaintext, BLOCK_SIZE);
    EVP_EncryptFinal_ex(ctx, ciphertext + iv_len + c_len, &len);
    return c_len + len + iv_len;
}


int decrypt_block(unsigned char *ciphertext, unsigned char *key, unsigned char *plaintext, EVP_CIPHER_CTX *ctx) {
    int p_len = 0;
    int len = 0;
    unsigned char iv[16];
    int iv_len = 16;
    memcpy(iv, ciphertext, 16);
    EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &p_len, ciphertext+iv_len, BLOCK_SIZE);
    EVP_DecryptFinal_ex(ctx, plaintext + p_len, &len);
    return p_len + len;
}

int init() {
    FILE *file;
    if ((file = fopen("key.txt", "r")) != NULL) {
        fread(key, 32, 1, file);
    }
    else {
        file = fopen("key.txt", "w");
        RAND_bytes(key, 32);
        fwrite(key, 32, 1, file);
    }
    fclose(file);
    if ((stash = fopen("stash.txt", "r")) != NULL) {
    
    }
    else {
        stash = fopen("stash.txt", "w");
    }

    if ((pos = fopen("pos.txt", "r")) != NULL) {

    }
    else {
        pos = fopen("pos.txt", "w")
        for (int i = 0; i < pow(HEIGHT) - 1; i++) {
            unsigned char random[4]
            RAND_bytes(random, 1);
            fwrite(random, 1, 1, pos);
            fprintf(pos, "\n");
        }
    }
    return 0;
}

int read_stash(int block_index){

}
int main(int argc, char *argv[]){
    init();
    FILE *file;
    file = fopen("test.txt", "r");
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char *contents = malloc(sizeof(char) * BLOCK_SIZE);
    fread(contents, 1024, 1, file);
    unsigned char *ciphertext = malloc(sizeof(char) * (BLOCK_SIZE+16));
    unsigned char *plaintext = calloc(BLOCK_SIZE * sizeof(char));
    int bytes_encrypted = 0;

    bytes_encrypted = encrypt_block(contents, key, ciphertext, ctx);
    int bytes_decrypted = 0;
    bytes_decrypted = decrypt_block(ciphertext, key, plaintext, ctx);
    
    return 0;
}