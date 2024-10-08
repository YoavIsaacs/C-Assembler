#include "firstPass.h"
#include "../DataStructures/symbolTableTrie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int first_pass(translation_unit *tr, const char *file_name, FILE *am_file) {
  char line[MAX_LINE_LENGTH];
  char temp_label[MAX_LABEL_LENGTH];
  int IC;
  int DC = 0;
  int i;
  int line_number = 1;
  int first_code = YES;
  int has_error = NO;
  assembler_AST *ast = NULL;
  symbol *temp_symbol = NULL;

  tr->number_of_symbols = 0;
  tr->symbol_table = create_symbol_table_node();

  while (fgets(line, sizeof(line), am_file)) {
    ast = create_assembler_AST(line);


    if (ast->error[0] != '\0') {
      printf("Sntax error in file %s line %d: %s\n", file_name, line_number,
             ast->error);
      has_error = YES;
      continue;
    }

    

    if (ast->label[0] != '\0' &&
        (ast->AST_type == AST_instruction ||
         (ast->AST_type == AST_directive &&
             (ast->AST_options.directive.AST_directive_type ==
                  AST_directive_data ||
              ast->AST_options.directive.AST_directive_type ==
                  AST_directive_string)))) {
                    if(first_code) {
                      IC = 100;
                      first_code = NO;
                    }
      strncpy(temp_label, ast->label, strlen(ast->label) - 1);
      temp_label[strlen(ast->label) - 1] = '\0';
      temp_symbol = search_symbol_table(tr->symbol_table, temp_label);
      if (temp_symbol) { /* SYMBOL IS IN THE TABLE */
        if (temp_symbol->type == symbol_table_type_entry || temp_symbol->type == symbol_table_type_data) {
          if (ast->AST_type == AST_instruction) {
            temp_symbol->type = symbol_table_type_entry_code;
            temp_symbol->address = IC;
          } else {
            temp_symbol->type = symbol_table_type_entry_data;
            temp_symbol->address = DC;
          }
        } else {
          printf("Error: redifinition of symbol %s in file %s line %d\n",
                 temp_label, file_name, line_number);
          has_error = YES;
        }
      } else { /* SYMBOL ISN'T IN THE TABLE */
        temp_symbol = (symbol*)malloc(sizeof(symbol));
        if (temp_symbol == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
        strcpy(temp_symbol->name, temp_label);
        if (ast->AST_type == AST_instruction) {
          temp_symbol->type = symbol_table_type_code;
          temp_symbol->address = IC;
        } else {
          temp_symbol->type = symbol_table_type_data;
          temp_symbol->address = DC;
        }
        insert_to_symbol_table(tr->symbol_table, temp_symbol);
        tr->number_of_symbols++;
      }
    }

    if (ast->AST_type == AST_instruction) {
      if (first_code) {
        IC = 99;
        first_code = NO;
      }
      IC++;
      if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register &&
          ast->AST_options.instruction.AST_instruction_operand_type[1]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register) {
        IC++;
      } else {
        if ((int)ast->AST_type <= AST_instruction_lea) {
          switch (ast->AST_options.instruction.AST_instruction_operand_type[0]
                      .AST_instruction_operand_option) {
          case AST_instruction_operand_option_constant:
            if (ast->AST_options.instruction.AST_instruction_operand_type[0].AST_instruction_operand_type.constant.type == op_label) {
              temp_symbol = search_symbol_table(tr->symbol_table, ast->AST_options.instruction.AST_instruction_operand_type[0].AST_instruction_operand_type.constant.constant_type.constant_label);
              if (!temp_symbol) {
                printf("Error: label %s is not defined in file %s line %d\n", ast->AST_options.instruction.AST_instruction_operand_type[0].AST_instruction_operand_type.constant.constant_type.constant_label, file_name, line_number);
                has_error = YES;
              }
            }   
            IC++;
            break;
          case AST_instruction_operand_option_label:
            IC++;
            break;
          case AST_instruction_operand_option_index:
            IC += 2;
            break;
          case AST_instruction_operand_option_register:
            IC++;
            break;
          case not_relevant:
          default:
            break;
          }
          switch (ast->AST_options.instruction.AST_instruction_operand_type[1]
                      .AST_instruction_operand_option) {
          case AST_instruction_operand_option_constant:
            if (ast->AST_options.instruction.AST_instruction_operand_type[1]
                    .AST_instruction_operand_type.constant.type == op_label) {
              temp_symbol = search_symbol_table(tr->symbol_table, ast->AST_options.instruction.AST_instruction_operand_type[1].AST_instruction_operand_type.constant.constant_type.constant_label);
              if (!temp_symbol) {
                printf("Error: label %s is not defined in file %s line %d\n", ast->AST_options.instruction.AST_instruction_operand_type[1].AST_instruction_operand_type.constant.constant_type.constant_label, file_name, line_number);
                has_error = YES;
              }
            }
            IC++;
            break;
          case AST_instruction_operand_option_label:
            IC++;
            break;
          case AST_instruction_operand_option_index:
            IC += 2;
            break;
          case AST_instruction_operand_option_register:
            IC++;
            break;
          case not_relevant:
          default:
            break;
          }
        }
      }
    } else if ((ast->AST_type == AST_directive && (ast->AST_options.directive.AST_directive_type == AST_directive_data ||
                                              ast->AST_options.directive.AST_directive_type == AST_directive_string))) {
      if (tr->data_image == NULL) {
        (tr->data_image) = (int*)malloc(sizeof(int) * ast->AST_options.directive.num_of_data_entries + 2);
        if (tr->data_image == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
      } else {
        tr->data_image = (int*)realloc(tr->data_image, sizeof(int) * (DC + ast->AST_options.directive.num_of_data_entries + 2));
        if (tr->data_image == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
      }
      if (ast->AST_options.directive.AST_directive_type == AST_directive_string) {
        memcpy(
            tr->data_image + DC,
            ast->AST_options.directive.AST_directive_options.string,
            sizeof(int) * ast->AST_options.directive.num_of_data_entries);
      } else if (ast->AST_options.directive.AST_directive_type == AST_directive_data) {
        for (i = 0; i < ast->AST_options.directive.num_of_data_entries; i ++) {
          if (ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_options == AST_directive_data_label) {
            temp_symbol = search_symbol_table(tr->symbol_table, ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_type.label);
            if (temp_symbol) {
              tr->data_image[DC + i] = temp_symbol->data;
            } else {
              temp_symbol = (symbol*)malloc(sizeof(symbol));
              if (temp_symbol == NULL) {
                printf("%s", MEMORY_ALLOCATION_ERROR);
                exit(1);
              }
              strcpy(temp_symbol->name, ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_type.label);
              temp_symbol->type = symbol_table_type_data;
              temp_symbol->data = DC + i;
              insert_to_symbol_table(tr->symbol_table, temp_symbol);
            }
          } else {
          tr->data_image[DC + i] = ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_type.constant;
        } 

        }
      }
      DC += ast->AST_options.directive.num_of_data_entries;
      tr->DC = DC;

    } else if (ast->AST_type == AST_directive && (ast->AST_options.directive.AST_directive_type == AST_directive_entry ||
                                                  ast->AST_options.directive.AST_directive_type == AST_directive_extern)) {
      temp_symbol = search_symbol_table(tr->symbol_table, ast->label);                                          
      if (temp_symbol && (ast->AST_options.directive.AST_directive_type == AST_directive_entry)) {
        if (temp_symbol->type == symbol_table_type_code) {
          temp_symbol->type = symbol_table_type_entry_code;
        } else if (temp_symbol->type == symbol_table_type_data) {
          temp_symbol->type = symbol_table_type_entry_data;
        } else {
          printf("Error: redifinition of symbol %s in file %s line %d\n",
                 ast->label, file_name, line_number);
          has_error = YES;
        }
      } else if (!temp_symbol) {
        temp_symbol = (symbol*)malloc(sizeof(symbol));
        if (temp_symbol == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
        strcpy(temp_symbol->name, ast->label);
        if (ast->AST_type == AST_instruction) {
          temp_symbol->type = symbol_table_type_code;
          temp_symbol->address = IC;
        } else {
          if (ast->AST_options.directive.AST_directive_type == AST_directive_extern)
            temp_symbol->type = symbol_table_type_extern;
          else 
            temp_symbol->type = symbol_table_type_data;
          temp_symbol->address = DC;
        }
        insert_to_symbol_table(tr->symbol_table, temp_symbol);
        tr->number_of_symbols++;
      } else {
        printf("Error: redifinition of symbol %s in file %s line %d\n",
               ast->label, file_name, line_number);
        has_error = YES;
      }
    } else if (ast->AST_type == AST_define) {
      temp_symbol = search_symbol_table(tr->symbol_table, ast->AST_options.define.label);
      if (temp_symbol) {
        printf("Error: redifinition of symbol %s in file %s line %d\n",
               ast->AST_options.define.label, file_name, line_number);
        has_error = YES;
      } else {
        temp_symbol = (symbol*)malloc(sizeof(symbol));
        if (temp_symbol == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
        strcpy(temp_symbol->name, ast->AST_options.define.label);
        temp_symbol->type = symbol_table_type_define;
        temp_symbol->data = ast->AST_options.define.number;
        insert_to_symbol_table(tr->symbol_table, temp_symbol);
        tr->number_of_symbols++;
      }
    }
    free(ast);
    line_number++;
  }

  if (look_for_last_entry(tr->symbol_table) == YES) {
    printf("Error: missing entry in file %s\n", file_name);
    has_error = YES;
  }

  increment_IC(tr->symbol_table, IC);
  point_to_entries(tr->symbol_table, tr->entries, &tr->number_of_entries);

  return has_error;
}





int look_for_last_entry(symbol_entry_node *node) {
  int i;

  if (node == NULL) {
    return NO;
  }

  if (node->is_symbol == YES) {
    return node->symbol_data->type == symbol_table_type_entry? YES : NO;
  }

  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    look_for_last_entry(node->next[i]);
  }
  return NO;
}


void increment_IC(symbol_entry_node *node, int IC){
  int i;

  if (node == NULL) {
    return;
  }

  if (node->is_symbol == YES) {
    if (node->symbol_data->type == symbol_table_type_data || node->symbol_data->type == symbol_table_type_entry_data) {
        node->symbol_data->address += IC;
      }
    }

  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    increment_IC(node->next[i], IC);
  }
}

void point_to_entries(symbol_entry_node *node, symbol* entries, int* num_of_entries) {
  int i;

  if (node == NULL) {
    return;
  }

  if (node->is_symbol == YES) {
    if (node->symbol_data->type == symbol_table_type_entry_code || node->symbol_data->type == symbol_table_type_entry_data) {
      entries[(*num_of_entries)++] = *node->symbol_data;
    }
  }

  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    point_to_entries(node->next[i], entries, num_of_entries);
  }
} 