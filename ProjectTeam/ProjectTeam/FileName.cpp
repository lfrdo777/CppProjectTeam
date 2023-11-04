#include <iostream>
#include <String>
#include <vector>

class TableColumn {
public:
	std::string name;
	std::string type;
	int size;
	std::string default_value;

	TableColumn(const std::string& name,const std::string& type, int size,const std::string& default_value = "")
		: name(name), type(type), size(size), default_value(default_value) {}
};

