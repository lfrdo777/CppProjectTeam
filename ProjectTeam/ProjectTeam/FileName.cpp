#include <iostream>
#include <String>
#include <vector>

class Column {
public:
	std::string name;
	std::string type;
	int size;
	std::string default_value;
};

class CreateTableCommand {
public:
	CreateTableCommand(const std::string& tableName, const std::vector<Column>& columns)
	: tableName(tableName), columns(Column){

	}


	void execute();
};