#include "secondPass.h"
#include "../DataStructures/symbolTableTrie.h"
#include "../Frontend/assemblerAST.h"
#include "../global/definitions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int second_pass(translation_unit *tr, char *filename, FILE *am_file) {
  char input[MAX_LINE_LENGTH];
  assembler_AST *ast = {0};
  int has_error = NO;
  int i;
  int first_instruction = YES;
  int first_data = YES;
  int temp_ic;
  symbol *symb;

  tr->number_of_externals = 0;
  temp_ic = tr->IC;
  tr->data_image_size = 0;
  fflush(stdout);

  while (fgets(input, sizeof(input), am_file)) {
    ast = create_assembler_AST(input);

    if (ast->AST_type == AST_instruction) {
      if (first_instruction) {
        tr->IC = temp_ic;
        first_instruction = NO;
      }
      add_to_code_and_data_and_external_image(&tr->code_image,
                                              &tr->code_image_size);
      tr->code_image[tr->IC] =
          (ast->AST_options.instruction.AST_instruction_option
           << OPCODE_OFFSET);
      if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register &&
          ast->AST_options.instruction.AST_instruction_operand_type[1]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register) {
        tr->code_image[tr->IC] |=
            (TWO_REGISTERS_OFFSET_VALUE << TWO_REGISTERS_OFFSET);
      } else if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                     .AST_instruction_operand_option != not_relevant) {
        if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                .AST_instruction_operand_option ==
            AST_instruction_operand_option_register) {
          tr->code_image[tr->IC] |=
              (ast->AST_options.instruction.AST_instruction_operand_type[0]
                   .AST_instruction_operand_type.register_number
               << FIRST_OPERAND_OFFSET);
        } else {
          tr->code_image[tr->IC] |=
              (ast->AST_options.instruction.AST_instruction_operand_type[0]
                   .AST_instruction_operand_option
               << FIRST_OPERAND_OFFSET);
        }
      }
      if (ast->AST_options.instruction.AST_instruction_operand_type[1]
              .AST_instruction_operand_option != not_relevant) {
        tr->code_image[tr->IC] |=
            (ast->AST_options.instruction.AST_instruction_operand_type[1]
                 .AST_instruction_operand_option
             << SECOND_OPERAND_OFFSET);
      }
      if (ast->AST_options.instruction.AST_instruction_operand_type[0]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register &&
          ast->AST_options.instruction.AST_instruction_operand_type[1]
                  .AST_instruction_operand_option ==
              AST_instruction_operand_option_register) {
        tr->IC++;
        add_to_code_and_data_and_external_image(&tr->code_image,
                                                &tr->code_image_size);
        tr->code_image[tr->IC] =
            ast->AST_options.instruction.AST_instruction_operand_type[0]
                .AST_instruction_operand_type.register_number
            << FIRST_REGISTER_OFFSET;
        tr->code_image[tr->IC] |=
            ast->AST_options.instruction.AST_instruction_operand_type[1]
                .AST_instruction_operand_type.register_number
            << SECOND_OPERAND_OFFSET;
      } else {
        for (i = 0; i < NUM_OF_OPERANDS; i++) {
          if (ast->AST_options.instruction.AST_instruction_operand_type[i]
                  .AST_instruction_operand_option != not_relevant) {
            if (ast->AST_options.instruction.AST_instruction_operand_type[i]
                    .AST_instruction_operand_option ==
                AST_instruction_operand_option_constant) {
              tr->IC++;
              add_to_code_and_data_and_external_image(&tr->code_image,
                                                      &tr->code_image_size);
              if (ast->AST_options.instruction.AST_instruction_operand_type[i]
                      .AST_instruction_operand_type.constant.type ==
                  op_constant) {
                tr->code_image[tr->IC] =
                    ast->AST_options.instruction.AST_instruction_operand_type[i]
                        .AST_instruction_operand_type.constant.constant_type
                        .constant_constant
                    << 2;
              } else {
                symb = search_symbol_table(
                    tr->symbol_table,
                    ast->AST_options.instruction.AST_instruction_operand_type[i]
                        .AST_instruction_operand_type.constant.constant_type
                        .constant_label);
                tr->IC--;
                has_error = code_labels(symb, tr, ast, i, NO);
              }
            } else if (ast->AST_options.instruction
                           .AST_instruction_operand_type[i]
                           .AST_instruction_operand_option ==
                       AST_instruction_operand_option_label) {
              symb = search_symbol_table(
                  tr->symbol_table,
                  ast->AST_options.instruction.AST_instruction_operand_type[i]
                      .AST_instruction_operand_type.label);
              has_error = code_labels(symb, tr, ast, i, NO);
            } else if (ast->AST_options.instruction
                               .AST_instruction_operand_type[i]
                               .AST_instruction_operand_option ==
                           AST_instruction_operand_option_register &&
                       i == 0) {
              tr->IC++;
              add_to_code_and_data_and_external_image(&tr->code_image,
                                                      &tr->code_image_size);
              tr->code_image[tr->IC] |=
                  (ast->AST_options.instruction.AST_instruction_operand_type[i]
                       .AST_instruction_operand_type.register_number
                   << FIRST_REGISTER_OFFSET);
            } else if (ast->AST_options.instruction
                           .AST_instruction_operand_type[i]
                           .AST_instruction_operand_option ==
                       AST_instruction_operand_option_index) {
              symb = search_symbol_table(
                  tr->symbol_table,
                  ast->AST_options.instruction.AST_instruction_operand_type[i]
                      .AST_instruction_operand_type.index.label);
              has_error = code_labels(symb, tr, ast, i, NO);

              if (ast->AST_options.instruction.AST_instruction_operand_type[i]
                      .AST_instruction_operand_type.index
                      .AST_instruction_operand_option_index_option ==
                  AST_instruction_operand_option_index_constant) {
                tr->IC++;
                add_to_code_and_data_and_external_image(&tr->code_image,
                                                        &tr->code_image_size);
                tr->code_image[tr->IC] =
                    (ast->AST_options.instruction
                         .AST_instruction_operand_type[i]
                         .AST_instruction_operand_type.index.index_type.constant
                     << 2);
              } else {
                symb = search_symbol_table(
                    tr->symbol_table,
                    ast->AST_options.instruction.AST_instruction_operand_type[i]
                        .AST_instruction_operand_type.index.index_type.label);
                has_error = code_labels(symb, tr, ast, i, NO);
              }
            }
          }
        }
      }
      tr->IC++;
    } else if (ast->AST_options.directive.AST_directive_type == AST_directive_data || ast->AST_options.directive.AST_directive_type == AST_directive_string) {
      if (first_data == YES) {
        tr->code_lines_count = tr->IC - 1;
        first_data = NO;
      }
      tr->IC--;
      for (i = 0; i < ast->AST_options.directive.num_of_data_entries; i++) {
        tr->data_image_size++;
        if (ast->AST_options.directive.AST_directive_type == AST_directive_data) {
          if (ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_options == AST_directive_data_constant) {
            tr->IC++;
            add_to_code_and_data_and_external_image(&tr->code_image, &tr->code_image_size);
            tr->code_image[tr->IC] = ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_type.constant;
          } else {
            symb = search_symbol_table(tr->symbol_table, ast->AST_options.directive.AST_directive_options.AST_directive_data[i].AST_directive_data_type.label);
            has_error = code_labels(symb, tr, ast, i, YES);
          }
        } else {
          add_to_code_and_data_and_external_image(&tr->code_image, &tr->code_image_size);
          tr->code_image[tr->IC] = ast->AST_options.directive.AST_directive_options.string[i];
          tr->IC++;
        }
      }
    } else if (ast->AST_options.directive.AST_directive_type == AST_directive_entry) {
      continue;
      tr->IC++;
    } else if (ast->AST_options.directive.AST_directive_type == AST_directive_extern) {
      tr->IC++;
      continue;
    } else if (ast->AST_type == AST_define) {
      tr->IC++;
      continue;
    }
    free(ast);

  }
  tr->code_image_size = tr->code_image_size - tr->data_image_size - 1;
  return has_error;
}

