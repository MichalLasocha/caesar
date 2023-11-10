#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void help() {
  printf("Usage: caesar-cli [OPERATION] [KEY] [MSG]\n\n");
  printf("operations:\nenc – encrypt\ndec – decrypt\n\n");
  printf("Caesar cipher cli tool by Michał Łasocha 2023\n\n");
}

bool key_valid(char key[]) {
  for (int i = 0; i < strlen(key); i++) {
    if (isdigit(key[i]) == false) {
      return false;
    }
  }
  return true;
}

void caesar_encrypt(int key, char *msg) {
  for (int i = 0; i < strlen(msg); i++)

    if (isupper(msg[i])) {
      msg[i] = ((msg[i] - 65 + key) % 26) + 65;
    } else if (islower(msg[i])) {
      msg[i] = ((msg[i] - 97 + key) % 26) + 97;
    }
}

void caesar_decrypt(int key, char *msg) {
  for (int i = 0; i < strlen(msg); i++)

    if (isupper(msg[i])) {
      msg[i] = ((msg[i] - 65 - key + 26) % 26) + 65;
    } else if (islower(msg[i])) {
      msg[i] = ((msg[i] - 97 - key + 26) % 26) + 97;
    }
}

/* Maybe ill learn gtk3? */
/* Key is 17 */

int main(int argc, char *argv[]) {
  if (argc == 4) {

    if (key_valid(argv[2]) == false) {
      printf("ERROR: invalid key format\n\n");
      help();
      exit (2);
    }
    int k = atoi(argv[2]);
    // Without this, encryption breaks at < 26 shift
    k = k % 26;

    char in[strlen(argv[3])];
    // Copy argv to mutable string
    strcpy(in, argv[3]);

    if (!strcmp(argv[1], "enc")) {
      // Finally the implementation
      caesar_encrypt(k, in);
    } else if (!strcmp(argv[1], "dec")) {

      caesar_decrypt(k, in);
    } else {

      printf("ERROR: invalid operation:%s\n", argv[1]);
      help();
      exit(1);
    }
    // Output
    printf("cipher: %s\n", in);

  } else {
    printf("ERROR: Not enough arguments!\n\n");
    help();
    exit(3);
  }

  return 0;
}
