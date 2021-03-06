#include<iostream>
#include<string>
#include<vector>
#include<map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <set>

using namespace std;

#define MAP_LOOP_1(ii,T) for(map<vector<int>, int>::iterator (ii)=(T).begin();(ii)!=(T).end();++(ii))
#define MAP_LOOP_2(jj,ii,T) 

int MIN_SUP = 0; 
int k = 0;
string input_transaction_file_path = "";
string output_file_path = "";

map<string, int> stringToNumber; // translation of transaction string to ineteger ID
map<int, string> numberToString; // translation of ineteger ID to transaction string
map<vector<int>, int> C; //candidate itemsets
map<vector<int>, int> F; //frequent itemsets
map<int, vector<int> > D; // database
set <int> vocab;
map<int, set<int> > invIndex;

void output_stringToNumber(); //for testing only
void output_numberToString(); //for testing only
void output_data(); //for testing only

void transactionsDB(); 
void get_data();
void initial_C();
void generate_C();
void generate_F();
//void removeInfreqTrsanctions();
void output(map<vector<int>, int>);
void outputResult(map<vector<int>, int>, bool);
void scan_data();
void prune();
bool isPreviousValueSame(vector<int>, vector<int>);
void list();
void scan();

int main(int argc, char *argv[])
{

	MIN_SUP = strtol(argv[1], NULL, 10);
	k = strtol(argv[2], NULL, 10);
	input_transaction_file_path = argv[3];
	output_file_path = argv[4];
	
	ofstream output_file;
	output_file.open(output_file_path.c_str());
	output_file.close();

	clock_t t1, t2;
	t1 = clock();
	cout << "Collecting the itemsets now..." << endl;
	transactionsDB();
	//output(D);
	//output_stringToNumber();
	//output_numberToString();
	C.clear();
	F.clear();
	cout << "Getting the data now..." << endl;
	get_data();
	list();
	//output_data();
	int loop = 1;
	bool isFirstOutput = true;

	while (true)
	{
		if(loop == 1)
		{
			cout << "Initial pass now..." << endl;
			initial_C();
			cout << "Generating frequent itemsets now..." << endl;
			generate_F();
			if (loop >= k) {
				outputResult(F, isFirstOutput);
				isFirstOutput = false;
			}	
		}
		else if(loop > 1){
			cout << "Generating the candidates now..." << endl;
			generate_C();
			if (C.size() == 0) break;

			cout << "Prunning the candidates now..." << endl;
			prune();
			if (C.size() == 0) break;

			cout << "Scanning the database now..." << endl;
			//scan_data();
			scan();
			cout <<  "Generating Frequent itemsets now..." << endl;
			generate_F();

			if (F.size() == 0) break;
			if (loop >= k) {
				outputResult(F, isFirstOutput);
				isFirstOutput = false;
			}
			//output(F);
		}
		loop++;
	}
	//cout << "Done!" << endl;
	t2 = clock();
	float diff((float)t2 - (float)t1);
	cout << "Clock time: "<< diff << endl;
	cout << "Minimum Support: " << MIN_SUP << " ------ K: "  << k << endl;

	cout << "1 : " << stringToNumber["A231WM2Z2JL0U3"] << endl;
	cout << "1 : " << stringToNumber["A5JLAU2ARJ0BO"] << endl;
	return 0;
}

//for testing only
void output_stringToNumber() {
	for (map<string, int>::iterator(ii) = (stringToNumber).begin(); (ii) != (stringToNumber).end(); (ii)++)
	{
		cout << ii->first << " == " << ii->second;
		cout << "\n";
	}
}

void output_numberToString() {
	for (map<int, string>::iterator(ii) = (numberToString).begin(); (ii) != (numberToString).end(); (ii)++)
	{
		cout << ii->first << " == " << ii->second;
		cout << "\n";
	}
	//cout << "= = =" << numberToString[644] << endl;
}

