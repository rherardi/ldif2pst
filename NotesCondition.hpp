// NotesCondition.hpp: interface for the CNotesCondition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTESCONDITION_HPP__79ADC945_2F9D_48D8_A931_DD242A441425__INCLUDED_)
#define AFX_NOTESCONDITION_HPP__79ADC945_2F9D_48D8_A931_DD242A441425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef NMAILXML_EXPORTS
#define NMAILXML_API __declspec(dllexport)
#else
#define NMAILXML_API __declspec(dllimport)
#endif

#define CONDITION_NOATTACHMENTS			_T("NoAttachments")
#define CONDITION_ATTACHMENTEXISTS		_T("AttachmentExists")
#define CONDITION_TEXTPART				_T("TextPart")
#define CONDITION_ATTACHMENTPART		_T("AttachmentPart")
#define CONDITION_ENDOFPARTS			_T("EndOfParts")
#define CONDITION_SENDER				_T("Sender")
#define CONDITION_RECIPIENTTO			_T("RecipientTo")
#define CONDITION_RECIPIENTCC			_T("RecipientCc")
#define CONDITION_RECIPIENTBCC			_T("RecipientBcc")

class CNotesCondition  
{
public:
	CNotesCondition();
	virtual ~CNotesCondition();

	NMAILXML_API static bool IsNotesCondition(NOTEHANDLE hNote, const _TCHAR *condition, STATUS *error);
	NMAILXML_API static bool CheckAttachmentExists(NOTEHANDLE hNote, STATUS *error);
};

#endif // !defined(AFX_NOTESCONDITION_HPP__79ADC945_2F9D_48D8_A931_DD242A441425__INCLUDED_)
