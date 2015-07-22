#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
vector<string> strTmp;

int main()
{
	
	for (int i = 100; i < 1000; i++)
	{
		if (i % 2){
			ostringstream ost;
			ost.sync_with_stdio();
			ost << i;
			string tmp;
			tmp = (ost.str());
			if (tmp.find('5') == tmp.npos){
				strTmp.push_back(tmp);
			}
		}
	}
	int i = 0;
	for (auto p : strTmp){
		if (!(++i % 10)){
			cout << endl;
		}
		cout << p << "  ";
	}
	cout << "Ò»¹²ÓÐ£º " << strTmp.size();
	
	cin.get();
	return 0;
}