// LDIF2PSTApp.cpp: implementation of the CLDIF2PSTApp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "LDIF2PSTApp.hpp"
#include "mapistuff.h"
#include "LdifProc.h"
#include "XMLProcSuppl.hpp"
#include "XMLProc.hpp"
#include "XMLPersonProc.hpp"
#include "XMLNameValue.hpp"
#include "PersonItem.hpp"
#include "Config.hpp"
#include "DirUtils.hpp"
#include "XGetopt.h"
#include "Resources.h"

#include "resource.h"
#include "inistuff.h"

HINSTANCE hInst = 0;

#include <time.h>

//#define EXPIRE30

#if defined EXPIRE30
#define BOMB 1100415
//#define BOMB 1100401
//#define BOMB 1100315
//#define BOMB 1100301
//#define BOMB 1091231
//#define BOMB 1090731
//#define BOMB 1090315
//#define BOMB 1091031
//#define BOMB 1091001
//#define BOMB 1080915
//#define BOMB 1080715
//#define BOMB 1080630
//#define BOMB 1080620
//#define BOMB 1080615
//#define BOMB 1080531
//#define BOMB 1080516
//#define BOMB 1080215
//#define BOMB 1080131
//#define BOMB 1080115
//#define BOMB 1071217
//#define BOMB 1071130
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLDIF2PSTApp::CLDIF2PSTApp() :
m_ldifProc(new CLdifProc()), m_xmlProc(new CXMLPersonProc()), m_verbose(false)
{
}

CLDIF2PSTApp::~CLDIF2PSTApp()
{

}

_TCHAR *CLDIF2PSTApp::PrefixWithCurDir(_TCHAR *fileName, _TCHAR *fileNameFull)
{
	if ((_tcschr(fileName, _T(':')) == NULL && 
		_tcschr(fileName, _T('\\')) == NULL) || 
		(_tcsncmp(fileName, _T(".\\"), 2) == 0))
	{
		TCHAR szCurrentDir[MAX_PATH+1] = {0};
		ULONG cchCurrentDir = 0;

		cchCurrentDir = GetCurrentDirectory(MAX_PATH, szCurrentDir);
		if (
			szCurrentDir[cchCurrentDir - 1] != _T(':') && 
			szCurrentDir[cchCurrentDir - 1] != _T('\\'))
		{
			_tcscat(szCurrentDir, _T("\\"));
		}
		_tcscpy(fileNameFull, szCurrentDir);
		if (_tcsncmp(fileName, _T(".\\"), 2) != 0)
			_tcscat(fileNameFull, fileName);
		else
			_tcscat(fileNameFull, fileName + 2);
	}
	else
		_tcscpy(fileNameFull, fileName);

	return fileNameFull;
}

