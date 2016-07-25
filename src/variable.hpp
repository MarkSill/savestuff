#pragma once
#ifndef SAVESTUFF_VARIABLE
#define SAVESTUFF_VARIABLE
#include <map>
#include "boost/variant.hpp"

namespace savestuff {
	enum VariableType {
		NUMBER, //double
		STRING, //std::string
		CHAR, //char
		BOOL, //bool
		TABLE, //std::map<Variable*, Variable*>
		NIL //bool (in this case)
	};

	//Trims leading and trailing whitespace from the given string.
	std::string trim(const std::string str);

	class Variable {
	public:
		//Creates a variable of the given type. Initializes automatically with default values according to type.
		Variable(VariableType type = NIL);
		//Creates a variable with the given type and data.
		Variable(VariableType type, boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data);
		Variable(double num);
		Variable(std::string str);
		Variable(char ch);
		Variable(bool boolean);
		Variable(std::map<Variable*, Variable*> map); //Not really sure if this constructor is actually worth using....
		//Gets the variable's stored information.
		boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> get() const;
		//Sets the variable's stored information to something of the same type.
		void set(boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data);
		//Sets the variable's type and stored information.
		void set(VariableType type, boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data);
		//Converts the variable to a string.
		std::string toString(bool file = false) const;
		//If a table, gets the variable at the given index. Returns null if not found.
		Variable* at(Variable* key) const;
		Variable* at(double num) const;
		Variable* at(std::string str) const;
		//If a table, stores a variable at the given index. If the index exists, it is replaced.
		void put(Variable *key, Variable *value);
		void put(double num, Variable *value);
		void put(const std::string str, Variable *value);
		//If a table, returns the number of elements in the table.
		unsigned int length() const;
		//If a table, returns the map that stores the information.
		std::map<Variable*, Variable*> getMap() const;
		//Loads an environment from the given file.
		void loadFromFile(const std::string &filename);
		//Saves the environment to a file.
		void saveToFile(const std::string &filename);
		//Gets the variable as the given type.
		double asDouble() const;
		std::string asString() const;
		char asChar() const;
		bool asBool() const;
		//Comparison.
		bool compare(const Variable &other) const;
		bool compare(double num) const;
		bool compare(const std::string str) const;
		bool compare(char ch) const;
		bool compare(bool boolean) const;
		bool compare(Variable *other) const;
		bool compare(VariableType type) const;
		VariableType getType() const;
		//See at(Variable*) const.
		Variable* operator[](Variable *key) const;
		Variable* operator[](double num) const;
		Variable* operator[](std::string str) const;
		Variable* operator[](const char *ch) const;
		//Evaluates variables. Note that strings that .compare to 0 will match.
		bool operator==(const Variable &other) const;
		bool operator==(double num) const;
		bool operator==(const std::string str) const;
		bool operator==(char ch) const;
		bool operator==(bool boolean) const;
		bool operator==(Variable *other) const;
		bool operator==(VariableType type) const;
		bool operator!=(const Variable &other) const;
		bool operator!=(double num) const;
		bool operator!=(const std::string str) const;
		bool operator!=(char ch) const;
		bool operator!=(bool boolean) const;
		bool operator!=(Variable *other) const;
		bool operator!=(VariableType type) const;
		bool operator<(double num) const;
		bool operator>(double num) const;
		bool operator<=(double num) const;
		bool operator>=(double num) const;
		Variable operator+(const Variable &other) const;
		Variable operator+(double num) const;
		Variable operator+(std::string str) const;
		Variable operator-(const Variable &other) const;
		Variable operator-(double num) const;
		Variable operator*(const Variable &other) const;
		Variable operator*(double num) const;
		Variable operator/(const Variable &other) const;
		Variable operator/(double num) const;
		Variable& operator+=(const Variable &other);
		Variable& operator+=(double num);
		Variable& operator+=(const std::string str);
		Variable& operator-=(const Variable &other);
		Variable& operator-=(double num);
		Variable& operator*=(const Variable &other);
		Variable& operator*=(double num);
		Variable& operator/=(const Variable &other);
		Variable& operator/=(double num);
		Variable& operator++();
		Variable operator++(int unused);
		Variable& operator--();
		Variable operator--(int unused);
		operator double() const;
		operator std::string() const;
		operator char() const;
		operator bool() const;
		//Allows for printing to streams more easily.
		friend std::ostream& operator<<(std::ostream &stream, const Variable &var);
	protected:
		//Creates a table from the given file.
		void loadFromFile(std::ifstream &file, bool first = false);

		//The type of the variable.
		VariableType type;
		//The variable's stored data.
		boost::variant<double, std::string, char, bool, std::map<Variable*, Variable*>> data;
	};
}

#endif
