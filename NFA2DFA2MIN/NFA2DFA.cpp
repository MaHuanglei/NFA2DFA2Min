#include"stdafx.h"
#include"NFA.h"
#include"DFA.h"

//将NFA转化为DFA
bool NFA2DFA(NFA nfa, DFA &dfa);

//将DFA最小化
bool DFA2MIN(DFA dfa, DFA &minDfa);

//把状态obj自己组成一个集合，将这个集合添加到stateSet中
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE obj);

//把obj添加到lead所在的集合中
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE lead, STATE obj);

//返回集合SonSet在二维集合BogSet中的索引
int getStateNum(vector<vector<STATE>> BigSet, vector<STATE> SonSet);

//将二维集合stateSet中obj2所在的集合合并到obj1所在的集合，并删除obj2所在的集合
bool mergeState(vector<vector<STATE>> &stateSet, STATE obj1, STATE obj2);

int main()
{
	NFA nfa;
	nfa.MatrixReadG("NFA_G1.txt");
	nfa.showNFAWithMatrix();

	DFA dfa;
	dfa.setDfaName("nfa2dfa");
	NFA2DFA(nfa, dfa);

	dfa.resetStateName("T");
	dfa.writeDFAtoFileWithMatrix("DFA_G1.txt");
	dfa.showDFAWithMatrix();

	DFA minDfa;
	minDfa.setDfaName("minDfa");
	DFA2MIN(dfa, minDfa);
	minDfa.resetStateName("Q");
	minDfa.writeDFAtoFileWithMatrix("MINDFA_G1.txt");
	minDfa.showDFAWithMatrix();

	system("pause");
	return 0;
}

//将NFA转化为DFA
bool NFA2DFA(NFA nfa, DFA &dfa)
{
	dfa.setInputChar(nfa.inputChars); //设置DFA的输入字符集

	vector<STATE> tempStateSet = nfa.getEpsilon(nfa.states[0]);
	dfa.addNewState(tempStateSet); //添加DFA的第一个状态
	dfa.begin_state = dfa.states[0]; //设置DFA的第一个状态为起始状态

	//定义一些临时变量
	vector<STATE> stateList;
	int nowState = 0;
	stateList.push_back(dfa.states[0]);

	//定义DFA转换矩阵的临时行变量
	vector<STATE> tempRow;

	//计算DFA的转换矩阵
	while (nowState < stateList.size()){
		STATE curState = stateList[nowState++];
		if (curState.stateName == "$")continue;
		tempStateSet = dfa.split2Set(curState, nfa);
		for (int i = 0; i < dfa.inputChars.size(); i++){
			vector<STATE> convertSet = nfa.getConvertSet(tempStateSet, dfa.inputChars[i]);
			STATE newstate = mergeToState(convertSet);
			newstate.num = dfa.findState(newstate);
			if (newstate.num == -1){
				newstate.num = dfa.addNewState(newstate);
				stateList.push_back(newstate);
			}
			tempRow.push_back(newstate);
		}
		dfa.convertMatrix.push_back(tempRow);
		tempRow.clear();
	}

	//设置DFA的结束状态
	dfa.setEndStates(nfa);

	//标记DFA的空集
	dfa.setEmptyState();

	return true;
}

