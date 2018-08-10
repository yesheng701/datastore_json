#include "stdafx.h"
#include "datastore_util.h"

NcString* DatastoreUtil::currentDirectoryName(NcString* currentPath)
{
	return currentPath->substringFrom(currentPath->locationOfCharacterBackwards('/') + 1);
}

NcString* DatastoreUtil::currentFileName(NcString* fileNamePath)
{
	return currentDirectoryName(fileNamePath);
}

NcString* DatastoreUtil::parentDirectoryName(NcString* directoryName)
{
	return directoryName->substringTo(directoryName->locationOfCharacterBackwards('/'));
}

int DatastoreUtil::versionNumberFromString(NcString* str)
{
	int location = str->locationOfCharacterBackwards('/');
	location = cq_max(location, str->locationOfCharacterBackwards('\\'));

	return str->substringFrom(location + 1)->toInt();
}
