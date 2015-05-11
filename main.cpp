#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <bson/bson.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include "sha256.h"
#include <sstream>

using namespace std;
namespace fs = boost::filesystem;

string test;

struct Fileinfo {
	string path;
	string hash;
	int size;
	char flag;
};

bool dirExists(const std::string& dirName_in)
{
DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
if (ftyp == INVALID_FILE_ATTRIBUTES)
return false; 

if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
return true; 

return false; 
}

void SaveBson (string filename, vector<Fileinfo> vec_finfo){
	bson::bob element;
	for (Fileinfo it : vec_finfo){
		stringstream result;
		string a;
		element.append("path", it.path);
		element.append("size", it.size);
		/*a = it.path;
		stringstream ss;
		int g = it.size;
		ss<<g;
		string str;
		ss>>str;
		res = a + str;
		*/		
		ifstream myfile;
		myfile.open(it.path, ios::binary);	
		result << myfile.rdbuf();
		a = result.str();
		SHA256 sha256;
		it.hash = sha256(a);
		element.append("hash", it.hash);
		cout << it.path << " /// " << it.size << " /// " << it.hash << endl;
	}
	bson::bo bo = element.obj();
	std::ofstream fbson( "./out.bson", std::ios::binary | std::ios::out | std::ios::trunc );
    fbson.clear();
    fbson.write(bo.objdata(), bo.objsize());
    fbson.close();
}

void get_dir_list(fs::directory_iterator iterator, std::vector<Fileinfo> * vec_finfo) {  
	Fileinfo finfo; 
	for (; iterator != fs::directory_iterator(); ++iterator)
	{
		if (fs::is_directory(iterator->status())) { 
			fs::directory_iterator sub_dir(iterator->path());
			get_dir_list(sub_dir, vec_finfo);

		}
		else 
		{
			finfo.path = iterator->path().string();
		    finfo.size = fs::file_size(iterator->path());
			finfo.hash = "NOT YET";
			finfo.flag = 'R';
			vec_finfo->push_back(finfo);
		}

	}
}

int main(){
	string path, dirpath;
	cout << "Enter path ->" << endl;
	vector<Fileinfo> vec_finfo;
	
	getline(cin, path);

	bool fg;
	fg = dirExists(path);
	if (fg == false) {
		cout << "NOT FOUND" << endl;
	}
	else {
		fs::directory_iterator home_dir(path);
		get_dir_list(home_dir, &vec_finfo);

		SaveBson ("project.bson", vec_finfo);

	    cin.clear();
		fflush(stdin);
	}
	cin.get();
	return 0;
}

