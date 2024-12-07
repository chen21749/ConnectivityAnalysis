#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <algorithm>
#include <map>

using namespace std;

int kf = 1;

//��ʼ��ͼ
int **initMap(map<int, vector<int>> pointsource) {

	int** map = new int*[20000];
	//��0
	for (int i = 0; i < 20000; i++) {

		map[i] = new int[20000];

		for (int j = 0; j < 20000; j++) {
			map[i][j] = 0;
		}
	}

	for (auto it = pointsource.begin(); it != pointsource.end(); ++it) {
		//cout << it->first << endl;
		vector<int> value = vector<int>(it->second);
		for (auto child = value.begin(); child != value.end(); ++child) {
			map[it->first][*child] = 1;
		}

	} 
	return map;
}


bool sort_token(vector<int>& s1, vector<int>& s2)
{
	return s1.size() < s2.size();
}

//�ж�V2�Ƿ���V1�Ӽ� V1����V2
bool isSubset(vector<int> v1, vector<int> v2) {
	int i = 0, j = 0;
	int m = v1.size();
	int n = v2.size();
	if (m < n) {
		return 0;
	}
	//sort(v1.begin(), v1.end());//��������
	//sort(v2.begin(), v2.end());
	while (i < n && j < m) {
		if (v1[j] < v2[i]) {
			j++;
		}
		else if (v1[j] == v2[i]) {
			j++;
			i++;
		}
		else if (v1[j] > v2[i]) {
			return 0;
		}
	}
	if (i < n) {
		return 0;
	}
	else {
		return 1;
	}
}


//Ѱ���ڵ�
void findParent(int** map, int item, vector<int>& vecResult) {//���ӽڵ㸸�ڵ�
	for (int i = 0; i < 20000; i++) {
		if (map[i][item] == 1) {
			vecResult.push_back(i);
		}
	}
}

//��ȡ��ͨ�Ե�
void getConnectivity(int** map, int start , vector<int>& vecResult) {
	bool isEnd = true;
	for (int i = 0; i < 20000; i++)
	{
		if (map[start][i] == 1) {
			isEnd = false;
			int parent = i;
			//cout << parent << ",";
			vecResult.push_back(parent);
			getConnectivity(map, parent, vecResult);
		}
    }
	if (isEnd) {
		vecResult.push_back(-1);//������־
		//cout << "����:" << kf <<endl;
		kf++;
		return;
	}
}

