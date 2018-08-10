#pragma once

#include "data_node.h"

class ReadmeTxt :public NcObjectCpp
{
public:
	static ReadmeTxt* globleInstance();

	void readFile(NcString* path);

	NcString* dataVersionChs() { return m_dataVersionChs; }
	NcString* dataVersionEng() { return m_dataVersionEng; }

protected:
	ReadmeTxt();
	~ReadmeTxt();

private:
	NcString* m_dataVersionChs;
	NcString* m_dataVersionEng;
};
