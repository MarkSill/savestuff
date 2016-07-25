#include <sstream>
#include <string>
#include <fstream>
#include <cctype>
#include "variable.hpp"

unsigned int indent = 0;

namespace savestuff {
	Variable::Variable(VariableType type): type(type) {
		switch (type) { //Initialize default values based on the given type.
		case NUMBER:
			data = 0.0;
			break;
		case STRING:
			data = std::string();
			break;
		case CHAR:
			data = ' ';
			break;
		case BOOL:
			data = false;
			break;
		case TABLE:
			data = std::map<Variable*, Variable*>();
			break;
		case NIL:
			data = false;
			break;
		}
	}

	Variable::Variable(VariableType type, boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data): type(type), data(data) {}

	Variable::Variable(double num): Variable(NUMBER, num) {}

	Variable::Variable(std::string str): Variable(STRING, str) {}

	Variable::Variable(char ch): Variable(CHAR, ch) {}

	Variable::Variable(bool boolean): Variable(BOOL, boolean) {}

	Variable::Variable(std::map<Variable*, Variable*> map): Variable(TABLE, map) {}

	boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> Variable::get() const {
		return data;
	}

	void Variable::set(boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data) {
		this->data = data;
	}

	void Variable::set(VariableType type, boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data) {
		this->type = type;
		this->data = data;
	}

	std::string Variable::toString(bool file) const {
		std::stringstream ss(""); //Create a stream for printing to.
		switch (type) {
		case NUMBER:
			ss << boost::get<double>(data);
			break;
		case STRING:
			ss << boost::get<std::string>(data);
			break;
		case CHAR:
			ss << boost::get<char>(data);
			break;
		case BOOL:
			if (boost::get<bool>(data)) { //Must convert booleans to strings this way, otherwise it'd be 1 or 0.
				ss << "true";
			} else {
				ss << "false";
			}
			break;
		case TABLE:
			{
				if (!file) {
					ss << "{";
					indent++;
				}
				bool first = true;
				for (auto kv : boost::get<std::map<Variable*, Variable*>>(data)) {
					if (!file || !first) {
						ss << "\n";
					}
					first = false;
					for (unsigned int i = 0; i < indent; i++) {
						ss << "	"; //Indent with tabs to make the file prettier.
					}
					Variable *key = kv.first, *value = kv.second;
					std::string kstr = key->toString(), vstr = value->toString();
					//Add appropriate characters around strings and chars.
					if (value->type == STRING) {
						vstr = "\"" + vstr + "\"";
					} else if (value->type == CHAR) {
						vstr = "'" + vstr + "'";
					}
					if (key->type == STRING) { //Don't print the index if a number.
						ss << kstr << " = ";
					}
					ss << vstr;
				}
				if (!file) {
					indent--;
					ss << "\n";
					for (unsigned int i = 0; i < indent; i++) {
						ss << "	"; //Indent properly.
					}
					ss << "}";
				}
			}
			break;
		case NIL:
			ss << "null";
			break;
		default:
			ss << "???"; //Just in case. (This is used if type is somehow not initialized.)
			break;
		}
		return ss.str();
	}

	Variable* Variable::at(Variable *key) const {
		if (type == TABLE) {
			for (auto kv : boost::get<std::map<Variable*, Variable*>>(data)) {
				if (*kv.first == *key) { //Because I've defined a custom equivallency operator, this works.
					return kv.second;
				}
			}
		}
		return new Variable(NIL);
	}

	Variable* Variable::at(double num) const {
		return at(new Variable(NUMBER, num));
	}

	Variable* Variable::at(std::string str) const {
		return at(new Variable(STRING, str));
	}

	void Variable::put(Variable *key, Variable *value) {
		if (type == TABLE) {
			for (auto kv : boost::get<std::map<Variable*, Variable*>>(data)) { //Check if the key already exists.
				if (*kv.first == *key) {
					boost::get<std::map<Variable*, Variable*>>(data)[kv.first] = value;
					return;
				}
			}
			boost::get<std::map<Variable*, Variable*>>(data)[key] = value;
		}
	}

	void Variable::put(double num, Variable *value) {
		put(new Variable(NUMBER, num), value);
	}

	void Variable::put(std::string str, Variable *value) {
		put(new Variable(STRING, str), value);
	}

	unsigned int Variable::length() const {
		if (type == TABLE) {
			return boost::get<std::map<Variable*, Variable*>>(data).size();
		}
		return -1; //Returns -1 to signify that it's not a table.
	}

	std::map<Variable*, Variable*> Variable::getMap() const {
		if (type == TABLE) {
			return boost::get<std::map<Variable*, Variable*>>(data);
		}
		return std::map<Variable*, Variable*>(); //Because the map isn't a pointer, it returns an empty map instead.
	}

	void Variable::loadFromFile(const std::string &filename) {
		std::ifstream in;
		in.open(filename);
		loadFromFile(in, true);
		in.close();
	}

