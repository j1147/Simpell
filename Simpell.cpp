#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "BlockReader.hpp"
#include "CodeWrapper.hpp"
#include "Tokenizer.hpp"
#include "Token.hpp"
#include "SyntaxParser.hpp"
#include "Routine.hpp"
#include <vector>

using std::vector;
using std::string;

int main()
{
    std::cout << "Starting to read file\n";
	string path = "C:/code.txt";

	std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file @" << path << ", please check that the file exists.\n";
		return 0;
	}

	// Get file length
	struct stat stat_buffer;
	int result = stat(path.c_str(), &stat_buffer);

	long file_size = result == 0 ? stat_buffer.st_size : 0;
	std::cout << "File sized detected as " << file_size << '\n';

	// Open file at the start
	char* code = new char[file_size];
	std::memset(code, 0, file_size * sizeof(char));
	std::streampos position(0);

	file.seekg(position);
	file.read(code, file_size);

	// Begin parsing
	const int pos = 0;
	CodeWrapper* wrapper = new CodeWrapper(code, file_size, pos);

	Tokenizer tokenizer(wrapper);
	vector<Token::token*>* tokens;
	try
	{
		tokens = tokenizer.scan();
	}
	catch (std::exception error)
	{
		std::cout << "Error: " << error.what() << '\n';
		std::cout << "Stopped scanning at position " << wrapper->pos << " out of " << wrapper->length << "\n";
		return 1;
	}

	std::cout << "Scanned " << tokens->size() << " tokens\n";

	vector<Token::token*>::iterator it = tokens->begin();
	Token::token* token;
	while (it != tokens->end())
	{
		token = *it;
		std::cout << Token::getTokenName(token) << " : " << token->content << '\n';
		++it;
	}

	std::cout << "Beginning parsing and AST construction...\n";
	SyntaxParser parser(tokens);
	vector<Routine*>* routines;
	try
	{
		routines = parser.parse();
	}
	catch (std::exception error)
	{
		std::cout << "Error: " << error.what() << '\n';
		return 1;
	}

	for (Routine*& r : *routines)
		delete r;
	delete routines;

	for (int t = 0; t < tokens->size(); ++t)
		delete (*tokens)[t];
	delete tokens;

	delete wrapper;
}