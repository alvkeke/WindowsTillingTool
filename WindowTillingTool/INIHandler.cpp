#include "INIHandler.h"


INIItem::INIItem(string key, string value)
{
	if (key.empty())
		mKey = "none";
	else
		mKey = key;

	mValue = value;
}

string INIItem::getKey()
{
	return mKey;
}

string INIItem::getValue()
{
	return mValue;
}

INISection::INISection(string name)
{
	mSectionName = name;
}

string INISection::getSectionName()
{
	return mSectionName;
}

void INISection::addItem(INIItem item)
{
	mItems.push_back(item);
}

void INISection::addItem(string key, string value)
{
	// 需不需要做存在判断？
	if (!key.empty())
		mItems.push_back(*new INIItem(key, value));
}

string INISection::getValue(string key)
{
	ITEMITR itr;
	for (itr = mItems.begin(); itr != mItems.end(); itr++)
	{
		if (itr->getKey() == key)
		{
			return itr->getValue();
		}
	}

	return "";
}

string INISection::getKey(int index)
{
	if (mItems.size() > index)
	{
		ITEMITR itr = mItems.begin();
		advance(itr, index);
		return itr->getKey();
	}
	return "";
}

string INISection::getValue(int index)
{
	if (mItems.size() > index)
	{
		ITEMITR itr = mItems.begin();
		advance(itr, index);
		return itr->getValue();
	}
	return VALUE_NOTFOUND_STRING;
}

ITEMITR INISection::getItemItr()
{
	return mItems.begin();
}

bool INISection::isItemItrEnd(ITEMITR itr)
{
	return itr == mItems.end();
}

void INIHandler::praseFile(string filename)
{
	ifstream iniIn;
	string sline;

	string section;
	string key;
	string value;

	int pos;
	SECTIONITR itr;

	iniIn.open(filename, ios::in);

	while (getline(iniIn, sline))
	{
		if (sline[0] == ';')continue;	//注释行
		// Section行
		if (sline[0] == '[')
		{
			int posend = sline.find(']');
			if (posend != string::npos)
			{
				posend--;
				section = sline.substr(1, posend);


				while (section[0] == ' ')section = section.substr(1);
				while (section[section.size() - 1] == ' ') section = section.substr(0, section.size() - 1);
				addSection(section);
				cout << endl << "[" << section << "]" << endl;
			}
			continue;
		}

		// Item行
		if ((pos= sline.find('=')) != string::npos)
		{
			if (section.empty())
			{
				//cout << "current section is empty" << endl;
				continue;
			}

			pos++;
			value = sline.substr(pos);
			pos--;
			key = sline.substr(0, pos);
			
			if (key.empty()) continue;	// 键值无论如何都不能为空

			// 清除两端无用空格
			if (!value.empty())
			{
				while (value[0] == ' ')value = value.substr(1);
				while (value[value.size() - 1] == ' ') value = value.substr(0, value.size() - 1);
			}

			while (key[0] == ' ')key = key.substr(1);
			while (key[key.size() - 1] == ' ') key = key.substr(0, key.size() - 1);
			
			addItem(section, key, value);

			cout << "|" << key << "|" << "\t";
			cout << "|" << value << "|" << endl;
		}	

	}
	iniIn.close();
}

void INIHandler::appendFile(string filename)
{
	ofstream iniOut;
	iniOut.open(filename, ios::app);
	SECTIONITR secIt;
	ITEMITR it;

	for (secIt = getSectionItr(); !isSectionItrEnd(secIt); secIt++)
	{
		iniOut << '[' << secIt->getSectionName() << ']' << endl;
		for (it = secIt->getItemItr(); !secIt->isItemItrEnd(it); it++)
		{
			iniOut << it->getKey() << '=' << it->getValue() << endl;
		}
		iniOut << endl;
	}

	iniOut.close();
}

void INIHandler::overwiteFile(string filename)
{
	ofstream iniOut;
	iniOut.open(filename);
	SECTIONITR secIt;
	ITEMITR it;

	for (secIt = getSectionItr(); !isSectionItrEnd(secIt); secIt++)
	{
		iniOut << '[' << secIt->getSectionName() << ']' << endl;
		for (it = secIt->getItemItr(); !secIt->isItemItrEnd(it); it++)
		{
			iniOut << it->getKey() << '=' << it->getValue() << endl;
		}
		iniOut << endl;
	}

	iniOut.close();
}

void INIHandler::addSection(string sectionName)
{
	for (SECTIONITR itr = mSections.begin(); itr != mSections.end(); itr++)
	{
		// 已存在，不添加
		if (sectionName == itr->getSectionName()) return;
	}
	mSections.push_back(*new INISection(sectionName));
}

string INIHandler::getSectionName(int iSection)
{
	SECTIONITR itr;
	if (mSections.size() <= iSection) return NULL;
	itr = mSections.begin();
	advance(itr, iSection);
	return itr->getSectionName();
}

INISection* INIHandler::getSection(string sectionName)
{
	SECTIONITR itr;
	for (itr = mSections.begin(); itr != mSections.end(); itr++)
	{
		if (itr->getSectionName() == sectionName)
		{
			return &(*itr);
		}
	}
	return nullptr;
}

INISection* INIHandler::getSection(int iSection)
{
	SECTIONITR itr;
	if (mSections.size() <= iSection) return nullptr;
	itr = mSections.begin();
	advance(itr, iSection);
	return &(*itr);
}

list<INISection>::iterator INIHandler::getSectionItr()
{
	return mSections.begin();
}

bool INIHandler::isSectionItrEnd(list<INISection>::iterator itr)
{
	return itr == mSections.end();
}

void INIHandler::addItem(string section, INIItem item)
{
	INISection* sec = getSection(section);
	sec->addItem(item);
}

void INIHandler::addItem(int iSection, INIItem item)
{
	INISection* sec = getSection(iSection);
	sec->addItem(item);
}

void INIHandler::addItem(string section, string key, string value)
{
	INISection* sec = getSection(section);
	if (!sec) {
		addSection(section);
	}
	sec = getSection(section);
	if (sec)sec->addItem(key, value);
}

void INIHandler::addItem(int iSection, string key, string value)
{
	INISection* sec = getSection(iSection);
	sec->addItem(key, value);
}

string INIHandler::getVaule(string section, string key)
{
	INISection* sec = getSection(section);
	return sec->getValue(key);
}

string INIHandler::getVaule(int iSection, string key)
{
	INISection* sec = getSection(iSection);
	return sec->getValue(key);
}

list<INIItem>::iterator INIHandler::getItemsItr(string section)
{
	INISection* sec = getSection(section);
	return sec->getItemItr();
}

bool INIHandler::isItemsItrEnd(string section, list<INIItem>::iterator itr)
{
	INISection* sec = getSection(section);
	return sec->isItemItrEnd(itr);
}

list<INIItem>::iterator INIHandler::getItemsItr(int iSection)
{
	INISection* sec = getSection(iSection);
	return sec->getItemItr();
}

bool INIHandler::isItemsItrEnd(int iSection, list<INIItem>::iterator itr)
{
	INISection* sec = getSection(iSection);
	return sec->isItemItrEnd(itr);
}

void INIHandler::clearAllData()
{
	mSections.clear();
}
