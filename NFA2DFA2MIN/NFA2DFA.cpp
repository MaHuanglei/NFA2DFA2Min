#include"stdafx.h"
#include"NFA.h"
#include"DFA.h"

//��NFAת��ΪDFA
bool NFA2DFA(NFA nfa, DFA &dfa);

//��DFA��С��
bool DFA2MIN(DFA dfa, DFA &minDfa);

//��״̬obj�Լ����һ�����ϣ������������ӵ�stateSet��
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE obj);

//��obj��ӵ�lead���ڵļ�����
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE lead, STATE obj);

//���ؼ���SonSet�ڶ�ά����BogSet�е�����
int getStateNum(vector<vector<STATE>> BigSet, vector<STATE> SonSet);

//����ά����stateSet��obj2���ڵļ��Ϻϲ���obj1���ڵļ��ϣ���ɾ��obj2���ڵļ���
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

//��NFAת��ΪDFA
bool NFA2DFA(NFA nfa, DFA &dfa)
{
	dfa.setInputChar(nfa.inputChars); //����DFA�������ַ���

	vector<STATE> tempStateSet = nfa.getEpsilon(nfa.states[0]);
	dfa.addNewState(tempStateSet); //���DFA�ĵ�һ��״̬
	dfa.begin_state = dfa.states[0]; //����DFA�ĵ�һ��״̬Ϊ��ʼ״̬

	//����һЩ��ʱ����
	vector<STATE> stateList;
	int nowState = 0;
	stateList.push_back(dfa.states[0]);

	//����DFAת���������ʱ�б���
	vector<STATE> tempRow;

	//����DFA��ת������
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

	//����DFA�Ľ���״̬
	dfa.setEndStates(nfa);

	//���DFA�Ŀռ�
	dfa.setEmptyState();

	return true;
}

//��DFA��С��
bool DFA2MIN(DFA dfa, DFA &minDfa){
	//Ԥ��
	vector<STATE> newStates;
	for (int i = 0; i < dfa.states.size(); i++){
		if (dfa.states[i].stateName != "$")
			newStates.push_back(dfa.states[i]);
	}

	//������ʽ�������Զ���һ����˵�� ��С���㷨
	//1. ׼������
	//�涨 ������ΪS0 S1 ......    ������ΪS1 S2 ......
	vector<vector<bool>> matrix;
	for (int i = 0; i < dfa.effectiveStateNum-1; i++){
		vector<bool> row;
		for (int j = 0; j < dfa.effectiveStateNum-1; j++){
			row.push_back(true);
		}
		matrix.push_back(row);
		row.clear();
	}

	//2. ������е�state1��F�� state2����F�������
	for (int row = 0; row < matrix.size(); row++){
		STATE xitem = newStates[row+1];
		for (int col = 0; col < row + 1; col++){
			STATE yitem = newStates[col];
			if (find(dfa.end_states, xitem) != find(dfa.end_states, yitem))
				matrix[row][col] = false;
		}
	}

	//3. �ظ�����
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

	//4. ��ʼ����minDFA
	//4.1 ����״̬����
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

	//4.2 ��minDfa���״̬
	for (int i = 0; i < stateSet.size(); i++){
		minDfa.addNewState(stateSet[i]);
	}
	//4.3 ����minDfa���ַ���
	minDfa.inputChars = dfa.inputChars;

	//4.4 ���ÿ�ʼ״̬
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], dfa.begin_state)){
			minDfa.begin_state = minDfa.states[i];
			break;
		}
	}

	//4.5 ���ý���״̬
	for (int i = 0; i < stateSet.size(); i++){
		for (int j = 0; j < dfa.end_states.size(); j++){
			if (find(stateSet[i], dfa.end_states[j])){
				minDfa.end_states.push_back(minDfa.states[i]);
				break;
			}
		}
	}

	//4.6 ����ת������
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

//��״̬obj�Լ����һ�����ϣ������������ӵ�stateSet��
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

//���ؼ���SonSet�ڶ�ά����BogSet�е�����
void AddStateToSet(vector<vector<STATE>> &stateSet, STATE lead, STATE obj){
	for (int i = 0; i < stateSet.size(); i++){
		if (find(stateSet[i], lead)){
			stateSet[i].push_back(obj);
			break;
		}
	}
}

//�ж�obj�Ƿ���stateSet���Ӽ�
bool JudgeBelong(vector<STATE> stateSet, vector<STATE> obj){
	for (int i = 0; i < obj.size(); i++){
		if (!find(stateSet, obj[i]))
			return false;
	}
	return true;
}

//�ж�obj�Ƿ���stateSet����һ������Ӽ�,��������ֵ
int getStateNum(vector<vector<STATE>> stateSet, vector<STATE> obj){
	for (int i = 0; i < stateSet.size(); i++){
		if (JudgeBelong(stateSet[i], obj))
			return i;
	}
	return -1;
}

//����ά����stateSet��obj2���ڵļ��Ϻϲ���obj1���ڵļ��ϣ���ɾ��obj2���ڵļ���
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