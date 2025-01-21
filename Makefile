OS := $(shell uname)
CXX = c++
SRC = $(wildcard src/*.cpp)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++11 -fsanitize=address -g -pthread 

ifeq ($(OS), Darwin)
    BOOST_LIB = $(shell brew --prefix boost)/lib
    OPENSSL_LIB = $(shell brew --prefix openssl)/lib
    BOOST_INC = $(shell brew --prefix boost)/include
    OPENSSL_INC = $(shell brew --prefix openssl)/include
else
    BOOST_LIB = /usr/lib
    OPENSSL_LIB = /usr/lib
    BOOST_INC = /usr/include
    OPENSSL_INC = /usr/include
endif

LIBS = -lcurl -lboost_system -lssl -lcrypto -L$(BOOST_LIB) -L$(OPENSSL_LIB)
INCLUDES = -I include -I$(BOOST_INC) -I$(OPENSSL_INC)

NAME = trade

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(LIBS) $(FLAGS) -o $(NAME) $(OBJ)

build/%.o: src/%.cpp $(HR) 
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all

