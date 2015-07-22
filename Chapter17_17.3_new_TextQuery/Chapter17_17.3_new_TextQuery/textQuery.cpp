#include "textQuery.h"
#include <iterator>
#include <algorithm>
#include <sstream>

TextQuery::TextQuery(std::ifstream & ifile)
	:file(new std::vector<std::string>)
{
	std::string line;
	while (getline(ifile, line)){
		file->push_back(line);
		int index = file->size() - 1;

		std::istringstream iss(line);
		std::string word;

		while (iss >> word){
			std::shared_ptr<std::set<indexTp>> & sp_lineIndex = wm[word];
			if (!sp_lineIndex){
				sp_lineIndex.reset(new std::set<indexTp>());
			sp_lineIndex->insert(index);
		}
	}
}


result_tuple TextQuery::query_return_tuple(const std::string & str)
{
	static std::shared_ptr<std::set<indexTp>> noData(new std::set<indexTp>);
	auto iter = wm.find(str);
	if (iter == wm.end()){
		return result_tuple <str, noData, file>;
	}
	else{
		return result_tuple < str, iter->second, file> ;
	}
}

