#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../global/definitions.h"
#include "code_output.h"
#include "../DataStructures/symbolTableTrie.h"



int code_output(translation_unit *tr, char* file_name) {
    FILE *ob_file;
    FILE *ent_file;
    FILE *ext_file;
    char *ob_file_name;
    char *ent_file_name;
    char *ext_file_name;
    int i;
    int j;
    int address;




    ob_file_name = (char*)malloc(strlen(file_name) + OB_FILE_EXTENTION_LENGTH + 1);
    if (ob_file_name == NULL) {
        printf("%s", MEMORY_ALLOCATION_ERROR);
        exit(1);
    }
    ext_file_name = (char*)malloc(strlen(file_name) + EXT_FILE_EXTENTION_LENGTH + 1);
    if (ext_file_name == NULL) {
        printf("%s", MEMORY_ALLOCATION_ERROR);
        exit(1);
    }
  
    strcpy(ob_file_name, file_name);

    strcat(ob_file_name, OB_FILE_EXTENTION);

    ob_file = fopen(ob_file_name, "w");
    if (ob_file == NULL) {
        printf("Error: File %s not found\n", ob_file_name);
        exit(1);
    }

    



    fprintf(ob_file, "  %d %d\n", tr->code_image_size, tr->data_image_size);

    for (i = 0, address = 0; i < tr->code_image_size; i++, address++) {
        if (i + 1 == tr->code_image_size) {
          printBase4(tr->code_image[i], address, ob_file, YES);
        } else {
          printBase4(tr->code_image[i], address, ob_file, NO);
        }
    }
    if (tr->data_image_size > 0) {
        fprintf(ob_file, "\n");
    }
    for (i = 0; i < tr->data_image_size; i++, address++) {
        if (i + 1 == tr->data_image_size) {
          printBase4(tr->data_image[i], address, ob_file, YES);
        } else {
          printBase4(tr->data_image[i], address, ob_file, NO);
        }
    }

    if (tr->number_of_entries > 0) {
      ent_file_name =
          (char *)malloc(strlen(file_name) + ENT_FILE_EXTENTION_LENGTH + 1);
      if (ent_file_name == NULL) {
        printf("%s", MEMORY_ALLOCATION_ERROR);
        exit(1);
      }
      strcpy(ent_file_name, file_name);
      strcat(ent_file_name, ENT_FILE_EXTENTION);

        ent_file = fopen(ent_file_name, "w");
        if (ent_file == NULL) {
            printf("Error: File %s not found\n", ent_file_name);
            exit(1);
        }

        for (i = 0; i < tr->number_of_entries; i++) {
            fprintf(ent_file, "%s ", tr->entries[i].name);
            if (tr->entries[i].address < 1000) {
                fprintf(ent_file, "0%d\n", tr->entries[i].address);
            } else {
                fprintf(ent_file, "%d\n", tr->entries[i].address);
            }
            fflush(ent_file);
        }
    }

    if (tr->number_of_entries > 0) {
      ext_file_name =
          (char *)malloc(strlen(file_name) + ENT_FILE_EXTENTION_LENGTH + 1);
      if (ent_file_name == NULL) {
        printf("%s", MEMORY_ALLOCATION_ERROR);
        exit(1);
      }
      strcpy(ent_file_name, file_name);
      strcat(ent_file_name, ENT_FILE_EXTENTION);

        ent_file = fopen(ent_file_name, "w");
        if (ent_file == NULL) {
            printf("Error: File %s not found\n", ent_file_name);
            exit(1);
        }

        for (i = 0; i < tr->number_of_entries; i++) {
            fprintf(ent_file, "%s ", tr->entries[i].name);
            if (tr->entries[i].address < 1000) {
                fprintf(ent_file, "0%d\n", tr->entries[i].address);
            } else {
                fprintf(ent_file, "%d\n", tr->entries[i].address);
            }
            fflush(ent_file);
        }
    }

    if (tr->number_of_externals > 0) {
      strcpy(ext_file_name, file_name);
      strcat(ext_file_name, EXT_FILE_EXTENTION);

      ext_file = fopen(ext_file_name, "w");
      if (ext_file == NULL) {
        printf("Error: File %s not found\n", ext_file_name);
        exit(1);
      }

      for (i = 0; i < tr->number_of_externals; i++) {
        for (j = 0; j < tr->external_table[i].number_of_addresses; j++) {
          if (i + 1 == tr->number_of_externals && j + 1 == tr->external_table[i].number_of_addresses) {
            fprintf(ext_file, "%s ", tr->external_table[i].name);
            if (tr->external_table[i].addresses[j] < 1000) {
              fprintf(ext_file, "0%d", tr->external_table[i].addresses[j]);
            } else {
              fprintf(ext_file, "%d", tr->external_table[i].addresses[j]);
            }
            fflush(ext_file);
            break;
          }
          fprintf(ext_file, "%s ", tr->external_table[i].name);
          if (tr->external_table[i].addresses[j] < 1000) {
            fprintf(ext_file, "0%d\n", tr->external_table[i].addresses[j]);
          } else {
            fprintf(ext_file, "%d\n", tr->external_table[i].addresses[j]);
          }
          fflush(ext_file);
        }
      }
    }

    return 0;

}

void printBase4(int input, int address, FILE *file, int last_line) {
  if (last_line == YES) {

    char base4[8];
    int i = 6;
    unsigned int num = (unsigned int)input;

    memset(base4, '*', 7);
    base4[7] = '\0';

    if (num == 0) {
      if (address < 1000) {
        fprintf(file, "0%d %s", address + 100, base4);
        fflush(file);
      } else {
        fprintf(file, "%d %s", address + 100, base4);
        fflush(file);
      }
      return;
    }

    while (num > 0 && i >= 0) {
      switch (num % 4) {
      case 0:
        base4[i] = '*';
        break;
      case 1:
        base4[i] = '#';
        break;
      case 2:
        base4[i] = '%';
        break;
      case 3:
        base4[i] = '!';
        break;
      }
      num /= 4;
      i--;
    }

    if (address < 1000) {
      fprintf(file, "0%d %s", address + 100, base4);
      fflush(file);
    } else {
      fprintf(file, "%d %s", address + 100, base4);
      fflush(file);
    }
  } else {

    char base4[8];
    int i = 6;
    unsigned int num = (unsigned int)input;

    memset(base4, '*', 7);
    base4[7] = '\0';

    if (num == 0) {
      if (address < 1000) {
        fprintf(file, "0%d %s\n", address + 100, base4);
        fflush(file);
      } else {
        fprintf(file, "%d %s\n", address + 100, base4);
        fflush(file);
      }
      return;
    }

    while (num > 0 && i >= 0) {
      switch (num % 4) {
      case 0:
        base4[i] = '*';
        break;
      case 1:
        base4[i] = '#';
        break;
      case 2:
        base4[i] = '%';
        break;
      case 3:
        base4[i] = '!';
        break;
      }
      num /= 4;
      i--;
    }

    if (address < 1000) {
      fprintf(file, "0%d %s\n", address + 100, base4);
      fflush(file);
    } else {
      fprintf(file, "%d %s\n", address + 100, base4);
      fflush(file);
    }
  } 
  }
