#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include<sstream>
#include <regex>

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos) {
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

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
		: name(name), columns(columns) {}
	void printTable() const {
		std::cout << "Table:" << name << std::endl;
		for (const TableColumn& column : columns) {
			std::cout << column.name << " (" << column.type << ", " << column.size << column.default_value << ") |";

		}
		std::cout << std::endl;
	}
};
class Database {
public:
	std::vector<Table> tables;
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
		IMPORT,
		INVALID
	};
	CommandType identifyCommandType(const std::string& command);

	void processCommand(const std::string& command);

private:
	void createTable(const std::string& command);
	void dropTable(const std::string& command);
	std::string getTableFileName(const std::string& tableName);
	void displayTable(const std::string& command);
	void importData(const std::string& command);
	std::string getTableFileName(const Table& table);
	void saveTableToFile(const Table& table);
	bool loadTableFromFile(const std::string& fileName, Table& table);
	void deleteTableFile(const std::string& tableName);
	
};
class FileManager {
public:
	static std::string readFile(const std::string& fileName);
	static void writeFile(const std::string& fileName, const std::string& data);
	static void appendToFile(const std::string& fileName, const std::string& data);
	static void	deleteFile(const std::string& fileName);
	static bool fileExists(const std::string& fileName);
};
Database::CommandType Database::identifyCommandType(const std::string& command) {

	if (command.find("CREATE TABLE") != std::string::npos) {
		return CREATE_TABLE;
	}
	else if (command.find("DROP TABLE") != std::string::npos) {
		return DROP_TABLE;
	}
	else if (command.find("DISPLAY TABLE") != std::string::npos) {
		return DISPLAY_TABLE;

	}
	else if (command.find("IMPORT") != std::string::npos) {
		return IMPORT;
	}
	return INVALID;
}
void Database::processCommand(const std::string& command) {
	std::cout << "Processing command: " << command << std::endl;
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
	case IMPORT:
		importData(command);
		break;

	default:
		std::cout << "Error Invalid command" << std::endl;
		std::abort();
	}

}
void Database::createTable(const std::string& command) {
	size_t start = command.find("CREATE TABLE") + std::string("CREATE TABLE").length();
	size_t end = command.find('(');
	std::string tableName = command.substr(start, end - start);
	tableName = trim(tableName);


	tableName = std::regex_replace(tableName, std::regex("^ +| +$|( ) +"), "$1");

	tables.emplace_back(tableName, std::vector<TableColumn>());
	start = end + 1;
	end = command.find(')', start);
	std::string columnsInfo = command.substr(start, end - start);
	std::istringstream columnStream(columnsInfo);
	std::string columnDetail;

	while (std::getline(columnStream, columnDetail, ',')) {
		std::istringstream detailStream(columnDetail);
		std::string columnName, columnType;
		int columnSize = -1; 
		std::string columnSizeStr;

		detailStream >> columnName >> columnType;
		if (detailStream >> columnSizeStr) {
			try {
				columnSize = std::stoi(columnSizeStr);
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "Error: Invalid column size for column " << columnName << std::endl;
				return;
			}
			catch (const std::out_of_range& e) {
				std::cerr << "Error: Column size out of range for column " << columnName << std::endl;
				return;
			}
		}


		columnName = std::regex_replace(columnName, std::regex("^ +| +$|( ) +"), "$1");
		columnType = std::regex_replace(columnType, std::regex("^ +| +$|( ) +"), "$1");

		tables.back().columns.emplace_back(columnName, columnType, columnSize, "");
	}

	std::cout << "Table " << tableName << " created successfully" << std::endl;
	saveTableToFile(tables.back());
}
void Database::displayTable(const std::string& command) {
	size_t start = command.find("DISPLAY TABLE") + std::string("DISPLAY TABLE").length();
	size_t end = command.find(';', start);
	std::string tableName = command.substr(start, end - start);
	tableName = trim(tableName);
	for (const Table& table : tables) {
		if (table.name == tableName) {
			table.printTable();
			return;
		}
	}
	std::cout << "Error: Table '" << tableName << "' not found." << std::endl;
	}

void Database::importData(const std::string& command)
{
}
	

