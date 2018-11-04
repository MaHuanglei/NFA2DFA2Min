#ifndef NFA_H
#define NFA_H

#include"stdafx.h"

/********************************************************************
	������NFA
	��Ԫʽ�� M = (S, ��, ��, s0, F)
	�洢��ʽ������洢
	��ֵ��ʽ����ȡ�ļ�
********************************************************************/

class NFA{
public:
	string nfaName;			//���ơ�����
	int epsilonNum = -1;		//������ĸ���е�����

	vector<STATE> states;			//S��״̬��
	vector<char> inputChars;		//�ƣ�������ĸ��
	vector<vector<vector<STATE>>> convertMatrix;		//�ģ�״̬ת������
	STATE begin_state;			//s0����̬
	vector<STATE> end_states;		//F����̬��

public:
	NFA(){ }
	~NFA();

	//��ȡ�ļ�����NFA��ֵ
	void MatrixReadG(string filename);

	//����״̬state��״̬���е�����
	int getStateNum(string state);

	//�õ�״̬state��epsilon��
	vector<STATE> getEpsilon(STATE stateNum);

	//�õ�״̬state�����ַ�inputChar֮���ת��״̬��
	vector<STATE> getConvertSet(STATE state, char inputChar);

	//�õ�״̬��stateSet�����ַ�inputChar֮���ת��״̬��
	vector<STATE> getConvertSet(vector<STATE> stateSet, char inputChar);

	//��ʾ���NFA
	void showNFAWithMatrix();
};

//��ȡ�ļ�����NFA��ֵ
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

	//��ȡ�Զ�������
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(line, "��");
	nfaName = strs1[1];
	strs1.clear();

	//��ȡ��ʽ
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());

	//��ȡ״̬��
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "��")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		tempState.stateName = strs1[i];
		if (tempState.stateName == "$")
			continue;
		tempState.num = i;
		states.push_back(tempState);
	}

	//��ȡ�����ַ���
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "��")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		if (strs1[i] != " "){
			inputChars.push_back(strs1[i][0]);
			if (strs1[i][0] == '@')
				epsilonNum = inputChars.size() - 1;
		}
	}


	//��ȡ��ʼ״̬
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	begin_state.stateName = split(line, "��")[1];
	begin_state.num = getStateNum(begin_state.stateName);

	//��ȡ����״̬��
	infile.getline(line, 200, '\n');
	fileCheck(infile.eof());
	strs1 = split(split(line, "��")[1], " ");
	for (int i = 0; i < strs1.size(); i++){
		tempState.stateName = strs1[i];
		tempState.num = getStateNum(tempState.stateName);
		end_states.push_back(tempState);
	}

	//��ȡת������
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

//����״̬state��״̬���е�����
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

//�õ�״̬state��epsilon��
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

//�õ�״̬state�����ַ�inputChar֮���ת��״̬��
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

//�õ�״̬��stateSet�����ַ�inputChar֮���ת��״̬��
vector<STATE> NFA::getConvertSet(vector<STATE> stateSet, char inputChar){
	vector<STATE> convertSet;
	for (int i = 0; i < stateSet.size(); i++){
		vector<STATE> tempSet = getConvertSet(stateSet[i], inputChar);
		mergeVector(convertSet, tempSet);
	}
	return convertSet;
}

//��ʾ���NFA
void NFA::showNFAWithMatrix(){
	//д���Զ�������
	cout << "NFA���ƣ�" << nfaName << endl;

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