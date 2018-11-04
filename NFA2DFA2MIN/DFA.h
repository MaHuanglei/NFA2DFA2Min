#ifndef DFA_H
#define DFA_H

#include"stdafx.h"

/********************************************************************
	������DFA
	��Ԫʽ�� M = (S, ��, ��, s0, F)
	�洢��ʽ������洢
	��ֵ��ʽ����ȡ�ļ�	
********************************************************************/
class DFA{
public:
	string dfaName;			//���ơ�����
	STATE EmptyState;			//�ռ�״̬
	int effectiveStateNum;		//��Ч״̬����

	vector<STATE> states;		//S��״̬��
	vector<char> inputChars;	//�ƣ�������ĸ��
	vector<vector<STATE>> convertMatrix;	//�ģ�״̬ת������
	STATE begin_state;		//s0����̬
	vector<STATE> end_states;	//F����̬��
public:
	DFA(){ }
	~DFA(){ };

	//�����Զ�������
	void setDfaName(string name){ dfaName = name; }

	//�����ַ���
	void setInputChar(vector<char> chars);

	//������̬��
	void setEndStates(NFA nfa);

	//���ÿռ�״̬
	void setEmptyState();

	//����״̬����
	void resetStateName(string leader = "S");

	//��״̬newState��ӵ�DFA
	int addNewState(STATE newState);

	//����״̬��ve,����ϲ�Ϊһ��״̬����ӵ�DFA
	int addNewState(vector<STATE> ve);

	//����nfa��״̬state�����һ��״̬��
	vector<STATE> split2Set(STATE state, NFA nfa);

	//����״̬obj��dfa.states�е�����
	int findState(STATE obj);

	//�õ�״̬state�����ַ�inputChar֮���ת��״̬
	STATE DFA::getConvertState(STATE state, char inputChar);

	//�õ�״̬��stateSet�����ַ�inputChar֮���ת��״̬��
	vector<STATE> getConvertSet(vector<STATE> stateSet, char inputChar);

	//��DFA�Ծ���ʽд���ļ�
	void writeDFAtoFileWithMatrix(string filename);

	//�Ծ���ʽ��ʾDFA
	void showDFAWithMatrix();
};

//�����ַ���
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

	//�޸�״̬������
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		sprintf_s(numToStr, "%d", states[i].num);
		states[i].stateName = leader + numToStr;
	}

	//�޸���ʼ״̬����
	begin_state.stateName = states[begin_state.num].stateName;

	//�޸Ľ���״̬������
	for (int i = 0; i < end_states.size(); i++){
		end_states[i].stateName = states[end_states[i].num].stateName;
	}


	//�޸�ת������
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

	//д���Զ�������
	wfile << "DFA���ƣ�" << dfaName << endl;

	//д��洢��ʽ
	wfile << "�洢��ʽ��Matrix" << endl;
	
	//д��״̬��
	wfile << "����״̬��S��";
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		wfile << states[i].stateName << " ";
	}
	wfile << "$" << endl;

	//д������ַ���
	wfile << "��ĸ��ƣ�";
	for (int i = 0; i < inputChars.size(); i++){
		wfile << inputChars[i] << " ";
	}
	wfile << endl;

	//д�뿪ʼ״̬
	wfile << "��̬s0��";
	wfile << begin_state.stateName << endl;

	//д�����״̬
	wfile << "��̬��F��";
	for (int i = 0; i < end_states.size(); i++){
		wfile << end_states[i].stateName << " ";
	}
	wfile << endl;

	//д��״̬ת������
	char str[100];
	sprintf_s(str, "%s%d%c%d%s", "״̬ת������(", states.size(), '*', inputChars.size(), ")��");
	wfile << str << endl;
	for (int i = 0; i < convertMatrix.size(); i++){
		for (int j = 0; j < convertMatrix[i].size(); j++){
			wfile << convertMatrix[i][j].stateName << " ";
		}
		wfile << endl;
	}
	wfile << endl;

	//д���ļ�˵��
	wfile << "˵����" << endl
		<< "1. $����ռ�" << endl
		<< endl;


	wfile.close();
}

void DFA::showDFAWithMatrix(){
	//д���Զ�������
	cout << "DFA���ƣ�" << dfaName << endl;

	//д��洢��ʽ
	cout << "�洢��ʽ��Matrix" << endl;

	//д��״̬��
	cout << "����״̬��S��";
	for (int i = 0; i < states.size(); i++){
		if (states[i].stateName == "$")continue;
		cout << states[i].stateName << " ";
	}
	cout << "$(�ռ�)" << endl;

	//д������ַ���
	cout << "��ĸ��ƣ�";
	for (int i = 0; i < inputChars.size(); i++){
		cout << inputChars[i] << " ";
	}
	cout << endl;

	//д�뿪ʼ״̬
	cout << "��̬s0��";
	cout << begin_state.stateName << endl;

	//д�����״̬
	cout << "��̬��F��";
	for (int i = 0; i < end_states.size(); i++){
		cout << end_states[i].stateName << " ";
	}
	cout << endl;

	//д��״̬ת������
	char str[100];
	sprintf_s(str, "%s%d%c%d%s", "״̬ת������(", states.size(), '*', inputChars.size(), ")��");
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