//将DFA最小化
bool DFA2MIN(DFA dfa, DFA &minDfa){
	//预备
	vector<STATE> newStates;
	for (int i = 0; i < dfa.states.size(); i++){
		if (dfa.states[i].stateName != "$")
			newStates.push_back(dfa.states[i]);
	}

	//采用形式语言与自动机一书所说的 极小化算法
	//1. 准备矩阵
	//规定 横坐标为S0 S1 ......    纵坐标为S1 S2 ......
	vector<vector<bool>> matrix;
	for (int i = 0; i < dfa.effectiveStateNum-1; i++){
		vector<bool> row;
		for (int j = 0; j < dfa.effectiveStateNum-1; j++){
			row.push_back(true);
		}
		matrix.push_back(row);
		row.clear();
	}

	//2. 针对所有的state1是F， state2不是F，做标记
	for (int row = 0; row < matrix.size(); row++){
		STATE xitem = newStates[row+1];
		for (int col = 0; col < row + 1; col++){
			STATE yitem = newStates[col];
			if (find(dfa.end_states, xitem) != find(dfa.end_states, yitem))
				matrix[row][col] = false;
		}
	}

	//3. 重复操作
	bool stop = false;
	while (!stop){
		stop = true;
		for (int row = 0; row < matrix.size(); row++){
			if (!stop)break;
			for (int col = 0; col < row + 1; col++){
				if (!stop)break;
				if (!matrix[row][col])
					continue;
				STATE xitem = newStates[row + 1];
				STATE yitem = newStates[col];
				
				STATE x_nextState, y_nextState;
				for (int i = 0; i < dfa.inputChars.size(); i++){
					x_nextState = dfa.getConvertState(xitem, dfa.inputChars[i]);
					y_nextState = dfa.getConvertState(yitem, dfa.inputChars[i]);
					if (x_nextState.stateName == "$" || y_nextState.stateName == "$"){
						matrix[row][col] = false;
						stop = false;
						break;
					}
					int x_index = getStateIndex(newStates, x_nextState), 
						y_index = getStateIndex(newStates, y_nextState);
					int max = x_index>y_index ? x_index : y_index;
					if (x_index == y_index)
						continue;
					y_index = x_index + y_index - max;
					x_index = max;

					if (!matrix[x_index-1][y_index]){
						matrix[row][col] = false;
						stop = false;
						break;
					}
					else continue;
				}
			}
		}
	}

	//4. 开始设置minDFA
	//4.1 计算状态个数
	vector<STATE> tempSet;
	vector<vector<STATE>> stateSet;
	for (int i = 0; i < newStates.size(); i++){
		tempSet.push_back(newStates[i]);
		stateSet.push_back(tempSet);
		tempSet.clear();
	}
	for (int row = 0; row < matrix.size(); row++){
		for (int col = 0; col < row + 1; col++){
			if (!matrix[row][col])
				continue;
			STATE x_state = newStates[row + 1];
			STATE y_state = newStates[col];
			mergeState(stateSet, x_state, y_state);
		}
	}

	//4.2 向minDfa添加状态
	for (int i = 0; i < stateSet.size(); i++){
		minDfa.addNewState(stateSet[i]);
	}
	//4.3 设置minDfa的字符集
	minDfa.inputChars = dfa.inputChars;

	//4.4 设置开始状态
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], dfa.begin_state)){
			minDfa.begin_state = minDfa.states[i];
			break;
		}
	}

	//4.5 设置结束状态
	for (int i = 0; i < stateSet.size(); i++){
		for (int j = 0; j < dfa.end_states.size(); j++){
			if (find(stateSet[i], dfa.end_states[j])){
				minDfa.end_states.push_back(minDfa.states[i]);
				break;
			}
		}
	}

	//4.6 设置转换矩阵
	for (int i = 0; i < stateSet.size(); i++){
		for (int j = 0; j < dfa.inputChars.size(); j++){
			vector<STATE> convertSet = dfa.getConvertSet(stateSet[i], dfa.inputChars[j]);
			int convertStateNum = getStateNum(stateSet, convertSet);
			STATE convertState = minDfa.states[convertStateNum];
			tempSet.push_back(convertState);
		}
		minDfa.convertMatrix.push_back(tempSet);
		tempSet.clear();
	}

	return true;
}

//把状态obj自己组成一个集合，将这个集合添加到stateSet中
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE obj){
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], obj)){
			return;
		}
	}
	vector<STATE> tempSet;
	tempSet.push_back(obj);
	stateSet.push_back(tempSet);
}

//返回集合SonSet在二维集合BogSet中的索引
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE lead, STATE obj){
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], lead)){
			stateSet[i].push_back(obj);
			break;
		}
	}
}

//判断obj是否是stateSet的子集
bool JudgeBelong(vector<STATE> stateSet, vector<STATE> obj){
	for (int i = 0; i < obj.size(); i++){
		if (!find(stateSet, obj[i]))
			return false;
	}
	return true;
}

//判断obj是否是stateSet的任一子项的子集,返回索引值
int getStateNum(vector<vector<STATE>> stateSet, vector<STATE> obj){
	for (int i = 0; i < stateSet.size(); i++){
		if (JudgeBelong(stateSet[i], obj))
			return i;
	}
	return -1;
}

//将二维集合stateSet中obj2所在的集合合并到obj1所在的集合，并删除obj2所在的集合
bool mergeState(vector<vector<STATE>> &stateSet, STATE obj1, STATE obj2){
	int obj1SetNum = -1, obj2SetNum = -1;
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], obj1))
			obj1SetNum = i;
		if (find(stateSet[i], obj2))
			obj2SetNum = i;
		if (obj1SetNum != -1 && obj2SetNum != -1)
			break;
	}

	if (obj1SetNum == -1 || obj2SetNum == -1)
		return false;
	if (obj1SetNum == obj2SetNum)
		return true;

	mergeVector(stateSet[obj1SetNum], stateSet[obj2SetNum]);
	
	vector<vector<STATE>>::iterator del2 = stateSet.begin();
	for (int i = 0; i < obj2SetNum; i++)
		del2++;
	stateSet.erase(del2);

	return true;
}