//��ͨ��ֵ������
vector<vector<int>> getConnectivityValue(int** map, int start) {
	vector<int> vecResult;
	getConnectivity(map, start, vecResult);//��ȡ��ͨ�Խڵ�;

	//printf
	cout << "�ݹ���: ";
	for (int i = 0; i < vecResult.size(); i++) {
		cout << vecResult[i] << ",";
	}
	cout << endl;
	cout << "---------------------------------------------------------------" << endl;


	vector<vector<int>> vecOutPutResult;
	if (vecResult.size() > 1)//��Ϊ��
	{
		vector<vector<int>> vecSlitResult;
		vector<int> branchResult;
		//�ָ�
		for (int i = 0; i < vecResult.size(); i++) {
			branchResult.push_back(vecResult[i]);
			if (vecResult[i] == -1) {
				branchResult.pop_back();
				vecSlitResult.push_back(branchResult);
				branchResult.clear();
			}
		}

		//printf
		cout << "�ָ���: " << endl;
		for (int i = 0; i < vecSlitResult.size(); i++) {
			vector<int> child = vecSlitResult[i];
			for (int j = 0; j < child.size(); j++) {
				cout << child[j]<<",";
			}
			cout << endl;
		}



		//step1:ȥ��ɾ��(����ͬ�Ĺ�������ɾ��)
		vector<vector<int>> vecSplitResultUnique;
		for (int i = 0; i < vecSlitResult.size(); i++) {
			bool isInsert = true; //Ĭ�϶�����
			vector<int> curItem = vecSlitResult[i];
			for (int j = 0; j < vecSplitResultUnique.size(); j++) {
				if (curItem == vecSplitResultUnique[j]) {
					isInsert = false;//������
					break;
				}
			}
			if (isInsert) {
				vecSplitResultUnique.push_back(curItem);
			}
		}


		//printf
		cout << "ȥ�ؽ��: " << endl;
		for (int i = 0; i < vecSplitResultUnique.size(); i++) {
			vector<int> child = vecSplitResultUnique[i];
			for (int j = 0; j < child.size(); j++) {
				cout << child[j] << ",";
			}
			cout << endl;
		}




		//ȥ����������
		sort(vecSplitResultUnique.begin(), vecSplitResultUnique.end(), sort_token);


		//printf
		cout << "ȥ��������: " << endl;
		for (int i = 0; i < vecSplitResultUnique.size(); i++) {
			vector<int> child = vecSplitResultUnique[i];
			for (int j = 0; j < child.size(); j++) {
				cout << child[j] << ",";
			}
			cout << endl;
		}


		//step2:��ȡ;����
		vector<int> vecPoint = vecResult;
		sort(vecPoint.begin(), vecPoint.end(), [](int a, int b) { return a > b; });
		vector<int>::iterator unique_item = unique(vecPoint.begin(), vecPoint.end());
		vecPoint.erase(unique_item, vecPoint.end());
		vecPoint.pop_back();
		vecPoint.push_back(start);

		//printf
		cout << ";����: " << endl;
		for (int i = 0; i < vecPoint.size(); i++) {
			cout << vecPoint[i] << ",";
			
		}
		cout << "------------------------------------------" << endl;



		//step3:��ÿ�����ߵĸ��ڵ�
		vector<vector<int>> vecParentResult;
		for (int i = 0; i < vecSplitResultUnique.size(); i++) {
			vector<int> currentParent;
			findParent(map, vecSplitResultUnique[i][0], currentParent);

			vector<int> curPath = vecSplitResultUnique[i];

			for (int j = 0; j < currentParent.size(); j++) {
				bool isInsert = false;//Ĭ�ϲ���
				for (int k = 0; k < vecPoint.size(); k++) {
					if (currentParent[j] == vecPoint[k]) {//���ڵ���������;����
						isInsert = true;
					}
				}
				if (isInsert) {
					curPath.insert(begin(curPath), currentParent[j]);
					vecParentResult.push_back(curPath);
					curPath = vecSplitResultUnique[i];//����
				}
			}
		}


		//printf
		cout << "��ÿ�����ߵĸ��ڵ�: " << endl;
		for (int i = 0; i < vecParentResult.size(); i++) {
			vector<int> child = vecParentResult[i];
			for (int j = 0; j < child.size(); j++) {
				cout << child[j] << ",";
			}
			cout << endl;
		}

		cout << "------------------------------------------" << endl;

		//�ٴ�����
		sort(vecParentResult.begin(), vecParentResult.end(), sort_token);


		//printf
		cout << "��ÿ�����ߵĸ��ڵ�����: " << endl;
		for (int i = 0; i < vecParentResult.size(); i++) {
			vector<int> child = vecParentResult[i];
			for (int j = 0; j < child.size(); j++) {
				cout << child[j] << ",";
			}
			cout << endl;
		}

		cout << "------------------------------------------" << endl;


	/*	cout << "_____________________" << endl;
		for (int i = 0; i < vecParentResult.size(); i++) {

			for (int j = 0; j < vecParentResult[i].size(); j++) {
				cout << vecParentResult[i][j] << ", ";
			}
			cout << endl;
		}*/


		//step4:�޳�����·��
		for (int i = 0; i < vecParentResult.size(); i++) {
			bool isInsert = true; //Ĭ�϶�����
			vector<int> curItem = vecParentResult[i];
			for (int j = i + 1; j < vecParentResult.size(); j++) {
				if (isSubset(vecParentResult[j], curItem)) {
					isInsert = false;//������
					break;
				}
			}
			if (isInsert) {
				vecOutPutResult.push_back(curItem);
			}
		}


		cout << "_____________________" << endl;
		for (int i = 0; i < vecOutPutResult.size(); i++) {

			for (int j = 0; j < vecOutPutResult[i].size(); j++) {
				cout << vecOutPutResult[i][j] << ", ";
			}
			cout << endl;
		}

		return vecOutPutResult;
	}
	return vecOutPutResult;
}



