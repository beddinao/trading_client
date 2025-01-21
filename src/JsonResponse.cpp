#include <JsonResponse.h>

/* trim string start/end */
void	trim_str(std::string &input, std::string s) {
	size_t buff_pos;

	/* for each char delimiter in s */
	for (std::string::iterator c = s.begin(); c != s.end(); ++c) {
		/* trimming from start */
		while ((buff_pos = input.find_first_of( *c )) == 0 && buff_pos != input.npos)
			input.erase(input.begin() + buff_pos);
		/* trimiing from the end */
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
   basic JSON string parsing
   .instead of implementing a complete tree based json parsing
   this function just splits the string by the highest level ','
   and only branch to lower levels for specific fields 
   */
void	JsonResponse::init( std::string &input ) {
	std::string name, val;
	size_t input_pos;

	if (this->fields.empty())
		this->fields.clear();

	trim_str(input, " \"[]{}");
	for (; (input_pos = input.find( "," )) != input.npos;) {
		std::string buffer(input.substr(0, input_pos));
		input = input.substr(input_pos + 1);

		/* json test */
		if (!polish_string(buffer, name, val))
			continue;

		/* merging back the string if it has opened brackets */
		if (val.find("[") != val.npos) {
			size_t brk(1), i(0);
			for (; i < input.size(); i++) {
				if (input[i] == '[')
					brk++;
				else if (input[i] == ']')
					brk--;
				if (!brk) break;
			}
			if (i < input.size()) {
				val += "," + input.substr(0, i);
				input = input.substr(i+1);
				trim_str(val, "[{ }]");
			}
		}
		if (val.find("{") != val.npos) {
			size_t brk(1), i(0);
			for (; i < input.size(); i++) {
				if (input[i] == '{')
					brk++;
				else if (input[i] == '}')
					brk--;
				if (!brk) break;
			}
			if (i < input.size()) {
				val += "," + input.substr(0, i);
				input = input.substr(i+1);
				trim_str(val, "[{ }]");
			}
		}

		/* final quotes removal */
		trim_str(name, "\"");
		trim_str(val, "\"");

		/* adding the result to the map */
		this->fields[name] = val;

		/* branching on specific name fields */
		if (name == "result" || name == "error"
			|| name == "trades" || name == "order")
			this->init(val);
	}
}

JsonResponse::JsonResponse() { };
JsonResponse::~JsonResponse() { };

JsonResponse::JsonResponse( const JsonResponse &J ) {
	*this = J;
}

JsonResponse &JsonResponse::operator = ( const JsonResponse &J ) {
	this->fields = J.fields;
	return *this;
}
