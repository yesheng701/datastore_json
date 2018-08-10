#include "stdafx.h"
#include "store_hashmap.h"
#include "base/nkvd/nkvd.h"
#include "base/nkvd/nkvd_differ.h"
#include "md5.h"

StoreHashmap* g_instance = NULL;

StoreHashmap::StoreHashmap()
{
	m_guidMap = NcHashmap<NcString, NcString>::alloc();
}

NcString* StoreHashmap::guidByFilePath(NcString* filePath)
{
	return m_guidMap->objectWithKey(filePath);
}

StoreHashmap::~StoreHashmap()
{
	release(m_guidMap);
}

static void _calculateMd5(FileObject* file, Md5Digest* digest)
{
	size_t length;
	uint8 fileData[200];
	MD5Context context;
	MD5Context_init(&context);

	length = File_getLength(file);

	if (length <= 200)
		File_read(file, fileData, length);
	else
	{
		File_seek(file, FileSeek_Set, 0);
		File_read(file, fileData, 100);
		File_seek(file, FileSeek_Set, length - 100);
		File_read(file, fileData + 100, 100);
	}

	MD5Context_update(&context, fileData, cq_min(200, length));
	MD5Context_getDigest(&context, digest);
}

static bool _mapDataUtil_getFileSizeAndMd5(const cqWCHAR* filename, size_t* size, char md5[16])
{
	FileObject file;
	File_construct(&file);
	if (!File_openSimpleFile(&file, filename, File_OpenForRead))
	{
		File_destruct(&file);
		return false;
	}

	*size = File_getLength(&file);

	_calculateMd5(&file, (Md5Digest*)md5);
	File_destruct(&file);

	return true;
}

void StoreHashmap::addFilesGuidFromDir(NcString* path)
{
	autoreleasepool
	{
		NcArray<NcString>* files = FileSys_findFilesInPath(path, true);

		for (int i = 0; i < files->count(); i++)
		{
			NcString* fileName = files->objectAtIndex(i);

			if (fileName->endsWith(L".nkvd") || fileName->endsWith(L".nkvds"))
			{
				Nkvd* nkvd = Nkvd::instanceWithFile(fileName, NkvdOpenFlag_read);
				if (nkvd != NULL)
				{
					cqCHAR text[38];
					nkvd->guid().toText(text);
					m_guidMap->addObjectForKey(fileName, NcString::stringWithAnsiCString(text));
				}
				nkvd->close();
			}
			else if (fileName->endsWith(L".dat"))
			{
				size_t size;
				char md5[16];
				cqWCHAR changeStr[33];
				_mapDataUtil_getFileSizeAndMd5(fileName->cstr(), &size, md5);
				Md5Digest_toString((Md5Digest*)md5, changeStr);
				m_guidMap->addObjectForKey(fileName, NcString::stringWithCString(changeStr));
			}
		}
	}
}

static void _cleanupStoreHashmap(void*)
{
	release(g_instance);
}

StoreHashmap* StoreHashmap::globleInstance()
{
	static volatile long lock = 0;
	if (g_instance == NULL)
	{
		spinlock(&lock)
		{
			if (g_instance == NULL)
			{
				cq_registerCleanupFunction(_cleanupStoreHashmap, NULL);
				g_instance = NcNew(StoreHashmap);
			}
		}
	}

	return g_instance;
}

void StoreHashmap::clear()
{
	m_guidMap->removeAllObjects();
}