void add_to_code_and_data_and_external_image(int **image, int *size) {
  if (*image == NULL) {
    *image = (int *)malloc(sizeof(int));
    if (*image == NULL) {
      printf("%s", MEMORY_ALLOCATION_ERROR);
      exit(1);
    }
    if (size == NULL) {
      size = (int *)malloc(sizeof(int));
    }
    *size = 1;
  } else {
    *size += 1;
    *image = (int *)realloc(*image, (*size) * sizeof(int));
    if (*image == NULL) {
      printf("%s", MEMORY_ALLOCATION_ERROR);
      exit(1);
    }
  }
  memset(*image + *size - 1, 0, sizeof(int));
}

int search_externals(external_table externals[], int num_of_externals, char *label) {
  int i;
  for (i = 0; i < num_of_externals; i++) {
    if (strcmp((const char *)externals[i].name, label) == 0) {
      return i;
    }
  }
  return -1;
}

int code_labels(symbol *symb, translation_unit *tr, assembler_AST *ast, int i, int data_entry) {
  int external_index;
  int has_error = NO;
  if (symb) {
    if (symb->type == symbol_table_type_extern) {
      tr->IC++;
      add_to_code_and_data_and_external_image(&tr->code_image,
                                              &tr->code_image_size);
      tr->code_image[tr->IC] = 1;
      external_index = search_externals(tr->external_table,
                                        tr->number_of_externals, symb->name);
      

      /* MAKE IT NOT DYNAMIC, THE TABKE IS AN ARRAY - ADD IT DIRECTLY */
      if (external_index != -1) {
        tr->external_table[external_index].addresses = (int *)realloc(
            tr->external_table[external_index].addresses,
            (tr->external_table[external_index].number_of_addresses + 1) *
                sizeof(int));
        tr->external_table[external_index].addresses[tr->external_table[external_index].number_of_addresses] = (tr->IC + 100);
        tr->external_table[external_index].number_of_addresses++;
        /*
        add_to_code_and_data_and_external_image(
            &tr->external_table[external_index].addresses,
            &tr->external_table[external_index].number_of_addresses);
        tr->external_table[external_index]
            .addresses[tr->external_table[external_index].number_of_addresses -
                       1] = tr->IC;
                       */
      } else {

        tr->external_table[tr->number_of_externals].number_of_addresses = 0;
        tr->external_table[tr->number_of_externals].addresses = (int *)malloc(sizeof(int));
        tr->external_table[tr->number_of_externals].addresses[tr->external_table[tr->number_of_externals].number_of_addresses] = tr->IC + 100;
        tr->external_table[tr->number_of_externals].number_of_addresses++;
        strcpy(tr->external_table[tr->number_of_externals].name, symb->name);
        tr->number_of_externals++;
        
        /*
        tr->number_of_externals++;
        strcpy(tr->external_table[tr->number_of_externals - 1].name,
               symb->name);
        add_to_code_and_data_and_external_image(
            &tr->external_table[tr->number_of_externals - 1].addresses, 0);
        tr->external_table[tr->number_of_externals - 1].number_of_addresses = 1;
        tr->external_table[tr->number_of_externals - 1].addresses[0] = tr->IC;
        */
      }
    } else if (symb->type == symbol_table_type_define) {
      tr->IC++;
      add_to_code_and_data_and_external_image(&tr->code_image,
                                              &tr->code_image_size);
      if (data_entry == NO) {

      tr->code_image[tr->IC] = symb->data << 2;
      } else {
        tr->code_image[tr->IC] = symb->data;
      }
    }

    else {
      tr->IC++;
      add_to_code_and_data_and_external_image(&tr->code_image,
                                              &tr->code_image_size);
      tr->code_image[tr->IC] = symb->address << 2;
      tr->code_image[tr->IC] |= 2;
    }
  } else {
    printf(SYMBOL_NOT_FOUND_ERROR, symb->name);
    has_error = YES;
  }
  return has_error;
}