// core
void transactionsDB() {

	ifstream transactions(input_transaction_file_path.c_str()); //transactionDB  or test or test2 , (input_transaction_file_path).c_str()
	string str;
	string item;
	int count = 0;
	//vector<string> v;
	vector<int> itemSet;

	ofstream newTransactions;
	newTransactions.open("newTransactions.txt"); // translate original transactions to numbers

	while (!transactions.eof()) {
		str = transactions.get();
		if (str == " ") {
			// translate steps
			if (!stringToNumber[item]) {
				count++;
				stringToNumber[item] = count;
				numberToString[count] = item;
				vocab.insert(count);
				newTransactions << count << " ";
			}
			else {
				newTransactions << stringToNumber[item] << " ";
			}

			item = "";
		}
		else if (str == "\n") {

			// translate steps
			if (!stringToNumber[item]) {
				count++;
				stringToNumber[item] = count;
				numberToString[count] = item;
				vocab.insert(count);
				newTransactions << count << " -1\n";
			}
			else {
				newTransactions << stringToNumber[item] << " -1\n";
			}

			item = "";

		}
		else {
			item += str;
		}
	}
	//for (int s: vocab) {
	//	cout << s << endl;
	//}

	transactions.close();
	newTransactions.close();
}

void list() {
	for (int s : vocab) {
		invIndex[s];
	}
	vector<int> a;
	//set<int> location;
	for (map<int, vector<int> >::iterator(ii) = (D).begin(); (ii) != (D).end(); ++(ii)) {
		a.clear();
		a = ii->second;
		int aSize = a.size();
		//cout << location << endl;
		for (int j = 0; j < aSize; j++) {
			//Get the actual word in position j of doc i
			int w = a[j];
		    invIndex[w].insert(ii->first);
		}
	}
	//set<int> b;
	//location = 0;
	//for (map<int, set<int> >::iterator(ii) = (invIndex).begin(); (ii) != (invIndex).end(); ++(ii)) {
	//	b.clear();
	//	b = ii->second;
	//	location++;
	//	cout << "Doc ID: ";
	//	for (set<int>::iterator i = b.begin(); i != b.end(); i++) {
	//		int element = *i;
	//		cout << element << " ";
	//	}

	//	cout << "\n";
	//}
}

void get_data()
{

	ifstream fin;
	fin.open("newTransactions.txt");
	if (!fin)
	{
		cout << "Error\n";
		exit(0);
	}

	int n;
	vector<int> v;
	int countNum = 0;
	while (fin >> n)
	{
		if (n == -1)
		{
			v.push_back(n);
			if (v.size() > k) {
				D[countNum] = v;
				countNum++;
			}
			v.clear();
		}
		else {
			v.push_back(n);
		}
	}
	fin.close();
}

void output_data()
{
	cout << "\n";
	vector<int> v;
	for (map<int, vector<int> >::iterator(ii) = (D).begin(); (ii) != (D).end(); (ii)++)
	{
		v.clear();
		v = ii->second;
		cout << ii->first << ":  ";
		for (int i = 0; i < v.size(); ++i)
		{
			cout << v[i] << " ";
		}
		cout << "\n";

	}
}

void initial_C()
{
	int n = 0;
	vector<int> a, v;
	for (map<int, vector<int> >::iterator(iii) = (D).begin(); (iii) != (D).end(); ++(iii)) // original
	{
		a.clear();
		a = iii->second;
		for (int i = 0; i < a.size(); ++i) {
			n = a[i];
			v.clear();
			if (n == -1) {
				continue;
			}
			v.push_back(n);
			if (C.count(v) > 0) {
				C[v]++;
			}else {
				C[v] = 1;
			}
		}
	}
}


void outputResult(map<vector<int>, int> T, bool isFirstOutput)
{
	cout << "\n";
	vector<int> v;

	//string output_file_name = "";
	//stringstream s, ss;
	//s << k;
	//ss << MIN_SUP;
	//string k_value = s.str();
	//string sup_value = ss.str();
	//output_file_name = string("out_s=") + sup_value + string("_k=") + k_value + string("+");

	ofstream output_file;
	if (isFirstOutput) {
		 output_file.open(output_file_path.c_str());
	}
	else {
		output_file.open(output_file_path.c_str(), std::ios_base::app);
	}

	MAP_LOOP_1(ii, T)
	{
		v.clear();
		v = ii->first;
		for (int i = 0; i < v.size(); ++i)
		{
			//cout << v[i] << " ";
			output_file << numberToString[v[i]] << " ";
		}
		output_file << "(" << ii->second << ")\r\n";
		//cout << "(" << ii->second << ")\n";
	}
}

