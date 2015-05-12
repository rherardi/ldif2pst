// NotesCondition.cpp: implementation of the CNotesCondition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "global.h"
#include "nsfdata.h"
#include "nsfnote.h"
#include "stdnames.h"
#include "nsferr.h"

#include "NotesCondition.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotesCondition::CNotesCondition()
{

}

CNotesCondition::~CNotesCondition()
{

}

bool CNotesCondition::IsNotesCondition(NOTEHANDLE hNote, const _TCHAR *condition, STATUS *error)
{
	bool checkCondition = false;
	if (_tcscmp(condition, CONDITION_NOATTACHMENTS) == 0) {
		return !CheckAttachmentExists(hNote, error);
	}
	else if (_tcscmp(condition, CONDITION_ATTACHMENTEXISTS) == 0) {
		return CheckAttachmentExists(hNote, error);
	}
	else if (_tcscmp(condition, CONDITION_TEXTPART) == 0) {
	}
	else if (_tcscmp(condition, CONDITION_ATTACHMENTPART) == 0) {
	}
	return checkCondition;
}

bool CNotesCondition::CheckAttachmentExists(NOTEHANDLE hNote, STATUS *error)
{
	STATUS Error = NO_ERROR;

	BLOCKID item_bid;
	WORD value_datatype;
	BLOCKID value_bid;
	DWORD value_size;

	Error = NSFItemInfo(hNote, ITEM_NAME_ATTACHMENT,
		strlen(ITEM_NAME_ATTACHMENT), &item_bid,
		&value_datatype, &value_bid,
		&value_size);

	if (Error == ERR_ITEM_NOT_FOUND) {
		*error = NO_ERROR;
		return false;
	}
	else if (Error) {
		*error = Error;
		return false;
	}

	*error = NO_ERROR;
	return true;
}
