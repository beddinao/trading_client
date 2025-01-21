C++ = c++
SRC = $(wildcard src/*.cpp)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++11 -fsanitize=address -g -pthread 
LIBS = -lcurl -lboost_system -lssl -lcrypto -L $(shell brew --prefix boost)/lib -L $(shell brew --prefix openssl)/lib
INCLUDES = -I include -I $(shell brew --prefix boost)/include -I $(shell brew --prefix openssl)/include
NAME = trade

all: $(NAME)

$(NAME): $(OBJ)
	$(C++) $(LIBS) $(FLAGS) -o $(NAME) $(OBJ)

build/%.o: src/%.cpp $(HR) 
	@mkdir -p $(dir $@)
	$(C++) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
