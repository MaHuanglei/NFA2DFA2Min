#ifndef STDAFX_H
#define STDAFX_H

/****************头文件*******************/
#include<iostream>
using namespace std;
#include<fstream>
#include<vector>
#include<string>
#include<stack>


/*****************数据结构*****************/
// 状态结构体
typedef struct state{
	string stateName;	//状态名
	int num;			//状态标号
}STATE;



/******************公共函数****************/

//根据分割标志sc将字符串str分割为若干段，
//保存在vector<string>中并返回
vector<string> split(string str, string sc)
{
	vector<string> words;
	string temp;
	int index_sc;
	while (str.size() > 0)
	{
		index_sc = str.find(sc, 0);
		if (index_sc != string::npos){
			temp = str.substr(0, index_sc);
			if (temp != "")
				words.push_back(temp);
			str.erase(0, index_sc + sc.size());
		}
		else{
			temp = str;
			words.push_back(temp);
			break;
		}
	}
	return words;
}

//检查是否已经读到文件结尾，若是则终结程序
void fileCheck(bool eof)
{
	if (eof){
		cout << "file is incompleteness!" << endl;
		exit(0);
	}
}

//检查vector<STATE> ve中是否含有obj
bool find(vector<STATE> ve, STATE obj)
{
	for (int i = 0; i < ve.size(); i++){
		if (ve[i].stateName == obj.stateName && ve[i].num == obj.num)
			return true;
	}
	return false;
}

//将ve2并入ve1，并去除重复的元素
void mergeVector(vector<STATE> &ve1, vector<STATE> ve2)
{
	for (int i = 0; i < ve2.size(); i++){
		if (!find(ve1, ve2[i]))
			ve1.push_back(ve2[i]);
	}
}

//将ve中的状态按一定规则组合成一个新的状态
STATE mergeToState(vector<STATE> ve)
{
	STATE newState;
	newState.num = -1;
	if (ve.empty()){
		newState.stateName = "$";
		return newState;
	}
	for (int i = 0; i < ve.size(); i++){
		newState.stateName += ve[i].stateName + "_";
	}
	return newState;
}

//得到状态obj在ve中的索引
int getStateIndex(vector<STATE> ve, STATE obj)
{
	for (int i = 0; i < ve.size(); i++){
		if (ve[i].stateName == obj.stateName && ve[i].num == obj.num)
			return i;
	}
	return -1;
}

#endif