#include "firstPass.h"
#include "../DataStructures/symbolTableTrie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int first_pass(translation_unit *tr, const char *file_name, FILE *am_file) {
  char line[MAX_LINE_LENGTH];
  int IC = 100;
  int DC = 0;
  int i;
  int line_number = 0;
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
         ast->AST_type == AST_directive &&
             (ast->AST_options.directive.AST_directive_type ==
                  AST_directive_data ||
              ast->AST_options.directive.AST_directive_type ==
                  AST_directive_string))) {

      temp_symbol = search_symbol_table(tr->symbol_table, ast->label);
      if (temp_symbol) { /* SYMBOL IS IN THE TABLE */
        if (temp_symbol->type == symbol_table_type_entry) {
          if (ast->AST_type == AST_instruction) {
            temp_symbol->type = symbol_table_type_entry_code;
            temp_symbol->address = IC;
          } else {
            temp_symbol->type = symbol_table_type_entry_data;
            temp_symbol->address = DC;
          }
        } else {
          printf("Error: redifinition of symbol %s in file %s line %d\n",
                 ast->label, file_name, line_number);
          has_error = YES;
        }
      } else { /* SYMBOL ISN'T IN THE TABLE */
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
          temp_symbol->type = symbol_table_type_data;
          temp_symbol->address = DC;
        }
        tr->number_of_symbols++;
      }
    }

    if (ast->AST_type == AST_instruction) {
      IC++;
      if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register &&
          ast->AST_options.instruction.AST_instruction_operand_type[1]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register) {
        IC++;
      } else {
        if (ast->AST_type <= AST_instruction_lea) {
          switch (ast->AST_options.instruction.AST_instruction_operand_type[0]
                      .AST_instruction_operand_option) {
          case AST_instruction_operand_option_constant:
            IC++;
            break;
          case AST_instruction_operand_option_label:
            IC++;
            break;
          case AST_instruction_operand_option_index:
            IC += 2;
            break;
          case AST_instruction_operand_option_register:
          case not_relevant:
          default:
            break;
          }
          switch (ast->AST_options.instruction.AST_instruction_operand_type[1]
                      .AST_instruction_operand_option) {
          case AST_instruction_operand_option_constant:
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
    } else if ((ast->AST_type =AST_directive && (ast->AST_options.directive.AST_directive_type == AST_directive_data ||
                                              ast->AST_options.directive.AST_directive_type == AST_directive_string))) {
      if (tr->data_image == NULL) {
        *(tr->data_image) = *(int*)malloc(sizeof(int) * ast->AST_options.directive.num_of_data_entries);
        if (tr->data_image == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
      } else {
        tr->data_image = (int*)realloc(tr->data_image, sizeof(int) * (DC + ast->AST_options.directive.num_of_data_entries));
        if (tr->data_image == NULL) {
          printf("%s", MEMORY_ALLOCATION_ERROR);
          exit(1);
        }
      }

      memcpy(tr->data_image + DC, ast->AST_options.directive.AST_directive_options.AST_directive_data, sizeof(int) * ast->AST_options.directive.num_of_data_entries);
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
          temp_symbol->type = symbol_table_type_data;
          temp_symbol->address = DC;
        }
        tr->number_of_symbols++;
      } else {
        printf("Error: redifinition of symbol %s in file %s line %d\n",
               ast->label, file_name, line_number);
        has_error = YES;
      }
    }
    line_number++;
  }

  if (look_for_last_entry(tr->symbol_table) == YES) {
    printf("Error: missing entry in file %s\n", file_name);
    has_error = YES;
  }

  increment_IC(tr->symbol_table, IC);
  point_to_entries(tr->symbol_table, tr->entries, tr->number_of_entries);

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


void increment_IC(symbol_entry_node *node, int IC) {
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

void point_to_entries(symbol_entry_node *node, symbol** entries, int num_of_entries) {
  int i;

  if (node == NULL) {
    return;
  }

  if (node->is_symbol == YES) {
    if (node->symbol_data->type == symbol_table_type_entry_code || node->symbol_data->type == symbol_table_type_entry_data) {
      *entries[num_of_entries] = *node->symbol_data;
      num_of_entries++;
    }
  }

  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    point_to_entries(node->next[i], entries, num_of_entries);
  }
} 