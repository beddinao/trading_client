#pragma once

#include <string>
#include <iostream>
#include <map>

class	JsonResponse {
	public:
		JsonResponse( std::string& );
		~JsonResponse();
		JsonResponse( const JsonResponse& );
		JsonResponse &operator = ( const JsonResponse& );
		std::map<std::string, std::string> fields;
};
