#include "stdafx.h"
#include "datastore_patch.h"

bool DatastorePatch::load(const cqWCHAR* patchPath)
{
	IniFile inifile;
	IniFile_construct(&inifile);
	if (!IniFile_load(&inifile, patchPath))
		return false;

	const IniSection* section = IniFile_getSection(&inifile, 0);

	if (section == NULL)
		return false;

	int count = (int)IniFile_getPropertyNumInSection(&inifile, NULL);

	for (int i = 0; i < count; i++)
	{
		const IniProperty* property = IniSection_getProperty(section, i);

		DataNode* item = DataNode::instance();
		item->setId(NcString::stringWithCString(property->key));

		NcString* valueStr = NcString::stringWithCString(property->value);

		if (valueStr->countOfString(L","))
		{
			NcArray<NcString>* valueArray = valueStr->componentsSeparatedByCharacters(L",");
			item->setName(valueArray->objectAtIndex(0));
			item->setEngName(valueArray->objectAtIndex(1));
			item->setPinyin(valueArray->objectAtIndex(2));
		}
		else
		{
			item->setName(NcString::stringWithCString(property->value));
		}
		m_dataNodes->addObject(item);
	}

	IniFile_destruct(&inifile);
	return true;
}

DatastorePatch::DatastorePatch()
{
	m_dataNodes = NcArray<DataNode>::alloc();
}

DatastorePatch::~DatastorePatch()
{
	release(m_dataNodes);
}