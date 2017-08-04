#include "ppmReader.h"

using namespace std;

ppmReader::ppmReader(string path) {
	ifstream file(path, ios::binary);
	string line;
	bool flag = true;


	if (!file.is_open()) {
		cout << "File opened failed!" << endl;
	}
	else {
		// ����ͷ
		file >> line;
		type = line;
		// ȥ��ע��һ��Ķ��� ���Ҷ��볤��
		do {
			file >> line;
			int index = line.find_first_not_of(' ', 0);
			if (line[index] != '#') {
				sscanf(line.c_str(), "%d %d", &(size.first), &(size.second));
				flag = false;
			}
		} while (flag);
		file >> line;
		sscanf(line.c_str(), "%d", &range);

		// Ȼ��������ε�����
		unsigned char r, g, b;

		for (int i = 0; i < size.first * size.second; ++i) {
			file >> r >> g >> b;

			data.push_back(r);
			data.push_back(g);
			data.push_back(b);
		}
	}

}