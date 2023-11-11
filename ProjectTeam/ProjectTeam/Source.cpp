#include <iostream>
#include <String>
#include <vector>

class TableColumn {
public:
	std::string name;
	std::string type;
	int size;
	std::string default_value;

	TableColumn(const std::string& name, const std::string& type, int size, const std::string& default_value = "")
		: name(name), type(type), size(size), default_value(default_value) {}
};

class Table {
public:
	std::string name;
	std::vector<TableColumn> columns;

	Table(const std::string& name, const std::vector<TableColumn>& columns)
		: name(name), columns(columns) {

	}
};
class Database {
public:
	std::vector<Table> tables;
	
	void createTable(const std::string& tableName, const std::vector<TableColumn>& columns);
	void dropTable(const std::string& tableName);
	void displayTable(const std::string& tableName);
};