	void Variable::loadFromFile(std::ifstream &file, bool first) {
		std::string line;
		double index = 0; //This is used to determine the index of non-named elements.
		while (std::getline(file, line)) {
			line = trim(line); //Get rid of leading and trailing whitespace.
			size_t comment = line.find("//"); //Find a comment. (FIXME: This will find // inside of strings as well, meaning "//" is invalid to have in strings.)
			if (comment != std::string::npos) {
				line = line.substr(0, comment);
			}
			if (line.compare("") == 0) { //If the line is empty after all this, continue on.
				continue;
			}
			Variable *vkey = nullptr, *vvalue = nullptr;
			size_t assignment = line.find('='); //Find the assignment operator.
			std::string key = line.substr(0, assignment), value;
			key = trim(key);
			if (assignment == std::string::npos) { //No index given. This means we're working with a numerical index.
				vkey = new Variable(index++);
				value = key;
			} else { //Index given. Find the string representation of the value so we can convert it.
				vkey = new Variable(key);
				value = line.substr(assignment + 1);
				if (value.front() == ' ') {
					value = value.substr(1);
				}
			}
			value = trim(value);
			if (value.compare("{") == 0) { //Create a new table.
				vvalue = new Variable(TABLE);
				vvalue->loadFromFile(file);
			} else if (value.compare("}") == 0) { //End the current table. This can be used to end a file early if needed for whatever reason.
				return;
			} else if (value.compare("false") == 0) {
				vvalue = new Variable(false);
			} else if (value.compare("true") == 0) {
				vvalue = new Variable(true);
			} else if (value.compare("null") == 0) {
				vvalue = new Variable(NIL);
			} else if (value.front() == '"') {
				vvalue = new Variable(value.substr(1, value.length() - 2));
			} else if (value.front() == '\'') {
				vvalue = new Variable(value[1]);
			} else { //NOTE: Most errors arising from parsing will likely end up here.
				vvalue = new Variable(std::stod(value));
			}
			put(vkey, vvalue);
		}
	}

	void Variable::saveToFile(const std::string &filename) {
		std::ofstream out;
		out.open(filename);
		out << toString(true); //Thanks to the toString function, this is easy.
		out.close();
	}

	double Variable::asDouble() const {
		return boost::get<double>(data);
	}

	std::string Variable::asString() const {
		return boost::get<std::string>(data);
	}

	char Variable::asChar() const {
		return boost::get<char>(data);
	}

	bool Variable::asBool() const {
		return boost::get<bool>(data);
	}

	bool Variable::compare(const Variable &other) const {
		if (other.type != type) { //If the types are different, there's no way they can be the same.
			return false;
		}
		switch (type) {
		case NUMBER:
			return boost::get<double>(data) == boost::get<double>(other.data);
		case STRING:
			return boost::get<std::string>(data).compare(boost::get<std::string>(other.data)) == 0; //See if the strings' content matches, rather than the strings themselves.
		case CHAR:
			return boost::get<char>(data) == boost::get<char>(other.data);
		case BOOL:
			return boost::get<bool>(data) == boost::get<bool>(other.data);
		case TABLE:
			return &other == this; //See if the tables are the same via pointers.
		case NIL:
			return true; //Two nulls are equal, as they point to same location.
		default:
			return false; //Just in case.
		}
	}

	bool Variable::compare(double num) const {
		if (type == NUMBER) {
			return asDouble() == num;
		}
		return false;
	}

	bool Variable::compare(const std::string str) const {
		if (type == STRING) {
			return asString().compare(str) == 0;
		}
		return false;
	}

	bool Variable::compare(char ch) const {
		if (type == CHAR) {
			return asChar() == ch;
		}
		return false;
	}

	bool Variable::compare(bool boolean) const {
		if (type == BOOL) {
			return asBool() == boolean;
		}
		return false;
	}

	bool Variable::compare(Variable *other) const {
		if (other == nullptr) {
			return type == NIL;
		}
		return compare(*other);
	}

	bool Variable::compare(VariableType type) const {
		return this->type == type;
	}

	VariableType Variable::getType() const {
		return type;
	}

	Variable* Variable::operator[](Variable *key) const {
		return at(key);
	}

	Variable* Variable::operator[](double num) const {
		return at(num);
	}

	Variable* Variable::operator[](std::string str) const {
		return at(str);
	}

	bool Variable::operator==(const Variable &other) const {
		return compare(other);
	}

	bool Variable::operator==(double num) const {
		return compare(num);
	}

	bool Variable::operator==(const std::string str) const {
		return compare(str);
	}

	bool Variable::operator==(char ch) const {
		return compare(ch);
	}

	bool Variable::operator==(bool boolean) const {
		return compare(boolean);
	}

	bool Variable::operator==(Variable *other) const {
		return compare(other);
	}

	bool Variable::operator==(VariableType type) const {
		return compare(type);
	}

	bool Variable::operator!=(const Variable &other) const {
		return !compare(other);
	}

	bool Variable::operator!=(double num) const {
		return !compare(num);
	}

	bool Variable::operator!=(const std::string str) const {
		return !compare(str);
	}

	bool Variable::operator!=(char ch) const {
		return !compare(ch);
	}

	bool Variable::operator!=(bool boolean) const {
		return !compare(boolean);
	}

	bool Variable::operator!=(Variable *other) const {
		return !compare(other);
	}

	bool Variable::operator!=(VariableType type) const {
		return !compare(type);
	}

	bool Variable::operator<(double num) const {
		if (type == NUMBER) {
			return asDouble() < num;
		}
		return false;
	}

	bool Variable::operator>(double num) const {
		if (type == NUMBER) {
			return asDouble() > num;
		}
		return false;
	}

	bool Variable::operator<=(double num) const {
		if (type == NUMBER) {
			return asDouble() <= num;
		}
		return false;
	}

	bool Variable::operator>=(double num) const {
		if (type == NUMBER) {
			return asDouble() >= num;
		}
		return false;
	}

	std::ostream& operator<<(std::ostream &stream, const Variable &var) {
		stream << var.toString();
		return stream;
	}

	std::string trim(const std::string str) {
		std::string copy(str);
		while (isspace(copy.front())) { //Remove leading whitespace.
			copy = copy.substr(1);
		}
		while (isspace(copy.back())) { //Remove trailing whitespace.
			copy = copy.substr(0, copy.length() - 1);
		}
		return copy;
	}
}
