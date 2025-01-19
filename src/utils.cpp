#include <trader.h>


void	show_menu() {
	std::cout << "\nTrading System Menu:\n";
	std::cout << "1. Place Order" << std::endl;
	std::cout << "2. Cancel Order" << std::endl;
	std::cout << "3. Modify Order" << std::endl;
	std::cout << "4. View Orderbook" << std::endl;
	std::cout << "5. View Open Positions" << std::endl;
	std::cout << "6. Start WebSocket Market Data" << std::endl;
	std::cout << "0. Exit" << std::endl;
	std::cout << "your choice: ";
}

void	clear_terminal( void ) {
	printf("%s", CLR_TERM);
}

