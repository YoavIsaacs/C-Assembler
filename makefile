CC			= gcc
CFLAGS		= -ansi -pedantic -Wall
LDFLAGS		= -lm
PROG_NAME 	= preproc
BUILD_DIR	= build
OBJ_DIR		= $(BUILD_DIR)/obj
BIN_DIR		= $(BUILD_DIR)/bin
ZIP_NAME	= Maman14.zip
ifdef DEBUG
CFLAGS += -g 
endif

.PHONY: clean build_env all

all: build_env $(PROG_NAME) 

$(PROG_NAME): test.o trie.o linkedList.o macro.o preprocessor.o assemblerAST.o firstPass.o symbolTableTrie.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$@ $(LDFLAGS)

test.o: test.c Preprocessor/preprocessor.h \
 Preprocessor/../DataStructures/macro.h \
 Preprocessor/../DataStructures/../global/definitions.h \
 Preprocessor/../DataStructures/linkedList.h \
 Preprocessor/../DataStructures/trie.h

symbolTableTrie.o: DataStructures/symbolTableTrie.c \
 DataStructures/../global/definitions.h DataStructures/symbolTableTrie.h

trie.o: DataStructures/trie.c DataStructures/trie.h \
 DataStructures/../global/definitions.h

linkedList.o: DataStructures/linkedList.c DataStructures/linkedList.h \
 DataStructures/../global/definitions.h

macro.o: DataStructures/macro.c DataStructures/macro.h \
 DataStructures/../global/definitions.h


preprocessor.o: Preprocessor/preprocessor.c Preprocessor/preprocessor.h \
 Preprocessor/../DataStructures/macro.h \
 Preprocessor/../DataStructures/../global/definitions.h \
 Preprocessor/../DataStructures/linkedList.h \
 Preprocessor/../DataStructures/trie.h


assemblerAST.o: Frontend/assemblerAST.c Frontend/assemblerAST.h \
 Frontend/../global/definitions.h Frontend/../Preprocessor/preprocessor.h \
 Frontend/../Preprocessor/../DataStructures/macro.h \
 Frontend/../Preprocessor/../DataStructures/../global/definitions.h \
 Frontend/../Preprocessor/../DataStructures/linkedList.h \
 Frontend/../Preprocessor/../DataStructures/trie.h

firstPass.o: Middle/firstPass.c Middle/firstPass.h \
 Middle/../global/definitions.h Middle/../FrontEnd/assemblerAST.h \
 Middle/../FrontEnd/../global/definitions.h \
 Middle/../FrontEnd/../Preprocessor/preprocessor.h \
 Middle/../FrontEnd/../Preprocessor/../DataStructures/macro.h \
 Middle/../FrontEnd/../Preprocessor/../DataStructures/../global/definitions.h \
 Middle/../FrontEnd/../Preprocessor/../DataStructures/linkedList.h \
 Middle/../FrontEnd/../Preprocessor/../DataStructures/trie.h \
 Middle/../DataStructures/symbolTableTrie.h \
 Middle/../DataStructures/../global/definitions.h

%.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

build_env:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

zip: clean
	rm -f $(ZIP_NAME)
	zip -r $(ZIP_NAME) *