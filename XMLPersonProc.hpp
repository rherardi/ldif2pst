// XMLMailProc.h: interface for the CXMLPersonProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPERSONPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_)
#define AFX_XMLPERSONPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_

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

#define PERSON_PERSON_ITEM_SELECTOR	_T("//person/personItem")
#define PERSON_PERSON_ITEM_CSELECTOR _T("//person/personItem[@condition=\"%s\"]")
#define PERSON_MESSAGE_CLASS_ATTR_SELECTOR _T("//person/personItem[@name=\"PR_MESSAGE_CLASS\"]/ldifAttribute")

#define XMLELEMENT_LDIFATTRIBUTE _T("ldifAttribute")

class CPersonItem;
class CEntryTransformer;

class CXMLPersonProc : public CXMLProc  
{
public:
	NMAILXML_API CXMLPersonProc();
	virtual ~CXMLPersonProc();

	NMAILXML_API CPersonItem * GetNextPersonItem(_TCHAR *partCondition = NULL);
	NMAILXML_API CEntryTransformer* getNextEntryTransformer(_TCHAR *nodeSelector = NULL);
	NMAILXML_API void resetEntryListIndex();
	NMAILXML_API void resetTransformerListIndex();
	void InitializePersonItemList();

	void GetMessageClassAttr(_TCHAR *attrValue);
};

#endif // !defined(AFX_XMLPERSONPROC_H__A27B6A75_17DA_4814_AD79_FE431D325F4C__INCLUDED_)
