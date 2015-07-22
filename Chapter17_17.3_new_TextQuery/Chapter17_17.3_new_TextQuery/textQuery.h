#ifndef TEXTQUERY_H
#define TEXTQUERY_H
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <set>
#include <tuple>
#include <map>

class TextQuery
{
public:
	typedef std::vector<std::string>::size_type indexTp;
	typedef std::tuple < std::string, std::shared_ptr<std::set<indexTp>>, 
		std::shared_ptr<std::vector<std::string>> > result_tuple;

	TextQuery(std::ifstream &);
	result_tuple query_return_tuple(const std::string & str);
private:
	std::shared_ptr<std::vector<std::string>> file;
	std::map<std::string, std::shared_ptr<std::set<indexTp>>> wm;
};

#endif