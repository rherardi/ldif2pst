// LdifRecord.cpp: implementation of the CLdifRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LdifRecord.hpp"
#include "Inistuff.h"

LIST *ldifRecordLst;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLdifRecord::CLdifRecord()
{
	ldifRecord = new CList();
}

CLdifRecord::~CLdifRecord()
{
	this->Release();
	delete ldifRecord;
}

_TCHAR *CLdifRecord::Add(_TCHAR *id, _TCHAR *value)
{
	LIST *item = ldifRecord->LST_Add(&ldifRecordLst, id, 256);
	if (item != NULL) {
		_tcscpy(item->data, value);
		return item->data;
	}
	else
		return NULL;
}

void CLdifRecord::Release()
{
	ldifRecord->LST_Release(&ldifRecordLst);
}

_TCHAR *CLdifRecord::Lookup(_TCHAR *id, bool exact)
{
	LIST *item = ldifRecord->LST_Lookup(&ldifRecordLst, id, exact);
	if (item != NULL)
		return item->data;
	else
		return NULL;
}

void CLdifRecord::Remove(_TCHAR *id)
{
	ldifRecord->LST_Remove(&ldifRecordLst, id);
}

_TCHAR *CLdifRecord::LookupAdd(_TCHAR *id, _TCHAR *value)
{
	LIST *item = ldifRecord->LST_LookupAdd(&ldifRecordLst, id, 256);
	if (item != NULL) {
		_tcscpy(item->data, value);
		return item->data;
	}
	else
		return NULL;
}

_TCHAR *CLdifRecord::LookupAdd(_TCHAR *id, long valLong)
{
	LIST *item = ldifRecord->LST_LookupAdd(&ldifRecordLst, id, 256);
	if (item != NULL) {
		_TCHAR valStr[32];
		wsprintf(valStr, _T("%lu"), valLong);
		_tcscpy(item->data, valStr);
		return item->data;
	}
	else
		return NULL;
}