void output(map<vector<int>, int> T)
{
	cout << "\n";
	vector<int> v;
	MAP_LOOP_1(ii, T)
	{
		v.clear();
		v = ii->first;
		for (int i = 0; i < v.size(); ++i)
		{
			cout << v[i] << " ";
		}
		cout << "(" << ii->second << ")\n";
	}
}

void generate_C()
{
	//clean(C);
	C.clear();
	int count = 0;
	int total = F.size();
	MAP_LOOP_1(ii, F)
	{
		count++;
		//cout << " generate C: " << count << "/"<< total<< endl;
		// do not move a to here
		for(map<vector<int>, int>::iterator (jj)=(ii);(jj)!=(F).end();++(jj))
		{
			if (jj == ii) 
				continue;
			vector<int> a, b;
			a.clear();
			b.clear();
			a = ii->first;
			b = jj->first;
			if (isPreviousValueSame(a, b))
			{
				a.push_back(b.back());
				sort(a.begin(), a.end());
				C[a] = 0;
			}
		}
	}
}

bool isPreviousValueSame(vector<int> a, vector<int> b)
{
	bool isSame = true;
	for (int i = 0; i < a.size() - 1; ++i)
	{
		if (a[i] != b[i])
		{
			isSame
			 = false;
			break;
		}
	}
	return isSame;
}

void prune()
{
	vector<int> a, b;

	MAP_LOOP_1(ii, C)
	{
		a.clear();
		b.clear();

		a = ii->first;
		for (int i = 0; i < a.size(); i++)
		{
			b.clear();
			for (int j = 0; j < a.size(); ++j)
			{
				if (j == i)
					continue;
				b.push_back(a[j]);
			}
			if (F.find(b) == F.end())
			{
				ii->second = -1;
				break;
			}
		}
	}
	map<vector<int>, int> temp;
	temp.clear();
	MAP_LOOP_1(ii, C)
	{
		if (ii->second != -1)
		{
			temp[ii->first] = ii->second;
		}
	}
	C.clear();
	C = temp;
	temp.clear();
}

vector<int> compare(vector<int> a) {
	vector<int> temResult;

	map<int, vector<int> > inter;
	vector<int> tempVec;

	for (int i = 0; i < a.size(); ++i) { 
		set<int> tempSet = invIndex[a[i]];
		tempVec.clear();
		tempVec.insert(tempVec.cend(), tempSet.cbegin(), tempSet.cend());
		inter[i] = tempVec;
	}
	vector<int> aaa, bbb;
	aaa = inter[0];
	for (int c = 1; c < a.size(); ++c) {
		bbb = inter[c];
		int i = 0, j = 0;

		while (i != aaa.size() && j != bbb.size()) {
			if (aaa[i] == bbb[j]) {
				temResult.push_back(aaa[i]);
				i++;
				j++;
			}
			else if (aaa[i] < bbb[j]) {
				i++;
			}
			else {
				j++;
			}
		}
		aaa = temResult;
		temResult.clear();
	}
	return aaa;
}
void scan() {
	vector<int> a, b;
	vector<int> result;
	int location = 1;
	int foundCount = 0;
	for (map<vector<int>, int>::iterator(ii) = (C).begin(); (ii) != (C).end(); ++(ii)) //candidate keys
	{
		a.clear();
		a = ii->first;
		result = compare(a);
		if (result.size() >= MIN_SUP) {
			cout << "found: " << foundCount << endl;
			foundCount++;
			ii->second = result.size();
		}
		location++;
	}
}


void generate_F()
{
	F.clear();
	MAP_LOOP_1(ii, C)
	{
		if (ii->second >= MIN_SUP)
		{
			F[ii->first] = ii->second;
		}
	}
}
