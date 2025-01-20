#pragma	once

# include <iostream>
# include <cstdlib>
# include <string>

# define KNRM     "\x1B[0m"
# define KRED     "\x1B[31m"
# define KGRN     "\x1B[32m"
# define KYEL     "\x1B[33m"
# define KBLU     "\x1B[34m"
# define KAG      "\x1B[35m"
# define KCYN     "\x1B[36m"
# define KWHT     "\x1B[37m"
# define KUND     "\033[4m"
# define KBGR     "\033[7m"
# define CLR_TERM "\e[1;1H\e[2J"

/*######### APIClient #########*/

# include <APIClient.h>

/*######### WebSocketServer #########*/

# include <WebSocketServer.h>

/*######## JsonResponse ########*/

//#include <JsonResponse.h>

/*######## utils ########*/

void		show_menu( void );
void		clear_terminal( void );
std::string	get_env( const char* );
bool		get_client_info( std::string&, std::string& );
void		get_endpoints_info( std::map<std::string, std::string>& );