void Database::dropTable(const std::string& command) {
	size_t start = command.find("DROP TABLE") + std::string("DROP TABLE").length();
	size_t end = command.find(';', start);
	std::string tableName = command.substr(start, end - start);
	tableName = trim(tableName);
	auto it = std::remove_if(tables.begin(), tables.end(), [&](const Table& table) {
		return table.name == tableName;
		});
	if (it != tables.end()) {
		tables.erase(it, tables.end());
		std::cout << "Table " << tableName << " dropped successfully" << std::endl;

		deleteTableFile(tableName);
	}else {
		std::cout << "ERROR : Table '" << tableName << "' not found." << std::endl;
	}
	
}
std::string Database::getTableFileName(const std::string& tableName) {
	return tableName + ".txt";
}
std::string Database::getTableFileName(const Table& table)
{
	return std::string();
}
void Database::saveTableToFile(const Table& table) {
	std::string fileName = getTableFileName(table.name);
	std::ofstream file(fileName);

	if (file.is_open()) {
		file << "Table: " << table.name << "\n";
		for (const TableColumn& column : table.columns) {
			file << column.name << " (" << column.type << ", " << column.default_value << ") |";
		}
		file << "\n";

		file.close();
		std::cout << "Table saved to file: " << fileName << std::endl;
	}
	else {
		std::cerr << "Error: Unable to open file for saving table." << std::endl;
	}
}
bool Database::loadTableFromFile(const std::string& fileName, Table& table) {
	std::ifstream file(fileName);
	if (file.is_open()) {

		std::string line;
		std::getline(file, line);
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			table.name = line.substr(pos + 2);
		}
		else {
			std::cerr << "Error: Invalid file format. Missing table name." << std::endl;
			file.close();
			return false;
		}
		std::getline(file, line);
		file.close();
		return true;
		
	}
	else {
		std::cerr << "Error: Unable to open file for loading table" << std::endl;
		return false;
	}

}
void Database::deleteTableFile(const std::string& tableName) {
	std::string fileName = getTableFileName(tableName);
	FileManager::deleteFile(fileName);
}
std::string FileManager::readFile(const std::string& fileName) {
	std::ifstream file(fileName);
	std::string content;

	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			content += line + "\n";
		}
		file.close();

	}
	else {
		std::cerr << "Error: Unable to open file for reading" << std::endl;
	}
	return content;
}
void FileManager::writeFile(const std::string& fileName, const std::string& data) {
	std::ofstream file(fileName);

	if (file.is_open()) {
		file << data;
		file.close();
		std::cout << "File '" << fileName << "' written successfully." << std::endl;
	}
	else {
		std::cerr << "Error: Unable to open file for writting." << std::endl;
	}

}
void FileManager::appendToFile(const std::string& fileName, const std::string& data) {
	std::ofstream file(fileName, std::ios::app);
	if (file.is_open()) {
		file << data;
		file.close();
		std::cerr << "Error: Unable to open file for appending." << std::endl;
	}

}
void FileManager::deleteFile(const std::string& fileName) {
	if (std::remove(fileName.c_str()) != 0) {
		std::cerr << "Error: Unable to delete file" << fileName << "'." << std::endl;
	}
	else {
		std::cout << "File'" << fileName << "' deleted successfully" << std::endl;

	}
}
bool FileManager::fileExists(const std::string& fileName) {
	std::ifstream file(fileName);
	return file.good();
}
void DatabaseimportData(const std::string& command) {
	size_t start = command.find("IMPORT") + std::string("IMPORT").length();
	size_t end = command.find(';', start);
	std::string fileName = command.substr(start, end - start);

	if (!FileManager::fileExists(fileName)) {
		std::cerr << "Error: File'" << fileName << "'not found." << std::endl;
		return;
	}
	std::string fileContent = FileManager::readFile(fileName);
	std::istringstream iss(fileContent);
	std::string line;

	while (std::getline(iss, line)) {
		std::istringstream lineStream(line);
		std::string columValue;
		std::vector<std::string> values;
		
		while (std::getline(lineStream, columValue, ',')) {
			values.push_back(columValue);
		}
	}
	std::cout << "Data imported successfully from file: " << fileName << std::endl;
}

int main() {
	Database myDatabase;

	myDatabase.processCommand("CREATE TABLE Students (id int, name text, age int)");
	myDatabase.processCommand("DISPLAY TABLE Students");
	myDatabase.processCommand("DROP TABLE Students");

	return 0;
}