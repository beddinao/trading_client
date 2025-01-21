#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <json.hpp>

# define RST     "\x1B[0m"
# define RED     "\x1B[31m"
# define GRN     "\x1B[32m"
# define YEL     "\x1B[33m"
# define BLU     "\x1B[34m"
# define AG      "\x1B[35m"
# define CYN     "\x1B[36m"
# define WHT     "\x1B[37m"
# define UND     "\033[4m"
# define BGR     "\033[7m"
# define CLR_TERM "\e[1;1H\e[2J"

void		show_menu( void );
void		clear_terminal( void );
std::string	get_env( const char* );
bool		get_client_info( std::string&, std::string& );
void		get_endpoints_info( std::map<std::string, std::string>& );
