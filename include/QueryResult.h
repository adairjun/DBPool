#ifndef _QUERYRESULT_H
#define _QUERYRESULT_H

#include <vector>
#include <string>

using std::vector;
using std::string;

// QueryResult是用来存放Select的结果集的
class QueryResult
{
public:
	//Row是横向的,行的意思
	int getRowCount(){return string_table.size();}

	//Column是纵向的,列的意思
	int getColumnCount(){return string_table[0].size();};

	//从二维数组里面取出行和列
	string getElement(int row,int column){return string_table[row][column];}
	
	//增加一行
	void addRow(const vector<string>& row){string_table.push_back(row);}
 private:
	//行列的二维数组
	vector<vector<string> > string_table;
};
#endif /* QUERYRESULT_H */
