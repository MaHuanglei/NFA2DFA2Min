#ifndef NFA_H
#define NFA_H

#include"stdafx.h"

/********************************************************************
	类名：NFA
	五元式： M = (S, ∑, δ, s0, F)
	存储格式：矩阵存储
	赋值方式：读取文件
********************************************************************/

class NFA{
public:
	string nfaName;			//名称、描述
	int epsilonNum = -1;		//ε在字母表中的索引

	vector<STATE> states;			//S：状态集
	vector<char> inputChars;		//∑：有穷字母表
	vector<vector<vector<STATE>>> convertMatrix;		//δ：状态转换矩阵
	STATE begin_state;			//s0：初态
	vector<STATE> end_states;		//F：终态集

public:
	NFA(){ }
	~NFA();

	//读取文件，给NFA赋值
	void MatrixReadG(string filename);

	//返回状态state在状态集中的索引
	int getStateNum(string state);

	//得到状态state的epsilon集
	vector<STATE> getEpsilon(STATE stateNum);

	//得到状态state接收字符inputChar之后的转换状态集
	vector<STATE> getConvertSet(STATE state, char inputChar);

	//得到状态集stateSet接收字符inputChar之后的转换状态集
	vector<STATE> getConvertSet(vector<STATE> stateSet, char inputChar);

	//显示输出NFA
	void showNFAWithMatrix();
};

//读取文件，给NFA赋值
void NFA::MatrixReadG(string filename)
{
	ifstream infile;
	infile.open(filename);
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

	//读取自动机名称
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(line, "：");
	nfaName = strs1[1];
	strs1.clear();

	//读取格式
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());

	//读取状态集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "：")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		tempState.stateName = strs1[i];
		if (tempState.stateName == "$")
			continue;
		tempState.num = i;
		states.push_back(tempState);
	}

	//读取输入字符集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "：")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		if (strs1[i] != " "){
			inputChars.push_back(strs1[i][0]);
			if (strs1[i][0] == '@')
				epsilonNum = inputChars.size() - 1;
		}
	}


	//读取开始状态
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	begin_state.stateName = split(line, "：")[1];
	begin_state.num = getStateNum(begin_state.stateName);

	//读取结束状态集
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "：")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		tempState.stateName = strs1[i];
		tempState.num = getStateNum(tempState.stateName);
		end_states.push_back(tempState);
	}

	//读取转换矩阵
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	for (int lineNum = 0; lineNum < states.size(); lineNum++){
		infile.getline(line, 200, '\n');
		fileCheck(infile.eof());

		strs1 = split(line, " ");
		for (int i = 0; i < strs1.size(); i++){
			strs2 = split(strs1[i], ",");
			for (int j = 0; j < strs2.size(); j++){
				tempState.stateName = strs2[j];
				if (tempState.stateName == "$")
					continue;
				tempState.num = getStateNum(tempState.stateName);
				temp1.push_back(tempState);
			}
			temp2.push_back(temp1);
			temp1.clear();
		}
		convertMatrix.push_back(temp2);
		temp2.clear();
	}

	infile.close();
}

//返回状态state在状态集中的索引
int NFA::getStateNum(string statename)
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

//得到状态state的epsilon集
vector<STATE> NFA::getEpsilon(STATE state){
	vector<STATE> epsilonSet;
	epsilonSet.push_back(state);
	if (epsilonNum == -1)
		return epsilonSet;

	vector<STATE> stateList;
	int nowState = 0;
	STATE curState, nextState;
	stateList.push_back(state);
	while (nowState < stateList.size()){
		curState = stateList[nowState++];

		for (int i = 0; i < convertMatrix[curState.num][epsilonNum].size(); i++){
			nextState = convertMatrix[curState.num][epsilonNum][i];
			if (!find(stateList, nextState)){
				stateList.push_back(nextState);
			}
			if (!find(epsilonSet, nextState)){
				epsilonSet.push_back(nextState);
			}
		}
	}
	return epsilonSet;
}

//得到状态state接收字符inputChar之后的转换状态集
vector<STATE> NFA::getConvertSet(STATE state, char inputChar){
	int charNum = -1;
	for (int i = 0; i < inputChars.size(); i++){
		if (inputChars[i] == inputChar){
			charNum = i;
			break;
		}
	}

	vector<STATE> convertSet;
	if (charNum == -1)return convertSet;
	
	convertSet = convertMatrix[state.num][charNum];
	for (int i = 0; i < convertMatrix[state.num][charNum].size(); i++){
		if (convertMatrix[state.num][charNum][i].stateName == "$")continue;
		STATE curState = convertMatrix[state.num][charNum][i];
		vector<STATE> tempSet = getEpsilon(curState);
		mergeVector(convertSet, tempSet);
	}

	return convertSet;
}

//得到状态集stateSet接收字符inputChar之后的转换状态集
vector<STATE> NFA::getConvertSet(vector<STATE> stateSet, char inputChar){
	vector<STATE> convertSet;
	for (int i = 0; i < stateSet.size(); i++){
		vector<STATE> tempSet = getConvertSet(stateSet[i], inputChar);
		mergeVector(convertSet, tempSet);
	}
	return convertSet;
}

//显示输出NFA
void NFA::showNFAWithMatrix(){
	//写入自动机名称
	cout << "NFA名称：" << nfaName << endl;

	//写入存储格式
	cout << "存储格式：Matrix" << endl;

	//写入状态集
	cout << "有穷状态集S：";
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		cout << states[i].stateName << " ";
	}
	cout << "$(空集)" << endl;

	//写入接收字符集
	cout << "字母表∑：";
	for (int i = 0; i < inputChars.size(); i++){
		cout << inputChars[i] << " ";
	}
	cout << endl;

	//写入开始状态
	cout << "初态s0：";
	cout << begin_state.stateName << endl;

	//写入结束状态
	cout << "终态集F：";
	for (int i = 0; i < end_states.size(); i++){
		cout << end_states[i].stateName << " ";
	}
	cout << endl;

	//写入状态转换矩阵
	char str[100];
	sprintf_s(str, "%s%d%c%d%s", "状态转换矩阵(", states.size(), '*', inputChars.size(), ")：");
	cout << str << endl;
	for (int i = 0; i < convertMatrix.size(); i++){
		for (int j = 0; j < convertMatrix[i].size(); j++){
			if (convertMatrix[i][j].size() == 0){
				cout << "$" << " ";
				continue;
			}
			for (int t = 0; t < convertMatrix[i][j].size(); t++){
				if (t != 0)cout << ",";
				cout << convertMatrix[i][j][t].stateName;
			}
			cout << " ";
		}
		cout << endl;
	}
	cout << endl;
}

NFA::~NFA(){
	states.clear();
	inputChars.clear();
	end_states.clear();
	convertMatrix.clear();
}

#endif