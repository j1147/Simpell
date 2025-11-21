#include "_global.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "CodeWrapper.hpp"
#include "Tokenizer.hpp"
#include "Token.hpp"
#include "SyntaxParser.hpp"
#include "Routine.hpp"
#include "Executor.hpp"
#include <vector>
#include <memory>

using std::vector;
using std::string;

int main(int argcount, char* args[])
{
    std::cout << "Starting to read file\n";

	if (argcount < 2)
	{
		std::cout << "File path not detected, aborting.\n";
		return 1;
	}

	string path(args[1]);

	std::cout << "Reading from file at " << path << "\n";

	std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file @" << path << ", please check that the file exists.\n";
		return 1;
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
	vector<Token::token*>* tokens = nullptr;
	try
	{
		tokens = tokenizer.scan();
	}
	catch (std::exception error)
	{
		std::cout << "Error: " << error.what() << '\n';
		std::cout << "Stopped scanning at position " << wrapper->pos << " out of " << wrapper->length << "\n";
		if (tokens)
		{
			deleteAll(tokens);
			delete tokens;
		}
		return 1;
	}

	delete wrapper;
	wrapper = nullptr;

	std::cout << "Scanned " << tokens->size() << " tokens\n";

	/*vector<Token::token*>::iterator it = tokens->begin();
	Token::token* token;
	while (it != tokens->end())
	{
		token = *it;
		std::cout << Token::getTokenName(token) << " : " << token->content << '\n';
		++it;
	}*/

	std::cout << "Beginning parsing and AST construction...\n";
	SyntaxParser parser(tokens);
	vector<Routine*>* routines = nullptr;
	try
	{
		routines = parser.parse();
	}
	catch (std::exception error)
	{
		std::cout << "Error: " << error.what() << "\n";
		goto cleanup;
	}

	std::cout << "Finished parsing.\n";
	std::cout << "Beginning execution...\n\n\n\n";

	try
	{
		Executor executor(routines);
		executor.execute();
	}
	catch (std::exception error)
	{
		std::cout << "Runtime error: " << error.what() << "\n";
		goto cleanup;
	}

	std::cout << "Program finished successfully.\n";

cleanup:
	if (routines)
	{
		deleteAll(routines);
		delete routines;
	}

	if (tokens)
	{
		deleteAll(tokens);
		delete tokens;
	}
}