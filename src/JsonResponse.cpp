#include <JsonResponse.h>

/* trim string start/end */
void	trim_str(std::string &input, std::string s) {
	size_t buff_pos;

	for (std::string::iterator c = s.begin(); c != s.end(); ++c) {
		while ((buff_pos = input.find_first_of( *c )) == 0 && buff_pos != input.npos)
			input.erase(input.begin() + buff_pos);
		while ((buff_pos = input.find_last_of( *c )) == input.size()-1 && buff_pos != input.npos)
			input.erase(input.begin() + buff_pos);
	}
}

/* to seperate name from val and clean both */
bool	polish_string(std::string &buffer, std::string &name, std::string &val) {
	size_t buff_pos;
	/* must be in json format */
	buff_pos = buffer.find(":");
	if (buff_pos == buffer.npos)
		return false;

	/* seperating the var name from the value */
	name = buffer.substr(0, buff_pos);
	val = buffer.substr(buff_pos + 1);

	/* must not be empty */
	if (val.empty() || name.empty())
		return false;

	/* trimming white spaces */
	trim_str(name, " ");
	trim_str(val, " ");

	if (name.empty() || val.empty())
		return false;
	return true;
}

/*
   basic JSON object formation in the constructor
   .instead of implementing a tree based json parsing
   this function just splits the string by the highest level ','
   and seperate individual fields by '|' (second level) and '-' (third)
   .there is another utility to split by those delimeters
   */
JsonResponse::JsonResponse( std::string &input ) {
	std::string name, val;
	size_t input_pos, brk(0);

	trim_str(input, " \"[]{}");
	std::cout << "\nmodified response: " << input << std::endl;
	for (; (input_pos = input.find(",")) != input.npos;) {
		std::string buffer(input.substr(0, input_pos));

		if (buffer.find('{') != buffer.npos)
			brk++;

		input = input.substr(input_pos + 1);
		if (!polish_string(buffer, name, val))
			continue;

		if (brk) {
			brk--;
			input_pos = input.find('}');
			if (input_pos == input.npos)
				continue;
			val += "," + input.substr(0, input_pos);
			/* small branching */
			JsonResponse childJson(val);
			for (std::map<std::string, std::string>::iterator it = childJson.fields.begin();
					it != childJson.fields.end(); ++it)
				this->fields[it->first] = it->second;
			input = input.substr(input_pos + 1);
		}

		this->fields[name] = val;
	}
}

JsonResponse::~JsonResponse() { };

JsonResponse::JsonResponse( const JsonResponse &J ) {
	*this = J;
}

JsonResponse &JsonResponse::operator = ( const JsonResponse &J ) {
	this->fields = J.fields;
	return *this;
}
