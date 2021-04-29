#include "Manager.h"
#include "Root\API.h"
string BinMapsFolder = "Bin";
string PackBin = "Bin/pack.bin";
vector<string> bug_maps;
extern vector<string> replacers;
extern vector<string> ignores;

void Manager()
{
	ReadSettings("Options.txt");
	Mhook_SetHook(&(PVOID&)CrFileA, NewCrFileA);
	//BreaksInit();
}

void ReadBugMaps(string map_list)
{
	string s;
	ifstream file(map_list);
	while (getline(file, s))
	{
		if(s != " " || s != "")
			bug_maps.push_back(s);
	}
	file.close();
}

void ReadReplacer(string replacer)
{
	string s;
	ifstream file(replacer);
	while (getline(file, s))
	{
		if (s.size() == 0)
			continue;
		if (s.size() >= 2 && s[0] == '/' && s[1] == '/')
			continue;
		if (s[0] == ' ')
			continue;
		replacers.push_back(s);
	}
	file.close();
}

void ReadIgnore(string ignore)
{
	string s;
	ifstream file(ignore);
	while (getline(file, s))
	{
		if (s != " " || s != "")
			ignores.push_back(s);
	}
	file.close();
}

void ReadSettings(string fname)
{
	string s;
	ifstream file(fname);
	while (getline(file, s))
	{
		if (s[0] == '/' && s[1] == '/')
		{
			//continue;
		}
		else if (int size = s.find("PackBin=") != string::npos)
		{
			string pack = s.erase(0, s.rfind("=") + 1);
			PackBin = pack;
		}
		else if (int size = s.find("BinMapsFolder=") != string::npos)
		{
			string bin = s.erase(0, s.rfind("=") + 1);
			BinMapsFolder = bin;
		}
		else if (int size = s.find("BugMaps=") != string::npos)
		{
			string maps = s.erase(0, s.rfind("=") + 1);
			ReadBugMaps(maps);
		}
		else if (int size = s.find("Replacer=") != string::npos)
		{
			string rep = s.erase(0, s.rfind("=") + 1);
			ReadReplacer(rep);
		}
		else if (int size = s.find("Ignore=") != string::npos)
		{
			string ign = s.erase(0, s.rfind("=") + 1);
			ReadIgnore(ign);
		}
	}
	file.close();
}