// Notes2PSTApp.hpp: interface for the CLDIF2PSTApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDIF2PSTAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_)
#define AFX_LDIF2PSTAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLdifProc;
class CXMLPersonProc;
class CPersonItem;
class CEntryTransformer;
class CXMLNameValue;

#define DEFAULT_CONFIG_FILE	_T("ldif2pst.xml")
#define DEFAULT_SCHEMA_FILE	_T("ldif2pst.xsd")

#define DEFAULT_SCHEMA_PREFIX		_T("http://www.gssnet.com/ldif2pst")
#define DEFAULT_LDIF_EXT _T(".LDIF")

class CLDIF2PSTApp  
{
public:
	CLDIF2PSTApp();
	virtual ~CLDIF2PSTApp();

	void ShowUsage();
	_TCHAR *PrefixWithCurDir(_TCHAR *fileName, _TCHAR *fileNameFull);
	int ProcessCommandLine(int argc, _TCHAR *argv[]);
	int StartMigration(_TCHAR* exec);
	CPersonItem * GetNextPersonItem(_TCHAR *partCondition = NULL);
	CEntryTransformer* getNextEntryTransformer(_TCHAR *nodeSelector = NULL);
	void resetConversionRuleIndex();
	void resetEntryListIndex();
	void resetTransformerListIndex();
	const _TCHAR* getConfigSettingValue(const _TCHAR* settingName);
	bool matchesRegExp(char *regExpPattern, char* matchString);
	CXMLPersonProc *getXMLProc();
	void InitializePersonItemList();
	CXMLNameValue* getNextNameValue(_TCHAR* selector, const _TCHAR *name_attr = _T("name"), const _TCHAR *value_attr = _T("value"));
	void GetMessageClassAttr(_TCHAR *attr);

protected:
	CLdifProc *m_ldifProc;
	CXMLPersonProc *m_xmlProc;
	bool m_verbose;

public:

	int IsVerbose() {
		return m_verbose == true;
	}

private:
	int ExtractSchemaFile(_TCHAR* tempDir);
};

#endif // !defined(AFX_LDIF2PSTAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_)
