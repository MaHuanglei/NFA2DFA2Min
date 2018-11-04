#ifndef DFA_H
#define DFA_H

#include"stdafx.h"

/********************************************************************
	类名：DFA
	五元式： M = (S, ∑, δ, s0, F)
	存储格式：矩阵存储
	赋值方式：读取文件	
********************************************************************/
class DFA{
public:
	string dfaName;			//名称、描述
	STATE EmptyState;			//空集状态
	int effectiveStateNum;		//有效状态个数

	vector<STATE> states;		//S：状态集
	vector<char> inputChars;	//∑：有穷字母表
	vector<vector<STATE>> convertMatrix;	//δ：状态转换矩阵
	STATE begin_state;		//s0：初态
	vector<STATE> end_states;	//F：终态集
public:
	DFA(){ }
	~DFA(){ };

	//设置自动机名称
	void setDfaName(string name){ dfaName = name; }

	//设置字符表
	void setInputChar(vector<char> chars);

	//设置终态集
	void setEndStates(NFA nfa);

	//设置空集状态
	void setEmptyState();

	//重置状态名称
	void resetStateName(string leader = "S");

	//将状态newState添加到DFA
	int addNewState(STATE newState);

	//根据状态集ve,将其合并为一个状态并添加到DFA
	int addNewState(vector<STATE> ve);

	//根据nfa将状态state分离成一个状态集
	vector<STATE> split2Set(STATE state, NFA nfa);

	//返回状态obj在dfa.states中的索引
	int findState(STATE obj);

	//得到状态state接收字符inputChar之后的转换状态
	STATE DFA::getConvertState(STATE state, char inputChar);

	//得到状态集stateSet接收字符inputChar之后的转换状态集
	vector<STATE> getConvertSet(vector<STATE> stateSet, char inputChar);

	//将DFA以矩阵方式写入文件
	void writeDFAtoFileWithMatrix(string filename);

	//以矩阵方式显示DFA
	void showDFAWithMatrix();
};

//设置字符表
void DFA::setInputChar(vector<char> chars){
	for (int i = 0; i < chars.size(); i++){
		if (chars[i] != '@')
			inputChars.push_back(chars[i]);
	}
}

int DFA::addNewState(STATE newState){

	newState.num = states.size();
	states.push_back(newState);
	return newState.num;
}

int DFA::addNewState(vector<STATE> ve){

	STATE newState = mergeToState(ve);
	newState.num = states.size();
	states.push_back(newState);
	return newState.num;
}

int DFA::findState(STATE obj){
	int stateNum = -1;
	for (int i = 0; i < states.size(); i++){
		if (obj.stateName == states[i].stateName){
			stateNum = i;
			break;
		}
	}
	return stateNum;
}

void DFA::setEndStates(NFA nfa){
	vector<STATE> tempSet;
	for (int i = 0; i < states.size(); i++){
		tempSet = split2Set(states[i], nfa);
		for (int j = 0; j < nfa.end_states.size(); j++){
			if (find(tempSet, nfa.end_states[j])){
				end_states.push_back(states[i]);
				break;
			}
		}
	}
}

vector<STATE> DFA::split2Set(STATE state, NFA nfa){
	vector<STATE> tempSet;
	STATE temp;
	vector<string> tempStateName = split(state.stateName, "_");
	for (int i = 0; i < tempStateName.size(); i++){
		temp.stateName = tempStateName[i];
		temp.num = nfa.getStateNum(temp.stateName);
		tempSet.push_back(temp);
	}
	return tempSet;
}

void DFA::resetStateName(string leader){
	char numToStr[10];

	//修改状态集名称
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		sprintf_s(numToStr, "%d", states[i].num);
		states[i].stateName = leader + numToStr;
	}

	//修改起始状态名称
	begin_state.stateName = states[begin_state.num].stateName;

	//修改结束状态集名称
	for (int i = 0; i < end_states.size(); i++){
		end_states[i].stateName = states[end_states[i].num].stateName;
	}


	//修改转换矩阵
	for (int i = 0; i < convertMatrix.size(); i++){
		for (int j = 0; j < convertMatrix[i].size(); j++){
			convertMatrix[i][j].stateName = states[convertMatrix[i][j].num].stateName;
		}
	}


}

void DFA::writeDFAtoFileWithMatrix(string filename)
{
	ofstream wfile(filename);
	if (!wfile){
		cout << "file create fail" << endl;
		return;
	}

	//写入自动机名称
	wfile << "DFA名称：" << dfaName << endl;

	//写入存储格式
	wfile << "存储格式：Matrix" << endl;
	
	//写入状态集
	wfile << "有穷状态集S：";
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		wfile << states[i].stateName << " ";
	}
	wfile << "$" << endl;

	//写入接收字符集
	wfile << "字母表∑：";
	for (int i = 0; i < inputChars.size(); i++){
		wfile << inputChars[i] << " ";
	}
	wfile << endl;

	//写入开始状态
	wfile << "初态s0：";
	wfile << begin_state.stateName << endl;

	//写入结束状态
	wfile << "终态集F：";
	for (int i = 0; i < end_states.size(); i++){
		wfile << end_states[i].stateName << " ";
	}
	wfile << endl;

	//写入状态转换矩阵
	char str[100];
	sprintf_s(str, "%s%d%c%d%s", "状态转换矩阵(", states.size(), '*', inputChars.size(), ")：");
	wfile << str << endl;
	for (int i = 0; i < convertMatrix.size(); i++){
		for (int j = 0; j < convertMatrix[i].size(); j++){
			wfile << convertMatrix[i][j].stateName << " ";
		}
		wfile << endl;
	}
	wfile << endl;

	//写入文件说明
	wfile << "说明：" << endl
		<< "1. $代表空集" << endl
		<< endl;


	wfile.close();
}

void DFA::showDFAWithMatrix(){
	//写入自动机名称
	cout << "DFA名称：" << dfaName << endl;

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
			cout << convertMatrix[i][j].stateName << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void DFA::setEmptyState(){
	EmptyState.num = -1;
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$"){
			EmptyState.num = i;
		}
	}
	EmptyState.stateName = "$";

	effectiveStateNum = states.size();
	if (EmptyState.num != -1)
		effectiveStateNum -= 1;
}

STATE DFA::getConvertState(STATE state, char inputChar){
	int charNum = -1;
	for (int i = 0; i < inputChars.size(); i++){
		if (inputChars[i] == inputChar){
			charNum = i;
			break;
		}
	}
	if (charNum == -1){
		return EmptyState;
	}
	return convertMatrix[state.num][charNum];
}

vector<STATE> DFA::getConvertSet(vector<STATE> stateSet, char inputChar){
	vector<STATE> convertSet;
	for (int i = 0; i < stateSet.size(); i++){
		STATE tempState = getConvertState(stateSet[i], inputChar);
		if (tempState.stateName == "$")continue;
		if (!find(convertSet, tempState)){
			convertSet.push_back(tempState);
		}
	}
	return convertSet;
}

#endif