// Process all command line options
int CLDIF2PSTApp::ProcessCommandLine(int argc, _TCHAR *argv[])
{
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR prompt[512];
	int c;
	_TCHAR szInputFile[MAX_PATH+1] = {0};

	if (argc == 1) {
		ShowUsage();
		return MIGRATION_ERR_NO_PARAMETERS;
	}

//	while ((c = getopt(argc, argv, _TEXT("S:b:fi:n:o:vx:"))) != EOF)
	while ((c = getopt(argc, argv, _TEXT("ai:l:n:o:p:x:v"))) != EOF)
	{
		 switch (c)
		 {
//			 case _T('S'):
//				 {
//					m_ldifProc->SetServer(argv[optind-1]);
//				 }
//				 break;
//			 case _T('b'):
//				 {
//					m_ldifProc->SetBaseDN(argv[optind-1]);
//				 }
//				 break;
			 case _T('a'):
				 {
					m_ldifProc->SetAppendLogFile(true);
				 }
				 break;
			 case _T('i'):
				 {
					m_ldifProc->SetInputDir(argv[optind-1]);
				 }
				 break;
			 case _T('l'):
				 {
					m_ldifProc->SetLogFile(PrefixWithCurDir(argv[optind-1], szInputFile));
				 }
				 break;
			 case _T('n'):
				 {
					m_ldifProc->SetOnlyLdifFile(PrefixWithCurDir(argv[optind-1], szInputFile));
				 }
				 break;
			 case _T('o'):
				 {
					m_ldifProc->SetOutputDir(argv[optind-1]);
				 }
				 break;
			 case _T('p'):
				 {
					m_ldifProc->SetOutlookFile(PrefixWithCurDir(argv[optind-1], szInputFile));
				 }
				 break;
			 case _T('x'):
				 {
					m_xmlProc->SetXmlConfig(PrefixWithCurDir(argv[optind-1], szInputFile));
				 }
				 break;
			 case _T('v'):
				 {
					m_verbose = true;
				 }
				 break;
//			 case _T('!'):
//				 //TRACE(_TEXT("ERROR: illegal option %s\n"), argv[optind-1]);
//				::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
//				::_tprintf(_TEXT("%s %s %d %d\n"), prompt, argv[optind-1], optind, argc);
//				ShowUsage();
//				 return MIGRATION_ERR_MISSEDARG;
//				 break;

			 case _T('?'):
				 //TRACE(_TEXT("ERROR: illegal option %s\n"), argv[optind-1]);
//				::LoadString(hInst, IDS_OPT_ERR_ILLEGAL_OPTION, prompt, 256);
//				::_tprintf(_TEXT("%s %s\n"), prompt, argv[optind-1]);
				ShowUsage();
				 return MIGRATION_ERR_ILLEGAL_OPTION;
				 break;

			 default:
				 {
				 //TRACE(_TEXT("WARNING: no handler for option %c\n"), c);
				::LoadString(hInst, IDS_OPT_ERR_NOHANDLER, prompt, 256);
				::_tprintf(_TEXT("%s %s\n"), prompt, argv[optind-1]);
				 }
				 return MIGRATION_ERR_NO_OPTION_HANDLER;
				 break;
		 }
	}
	//
	// check for non-option args here
	//
	if (optind != argc) {
		::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
		::_tprintf(_TEXT("%s %s\n"), prompt, argv[1]);
		ShowUsage();
		return MIGRATION_ERR_MISSEDARG;
	}

	_TCHAR tempDir[256];
	::GetEnvironmentVariable(_T("TEMP"), tempDir, 256);
	if (_tcslen(tempDir) == 0) {
		::GetEnvironmentVariable(_T("TMP"), tempDir, 256);
	}
//	_tprintf(_T("before ExtractSchemaFile\n"));
	if (ExtractSchemaFile(tempDir) != MIGRATION_STEP_SUCCESS) {
		return MIGRATION_ERR_FAILED;
	}

	if (m_verbose) {
		m_xmlProc->SetVerbose(true);
	}
//	_tprintf(_T("before GetXmlConfig\n"));
	const _TCHAR* xmlConfig = m_xmlProc->GetXmlConfig();
	if (_tcslen(xmlConfig) == 0) {
		_TCHAR xmlFileName[512];

		::GetModuleFileName(hInst, xmlFileName, 512);
		for(int i = _tcslen(xmlFileName) - 1; i >= 0; i--) {
			if (xmlFileName[i] == _T('\\')) {
				xmlFileName[i] = _T('\0');
				break;
			}
		}
		if (i > 0) {
			_tcscat(xmlFileName, _T("\\"));
		}
		_tcscat(xmlFileName, DEFAULT_CONFIG_FILE);

//		_tprintf(_T("xmlFileName: %s\n"), xmlFileName);
		m_xmlProc->SetXmlConfig(xmlFileName);
	}
//	_tprintf(_T("before Parse\n"));
	if (m_xmlProc->Parse() != MIGRATION_STEP_SUCCESS) {
//		_tprintf(_T("m_xmlProc->Parse() failed!\n"));
		::DeleteFile(m_xmlProc->GetSchemaName());
		return MIGRATION_ERR_FAILED;
	}
	::DeleteFile(m_xmlProc->GetSchemaName());

//	_tprintf(_T("xmlConfig len: %d\n"), _tcslen(xmlConfig));
	if (_tcslen(xmlConfig) != 0) {
		const _TCHAR *logDir;
		logDir = getConfigSettingValue(XMLCONFIG_LOGDIR);

//		_tprintf(_T("logDir: %s\n"), logDir);
		if (logDir != NULL) {
			if (CDirUtils::MakeDir(logDir) != 0) {
				_tprintf(_T("!= 0\n"));
//				::LoadString(hInst, IDS_ERR_DIR_NOT_CREATED, usage, 512);
//				::_tprintf(usage, output_dir, field_textW);
				// ???
			}
			else {
//				_tprintf(_T("before INI_LogInit\n"));
				unsigned int logSizeWrap = (unsigned int)_ttoi(m_xmlProc->getConfigSettingValue(XMLCONFIG_LOGSIZEWRAP));
				if (logSizeWrap > 0)
					INI_SetLogSizeWrap(logSizeWrap);
				if (_tcslen(m_ldifProc->GetLogFile()) != 0)
					INI_LogInit(m_ldifProc->GetLogFile(), true, m_ldifProc->GetAppendLogFile());
				else
					INI_LogInit(logDir, false, m_ldifProc->GetAppendLogFile());
			}
		}

		if (_tcslen(m_ldifProc->GetFilter()) == 0) {
			m_ldifProc->SetFilter(m_xmlProc->getConfigSettingValue(XMLCONFIG_FILTER));
		}
		if (_tcslen(m_ldifProc->GetOutputDir()) == 0) {
			m_ldifProc->SetOutputDir(m_xmlProc->getConfigSettingValue(XMLCONFIG_OUTPUTDIR));
		}
		if (_tcslen(m_ldifProc->GetOutlookFile()) == 0) {
			m_ldifProc->SetOutlookFile(m_xmlProc->getConfigSettingValue(XMLCONFIG_OUTLOOKFILE));
		}
		if (_tcslen(m_ldifProc->GetInputDir()) == 0) {
			_TCHAR *attrValue = (_TCHAR *)m_xmlProc->getConfigSettingValue(XMLCONFIG_INPUTDIR);
			_TCHAR *ext = attrValue;
			if (_tcslen(attrValue) > _tcslen(DEFAULT_LDIF_EXT))
				ext = attrValue + _tcslen(attrValue) - _tcslen(DEFAULT_LDIF_EXT);
			// check if we have directory or single file
			if (_tcslen(attrValue) > _tcslen(DEFAULT_LDIF_EXT) &&
				_tcsicmp(ext, DEFAULT_LDIF_EXT) == 0)
				m_ldifProc->SetOnlyLdifFile(attrValue);
			else
				m_ldifProc->SetInputDir(attrValue);
		}
	}

	if (m_verbose) {
		::LoadString(hInst, IDS_CONFIG_SETTING, prompt, 256);
		::_tprintf(prompt, XMLCONFIG_FILTER, m_ldifProc->GetFilter());
	}

	return MIGRATION_PARAMETERS_OK;
}

