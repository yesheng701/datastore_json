#include "stdafx.h"
#include "readme_txt.h"

ReadmeTxt* g_instance = NULL;

ReadmeTxt::ReadmeTxt()
{

}

ReadmeTxt::~ReadmeTxt()
{
	release(m_dataVersionChs);
	release(m_dataVersionEng);
}

void ReadmeTxt::readFile(NcString* path)
{
	autoreleasepool
	{
		release(m_dataVersionChs);
		release(m_dataVersionEng);
		m_dataVersionChs = NULL;
		m_dataVersionEng = NULL;

		size_t size;
		cqWCHAR* content = Util_readTextFileIntoBufferW(path->cstr(), &size);
		NcString* contentString = NcString::stringWithCString(content);
		Util_freeFileInBuffer(content);
		NcArray<NcString>* contentItem = contentString->componentsSeparatedByCharacters(L"\r\n");

		if (contentItem->count() > 3)
		{
			m_dataVersionChs = NcString::allocWithString(contentItem->objectAtIndex(0));
			m_dataVersionEng = NcString::allocWithString(contentItem->objectAtIndex(1));			
		}
	}
}

static void _cleanupReadmeTxt(void*)
{
	release(g_instance);
}

ReadmeTxt* ReadmeTxt::globleInstance()
{
	static volatile long lock = 0;
	if (g_instance == NULL)
	{
		spinlock(&lock)
		{
			if (g_instance == NULL)
			{
				cq_registerCleanupFunction(_cleanupReadmeTxt, NULL);
				g_instance = NcNew(ReadmeTxt);
			}
		}
	}

	return g_instance;
}
