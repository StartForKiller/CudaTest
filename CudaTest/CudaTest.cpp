#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "./sha1.cuh"

#define BUFFER_SIZE 20

char get_char(int value)
{
    int num = 0;
    int num2_values[] = { 33, 35, 36, 37, 38, 47, 92, 95, 46, 0 };

    for (int i = 0; i < 10; i++)
    {
        if (value + num >= num2_values[i] && 65 <= num2_values[i])
            num++;
    }

    return (char)(value + num);
}

char* hash_string(char* input, char* seed, int seed_length, int batch_count)
{
    int len = strlen(input) + seed_length;
    char* big_str = (char *)malloc(len * batch_count);
    //// Use the str pointer
    for (int i = 0; i < batch_count; i++) {
        strcpy(big_str + (len * i), seed + (i * seed_length));
        strcpy(big_str + (len * i) + seed_length, input);
    }
    //printf("%s\n", big_str);
    //int new_len = (strlen(str)) * 4;
    //unsigned char* new_arr = (unsigned char*)malloc(new_len);
    //// Use the new_arr pointer
    //int i, j = 0;
    //for (i = 0; i < len - 1; i++) {
    //    new_arr[j++] = str[i];
    //    new_arr[j++] = 0;
    //    new_arr[j++] = 0;
    //    new_arr[j++] = 0;
    //}
    unsigned char* hash = (unsigned char*)malloc(20*batch_count);
    mcm_cuda_sha1_hash_batch((BYTE*)big_str, len, hash, batch_count);
    return (char *)hash;
}

int main() {
    char* input = (char *)"_2ndTabID2CurScrollerIndexDict";
    char* known_hash = (char *)"MIPMIODJLIG";
    int seed_len = 16;

    int batch_count = 2048;

    // Generate all possible hexadecimal seeds
    char* hex_chars = (char *)"0123456789abcdef";
    char* seed = (char *)malloc((seed_len + 1) * batch_count);
    memset(seed, 0, (seed_len + 1) * batch_count);

    for (int i = 0; i < pow(16, seed_len); i += batch_count)
    {
        // Compute the hexadecimal digits of the seed
        for (int b = 0; b < batch_count; b++) {
            for (int j = 0; j < seed_len; j++)
            {
                int digit = (int)(i / pow(16, seed_len - j - 1)) % 16;
                seed[j + b * seed_len] = hex_chars[digit];
            }
            seed[b * seed_len + seed_len] = 0;
        }
        // Hash the input and generated seed
        char* hash = hash_string(input, seed, seed_len, batch_count);

        if ((i % 10000) == 0) {
            printf("%d Hashes calculated\n", i);
        }

        for (int b = 0; b < batch_count; b++) {
            // Check if the generated hash matches the known hash
            if (strcmp(hash + b * 20, known_hash) == 0)
            {
                printf("Found matching seed: %s\n", seed);
                free(hash);
                free(seed);
                return 0;
            }
        }

        free(hash);
    }

    printf("No matching seed found\n");
    free(seed);
    return 0;
}