// Show utility full title and copyright

void CLDIF2PSTApp::ShowUsage()
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);

// Usage: NSF2PST [OPTION]...
// -i <dir> Input directory for NSF files (optional, supersedes XML file setting) 
// -l <logFile> Log file (optional, supersedes XML file setting) 
// -n <fileName> NSF file to convert (optional, supersedes XML file setting) 
// -o <dir> Output directory for PST files (optional, supersedes XML file setting) 
// -p <file> \tAlready existing MS Outllok .pst file name
// -x <xmlConfig> XML configuration file (required) 
// -v Verbose/debug logging (optional, see User Guide)

	::LoadString(hInst, IDS_USAGE, usage, 256);
	::_tprintf(_TEXT("%s\n\n"), usage);

	// -a Append log file
	::LoadString(hInst, IDS_OPTION_LOGFILE_APPEND, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -i <dir> Input directory for NSF files (optional, supersedes XML file setting) 
	::LoadString(hInst, IDS_OPTION_INPUTDIR, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -l <logFile> Log file (optional, supersedes XML file setting) 
	::LoadString(hInst, IDS_OPTION_LOGFILE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -n <fileName> LDIF file to convert (optional, supersedes XML file setting) 
	::LoadString(hInst, IDS_OPTION_FILENAME, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -o <dir> Output directory for PST files (optional, supersedes XML file setting) 
	::LoadString(hInst, IDS_OPTION_OUTPUTDIR, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -p <file> \tAlready existing MS Outllok .pst file name
	::LoadString(hInst, IDS_OPTION_OUTLOOKFILE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -x <xmlConfig> XML configuration file (required) 
	::LoadString(hInst, IDS_OPTION_XMLCONFIG, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -v Verbose/debug logging (optional, see User Guide)
	::LoadString(hInst, IDS_OPTION_VERBOSE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
}

CPersonItem* CLDIF2PSTApp::GetNextPersonItem(_TCHAR *partCondition)
{
	return m_xmlProc->GetNextPersonItem(partCondition);
}

CEntryTransformer* CLDIF2PSTApp::getNextEntryTransformer(_TCHAR *nodeSelector)
{
	return m_xmlProc->getNextEntryTransformer(nodeSelector);
}

void CLDIF2PSTApp::resetEntryListIndex()
{
	m_xmlProc->resetEntryListIndex();
}

void CLDIF2PSTApp::resetTransformerListIndex()
{
	m_xmlProc->resetTransformerListIndex();
}

void CLDIF2PSTApp::InitializePersonItemList()
{
	m_xmlProc->InitializePersonItemList();
}

const _TCHAR* CLDIF2PSTApp::getConfigSettingValue(const _TCHAR* settingName)
{
	return m_xmlProc->getConfigSettingValue(settingName);
}

CXMLNameValue* CLDIF2PSTApp::getNextNameValue(_TCHAR* selector, const _TCHAR *name_attr, const _TCHAR *value_attr)
{
	return m_xmlProc->getNextNameValue(selector, name_attr, value_attr);
}

bool CLDIF2PSTApp::matchesRegExp(char *regExpPattern, char* matchString)
{
	return m_xmlProc->matchesRegExp(regExpPattern, matchString);
}

void CLDIF2PSTApp::GetMessageClassAttr(_TCHAR *attr)
{
	m_xmlProc->GetMessageClassAttr(attr);
}

int CLDIF2PSTApp::ExtractSchemaFile(_TCHAR* tempDir)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLDIF2PSTApp::ExtractSchemaFile");
	if (IsVerbose()){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	int retCode = MIGRATION_STEP_SUCCESS;
	HRSRC hSchema = ::FindResourceEx(hInst, _T("Schema"), MAKEINTRESOURCE(IDR_SCHEMA), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	HGLOBAL hResource;
	FILE *f;
	_TCHAR fullSchemaName[256];

	if (hSchema != NULL) {
		unsigned long schemaSize = SizeofResource(hInst, hSchema);
		// now get a handle to the resource
		if ((hResource = LoadResource(hInst, hSchema)) == NULL) {
			retCode = MIGRATION_ERR_FAILED;
			return retCode;
		}
		// finally get and return a pointer to the resource
		UCHAR* pResource = ((UCHAR*)LockResource(hResource));

		_tcscpy(fullSchemaName, tempDir);
		_tcscat(fullSchemaName, _T("\\"));
		_tcscat(fullSchemaName, DEFAULT_SCHEMA_FILE);
		f = _tfopen(fullSchemaName, _TEXT("w"));

		if (f == NULL) {
			retCode = MIGRATION_ERR_FAILED;
			return retCode;
		}

		unsigned int itemsWritten = fwrite(pResource, sizeof(UCHAR), schemaSize, f);
		if (itemsWritten < schemaSize) {
			fclose(f);
			retCode = MIGRATION_ERR_FAILED;
			return retCode;
		}

		fclose(f);
		m_xmlProc->SetSchemaName(DEFAULT_SCHEMA_PREFIX, fullSchemaName);

	}
	else {
//		_tprintf(_T("hSchema == NULL\n"));
		retCode = MIGRATION_ERR_FAILED;
		return retCode;
	}

	if (IsVerbose()){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}

int CLDIF2PSTApp::StartMigration(_TCHAR* exec)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLDIF2PSTApp::StartMigration");
	if (IsVerbose()){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	m_ldifProc->SetVerbose(m_verbose);
	m_xmlProc->SetVerbose(m_verbose);
//	::_tprintf(L"GetServer2: %s\n", exec);
	int retCode = m_ldifProc->Start(exec, this);
	if (IsVerbose()){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}

#if defined EXPIRE30
void exp() {
	time_t timer;
	struct tm *today;
	int daysleft = 1;
	unsigned long currdate;
	unsigned long bomb = BOMB;

	time(&timer);
	today = localtime(&timer);
	currdate = (today->tm_year)*10000 + (today->tm_mon+1)*100 + today->tm_mday;
//	_tprintf(_T("currdate: %lu, bomb: %lu\n"), currdate, bomb);
	if (currdate > bomb)
		daysleft = 0;
	if (daysleft == 0) {
		_tprintf(_T("Expired!\n"));
		exit(-1);
	}
}
#endif

int wmain(int argc, _TCHAR* argv[])
{
	int retCode;
	_TCHAR usage[256];
	hInst = ::GetModuleHandle(NULL);
#if defined EXPIRE30
	exp();
#endif

	CLDIF2PSTApp *theApp = new CLDIF2PSTApp();
	CResources::ShowTitle(IDS_TITLE, IDS_VERSION);

	retCode = theApp->ProcessCommandLine(argc, argv);
	if (retCode == MIGRATION_PARAMETERS_OK) {
		retCode = theApp->StartMigration(argv[0]);
		::INI_LogWriteWithResourcePrefix(IDS_LOG_CONV_END, _T(""));
		if (retCode == MIGRATION_STEP_SUCCESS) {
			retCode = MIGRATION_SUCCESS;
		}
	}
	if (theApp->IsVerbose()){
		::LoadString(hInst, IDS_EXIT_CODE, usage, 256);
		::_tprintf(_TEXT("%s %d\n"), usage, retCode);
	}

	INI_LogWriteWithResourcePrefixNum(IDS_LOG_EXIT_CODE, retCode);

	INI_LogClose();
	theApp->~CLDIF2PSTApp();
	return retCode;
}
