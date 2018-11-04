#ifndef STDAFX_H
#define STDAFX_H

/****************ͷ�ļ�*******************/
#include<iostream>
using namespace std;
#include<fstream>
#include<vector>
#include<string>
#include<stack>


/*****************���ݽṹ*****************/
// ״̬�ṹ��
typedef struct state{
	string stateName;	//״̬��
	int num;			//״̬���
}STATE;



/******************��������****************/

//���ݷָ��־sc���ַ���str�ָ�Ϊ���ɶΣ�
//������vector<string>�в�����
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

//����Ƿ��Ѿ������ļ���β���������ս����
void fileCheck(bool eof)
{
	if (eof){
		cout << "file is incompleteness!" << endl;
		exit(0);
	}
}

//���vector<STATE> ve���Ƿ���obj
bool find(vector<STATE> ve, STATE obj)
{
	for (int i = 0; i < ve.size(); i++){
		if (ve[i].stateName == obj.stateName && ve[i].num == obj.num)
			return true;
	}
	return false;
}

//��ve2����ve1����ȥ���ظ���Ԫ��
void mergeVector(vector<STATE> &ve1, vector<STATE> ve2)
{
	for (int i = 0; i < ve2.size(); i++){
		if (!find(ve1, ve2[i]))
			ve1.push_back(ve2[i]);
	}
}

//��ve�е�״̬��һ��������ϳ�һ���µ�״̬
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

//�õ�״̬obj��ve�е�����
int getStateIndex(vector<STATE> ve, STATE obj)
{
	for (int i = 0; i < ve.size(); i++){
		if (ve[i].stateName == obj.stateName && ve[i].num == obj.num)
			return i;
	}
	return -1;
}

#endif