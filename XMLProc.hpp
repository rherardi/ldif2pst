// XMLProc.h: interface for the CXMLProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
#define AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef JESCOMMON_EXPORTS
#define JESCOMMON_API __declspec(dllexport)
#else
#define JESCOMMON_API __declspec(dllimport)
#endif

#if defined(JESCOMMON_EXPORTS) || defined(JESCOMMON_IMPORTS)
XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE
#endif

#define TRANSFORMER_SELECTOR 	_T("child::*")

#ifdef NDIR2JES
#define DEFAULT_SCHEMA_PREFIX	_T("http://www.gssnet.com/ndir2jes")


#define LDIF_PERSON_SELECTOR		_T("//person/ldifEntry")
#define LDIF_GROUP_SELECTOR			_T("//ldifGroupEntry/ldifEntry")
#define XMLATTRIBUTE_ATTRIBUTE		_T("attribute")
#define XMLATTRIBUTE_ORG_HIERARCHY	_T("orgHierarchy")
#define XMLATTRIBUTE_RESOLVE_DOC_FIELD	_T("resolveDocField")
#define XMLATTRIBUTE_RESOLVE_DOC_VIEW	_T("resolveDocView")
#define XMLATTRIBUTE_RESOLVE_DOC_FORM	_T("resolveDocForm")
#define XMLATTRIBUTE_RESOLVE_DOC_LOOKUP_FIELD	_T("resolveDocLookupField")
#endif

#define CONFIG_ENTRY_SELECTOR		_T("//configEntry[@setting=\"%s\"]")
#define XMLATTRIBUTE_VALUE			_T("value")
#define XMLATTRIBUTE_NAME			_T("name")
#define XMLATTRIBUTE_PLACEHOLDER	_T("placeholder")
#define XMLATTRIBUTE_NUMERIC		_T("numeric")
#define XMLATTRIBUTE_VALUEMODIFIER	_T("valueModifier")
#define XMLATTRIBUTE_INPUT			_T("input")
#define XMLATTRIBUTE_CONDITION		_T("condition")
#define XMLATTRIBUTE_NAMEDPROPERTY	_T("namedProperty")

class CXMLNameValue;

class CXMLProc  
{
public:
	JESCOMMON_API CXMLProc();
	JESCOMMON_API virtual ~CXMLProc();
	JESCOMMON_API int CXMLProc::Parse();
	JESCOMMON_API const _TCHAR* getConfigSettingValue(const _TCHAR* settingName, const _TCHAR* selector = CONFIG_ENTRY_SELECTOR);
	JESCOMMON_API CXMLNameValue* getNextNameValue(_TCHAR* selector, const _TCHAR *name_attr = _T("name"), const _TCHAR *value_attr = _T("value"));
	JESCOMMON_API bool matchesRegExp(char *regExp, char* matchString);

	// Get/Set m_xmlConfig
	JESCOMMON_API _TCHAR const * GetXmlConfig();
	JESCOMMON_API void SetXmlConfig(const _TCHAR *xmlConfig);

	// Get/Set m_xmlConfig
	JESCOMMON_API _TCHAR const * GetSchemaName();
	JESCOMMON_API void SetSchemaName(const _TCHAR *schemaPrefix, const _TCHAR *schemaName);

	// Get/Set m_verbose
	JESCOMMON_API bool const GetVerbose();
	JESCOMMON_API void SetVerbose(const bool verbose);


#ifdef NMAIL2JES
	CMailbox* getMailboxInfo();
	CMessageItem * GetNextMessageItem(_TCHAR *partCondition = NULL);
	CEntryTransformer* getNextEntryTransformer();
	CConversionRuleEntry* getNextConversionRuleEntry();
	void resetConversionRuleIndex();
#endif

private:

protected:
	JESCOMMON_API void getEntryList(_TCHAR* selector);
	bool m_verbose;
	_TCHAR m_xmlConfig[256];
	_TCHAR m_schemaName[256];

//#ifdef XML_INTERNAL

#if defined(JESCOMMON_EXPORTS) || defined(JESCOMMON_IMPORTS)
	XercesDOMParser *parser;
	SAXErrorHandler *errHandler;
	DOMDocument* theXercesDocument;
	XalanDocument* theDocument;
//	XalanSourceTreeDOMSupport*		theDOMSupport;
	XercesDOMSupport*		theDOMSupport;
	XalanDocumentPrefixResolver*		thePrefixResolver;
//	XalanSourceTreeParserLiaison*	theLiaison;
	XercesParserLiaison* theLiaison;

	MutableNodeRefList* m_entryList;
	int m_currentEntryIndex;
	bool m_entryListInitialized;

	MutableNodeRefList* m_transformerList;
	int m_currentTransformerIndex;
	bool m_transformerListInitialized;
#endif
//#endif
};

#endif // !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
