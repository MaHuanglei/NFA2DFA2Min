#include"stdafx.h"

typedef struct state{
	string stateName;
	int num;
}STATE;

vector<STATE> states;
vector<char> inputChars;

STATE begin_state;
vector<STATE> end_states;
vector<vector<vector<STATE>>> convertMatrix;

void MatrixReadG();
vector<string> split(string str, string sc);
void fileCheck(bool eof);
int getStateNum(string state);


void MatrixReadG()
{
	ifstream infile;
	infile.open("NFA_G1.txt");
	if (!infile){
		cout << "file open fail!" << endl;
		exit(0);
	}

	char line[200];
	string strLine;
	STATE tempState;
	vector<STATE> temp1;
	vector<vector<STATE>> temp2;
	vector<string> strs1, strs2;

	//读取格式
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());

	//读取状态集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strLine = line;
	strs1 = split(strLine, " ");
	for (int i = 0; i < strs1.size(); i++){
		tempState.stateName = strs1[i];
		tempState.num = i;
		states.push_back(tempState);
	}

	//读取输入字符集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	for (int i = 0; i < 200; i++){
		if (line[i] == '\0')break;
		if (line[i] != ' ')
			inputChars.push_back(line[i]);
	}
	

	//读取开始状态
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	begin_state.stateName = line;
	begin_state.num = getStateNum(line);

	//读取结束状态集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strLine = line;
	vector<string> end_state_str = split(strLine, " ");
	for (int i = 0; i < end_state_str.size(); i++){
		tempState.stateName = end_state_str[i];
		tempState.num = getStateNum(tempState.stateName);
		end_states.push_back(tempState);	
	}

	//读取转换矩阵
	infile.getline(line, 200, '\n');
	while (!infile.eof()){
		strLine = line;
		strs1 = split(strLine, " ");
		for (int i = 0; i < strs1.size(); i++){
			strs2 = split(strs1[i], ",");
			for (int j = 0; j < strs2.size(); j++){
				tempState.stateName = strs2[j];
				tempState.num = getStateNum(tempState.stateName);
				temp1.push_back(tempState);
			}
			temp2.push_back(temp1);
			temp1.clear();
		}
		convertMatrix.push_back(temp2);
		temp2.clear();
		infile.getline(line, 200, '\n');
	}

	infile.close();
}

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

void fileCheck(bool eof){
	if (eof){
		cout << "file is incompleteness!" << endl;
		exit(0);
	}
}

int getStateNum(string statename)
{
	int num = -1;
	for (int i = 0; i < states.size(); i++){
		if (statename == states[i].stateName){
			num = i;
			break;
		}
	}
	return num;
}
/*
Matrix		//格式
S0 S1 S2 S3 S4 S5 S6 S7 $	//状态集，$代表空
1 2 @	//输入字符集，@代表ε
S0	//开始状态
S7	//结束状态

//状态转换矩阵
$ $ S1,S4
S5 S2 $
S5 S3 $
$ $ S7
S5 S2 $
S6 S2 $
$ $ S7
S7 S7 $
*/