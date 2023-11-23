#include <iostream>
#include <String>
#include <vector>

enum CommandType {
	CREATE_TABLE,
	DROP_TABLE,
	DISPLAY_TABLE,
	CREATE_INDEX,
	DROP_INDEX,
	INSERT,
	SELECT,
	UPDATE,
	DELETE,
	INVALID
};

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
	
	void createTable(const std::string& command);
	void dropTable(const std::string& command);
	void displayTable(const std::string& command);
	CommandType identifyCommandType(const std::string& command);
	void processCommand(const std::string& command);
};
CommandType Database::identifyCommandType(const std::string& command) {
	if (command.find("CREATE TABLE") != std::string::npos) {
		return CREATE_TABLE;
	}
	else if (command.find("DROP TABLE") != std::string::npos) {
		return DROP_TABLE;
	}
	else if (command.find("DISPLAY TABLE") != std::string::npos) {
		return DISPLAY_TABLE;
	}
	return INVALID;
}
void Database::processCommand(const std::string& command) {
	CommandType type = identifyCommandType(command);
	
	switch (type) {
	case CREATE_TABLE:
		createTable(command);
		break;
	case DROP_TABLE:
		dropTable(command);
		break;
	case DISPLAY_TABLE:
		displayTable(command);
		break;
		
	default:
		std::cout << "Error Invalid command" << std::endl;

	}

}
void Database::createTable(const std::string& command) {
	size_t start = command.find("CREATE TABLE") + std::string("CREATE TABLE").length();
	size_t end = command.find('(');
	std::string tableName = command.substr(start, end - start);

	tables.emplace_back(tableName, std::vector<TableColumn>());
	start = end + 1;
	end = command.find(')', start);
	std::string columnsInfo = command.substr(start, end - start);

	size_t pos = 0;
	while ((pos = columnsInfo.find(','))!= std::string::npos)  {
		std::string token = columnsInfo.substr(0, pos);

		size_t spacePos = token.find(' ');
		std::string columnName = token.substr(0, spacePos);
		token.erase(0, spacePos + 1);
		
		spacePos = columnsInfo.find(' ');
		std::string columnType = columnsInfo.substr(0, spacePos);
		columnsInfo.erase(0, spacePos + 1);

		spacePos = columnsInfo.find(' ');
		int columnSize = std::stoi(columnsInfo.substr(0, spacePos));
		columnsInfo.erase(0, spacePos + 1);

		std::string defaultValue;
		if (!columnsInfo.empty() && columnsInfo[0 == ' ']) {
			defaultValue = columnsInfo.substr(1);
		}
		
		tables.back().columns.emplace_back(columnName, columnType, columnSize, defaultValue);

		columnsInfo.erase(0, pos + 1);

	}

	std::cout << "Table " << tableName << " created successfully" << std::endl;
	

}
void Database::displayTable(const std::string& tableName) {
	for (const Table& table : tables) {
		if (table.name == tableName) {
			std::cout << "Displaying contents of table" << tableName << " : " << std::endl;
			for (const TableColumn& column : table.columns) {
				std::cout << column.name << " |";
			}
			std::cout << std::endl;
			std::cout << "(No data)" << std::endl;
			return;
		}
	}
	std::cout << "Error: Table '"<<tableName << "not found." << std::endl;
}
void Database::dropTable(const std::string& command) {
	size_t start = command.find("DROP TABLE") + std::string("DROP TABLE").length();
	size_t end = command.find(';', start);
	std::string tableName = command.substr(start, end - start);
	auto it = std::remove_if(tables.begin(), tables.end(), [&](const Table& table) {
		return table.name == tableName;
		});
	if (it != tables.end()) {
		tables.erase(it, tables.end());
		std::cout << "Table " << tableName << "dropped successfully" << std::endl;
	}
	else {
		std::cout << "ERROR : Table '" << tableName << "' not found." << std::endl;
	}
}
int main() {
	Database myDatabase;

	std::string command1 = "CREATE TABLE LUIS (column1 int, column2 text);";
	std::string command2 = "DISPLAY TABLE LUIS";
	std::string command3 = "DROP TABLE LUIS;";

	myDatabase.processCommand(command1);
	myDatabase.processCommand(command2);
	myDatabase.processCommand(command3);
	
	return 0;
	
}