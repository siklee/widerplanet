#include <iostream>
#include <fstream>
#include "string.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <time.h>

using namespace std;

template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

int main()
{

clock_t begin, end;
    begin = clock();  


	// input file의 형식 : name \t purl \t iurl \t c1 \t c2 \t c3 \t c4
	ifstream file("input");

	// checkstr file은 걸러내고자 하는 단어들의 모음집이다. 이는 사람이 단어를 추가해 주어야 한다.
	ifstream checkfile("checkstr");

	// 정상상품들을 저장하는 파일
	ofstream out("safe");

	// checkstr에 있는 단어에 걸러진 상품들을 저장 (비정상 상품)들
	ofstream fout("unsafe");

	string line; // input file에서 한 줄 읽을때 사용할 string 변수
	string checkstr;

	char delimit_string[] = "\t, /[]<>()-#|\\+_{}&?';:!~@$%^&*"; // 단어 추출할 때 사용할 구분자

	unordered_map<string, int> map; //hash를 사용해 검색속도 보장


	while (checkfile >> checkstr)
		map.insert(pair<string, int>(checkstr, 1)); // hash에 걸러낼 단어모음 추가

	checkfile.close();
	char li[500];
	while (file.getline(li, sizeof(li)))
	{
		if(li == NULL)
			break;

		vector<string> vec;
		line = li;
		vec = split(line, '\t');

		// vec[0]  vec[1]  vec[2]   vec[3]   vec[4]   vec[5]   vec[6]
		// 상품명    purl    iurl     c1       c2        c3      c4

		char *p_token = NULL;
		p_token = strtok(li, delimit_string);
		while (p_token != NULL){

			if (map.find(p_token) != map.end())	// 상품명과 카테고리에 걸러내고자 하는 단어가 있는 경우를 처리
			{
				fout<<vec[0]<<"\t"<<vec[1]<<"\t"<<vec[2]<<"\t";

				for (int i = 3; i < vec.size(); i++)     
					fout << vec[i] << "\t";
				fout << endl;

				break;
			}

			p_token = strtok(NULL, delimit_string);
		}

		if (p_token == NULL)	// 상품명과 카테고리에 걸러내고자 하는 단어가 없는 경우를 처리
		{
			out<<vec[0]<<"\t"<<vec[1]<<"\t"<<vec[2]<<"\t";

			for (int i = 3; i < vec.size(); i++)
				out << vec[i] << "\t";
			out << endl;
		}
	}
	
	out.close();
	fout.close();
	file.close();

end = clock();          // 시간설정
    cout<<"수행시간 : "<<((end-begin+4000000)/CLOCKS_PER_SEC)<<endl;


}
