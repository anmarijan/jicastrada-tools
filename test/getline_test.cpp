#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {

	std::string str;
	std::ifstream ifs(argv[1]);
	
	std::getline(ifs, str);
	std::cout << "[" << str << "]";
	
	return 0;
}