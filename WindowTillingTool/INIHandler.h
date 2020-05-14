#pragma once
#include <string>
#include <list>
#include <iostream>
#include <fstream>

using namespace std;

#define VALUE_NOTFOUND_STRING "key=none&value=none"

class INIItem {
public:
	INIItem(string key, string value);
	string getKey();
	string getValue();

private:
	string mKey;
	string mValue;
};

typedef list<INIItem>::iterator ITEMITR;

class INISection {
public:
	INISection(string name);

	string getSectionName();

	void addItem(INIItem);
	void addItem(string key, string value);
	string getValue(string key);
	string getKey(int index);
	string getValue(int index);

	ITEMITR getItemItr();
	bool isItemItrEnd(ITEMITR itr);

private:
	string mSectionName;
	list<INIItem> mItems;
};

typedef list<INISection>::iterator SECTIONITR;

class INIHandler
{
public:
	void praseFile(string filename);
	void appendFile(string filename);
	void overwiteFile(string filename);

	void addSection(string sectionName);
	string getSectionName(int iSection);
	INISection* getSection(string sectionName);
	INISection* getSection(int iSection);
	list<INISection>::iterator getSectionItr();
	bool isSectionItrEnd(list<INISection>::iterator itr);

	void addItem(string section, INIItem item);
	void addItem(int iSection, INIItem item);
	void addItem(string section, string key, string value);
	void addItem(int iSection, string key, string value);
	string getVaule(string section, string key);
	string getVaule(int iSection, string key);
	list<INIItem>::iterator getItemsItr(string section);
	bool isItemsItrEnd(string section, list<INIItem>::iterator itr);	
	list<INIItem>::iterator getItemsItr(int iSection);
	bool isItemsItrEnd(int iSection, list<INIItem>::iterator itr);

	void clearAllData();

private:
	list<INISection> mSections;
};

