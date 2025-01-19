C++ = c++
SRC = $(wildcard src/*.cpp)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++98 
NAME = trade

all: $(NAME)

$(NAME): $(OBJ)
	$(C++) -lcurl $(FLAGS) -o $(NAME) $(OBJ)

build/%.o: src/%.cpp $(HR) 
	@mkdir -p $(dir $@)
	$(C++) $(FLAGS) -I include -c $< -o $@

clean:
	rm -rf build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
