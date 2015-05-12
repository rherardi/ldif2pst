// XMLMailProc.h: interface for the CXMLMailProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLMAILPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_)
#define AFX_XMLMAILPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef NMAILXML_EXPORTS
#define NMAILXML_API __declspec(dllexport)
#else
#define NMAILXML_API __declspec(dllimport)
#endif

#ifdef NMAILXML_EXPORTS
XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE
#endif

#define LDIF2PST_PERSON_ITEM_SELECTOR	_T("//person/personItem")

class CMessageItem;
class CEntryTransformer;

class CXMLMailProc : public CXMLProc  
{
public:
	NMAILXML_API CXMLMailProc();
	virtual ~CXMLMailProc();

	NMAILXML_API CMessageItem * GetNextMessageItem(_TCHAR *partCondition = NULL);
	NMAILXML_API CEntryTransformer* getNextEntryTransformer(_TCHAR *nodeSelector = NULL);
	NMAILXML_API void resetConversionRuleIndex();
	NMAILXML_API void resetEntryListIndex();
	NMAILXML_API void resetTransformerListIndex();
	void InitializeMessageItemList();
};

#endif // !defined(AFX_XMLMAILPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_)
