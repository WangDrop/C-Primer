#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
	int A[5] = { 1, 2, 3, 4, 5 };
	vector<int> V;
	copy(A, A + 5, V);
	cin.get();
	return 0;
}