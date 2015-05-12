// NotesProc.cpp: implementation of the CLdifProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define DONT_USE_OUTPUT_UNICODE
#define OUTLOOK_UNICODE

#include "mapistuff.h"

#include <initguid.h>
#include "LdifProc.h"
#include "resource.h"
#include "LDIF2PSTApp.hpp"
#include "Config.hpp"
#include "PersonItem.hpp"
#include "LdifRecord.hpp"
#include "XMLNameValue.hpp"
#include "EntryTransformer.hpp"
#include "DirUtils.hpp"
#include "Decode.hpp"

#include "JESCommon.h"
#include "inistuff.h"
#include "list.hpp"

//#define DEBUG_LOG 1

static char smtp[] = "SMTP";

#define XML_PROP_TAG(NAME)  { _T(#NAME), NAME }

struct PropTag {
	_TCHAR *prop;
	ULONG value;
};

struct PropTag PropTags[] = {
	XML_PROP_TAG(PR_MESSAGE_CLASS),
	XML_PROP_TAG(PR_MESSAGE_FLAGS),
	XML_PROP_TAG(PR_SENDER_ENTRYID),
	XML_PROP_TAG(PR_SENDER_NAME),
	XML_PROP_TAG(PR_SENDER_EMAIL_ADDRESS),
	XML_PROP_TAG(PR_SENDER_ADDRTYPE),
	XML_PROP_TAG(PR_SENT_REPRESENTING_ENTRYID),
	XML_PROP_TAG(PR_SENT_REPRESENTING_NAME),
	XML_PROP_TAG(PR_SENT_REPRESENTING_EMAIL_ADDRESS),
	XML_PROP_TAG(PR_SENT_REPRESENTING_ADDRTYPE),
	XML_PROP_TAG(PR_IMPORTANCE),
	XML_PROP_TAG(PR_CLIENT_SUBMIT_TIME),
	XML_PROP_TAG(PR_MESSAGE_DELIVERY_TIME),
	XML_PROP_TAG(PR_DISPLAY_NAME),
	XML_PROP_TAG(PR_RECIPIENT_TYPE),
	XML_PROP_TAG(PR_SUBJECT),
	XML_PROP_TAG(PR_BODY),
	XML_PROP_TAG(PR_ATTACH_METHOD),
	XML_PROP_TAG(PR_ATTACH_FILENAME),
	XML_PROP_TAG(PR_ATTACH_LONG_FILENAME),
	XML_PROP_TAG(PR_GIVEN_NAME),
	XML_PROP_TAG(PR_SURNAME),
	XML_PROP_TAG(PR_BUSINESS_ADDRESS_CITY),
	XML_PROP_TAG(PR_BUSINESS_ADDRESS_COUNTRY),
	XML_PROP_TAG(PR_BUSINESS_ADDRESS_POSTAL_CODE),
	XML_PROP_TAG(PR_BUSINESS_ADDRESS_STATE_OR_PROVINCE),
	XML_PROP_TAG(PR_BUSINESS_ADDRESS_STREET),
	XML_PROP_TAG(PR_BUSINESS_HOME_PAGE),
	XML_PROP_TAG(PR_HOME_ADDRESS_CITY),
	XML_PROP_TAG(PR_HOME_ADDRESS_COUNTRY),
	XML_PROP_TAG(PR_HOME_ADDRESS_POSTAL_CODE),
	XML_PROP_TAG(PR_HOME_ADDRESS_STATE_OR_PROVINCE),
	XML_PROP_TAG(PR_HOME_ADDRESS_STREET),
	XML_PROP_TAG(PR_PERSONAL_HOME_PAGE),
	XML_PROP_TAG(PR_BUSINESS_FAX_NUMBER),
	XML_PROP_TAG(PR_BUSINESS_TELEPHONE_NUMBER),
	XML_PROP_TAG(PR_CELLULAR_TELEPHONE_NUMBER),
	XML_PROP_TAG(PR_HOME_TELEPHONE_NUMBER),
	XML_PROP_TAG(PR_PAGER_TELEPHONE_NUMBER),
	XML_PROP_TAG(PR_TITLE),
	XML_PROP_TAG(PR_COMPANY_NAME),
	XML_PROP_TAG(PR_DEPARTMENT_NAME),
	XML_PROP_TAG(PR_NICKNAME)
};

#ifndef MAX_LINE
#define MAX_LINE 256
#endif

/* constants */
#define     READMAIL_BODY_LINELEN   40
char szDriveLetter_blank[] = "";
char szExtensionName_TMP[] = "TMP";
char szNewline_Terminate[] = "\r\n";
#define CHARS_PER_LINE 80

#define SHORT_PRODUCT_TITLE	"ldif2pst"
#define SHORT_PRODUCT_TITLE_W	L"ldif2pst"

LIST *lst;
LIST *notFoundLst;

CLdifProc *pLdifProc;

//#define MALLOC_AMOUNT   6048
#define STRING_LENGTH   256

// MAPI Globals
LPMAPISESSION pses = NULL;
LPMDB pmdb = NULL;
LPADRBOOK pAddrBook = NULL;

struct NamedProperty {
	ULONG pID;
	ULONG pType;
};

// The array is the list of named properties to be set.
NamedProperty aulContactProps[] = {
	{PidLidFileUnderList, PT_MV_LONG},
	{PidLidAddressBookProviderEmailList, PT_MV_LONG},
	{PidLidAddressBookProviderArrayType, PT_LONG},
	{PidLidFileUnder, PT_UNICODE},
	{PidLidFileUnderId, PT_LONG},
	{PidLidContactItemData, PT_MV_LONG},

	{PidLidEmail1DisplayName, PT_UNICODE},
	{PidLidEmail1AddressType, PT_UNICODE},
	{PidLidEmail1EmailAddress, PT_UNICODE},
	{PidLidEmail1OriginalDisplayName, PT_UNICODE},
	{PidLidEmail1OriginalEntryID, PT_BINARY},

	{PidLidEmail2DisplayName, PT_UNICODE},
	{PidLidEmail2AddressType, PT_UNICODE},
	{PidLidEmail2EmailAddress, PT_UNICODE},
	{PidLidEmail2OriginalDisplayName, PT_UNICODE},
	{PidLidEmail2OriginalEntryID, PT_BINARY},

	{PidLidWorkAddressStreet, PT_UNICODE},
	{PidLidWorkAddressCity, PT_UNICODE},
	{PidLidWorkAddressState, PT_UNICODE},
	{PidLidWorkAddressPostalCode, PT_UNICODE},
	{PidLidWorkAddressCountry, PT_UNICODE},
	{PidLidWorkAddressCountryCode, PT_UNICODE},
	{PidLidHomeAddress, PT_UNICODE},

	{PidMyLidUserField1, PT_UNICODE},
	{PidMyLidUserField2, PT_UNICODE},
	{PidMyLidUserField3, PT_UNICODE},
	{PidMyLidUserField4, PT_UNICODE},

	{PidLidHtml, PT_UNICODE},
	{PidMyInstMsg, PT_UNICODE}
};
#define ulContactProps (sizeof(aulContactProps)/sizeof(aulContactProps [0]))

// The enum is to aid in building the property values for SetProps.
// The beginning of the list must match aulContactProps. All non-named properties must come at the end.
enum {
	p_PidLidFileUnderList,
	p_PidLidAddressBookProviderEmailList,
	p_PidLidAddressBookProviderArrayType,
	p_PidLidFileUnder,
	p_PidLidFileUnderId,
	p_PidLidContactItemData,
	p_PidLidEmail1DisplayName,
	p_PidLidEmail1AddressType,
	p_PidLidEmail1EmailAddress,
	p_PidLidEmail1OriginalDisplayName,
	p_PidLidEmail1OriginalEntryID,
	p_PidLidEmail2DisplayName,
	p_PidLidEmail2AddressType,
	p_PidLidEmail2EmailAddress,
	p_PidLidEmail2OriginalDisplayName,
	p_PidLidEmail2OriginalEntryID,
	p_PidLidWorkAddressStreet,
	p_PidLidWorkAddressCity,
	p_PidLidWorkAddressState,
	p_PidLidWorkAddressPostalCode,
	p_PidLidWorkAddressCountry,
	p_PidLidWorkAddressCountryCode,
	p_PidLidHomeAddress,
	p_PidMyLidUserField1,
	p_PidMyLidUserField2,
	p_PidMyLidUserField3,
	p_PidMyLidUserField4,
	p_PidLidHtml,
	p_PidMyInstMsg,
	NUM_PROPS
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLdifProc::CLdifProc()
{
	m_verbose = false;
	m_language[0] = _T('\0');
	m_inputdir[0] = _T('\0');
	m_logFile[0] = _T('\0');
	m_outputdir[0] = _T('\0');
	m_onlyLdifFile[0] = _T('\0');
	pLdifProc = this;

	listSuppl = new CList();
}

CLdifProc::~CLdifProc()
{

}

// Get/Set m_language
_TCHAR const * CLdifProc::GetLanguage()
{
	return &m_language[0];
}
void CLdifProc::SetLanguage(const _TCHAR *language)
{
	if (language != NULL) {
		::_tcscpy(m_language, language);
	}
}

// Get/Set m_filter
_TCHAR const * CLdifProc::GetFilter()
{
	return m_filter;
}
void CLdifProc::SetFilter(const _TCHAR *filter)
{
	if (filter != NULL) {
		::_tcscpy(m_filter, filter);
	}
}

// Get/Set m_inputdir
_TCHAR const * CLdifProc::GetInputDir()
{
	return &m_inputdir[0];
}
void CLdifProc::SetInputDir(const _TCHAR *inputDir)
{
	if (inputDir != NULL) {
		::_tcscpy(m_inputdir, inputDir);
	}
}

// Get/Set m_logFile
_TCHAR const * CLdifProc::GetLogFile()
{
	return &m_logFile[0];
}
void CLdifProc::SetLogFile(const _TCHAR *logFile)
{
	if (logFile != NULL) {
		::_tcscpy(m_logFile, logFile);
	}
}

// Get/Set m_logSizeWrap
unsigned int CLdifProc::GetLogSizeWrap()
{
	return m_logSizeWrap;
}
void CLdifProc::SetLogSizeWrap(unsigned int logSizeWrap)
{
	m_logSizeWrap = logSizeWrap;
}

// Get/Set m_outputdir
_TCHAR const * CLdifProc::GetOutputDir()
{
	return &m_outputdir[0];
}
void CLdifProc::SetOutputDir(const _TCHAR *outputDir)
{
	if (outputDir != NULL) {
		::_tcscpy(m_outputdir, outputDir);
	}
}

// Get/Set m_defaultName
_TCHAR const * CLdifProc::GetDefaultName()
{
	return &m_defaultName[0];
}
void CLdifProc::SetDefaultName(const _TCHAR *defaultName)
{
	if (defaultName != NULL) {
		::_tcscpy(m_defaultName, defaultName);
	}
}

// Get/Set m_defaultEmail
_TCHAR const * CLdifProc::GetDefaultEmail()
{
	return &m_defaultEmail[0];
}
void CLdifProc::SetDefaultEmail(const _TCHAR *defaultEmail)
{
	if (defaultEmail != NULL) {
		::_tcscpy(m_defaultEmail, defaultEmail);
	}
}


// Get/Set m_onlyLdifFile
_TCHAR const * CLdifProc::GetOnlyLdifFile()
{
	return &m_onlyLdifFile[0];
}
void CLdifProc::SetOnlyLdifFile(const _TCHAR *onlyLdifFile)
{
	if (onlyLdifFile != NULL) {
		::_tcscpy(m_onlyLdifFile, onlyLdifFile);
	}
}

// Get/Set m_outlookFile
_TCHAR const * CLdifProc::GetOutlookFile()
{
	return &m_outlookFile[0];
}
void CLdifProc::SetOutlookFile(const _TCHAR *outlookFile)
{
	if (outlookFile != NULL) {
		::_tcscpy(m_outlookFile, outlookFile);
	}
}


// Get/Set m_verbose
bool const CLdifProc::GetVerbose()
{
	return m_verbose;
}
void CLdifProc::SetVerbose(const bool verbose)
{
	m_verbose = verbose;
}

// Get/Set m_appendLogFile
bool const CLdifProc::GetAppendLogFile()
{
	return m_appendLogFile;
}
void CLdifProc::SetAppendLogFile(const bool appendLogFile)
{
	m_appendLogFile = appendLogFile;
}

int CLdifProc::Start(_TCHAR* exec, CLDIF2PSTApp *theApp)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLdifProc::Start");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	int retCode = MIGRATION_SUCCESS;

	theParentApp = theApp;

	retCode = PerformMigrationLocal((_TCHAR *)this->GetInputDir());

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

//-------------------------------------------------------

// Go through local nsf files and process them
int CLdifProc::PerformMigrationLocal(_TCHAR *inputDir)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLdifProc::PerformMigrationLocal");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	int retCode = MIGRATION_STEP_SUCCESS;
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	bool fFinished = FALSE;
	_TCHAR fullmask[1024];
	_TCHAR ldifFileFull[1024];
	UINT totalFiles = 0, totalFilesError = 0;

	_tcscpy(fullmask, inputDir);
	_tcscat(fullmask, _T("/"));
	_tcscat(fullmask, LDIFMASK);
//	_tprintf(_T("-->fullmask: %s\n"), fullmask);

	if (this->GetOnlyLdifFile() != NULL && _tcslen(this->GetOnlyLdifFile()) > 0) {
		_TCHAR outputdir[512];
		_tcscpy(outputdir, this->GetOutputDir());
		_tcscat(outputdir, _T("/"));
		PerformOneLdifMigration(outputdir, this->GetOnlyLdifFile(), &totalFiles, &totalFilesError);
	}
	else
	{
		// go through all files with .ldif extension in inputDir
		INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_OPEN, _T(""));
		hSearch = FindFirstFile(fullmask, &fd);
		if (hSearch != INVALID_HANDLE_VALUE) {
			while (!fFinished) { 
				if (fd.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_HIDDEN ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_COMPRESSED ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_NORMAL) {
					_TCHAR outputdir[512];
					_tcscpy(outputdir, this->GetOutputDir());
					_tcscat(outputdir, _T("/"));
					_tcscpy(ldifFileFull, inputDir);
					_tcscat(ldifFileFull, _T("/"));
					_tcscat(ldifFileFull, fd.cFileName);
					INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_FILE, _T(""));
//					_tprintf(_T("-->ldifFileFull: %s\n"), ldifFileFull);
					PerformOneLdifMigration(outputdir, ldifFileFull, &totalFiles, &totalFilesError);
				}
				if (!FindNextFile(hSearch, &fd)) {
					INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_CLOSE, _T(""));
					FindClose(hSearch);
					fFinished = TRUE;
				}
			}
		}
	}

	_tprintf(_T("\n"));
	INI_LogWriteWithResourcePrefixNum(IDS_LOG_TOTAL_FILES_DONE, totalFiles);
	INI_LogWriteWithResourcePrefixNum(IDS_LOG_TOTAL_FILES_ERROR, totalFilesError);

	if (totalFilesError > 0)
		retCode = MIGRATION_SUCCESS_SOME_ERRORS;

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

int CLdifProc::PerformOneLdifMigration(const _TCHAR *outputdir, const _TCHAR *ldifFile,
										   UINT *totalFiles, UINT *totalFilesError)
{
	int retCode = MIGRATION_STEP_SUCCESS;
	_TCHAR usage[1024];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLdifProc::PerformOneLdifMigration");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
//	::_tprintf(_TEXT("-->outputdir: %s\n"), outputdir);
	const _TCHAR *output_dir;
	const _TCHAR *outlookFile;
	bool local = (ldifFile != NULL ? true : false);

	if (local) {
		::INI_LogWriteWithResourcePrefix(IDS_LOG_LDIFFILE, (_TCHAR *)ldifFile);
	}

	UINT totalPersons = 0, totalPersonsDone = 0, 
		totalPersonsSkipped = 0, totalPersonsError = 0;

	LIST *lstelem = NULL, *pstname;

	LPMAPIFOLDER lpfOutF = NULL, lpfRootF = NULL;
	ULONG  ulObjType = 0;
	LPMESSAGE lpmResM = NULL;
	HRESULT hr;
	wchar_t fullPSTName[512];

    // Open ldif file.

	FILE *hLdifFile = _tfopen(ldifFile, _T("r"));
	if (hLdifFile == NULL)
		goto cleanup;

	output_dir = GetOutputDir();
	if (CDirUtils::MakeDir(output_dir) != 0) {
		::LoadString(hInst, IDS_ERR_DIR_NOT_CREATED1, usage, 512);
		::_tprintf(usage, output_dir);
		goto cleanup;
	}

	outlookFile = GetOutlookFile();
	if (outlookFile != NULL && _tcslen(outlookFile) > 0) {
		pstname = listSuppl->LST_LookupAdd(&lst, ID_OPT_PSTFILENAME, MAX_LINE);
		_tcscpy(pstname->data, outlookFile);
		_TCHAR *lastDot = _tcsrchr(pstname->data, '.');
		if (lastDot != NULL)
			*lastDot = _T('\0');
	}
	else
		CreateOutlookFile((_TCHAR *)outputdir, NULL, 1, false, (_TCHAR *)ldifFile);

	if (pses != NULL) {
		ULRELEASE(pmdb);
		pses->Logoff((ULONG)NULL, (ULONG)NULL, 0);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_LOGOFF, _T(""));

		ULRELEASE(pses);
		MAPIUninitialize();
		pmdb = NULL;
		pses = NULL;
	}

	// First - delete same profile if exists
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECHECK, PROFILE_NAME);
	if (!DeleteProfileWithIProfAdmin(PROFILE_NAME)) {
		retCode = MIGRATION_ERR_FAILED;
		goto cleanup;
	}

	// Create MAPI profile
	pstname = listSuppl->LST_Lookup(&lst, ID_OPT_PSTFILENAME);
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PSTMASK, pstname->data);
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECR, PROFILE_NAME);
	if (outlookFile != NULL && _tcslen(outlookFile) > 0) {
		if (!CreateProfileWithIProfAdmin(PROFILE_NAME, NULL, pstname->data, fullPSTName)) {
			retCode = MIGRATION_ERR_FAILED;
			goto cleanup;
		}
	}
	else {
		if (!CreateProfileWithIProfAdmin(PROFILE_NAME, (_TCHAR *)outputdir, pstname->data, fullPSTName)) {
			retCode = MIGRATION_ERR_FAILED;
			goto cleanup;
		}
	}
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECR, _T("-->Done"));
	if (local) {
		_tcscpy(pstname->data, _T(""));
	}

	if (pses == NULL) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_INIT, _T("-->1"));
		MAPIInitialize(NULL);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_INIT, _T("-->2"));
//		printf("-->here 00\n");
#if defined(UNICODE)
		if (FAILED(hr = MAPILogonEx((ULONG)NULL, PROFILE_NAME, NULL,
			MAPI_UNICODE | MAPI_EXTENDED | MAPI_EXPLICIT_PROFILE |	MAPI_NEW_SESSION, &pses))) {
#else
		if (FAILED(hr = MAPILogonEx((ULONG)NULL, PROFILE_NAME, NULL,
			MAPI_EXTENDED | MAPI_EXPLICIT_PROFILE |	MAPI_NEW_SESSION, &pses))) {
#endif
//			item2 = LST_LookupAdd(&lst, ID_OPT_TEXT2, MAX_LINE);
//			LoadString(hInst, IDS_PROFILE_NOTOPEN, item2->data, MAX_LINE);
//				MessageBox(hDlg, PROFILE_NAME, item2->data, MB_OK);
			::INI_LogWriteWithResourcePrefix(IDS_PROFILE_NOTOPEN, PROFILE_NAME);
			return FALSE;
		}
	}

//	printf("-->here 01\n");

	if (pmdb == NULL) {
//		printf("-->here 02\n");
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENSTORE, _T("-->1"));
		pmdb = OpenDefaultStore(pses);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENSTORE, _T("-->2"));

		// Open Root Folder first, i.e. "Personal Folders"
//							pmdb->lpVtbl->OpenEntry(pmdb, 0, (ULONG)NULL, NULL, MAPI_MODIFY, &ulObjType, (LPUNKNOWN FAR *)&lpfRootF);
//							spvMsg.ulPropTag = PR_DISPLAY_NAME;
//							spvMsg.Value.lpszA = "Renamed Folder";
//							HrSetOneProp((LPMAPIPROP)lpfRootF, &spvMsg);
//							ULRELEASE(lpfRootF);
	}

	// Open "Top Of Personal Folders" folder
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENFOLDER, _T("-->1"));
	lpfRootF = OpenMsgStoreFolderByName(pmdb, MAPI_FOLDER_TOP);
	if (lpfRootF == NULL)
		INI_LogWrite(_T("-->lpfRootF == NULL"));
	else
		INI_LogWrite(_T("-->lpfRootF != NULL"));
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENFOLDER, _T("-->2"));

	// Delete default "Sent Items" folder
	// DeleteFolder(lpfRootF, MAPI_FOLDER_SENT_ITEMS); --> leave it, cause of columns order
	// Delete default "Outbox" folder
//	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_DELETEOUTBOX, _T("-->1"));
//	DeleteFolder(lpfRootF, MAPI_FOLDER_SENT_OUTBOX);
//	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_DELETEOUTBOX, _T("-->2"));
	//???
//	{
//		SizedSPropTagArray(4, cols) = { 4, {PR_VALID_FOLDER_MASK, PR_IPM_OUTBOX_ENTRYID, PR_IPM_SENTMAIL_ENTRYID, PR_IPM_WASTEBASKET_ENTRYID} };
//		ULONG pcount; 
//		SPropValue *props;
//		hr = pmdb->GetProps((SPropTagArray*)&cols, 0, &pcount, &props);
//		if (FAILED(hr))
//			return FALSE;
//		LONG mask; 
//		if (props[0].ulPropTag == PR_VALID_FOLDER_MASK) 
//			mask=props[0].Value.ul; 
//		else 
//			mask=0;
//		INI_LogWriteWithResourcePrefixNum(IDS_LOG_MAPI_DELETEOUTBOX, mask, true);
//
//		LPSPropValue	pStorePropValues		= NULL;
//		UINT			uSize			= 1;
//		SCODE			sCode			= S_OK;
//		UINT			c				= 1;
//
//		//	Grab memory for a store property array
//		sCode = MAPIAllocateBuffer(uSize * sizeof(SPropValue),(void **)&pStorePropValues);
//
//		pStorePropValues[0].ulPropTag = PR_VALID_FOLDER_MASK;
//		pStorePropValues[0].Value.ul = FOLDER_IPM_SUBTREE_VALID | FOLDER_IPM_INBOX_VALID |
//			FOLDER_IPM_WASTEBASKET_VALID | FOLDER_COMMON_VIEWS_VALID | FOLDER_FINDER_VALID;
//
//		pmdb->SetProps((ULONG)c, pStorePropValues, NULL);
//
//		MAPIFreeBuffer(pStorePropValues);
//
//		SizedSPropTagArray(1, delcols) = { 1, {PR_IPM_OUTBOX_ENTRYID} };
//		hr = pmdb->DeleteProps((SPropTagArray*)&delcols, NULL);
//
//	}

	// Delete default "Deleted Items" folder --> created again after open
	//DeleteFolder(lpfRootF, FOLDER_DELETED_ITEMS);
	// Delete default "Drafts" folder --> created again after open
	//DeleteFolder(lpfRootF, MAPI_FOLDER_DRAFTS);

	INI_LogWrite(_T("-->before ProcessLdifFile"));
	ProcessLdifFile(hLdifFile, lpfRootF, &totalPersons, &totalPersonsDone, 
		&totalPersonsSkipped, &totalPersonsError);
	INI_LogWrite(_T("-->after ProcessLdifFile"));

	_tprintf(_T("\n"));
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_PERSONS_FILE, totalPersons, (_TCHAR *)ldifFile, true);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_PERSONS_FILE_DONE, totalPersonsDone, fullPSTName, true);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_PERSONS_FILE_SKIPPED, totalPersonsSkipped, fullPSTName, true);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_PERSONS_FILE_ERROR, totalPersonsError, fullPSTName, true);

	if (local) {
		(*totalFiles) += 1;
		if (totalPersonsError > 0)
			(*totalFilesError) += 1;
	}
//	printf("Resolved folders size: %d\n", resolvedFolders.getLength());

//	resolvedFolders.Print();

//	FilterFolders(resolvedFolders, mailbox->GetFolders());
//	if (m_verbose) {
//		resolvedFolders.Print();
//	}

cleanup:
	if (hLdifFile != NULL) {
//		MailCloseMessageFile(hMessageFile);
		fclose(hLdifFile);
		hLdifFile = NULL;
	}

	if (pses != NULL) {
		ULRELEASE(pmdb);
		pses->Logoff((ULONG)NULL, (ULONG)NULL, 0);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_LOGOFF, _T(""));

		ULRELEASE(pses);
		MAPIUninitialize();
		pmdb = NULL;
		pses = NULL;
	}
	// Delete profile if still exists
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILEDELETED, _T("-->10"));
	DeleteProfileWithIProfAdmin(PROFILE_NAME);
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILEDELETED, _T("-->11"));

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

int CLdifProc::ProcessOnePerson(CLdifRecord *ldifRecord, LPMAPIFOLDER lpfOutF,
								unsigned int *totalPersonsSkipped, unsigned int *totalPersonsDone)
{
	int retCode = MIGRATION_STEP_SUCCESS;
	_TCHAR usage[1024];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CLdifProc::ProcessOneMessage");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	LPMESSAGE		lpMessage = NULL;
	LPSPropValue	pContactPropValues		= NULL;
	UINT			uSize			= 200;
	SCODE			sCode			= S_OK;
	UINT			c				= 0;

	CPersonItem* item = NULL;
	int i = 0;

	// Check if attribute specified for PR_MESSAGE_CLASS
	// in XML config actually has value in ldifRecord list
	_TCHAR attr[256];
	_TCHAR *attrValue;
	int attValueLen = 0;
	attr[0] = _T('\0');
	theParentApp->GetMessageClassAttr(attr);
	if (_tcslen(attr) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CLASS_ATTR_EMPTY, _T(""));
		(*totalPersonsSkipped) += 1;
		goto cleanup;
	}

	attrValue = ldifRecord->Lookup(attr, true);
	if (attrValue == NULL || _tcslen(attrValue) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CLASS_ATTR_EMPTY, _T(""));
		(*totalPersonsSkipped) += 1;
		goto cleanup;
	}

	// try trim right spaces in case we have only them
	attValueLen = _tcslen(attrValue);
	for (i=attValueLen-1; i>=0; i--) {
		if (attrValue[i] == _T(' '))
			attrValue[i] = _T('\0');
		else
			break;
	}
	if (_tcslen(attrValue) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CLASS_ATTR_EMPTY, _T(""));
		(*totalPersonsSkipped) += 1;
		goto cleanup;
	}

	// In addition check cn to be not empty
	attrValue = ldifRecord->Lookup(_T("cn"), true);
	if (attrValue == NULL || _tcslen(attrValue) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CN_ATTR_EMPTY, _T(""));
		(*totalPersonsSkipped) += 1;
		goto cleanup;
	}

	// try trim right spaces in case we have only them
	attValueLen = _tcslen(attrValue);
	for (i=attValueLen-1; i>=0; i--) {
		if (attrValue[i] == _T(' '))
			attrValue[i] = _T('\0');
		else
			break;
	}
	if (_tcslen(attrValue) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CN_ATTR_EMPTY, _T(""));
		(*totalPersonsSkipped) += 1;
		goto cleanup;
	}


//	char messageItem[1024];
//	WORD messageItemLen;

//    char        Str[MAXSPRINTF+1];
//    WORD        StrLength = 0;

	if (pses != NULL) {
		if (FAILED(lpfOutF->CreateMessage(
			NULL,
			MAPI_DEFERRED_ERRORS,
			&lpMessage))) {
			INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_NOTCREATED, _T(""));
			return -1;
		}
		else {
			INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T(""));
		}

		//	Grab memory for a message property array
		sCode = MAPIAllocateBuffer(uSize * sizeof(SPropValue),(void **)&pContactPropValues);
	}
	else {
		return -1;
	}

//	printf("-->one 000\n");
//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->1"));
	theParentApp->resetEntryListIndex();
	theParentApp->InitializePersonItemList();
//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->2"));
#ifdef DEBUG_LOG
	_tprintf(_T("-->800\n"));
#endif
	while ((item = theParentApp->GetNextPersonItem()) != NULL) {
#ifdef DEBUG_LOG
		_tprintf(_TEXT("item name: %s, item value: %s\n"), item->GetName(), item->GetValue());
#endif
//		_tprintf(_T("-->one 00 %s\n"), item->GetName());
		theParentApp->resetTransformerListIndex();
#ifdef DEBUG_LOG
		_tprintf(_T("-->801\n"));
#endif
		ApplyTransformers(ldifRecord, item, lpfOutF, lpMessage, pContactPropValues, &c);
#ifdef DEBUG_LOG
		_tprintf(_T("-->802\n"));
#endif
		delete(item);
//		_tprintf(_T("-->one 001\n"));
	}
#ifdef DEBUG_LOG
	_tprintf(_T("-->899\n"));
#endif
//	printf("-->one 00 %d\n", c);

	// check if we have PR_MESSAGE_DELIVERY_TIME set ???
	//CheckAndMessageDeliveryTime(hNote, lpMessage, pContactPropValues, &c);

//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->3"));
	// now process message sender
	INI_LogWrite(_T("-->PutMessageSender"));
	PutMessageSender(ldifRecord, lpfOutF, lpMessage, pContactPropValues, &c);
	INI_LogWrite(_T("<--PutMessageSender"));

//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->4"));
//	printf("-->one 01 %d\n", c);

//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->5"));
//	printf("-->one 02\n");

//	INI_LogWrite(_T("-->500"));
	lpMessage->SetProps((ULONG)c, pContactPropValues, NULL);
//	INI_LogWrite(_T("-->501"));
	c = 0;

//	printf("-->one 03\n");

	if (FAILED(lpMessage->SaveChanges(0))) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_MSG_SAVEFAILED, _T(""));
	}
	else {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_MSG_SAVED, _T(""));
	}
//	INI_LogWrite(_T("-->502"));

	MAPIFreeBuffer(pContactPropValues);

	(*totalPersonsDone) += 1;

cleanup:
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

//	INI_LogWrite(_T("-->503"));
	return retCode;
}

void CLdifProc::PutMessageSender(CLdifRecord *ldifRecord, LPMAPIFOLDER lpFolder, LPMESSAGE lpMessage, LPSPropValue pContactPropValues, UINT *c)
{
	LPADRBOOK		pAddrBook;
	ULONG           cbEid           = 0;          // number of bytes in entry identifier
	LPENTRYID       lpEid           = NULL;       // entry identifier
	LIST *prSenderName = NULL;
	UINT cBefore = *c;

	CPersonItem *item = NULL;
	theParentApp->resetEntryListIndex();
//	int j = 0;
//	_TCHAR buf[64];
//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->3-0"));
#ifdef DEBUG_LOG
	_tprintf(_T("-->1 Sender\n"));
#endif
	while ((item = theParentApp->GetNextPersonItem(CONDITION_SENDER)) != NULL) {
//		_tprintf(_T("-->2 Sender\n"));
//		j++;
//		buf[0] = _T('\0');
//		wsprintf(buf, _T("-->trans: %d %s"), j, item->GetName());
//		INI_LogWrite(buf);
#ifdef DEBUG_LOG
		_tprintf(_TEXT("item name: %s, item value: %s\n"), item->GetName(), item->GetValue());
#endif
		theParentApp->resetTransformerListIndex();
		ApplyTransformers(ldifRecord, item, lpFolder, lpMessage, pContactPropValues, c);
		delete(item);
	}
#ifdef DEBUG_LOG
	_tprintf(_T("-->2 Sender\n"));
#endif

//	printf("-->PutMessageSender %d\n", *c);

//	INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->31"));
	for (unsigned int i=0; i<*c; i++) {
		if (pContactPropValues[i].ulPropTag == PR_SENDER_NAME) {
			prSenderName = listSuppl->LST_Lookup(&lst, ID_OPT_PROPBASE + i);
			break;
		}
	}

	if (prSenderName != NULL) {
		pses->OpenAddressBook (0, NULL, AB_NO_DIALOG, &pAddrBook);

#if defined(UNICODE)
		pAddrBook->CreateOneOff(
			prSenderName->data,         // PR_DISPLAY_NAME
			L"SMTP",   // PR_ADDRTYPE
			prSenderName->data,         // PR_EMAIL_ADDRESS
			MAPI_UNICODE,              // flags
			&cbEid,         // #bytes in entry ID
			&lpEid);        // entry identifier
#else
		pAddrBook->CreateOneOff(
			prSenderName->data,         // PR_DISPLAY_NAME
			(LPTSTR) "SMTP",   // PR_ADDRTYPE
			prSenderName->data,         // PR_EMAIL_ADDRESS
			0,              // flags
			&cbEid,         // #bytes in entry ID
			&lpEid);        // entry identifier
#endif

		pContactPropValues[*c  ].ulPropTag = PR_SENDER_ENTRYID;
		pContactPropValues[*c  ].Value.bin.cb = cbEid;
		pContactPropValues[(*c)++].Value.bin.lpb = (BYTE *) lpEid;

//        MAPIAllocateMore(
//                    cbEid,              // # bytes to add
//                    pContactPropValues,
//                    (VOID **) &(pContactPropValues[*c].Value.bin.lpb));         // entry identifier buffer
//		CopyMemory(
//			   pContactPropValues[*c].Value.bin.lpb,   // destination buffer
//			   lpEid,                               // source buffer
//			   cbEid);                              // count bytes
//		(*c)++;

		pContactPropValues[*c  ].ulPropTag = PR_SENT_REPRESENTING_ENTRYID;
		pContactPropValues[*c  ].Value.bin.cb = cbEid;
		pContactPropValues[(*c)++].Value.bin.lpb = (BYTE *) lpEid;

//        MAPIAllocateMore(
//                    cbEid,              // # bytes to add
//                    pContactPropValues,
//                    (VOID **) &(pContactPropValues[*c].Value.bin.lpb));         // entry identifier buffer
//		CopyMemory(
//			   pContactPropValues[*c].Value.bin.lpb,   // destination buffer
//			   lpEid,                               // source buffer
//			   cbEid);                              // count bytes
//		(*c)++;

//		INI_LogWriteWithResourcePrefix(IDS_LOG_MSGEMPTY_CREATED, _T("-->33"));

		ULRELEASE(pAddrBook);
	}
	else {
		*c = cBefore;
	}

//	lpMessage->SetProps((ULONG)c, pContactPropValues, NULL);
//	*c = 0;
}

int CLdifProc::ApplyTransformers(CLdifRecord *ldifRecord, CPersonItem *item, 
									 LPMAPIFOLDER lpFolder, LPMESSAGE lpMessage, LPSPropValue pContactPropValues, UINT *c)
{
	int error = FALSE;

	_TCHAR formattedValue[512];
	bool ignoreOutput = false;
	bool placeholderLeft = false;
	int timesTransformed = 0;

	_TCHAR inputValue[512];
	_TCHAR* ptr = inputValue;
	_tcscpy(inputValue, item->GetValue());
	_TCHAR* ptrprev = inputValue;
	CEntryTransformer *transformerEntry;

	LIST *propValue;
	_TCHAR *attr;

	MAPINAMEID  rgnmid[ulContactProps];
	LPMAPINAMEID rgpnmid[ulContactProps];
	LPSPropTagArray lpNamedPropTags = NULL;

	ULONG i = 0;
	for (i = 0 ; i < ulContactProps ; i++)
	{
		rgnmid[i].lpguid = (LPGUID)&PSETID_Address;
		rgnmid[i].ulKind = MNID_ID;
		rgnmid[i].Kind.lID = aulContactProps[i].pID;
#ifdef DEBUG_LOG
		if (i == 8) {
			_tprintf(_T("aulContactProps[i].pID: %lx\n"), aulContactProps[i].pID);
		}
#endif
		rgpnmid[i] = &rgnmid[i];
	}

	if (FAILED(lpFolder->GetIDsFromNames(
		ulContactProps,
		(LPMAPINAMEID*) &rgpnmid,
		MAPI_CREATE, //NULL,
		&lpNamedPropTags))) {
		::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_CANT_GET_ID, _T(""));
		return TRUE;
	}
#ifdef DEBUG_LOG
	_tprintf(_T("lpNamedPropTags->aulPropTag[0]: %lx, ulContactProps: %d\n"),
		lpNamedPropTags->aulPropTag[0], ulContactProps);
	_tprintf(_T("lpNamedPropTags->aulPropTag[namePropertyIdx]: %lx\n"),
		lpNamedPropTags->aulPropTag[8]);
#endif

	while((transformerEntry = theParentApp->getNextEntryTransformer()) != NULL) {
		int offset = 0;
		int valueLen = _tcslen(inputValue);
		bool eatNextComma = false;

		const _TCHAR* transformerName = transformerEntry->GetName();
		const _TCHAR* placeholder = transformerEntry->GetPlaceholder();
#ifdef DEBUG_LOG
			::_tprintf(_TEXT("transformerName: %s, placeholder: %s\n"), transformerName, placeholder);
#endif
		int placeholderLen = _tcslen(placeholder);

		while ((ptr = _tcsstr(ptr, placeholder)) != NULL) {
	
#ifdef DEBUG_LOG
			_tprintf(_T("ptr: %s\n"), ptr);
#endif
			_tcsncpy(formattedValue+offset, ptrprev, ptr - ptrprev);
			offset += (ptr - ptrprev);
			int placeholderValueLen = 0;

			if (transformerEntry->IsLdifAttribute()) {
#ifdef DEBUG_LOG
				_tprintf(_T("-->999\n"));
#endif
				_TCHAR outputW[512];

				attr = ldifRecord->Lookup((_TCHAR *)transformerName, true);
				
#ifdef DEBUG_LOG
				_tprintf(_T("-->9990\n"));
#endif
				if (attr == NULL) {
#ifdef DEBUG_LOG
					_tprintf(_T("-->998\n"));
#endif
					error = TRUE;
					placeholderLeft = true;
					ignoreOutput = true;
				}

				if (error) {
					goto cleanup;
				}

				if (attr != NULL && _tcslen(attr) == 0) {
#ifdef DEBUG_LOG
					_tprintf(_T("-->997\n"));
#endif
					ignoreOutput = true;
				}

				if (attr != NULL && _tcslen(attr) > 0) {
#ifdef DEBUG_LOG
					_tprintf(_T("-->996\n"));
#endif
					_tcscpy(outputW, attr);
				}
#ifdef DEBUG_LOG
				_tprintf(_T("-->9999\n"));
#endif

				placeholderValueLen = _tcslen(outputW);
				if (placeholderValueLen == 0) {
					eatNextComma = true;
				}
				_tcsncpy(formattedValue+offset, outputW, placeholderValueLen);
				offset+=placeholderValueLen;
			}
			else {
				const _TCHAR* configSettingValue = NULL;
				configSettingValue = theParentApp->getConfigSettingValue(transformerName);
				placeholderValueLen = _tcslen(configSettingValue);
				_tcsncpy(formattedValue+offset, configSettingValue, placeholderValueLen);
				offset+=placeholderValueLen;
			}
			timesTransformed++;
			ptr = ptr + placeholderLen;
			ptrprev = ptr;
		}
		// remove comma or space after empty value
		if (ptrprev != _T('\0') && _tcslen(ptrprev)>0) {
			if ((*ptrprev == _T(',') && (eatNextComma || offset == 0)) ||
				(*ptrprev == _T(' ') && (eatNextComma || offset == 0))) {
				_tcsncpy(formattedValue+offset, ptrprev+1, _tcslen(ptrprev+1));
				*(formattedValue+offset+_tcslen(ptrprev+1)) = _T('\0');
			}
			else {
				_tcsncpy(formattedValue+offset, ptrprev, _tcslen(ptrprev));
				*(formattedValue+offset+_tcslen(ptrprev)) = _T('\0');
			}
		}
		else {
			*(formattedValue+offset) = _T('\0');
		}
#ifdef DEBUG_LOG
		_tprintf(_TEXT("transformerName: %s, formattedValue: %s\n"), transformerName, formattedValue);
#endif
		_tcscpy(inputValue, formattedValue);
		ptr = inputValue;
		ptrprev = ptr;
//		transformerEntry = theParentApp->getNextEntryTransformer();
		delete(transformerEntry);
	}

#ifdef DEBUG_LOG
		_tprintf(_T("-->001\n"));
#endif
		if (!ignoreOutput) {
#ifdef DEBUG_LOG
			_tprintf(_T("-->002\n"));
#endif
			if (timesTransformed == 0) {
#ifdef DEBUG_LOG
				_tprintf(_T("-->003\n"));
#endif
//				_ftprintf(f, mask2, attribute, value);
//				if (placeholderLeft) {
//					_tprintf(_T("-->placeholderLeft: %s\n"), item->GetName());
//				}
				if (placeholderLeft || item->GetName() == NULL || _tcslen(item->GetName()) == 0) {
#ifndef DONT_USE_OUTPUT_UNICODE
//					_ftprintf(outputFile, _TEXT("%s\r\n"), item->GetValue());
#else
//					wcstombs(valueCh, item->GetValue(), sizeof(valueCh));
//					fprintf(outputFile, "%s\r\n", valueCh);
#endif
				}
				else {
#ifndef DONT_USE_OUTPUT_UNICODE
//					_ftprintf(outputFile, _TEXT("%s: %s\r\n"), name, value);
#else
//					wcstombs(valueCh, item->GetValue(), sizeof(valueCh));
//					wcstombs(propertyCh, item->GetName(), sizeof(propertyCh));
//					fprintf(outputFile, "%s: %s\r\n", propertyCh, valueCh);
//					_tprintf(_T("-->ulPropTag %s\n"), (_TCHAR *)item->GetName());
					if (item->IsNamedProperty()) {
						ULONG namedPropertyHex = (ULONG)_tcstoul(item->GetName(), NULL, 16);
						int namePropertyIdx = GetNamedPropertyIdx(namedPropertyHex);
						if (namePropertyIdx != -1) {
							ULONG tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[namePropertyIdx], aulContactProps[namePropertyIdx].pType);
							pContactPropValues[*c  ].ulPropTag = tag;
						}
						else
							pContactPropValues[*c  ].ulPropTag = GetXMLPropTagValue((_TCHAR *)item->GetName());
					}
					else
						pContactPropValues[*c  ].ulPropTag = GetXMLPropTagValue((_TCHAR *)item->GetName());
					if (item->IsNumeric()) {
//						_tprintf(_T("-->Numeric-----------------------------!!!\n"));
//						long *p = &pContactPropValues[(*c)++].Value.l;
//						*p = item->GetValueNumeric();
						pContactPropValues[(*c)++].Value.l = item->GetValueNumeric();
//						_tprintf(_T("-->value %d, %ul\n"), item->GetValueNumeric(), *p);
//						_tprintf(_T("-->value %u\n"), item->GetValueNumeric());
					}
					else {
//						_tprintf(_T("-->*c %d\n"), *c);
						propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
						propValue->data[0] = '\0';
						_tcscpy(propValue->data, (_TCHAR *)item->GetValue());
						pContactPropValues[(*c)++].Value.lpszW = propValue->data;
//						pContactPropValues[(*c)++].Value.lpszW = (_TCHAR *)item->GetValue();
//						_tprintf(_T("-->value %s\n"), (_TCHAR *)item->GetValue());
						if (pContactPropValues[(*c)-1  ].ulPropTag == PR_SENDER_NAME &&
							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0)
							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_NAME, (_TCHAR *)item->GetValue());
						else if (pContactPropValues[(*c)-1  ].ulPropTag == PR_SENDER_EMAIL_ADDRESS &&
							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0)
							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_EMAIL, (_TCHAR *)item->GetValue());
//						else if (pContactPropValues[(*c)-1  ].ulPropTag == PR_SENDER_ADDRTYPE &&
//							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0) {
//							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_ADDRTYPE, (_TCHAR *)item->GetValue());
//							//pContactPropValues[(*c)-1].Value.lpszA = smtp;
//						}
					}
#endif
				}
			}
			else {
#ifdef DEBUG_LOG
				_tprintf(_T("-->004\n"));
				INI_LogWrite(_T("-->500"));
#endif
//				if (m_verbose){
//					::LoadString(hInst, IDS_PROCESSING_CHOICE, usage, 256);
//					::_tprintf(_TEXT("%s formattedValue: %s\n"), usage, formattedValue);
//				}
//				_ftprintf(f, mask2, attribute, formattedValue);
				if (item->GetName() == NULL || _tcslen(item->GetName()) == 0) {
#ifndef DONT_USE_OUTPUT_UNICODE
//					_ftprintf(outputFile, _TEXT("%s\r\n"), formattedValue);
#else
//					wcstombs(valueCh, formattedValue, sizeof(valueCh));
//					fprintf(outputFile, "%s\r\n", valueCh);
#endif
				}
				else if (_tcscmp(item->GetName(), _T("PR_BODY")) == 0) {
					_TCHAR *offsetStr = ldifRecord->Lookup(DESCRIPTION_ATTR, true);
					long offsetAttr = _ttol(offsetStr);
					ProcessBody(lpMessage, offsetAttr);
				}
				else {
#ifdef DEBUG_LOG
					INI_LogWrite((_TCHAR *)item->GetName());
					INI_LogWrite((_TCHAR *)formattedValue);
					INI_LogWrite(_T("<--500"));
#endif
#ifndef DONT_USE_OUTPUT_UNICODE
//					_ftprintf(outputFile, _TEXT("%s: %s\r\n"), item->GetName(), formattedValue);
#else
//					wcstombs(valueCh, formattedValue, sizeof(valueCh));
//					wcstombs(propertyCh, item->GetName(), sizeof(propertyCh));
//					fprintf(outputFile, "%s: %s\r\n", propertyCh, valueCh);
					if (item->IsNamedProperty()) {
						ULONG namedPropertyHex = (ULONG)_tcstoul(item->GetName(), NULL, 16);
						int namePropertyIdx = GetNamedPropertyIdx(namedPropertyHex);
#ifdef DEBUG_LOG
						_tprintf(_T("Named property: %lx, %d\n"), namedPropertyHex, namePropertyIdx);
#endif
						if (namePropertyIdx != -1) {
							ULONG tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[namePropertyIdx], aulContactProps[namePropertyIdx].pType);
							pContactPropValues[*c  ].ulPropTag = tag;
						}
						else
							pContactPropValues[*c  ].ulPropTag = GetXMLPropTagValue((_TCHAR *)item->GetName());
					}
					else
						pContactPropValues[*c  ].ulPropTag = GetXMLPropTagValue((_TCHAR *)item->GetName());
#ifdef DEBUG_LOG
					_tprintf(_T("-->formattedValue %s, tag: %lx, tag Email1Address: %lx\n"), 
						formattedValue, pContactPropValues[*c  ].ulPropTag, 
						CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1EmailAddress],PT_UNICODE));
#endif
					if (item->IsNumeric()) {
						int value = _ttoi(formattedValue);
						if (pContactPropValues[*c  ].ulPropTag == PR_IMPORTANCE) {
							if (value == 1) // high
								value = IMPORTANCE_HIGH;
							else if (value == 3) //low
								value = IMPORTANCE_LOW;
							else
								value = IMPORTANCE_NORMAL;
						}
						pContactPropValues[(*c)++].Value.l = value;

					}
					else {
//						_TCHAR msg[512];
						if (pContactPropValues[*c  ].ulPropTag == PR_SENDER_NAME &&
							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0)
							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_NAME, formattedValue);
						else if (pContactPropValues[*c  ].ulPropTag == PR_SENDER_EMAIL_ADDRESS &&
							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0)
							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_EMAIL, formattedValue);
						else if (pContactPropValues[*c  ].ulPropTag == CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2EmailAddress],PT_UNICODE)) {
							_TCHAR *ptrName = NULL;
							ULONG           cbEid           = 0;          // number of bytes in entry identifier
							LPENTRYID       lpEid           = NULL;       // entry identifier

							for (unsigned int propIdx = 0; propIdx < (*c); propIdx++) {
//								if (pContactPropValues[propIdx].ulPropTag == PR_CARD_NAME2) {
								if (pContactPropValues[propIdx].ulPropTag == 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2DisplayName],PT_UNICODE)) {
									ptrName = pContactPropValues[propIdx].Value.lpszW;
									break;
								}
							}
							if (ptrName != NULL) {
								pses->OpenAddressBook (0, NULL, AB_NO_DIALOG, &pAddrBook);
//								INI_LogWrite(_T("-->504"));
								pAddrBook->CreateOneOff(
									ptrName,         // PR_DISPLAY_NAME
									L"SMTP",   // PR_ADDRTYPE
									formattedValue,         // PR_EMAIL_ADDRESS
									MAPI_UNICODE,              // flags
									&cbEid,         // #bytes in entry ID
									&lpEid);        // entry identifier

//								pContactPropValues[*c  ].ulPropTag = PR_CARD_ENTRYID2;
								pContactPropValues[*c  ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2OriginalEntryID],PT_BINARY);
								pContactPropValues[*c  ].Value.bin.cb = cbEid;
								propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
								propValue->data[0] = '\0';
								memcpy(propValue->data, lpEid, cbEid);
								pContactPropValues[(*c)++].Value.bin.lpb = (BYTE *) propValue->data;

								ULRELEASE(pAddrBook);

								pContactPropValues[*c  ].ulPropTag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2EmailAddress],PT_UNICODE);
							}
						}
//						else if (pContactPropValues[*c  ].ulPropTag == PR_CARD_MAIL) {
						else if (pContactPropValues[*c  ].ulPropTag == 
							CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1EmailAddress],PT_UNICODE)) {
#ifdef DEBUG_LOG
							INI_LogWrite(_T("-->501"));
#endif
/*
							MAPINAMEID  rgnmid[ulContactProps];
							LPMAPINAMEID rgpnmid[ulContactProps];
							LPSPropTagArray lpNamedPropTags = NULL;

							ULONG i = 0;
							for (i = 0 ; i < ulContactProps ; i++)
							{
								rgnmid[i].lpguid = (LPGUID)&PSETID_Address;
								rgnmid[i].ulKind = MNID_ID;
								rgnmid[i].Kind.lID = aulContactProps[i];
								rgpnmid[i] = &rgnmid[i];
							}

							lpFolder->GetIDsFromNames(
								ulContactProps,
								(LPMAPINAMEID*) &rgpnmid,
								NULL,
								&lpNamedPropTags);

							ULONG tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidContactItemData],PT_MV_LONG);
							_stprintf(msg, _T("-->tag p_PidLidContactItemData: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidFileUnderList],PT_MV_LONG);
							_stprintf(msg, _T("-->tag p_PidLidFileUnderList: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidAddressBookProviderEmailList],PT_MV_LONG);
							_stprintf(msg, _T("-->tag p_PidLidAddressBookProviderEmailList: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidAddressBookProviderArrayType],PT_LONG);
							_stprintf(msg, _T("-->tag p_PidLidAddressBookProviderArrayType: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidFileUnder],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidFileUnder: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidFileUnderId],PT_LONG);
							_stprintf(msg, _T("-->tag p_PidLidFileUnderId: %lx<--"), tag);
							INI_LogWrite(msg);

							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1DisplayName],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail1DisplayName: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1AddressType],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail1AddressType: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1EmailAddress],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail1EmailAddress: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1OriginalDisplayName],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail1OriginalDisplayName: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1OriginalEntryID],PT_BINARY);
							_stprintf(msg, _T("-->tag p_PidLidEmail1OriginalEntryID: %lx<--"), tag);
							INI_LogWrite(msg);

							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2DisplayName],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail2DisplayName: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2AddressType],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail2AddressType: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2EmailAddress],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail2EmailAddress: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2OriginalDisplayName],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidEmail2OriginalDisplayName: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail2OriginalEntryID],PT_BINARY);
							_stprintf(msg, _T("-->tag p_PidLidEmail2OriginalEntryID: %lx<--"), tag);
							INI_LogWrite(msg);

							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressStreet],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressStreet: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressCity],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressCity: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressState],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressState: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressPostalCode],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressPostalCode: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressCountry],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressCountry: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidWorkAddressCountryCode],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidWorkAddressCountryCode: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidHomeAddress],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidHomeAddress: %lx<--"), tag);
							INI_LogWrite(msg);

							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidMyLidUserField1],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidMyLidUserField1: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidMyLidUserField2],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidMyLidUserField2: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidMyLidUserField3],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidMyLidUserField3: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidMyLidUserField4],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidMyLidUserField4: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidHtml],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidLidHtml: %lx<--"), tag);
							INI_LogWrite(msg);
							tag = CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidMyInstMsg],PT_UNICODE);
							_stprintf(msg, _T("-->tag p_PidMyInstMsg: %lx<--"), tag);
							INI_LogWrite(msg);
*/
							_TCHAR *ptrName = NULL;
							ULONG           cbEid           = 0;          // number of bytes in entry identifier
							LPENTRYID       lpEid           = NULL;       // entry identifier

							for (unsigned int propIdx = 0; propIdx < (*c); propIdx++) {
//								if (pContactPropValues[propIdx].ulPropTag == PR_CARD_NAME) {
								if (pContactPropValues[propIdx].ulPropTag == 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1DisplayName],PT_UNICODE)) {
									ptrName = pContactPropValues[propIdx].Value.lpszW;
									break;
								}
							}
#ifdef DEBUG_LOG
							INI_LogWrite(_T("-->502"));
#endif
							if (ptrName != NULL) {
								_TCHAR msg[512];
#ifdef DEBUG_LOG
								INI_LogWrite(_T("-->503"));
								_stprintf(msg, _T("-->ptrName: %s<--"), ptrName);
								INI_LogWrite(msg);
#endif
								_stprintf(msg, _T("-->formattedValue: %s<--"), formattedValue);
								INI_LogWrite(msg);

								pses->OpenAddressBook (0, NULL, AB_NO_DIALOG, &pAddrBook);
//								INI_LogWrite(_T("-->504"));
								pAddrBook->CreateOneOff(
									ptrName,         // PR_DISPLAY_NAME
									L"SMTP",   // PR_ADDRTYPE
									formattedValue,         // PR_EMAIL_ADDRESS
									MAPI_UNICODE,              // flags
									&cbEid,         // #bytes in entry ID
									&lpEid);        // entry identifier

//								INI_LogWrite(_T("-->505"));
//								pContactPropValues[*c  ].ulPropTag = PR_CARD_ENTRYID;
								pContactPropValues[*c  ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1OriginalEntryID],PT_BINARY);
								pContactPropValues[*c  ].Value.bin.cb = cbEid;
//								_stprintf(msg, _T("-->cbEid: %d"), cbEid);
//								INI_LogWrite(msg);
								propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
								propValue->data[0] = '\0';
								memcpy(propValue->data, lpEid, cbEid);
								pContactPropValues[(*c)++].Value.bin.lpb = (BYTE *) propValue->data;

								ULRELEASE(pAddrBook);

								// Special named properties go here

								//spvProps[p_PidLidAddressBookProviderArrayType].Value.l = 0x0000001; // Email1 is defined
//								pContactPropValues[*c    ].ulPropTag = PR_ADDRESS_BOOK_PROVIDER_ARRAY_TYPE;
								pContactPropValues[*c    ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidAddressBookProviderArrayType], PT_LONG);
								pContactPropValues[(*c)++].Value.l = 0x0000001; // Email1 is defined

								//spvProps[p_PidLidAddressBookProviderEmailList].Value.MVl.cValues = 1;
								//LONG lAddressBookProviderEmail[1];
								//lAddressBookProviderEmail[0] = 0x00000000; // Email1 is defined
								//spvProps[p_PidLidAddressBookProviderEmailList].Value.MVl.lpl = lAddressBookProviderEmail;
//								pContactPropValues[*c    ].ulPropTag = PR_ADDRESS_BOOK_PROVIDER_EMAIL_LIST;
								pContactPropValues[*c    ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidAddressBookProviderEmailList],PT_MV_LONG);
								pContactPropValues[*c    ].Value.MVl.cValues = 1;
								LONG lAddressBookProviderEmail[1];
								lAddressBookProviderEmail[0] = 0x00000000; // Email1 is defined
								propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
								propValue->data[0] = '\0';
								memcpy(propValue->data, lAddressBookProviderEmail, sizeof(lAddressBookProviderEmail));
								pContactPropValues[(*c)++].Value.MVl.lpl = (LONG *)propValue->data;

								//spvProps[p_PidLidFileUnderList].Value.MVl.cValues = 5;
								//LONG lFileUnder[5];
								//lFileUnder[0] = 0x00008017; // Surname, Given Middle
								//lFileUnder[1] = 0x00008037; // Given Middle Surname Generation
								//lFileUnder[2] = 0x00003a16; // Company Name (PR_COMPANY_NAME)
								//lFileUnder[3] = 0x00008019; // Surname, Given Middle\r\nCompany Name
								//lFileUnder[4] = 0x00008018; // Company Name\r\nSurname, Given Middle
								//spvProps[p_PidLidFileUnderList].Value.MVl.lpl = lFileUnder;
//								pContactPropValues[*c    ].ulPropTag = PR_FILE_UNDER_LIST;
								pContactPropValues[*c    ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidFileUnderList],PT_MV_LONG);
								pContactPropValues[*c    ].Value.MVl.cValues = 5;
								LONG lFileUnder[5];
								lFileUnder[0] = 0x00008017; // Surname, Given Middle
								lFileUnder[1] = 0x00008037; // Given Middle Surname Generation
								lFileUnder[2] = 0x00003a16; // Company Name (PR_COMPANY_NAME)
								lFileUnder[3] = 0x00008019; // Surname, Given Middle\r\nCompany Name
								lFileUnder[4] = 0x00008018; // Company Name\r\nSurname, Given Middle
								propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
								propValue->data[0] = '\0';
								memcpy(propValue->data, lFileUnder, sizeof(lFileUnder));
								pContactPropValues[(*c)++].Value.MVl.lpl = (LONG *)propValue->data;

								//spvProps[p_PidLidFileUnderId].Value.l = 0x8017; // Surname, Given Middle
//								pContactPropValues[*c    ].ulPropTag = PR_FILE_UNDER_ID;
								pContactPropValues[*c    ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidFileUnderId],PT_LONG);
								pContactPropValues[(*c)++].Value.l = 0x8017; // Surname, Given Middle

								//spvProps[p_PidLidContactItemData].Value.MVl.cValues = 6;
								//LONG lContactItemData[6];
								//lContactItemData[0] = 0x00000002; // Display work address
								//lContactItemData[1] = 0x00008080; // Display Email 1 (PidLidEmail1DisplayName)
								//lContactItemData[2] = 0x3A08; // Display Business Telephone (PR_BUSINESS_TELEPHONE_NUMBER)
								//lContactItemData[3] = 0x3A09; // Display Home Telephone (PR_HOME_TELEPHONE_NUMBER)
								//lContactItemData[4] = 0x3A24; // Display Business Fax (PR_BUSINESS_FAX_NUMBER)
								//lContactItemData[5] = 0x3A1C; // Display Mobile Telephone (PR_MOBILE_TELEPHONE_NUMBER)
								//spvProps[p_PidLidContactItemData].Value.MVl.lpl = lContactItemData;
//								pContactPropValues[*c    ].ulPropTag = PR_CONTACT_ITEM_DATA;
								pContactPropValues[*c    ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidContactItemData],PT_MV_LONG);
								pContactPropValues[*c    ].Value.MVl.cValues = 6;
								LONG lContactItemData[6];
								lContactItemData[0] = 0x00000002; // Display work address
								lContactItemData[1] = 0x00008080; // Display Email 1 (PidLidEmail1DisplayName)
								lContactItemData[2] = 0x3A08; // Display Business Telephone (PR_BUSINESS_TELEPHONE_NUMBER)
								lContactItemData[3] = 0x3A09; // Display Home Telephone (PR_HOME_TELEPHONE_NUMBER)
								lContactItemData[4] = 0x3A24; // Display Business Fax (PR_BUSINESS_FAX_NUMBER)
								lContactItemData[5] = 0x3A1C; // Display Mobile Telephone (PR_MOBILE_TELEPHONE_NUMBER)
								propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
								propValue->data[0] = '\0';
								memcpy(propValue->data, lContactItemData, sizeof(lContactItemData));
								pContactPropValues[(*c)++].Value.MVl.lpl = (LONG *)propValue->data;

//								pContactPropValues[*c  ].ulPropTag = PR_CARD_MAIL;
								pContactPropValues[*c  ].ulPropTag = 
									CHANGE_PROP_TYPE(lpNamedPropTags->aulPropTag[p_PidLidEmail1EmailAddress],PT_UNICODE);

							}
						}
//						else if (pContactPropValues[(*c) ].ulPropTag == PR_SENDER_ADDRTYPE &&
//							_tcscmp(item->GetCondition(), CONDITION_SENDER) == 0)
//							::INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FROM_ADDRTYPE, formattedValue);
	//					_tprintf(_T("-->*c %d\n"), *c);
						propValue = listSuppl->LST_LookupAdd(&lst, ID_OPT_PROPBASE + *c, 256);
						propValue->data[0] = '\0';
//						_stprintf(msg, _T("-->formattedValue: %s<--"), formattedValue);
//						INI_LogWrite(msg);
						_tcscpy(propValue->data, formattedValue);
						pContactPropValues[(*c)++].Value.lpszW = propValue->data;
					}
#endif
				}
			}
		}

cleanup:
	return error;
}

int CLdifProc::CreateOutlookFile(_TCHAR *targetdir, const _TCHAR *name, int how, 
								  bool splitName, _TCHAR *ldifFile)
{
	HGLOBAL rc;
	HRSRC src;
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR strID[MAX_LINE], fname[MAX_LINE], fullfname[MAX_LINE];
	DWORD size;
	FILE *f1;
	LIST *pstname;

	fname[0] = '\0';
	fullfname[0] = '\0';

//	_tprintf(_T("-->pstmask %s, name: %s\n"), pstmask->data, name);

//	INI_LogWrite(pstmask->data);
//		_tprintf(_T("-->1 %s\n"), ldifFile);
	_TCHAR ldifFileNamePart[512];
	_tcscpy(ldifFileNamePart, ldifFile);
	_TCHAR *lastSlash = _tcsrchr(ldifFileNamePart, '\\');
	_TCHAR *lastBackSlash = _tcsrchr(ldifFileNamePart, '/');
	_TCHAR *lastDot = _tcsrchr(ldifFileNamePart, '.');
	_TCHAR *pstDefaultSuffix = PST_DEFAULT_SUFFIX;
	if (lastDot != NULL)
		*lastDot = _T('\0');
	if (lastBackSlash != NULL && ((lastSlash != NULL && lastBackSlash > lastSlash) || (lastSlash == NULL)))
		lastSlash = lastBackSlash;
	if (lastSlash == NULL) {
		pstname = listSuppl->LST_LookupAdd(&lst, ID_OPT_PSTFILENAME, MAX_LINE);
		_tcscpy(pstname->data, ldifFileNamePart);
	}
	else {
		pstname = listSuppl->LST_LookupAdd(&lst, ID_OPT_PSTFILENAME, MAX_LINE);
		_tcscpy(pstname->data, lastSlash + 1);
	}
	wsprintf(fname, _T("%s%s%s"), targetdir, pstname->data, pstDefaultSuffix);
	wsprintf(strID, _T("#%d"), IDR_OUTLOOK_2003);
	if (how == 1) {
		src = FindResource(hInst, strID, _T("BINARY"));
		size = SizeofResource(hInst, src);
		rc = LoadResource(hInst, src);
		f1 = _tfopen(fname, _T("wb+"));
		fwrite(rc, size, 1, f1);
		fclose(f1);
	}
	else {
		if (_taccess(fname, 0) == 0)
			_tremove(fname);
	}
	return 1;
}

ULONG CLdifProc::GetXMLPropTagValue(_TCHAR *propTag)
{
	ULONG found = 0;
	for (int i=0; i<sizeof(PropTags)/sizeof(PropTag); i++) {
		if (_tcscmp((PropTags + i)->prop, propTag) == 0) {
			found = (PropTags + i)->value;
			break;
		}
	}
	// If we need to set special not well known MAPI properties
	if (!found) {
//		_tprintf(_T("-->before tcstol: %s\n"), propTag);
		ULONG l1 = (ULONG)_tcstoul(propTag, NULL, 16);
//		_tprintf(_T("-->after tcstol: %u\n"), l1);
		if (l1 > 0) found = l1;
	}
	return found;
}

int CLdifProc::ProcessLdifFile(FILE *hLdifFile, LPMAPIFOLDER lpfRootF, unsigned int *totalPersons, unsigned int *totalPersonsDone, 
	unsigned int *totalPersonsSkipped, unsigned int *totalPersonsError)
{
	int retCode = 0;
	_TCHAR ldifFileLine[256];
	_TCHAR ldifAttrName[256];
	_TCHAR ldifAttrValue[256];
	_TCHAR *ptr = NULL;
	CLdifRecord *ldifRecord = new CLdifRecord();
	LPMAPIFOLDER lpfOutF = NULL;
	_TCHAR msg[256];

	LPSPropValue	pFolderPropValues		= NULL;
	UINT			uSize			= 10;
	SCODE			sCode			= S_OK;
	UINT			c				= 0;
	_TCHAR			containerClass[64];
	_TCHAR			defPostDisplayName[64];
	_TCHAR			comment[64];
	_TCHAR			msgClass[128];

	INI_LogWrite(_T("-->before OpenMsgStoreFolderByName"));

	#if defined(UNICODE)
		if (FAILED(lpfRootF->CreateFolder(
			FOLDER_GENERIC,
			MAPI_FOLDER_CONTACTS,
			NULL,
			&IID_IMAPIFolder, //NULL,
//			OPEN_IF_EXISTS | MAPI_DEFERRED_ERRORS | MAPI_UNICODE,
			OPEN_IF_EXISTS | MAPI_UNICODE,
			&lpfOutF))) {
	#else
		if (FAILED(lpfRootF->CreateFolder(
			FOLDER_GENERIC,
			MAPI_FOLDER_CONTACTS,
			NULL,
			&IID_IMAPIFolder, //NULL,
//			OPEN_IF_EXISTS | MAPI_DEFERRED_ERRORS,
			OPEN_IF_EXISTS,
			&lpfOutF))) {
	#endif

			INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_FOLDER_NOTCREATED, MAPI_FOLDER_CONTACTS);
//			return -1;
		}


//	lpfOutF = OpenMsgStoreFolderByName(pmdb, fullContactsFolderName);
	if (lpfOutF == NULL) {
		INI_LogWrite(_T("-->lpfOutF == NULL"));
		return -1;
	}
	else
		INI_LogWrite(_T("-->lpfOutF != NULL"));

	//	Grab memory for a folder property array
	sCode = MAPIAllocateBuffer(uSize * sizeof(SPropValue),(void **)&pFolderPropValues);

	pFolderPropValues[c].ulPropTag = PR_CONTAINER_CLASS;
	_tcscpy(containerClass,  _T("IPF.Contact"));
	pFolderPropValues[c++].Value.lpszW = containerClass;

	pFolderPropValues[c].ulPropTag = PR_DEF_POST_DISPLAYNAME;
	_tcscpy(defPostDisplayName,  _T("Contact"));
	pFolderPropValues[c++].Value.lpszW = defPostDisplayName;

	pFolderPropValues[c].ulPropTag = PR_COMMENT;
	_tcscpy(comment,  _T("Contacts Comment"));
	pFolderPropValues[c++].Value.lpszW = comment;

 	pFolderPropValues[c].ulPropTag = PR_DEF_POST_MSGCLASS;
	_tcscpy(msgClass,  _T("IPM.Contact.Contact status unknown"));
	pFolderPropValues[c++].Value.lpszW = msgClass;

// 	pFolderPropValues[c].ulPropTag = PR_VIEW_SELECTION;
//	pFolderPropValues[c].Value.bin.cb = ???;
//	pFolderPropValues[c++].Value.bin.lpb = ???;

	lpfOutF->SetProps((ULONG)c, pFolderPropValues, NULL);

	MAPIFreeBuffer(pFolderPropValues);

	INI_LogWrite(_T("-->after OpenMsgStoreFolderByName"));

	long initialOffset = 0;
	ptr = _fgetts(ldifFileLine, 256, hLdifFile);
	int ptrprevlen = -1;
//	INI_LogWrite(_T("-->100"));
	while (ptr != NULL) {
		initialOffset += _tcslen(ptr) + 1; // add 1 byte back for x0A
#ifdef DEBUG_LOG
		INI_LogWrite(_T("-->101"));
		_stprintf(msg, _T("-->len: %d"), _tcslen(ptr));
		INI_LogWrite(msg);
		_stprintf(msg, _T("-->offset1: %lu"), initialOffset);
		INI_LogWrite(msg);
#endif
		if (_tcslen(ptr) == 0 || _tcslen(ptr) == 1) {
			if (ptrprevlen == 0 || ptrprevlen == 1) {
				// ignore another new line
				ptrprevlen = _tcslen(ptr);
				ptr = _fgetts(ldifFileLine, 256, hLdifFile);
				continue;
			}
//			INI_LogWrite(_T("-->102"));
			// we read here empty line between 2 LDIF entries
			(*totalPersons) += 1;
			if (ProcessOnePerson(ldifRecord, lpfOutF, totalPersonsSkipped, totalPersonsDone) != MIGRATION_STEP_SUCCESS) {
				(*totalPersonsError) += 1;
			}
//			INI_LogWrite(_T("-->103"));

			if (ldifRecord != NULL) {
//				INI_LogWrite(_T("-->1031"));
				delete ldifRecord;
//				INI_LogWrite(_T("-->1032"));
				ldifRecord = new CLdifRecord();
			}
		}
//		INI_LogWrite(_T("-->104"));
		ldifAttrName[0] = _T('\0');
		ldifAttrValue[0] = _T('\0');
//		INI_LogWrite(ldifFileLine);
		_TCHAR *ptrSep = _tcschr(ldifFileLine, _T(':'));
		if (ptrSep != NULL) {
			_tcsncpy(ldifAttrName, ldifFileLine, ptrSep - ldifFileLine);
			ldifAttrName[ptrSep - ldifFileLine] = _T('\0');
			if (ptrSep + 2 != NULL && _tcslen(ptrSep + 2) > 0) {
				_tcscpy(ldifAttrValue, ptrSep + 2);
				for (int i = _tcslen(ldifAttrValue) - 1; i>=0; i--) {
					if (ldifAttrValue[i] == _T('\r') ||
						ldifAttrValue[i] == _T('\n'))
						ldifAttrValue[i] = _T('\0');
					else
						break;
				}
			}

			if (_tcslen(ldifAttrName) > 0 && _tcslen(ldifAttrValue) > 0) {
//				INI_LogWrite(ldifAttrName);
//				INI_LogWrite(ldifAttrValue);

				// in case of decription field we need save offset instead
				if (_tcscmp(ldifAttrName, DESCRIPTION_ATTR) == 0) {
					_stprintf(msg, _T("-->offset1: %lu"), initialOffset);
					INI_LogWrite(msg);
					long descriptionOffset = initialOffset - _tcslen(ldifFileLine) 
						+ (ptrSep - ldifFileLine);
					ldifRecord->LookupAdd(ldifAttrName, descriptionOffset);
					_stprintf(msg, _T("-->offset2: %lu"), descriptionOffset);
					INI_LogWrite(msg);
				}
				else {
#ifdef DEBUG_LOG
					_tprintf(_T("ldifAttrName: %s, ldifAttrValue: %s\n"),
						ldifAttrName, ldifAttrValue);
#endif
					ldifRecord->LookupAdd(ldifAttrName, ldifAttrValue);
				}
			}
		}
//		INI_LogWrite(_T("-->105"));
		ptrprevlen = _tcslen(ptr);
		ptr = _fgetts(ldifFileLine, 256, hLdifFile);
	}

	if (ldifRecord != NULL) {
		delete ldifRecord;
	}

	return retCode;
}

void CLdifProc::ProcessBody(LPMESSAGE lpMessage, long offset)
{
	INI_LogWrite(_T("-->ProcessBody"));
	_TCHAR buffer[8192];
	char bufferCh[8192];
	char outbuf[16384];
	_TCHAR outbufW[32768];
	DWORD value_len = 0;
	_TCHAR *ptr, *ptrConverted;
	char* line_delimiter = "\r\n"; //"\\line "; //"\r\n";  //"\\line";
	WORD chars_per_line = CHARS_PER_LINE;
	wchar_t lineW[CHARS_PER_LINE+3];
	BOOL  fStripTabs = false;
	DWORD retBufferLength = 0;

	char rtfUniChar[32];
	char rtfUniBuf[512];
	bool rtfHeader = false;
	unsigned int i;
	unsigned int uniChar;
	BOOL base64encoded = true;

	LPSTREAM lpStream = NULL;
	HRESULT hr = hrSuccess;
	ULONG cbWritten	= 0;
	LPSTREAM            pStream=NULL;
	LPSTREAM            pUnStream=NULL;

	FILE *hLdifFile = _tfopen(this->GetOnlyLdifFile(), _T("r"));
	if (hLdifFile == NULL)
		goto cleanup;

	// offset here is right after first ':'
	fseek(hLdifFile, offset, SEEK_SET);
	ptr = _fgetts(buffer, sizeof(buffer), hLdifFile);
	if (*ptr == _T(':')) {
		INI_LogWrite(_T("-->ProcessBody encoded"));
		INI_LogWrite(ptr);
		INI_LogWrite(_T("<--"));
		base64encoded = true;
		// ': ' - 2 symbols
		ptr = ptr + 2;
	}
	else {
		INI_LogWrite(_T("-->ProcessBody not encoded"));
		INI_LogWrite(ptr);
		INI_LogWrite(_T("<--"));
		base64encoded = false;
		// ' ' - 1 symbol
		ptr = ptr + 1;
	}


	hr = lpMessage->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream,
			STGM_CREATE | STGM_WRITE, MAPI_CREATE | MAPI_MODIFY,
			(LPUNKNOWN FAR *) &pStream);

	hr = WrapCompressedRTFStream(pStream, MAPI_MODIFY |
		STORE_UNCOMPRESSED_RTF, &pUnStream);

#if defined(UNICODE)
	hr = lpMessage->OpenProperty(PR_BODY,
			&IID_IStream, STGM_CREATE | STGM_WRITE, MAPI_CREATE | MAPI_MODIFY, (LPUNKNOWN *)&lpStream);
#else
	hr = lpMessage->OpenProperty(PR_BODY,
			&IID_IStream, 0, MAPI_CREATE | MAPI_MODIFY, (LPUNKNOWN *)&lpStream);
#endif
	if (FAILED(hr))
		goto cleanup;

	while (ptr != NULL) {
		if (_tcslen(ptr) == 1 && _tcslen(buffer) > 1) {
			// we have empty base64 encoded string here
			ptr = _fgetts(buffer, sizeof(buffer), hLdifFile);
			continue;
		}
		if (_tcslen(ptr) == 1) // new line here
			break;
		// assume we have it here decoded
		if (base64encoded) {
			INI_LogWrite(_T("-->ProcessBody decoding base64"));
			memset(outbuf, 0, sizeof(outbuf));
			long decoded = 0;
			wcstombs(bufferCh, buffer, _tcslen(buffer));
			bufferCh[_tcslen(buffer)] = '\0';
		    memset (&outbuf, 0, sizeof(outbuf));
			decode(bufferCh, strlen(bufferCh), outbuf, &decoded);
			mbstowcs(outbufW, outbuf, strlen(outbuf));
			outbufW[strlen(outbuf)] = '\0';
			retBufferLength = _tcslen(outbufW);
			ptrConverted = outbufW;
			INI_LogWrite(_T("-->ProcessBody decoded base64"));
		}
		else {
			retBufferLength = _tcslen(buffer);
			ptrConverted = buffer;
		}

		if (retBufferLength > 0) {
			for (i=0; i<retBufferLength/CHARS_PER_LINE; i++) {
				_tcsncpy(lineW, ptrConverted + i*CHARS_PER_LINE, CHARS_PER_LINE);
				lineW[CHARS_PER_LINE] = '\0';

	#if defined(UNICODE)
				if (i == 0) {
					char unicodeStart[] = "{\\rtf1\\ansi\\ud{";
					hr = pUnStream->Write(unicodeStart, (ULONG)(strlen(unicodeStart))*sizeof(char), &cbWritten);
					rtfHeader = true;
				}
				rtfUniBuf[0] = '\0';
				for (uniChar = 0; uniChar < _tcslen(lineW); uniChar++) {
					if (lineW[uniChar] == _T('\r') && lineW[uniChar+1] == _T('\n')) {
						strcat(rtfUniBuf, "\\line");
						uniChar += 1;
					}
					else {
						rtfUniChar[0] = '\0';
						sprintf(rtfUniChar, "\\u%d?", lineW[uniChar]);
						strcat(rtfUniBuf, rtfUniChar);
					}
				}
				hr = pUnStream->Write(rtfUniBuf, (ULONG)(strlen(rtfUniBuf))*sizeof(char), &cbWritten);
				if (FAILED(hr))
					goto cleanup;
	#else
				hr = lpStream->Write(line, lstrlenA(line), &cbWritten);
	#endif

			}
			if (retBufferLength%CHARS_PER_LINE > 0) {
				_tcsncpy(lineW, ptrConverted + i*CHARS_PER_LINE, retBufferLength%CHARS_PER_LINE);
				lineW[retBufferLength%CHARS_PER_LINE] = '\0';

	#if defined(UNICODE)
				rtfUniBuf[0] = '\0';
				for (uniChar = 0; uniChar < _tcslen(lineW); uniChar++) {
					if (lineW[uniChar] == _T('\r') && lineW[uniChar+1] == _T('\n')) {
						strcat(rtfUniBuf, "\\line");
						uniChar += 1;
					}
					else {
						rtfUniChar[0] = '\0';
						sprintf(rtfUniChar, "\\u%d?", lineW[uniChar]);
						strcat(rtfUniBuf, rtfUniChar);
					}
				}
				hr = pUnStream->Write(rtfUniBuf, (ULONG)(strlen(rtfUniBuf))*sizeof(char), &cbWritten);
				if (FAILED(hr))
					goto cleanup;
	#else
				hr = lpStream->Write(line, lstrlenA(line), &cbWritten);
	#endif
			}
		}

#ifdef _UNICODE
		if (rtfHeader) {
			char unicodeEnd[] = "}";
			hr = pUnStream->Write(unicodeEnd, (ULONG)(strlen(unicodeEnd))*sizeof(char), &cbWritten);
		}
#endif			

		ptr = _fgetts(buffer, sizeof(buffer), hLdifFile);
	}

	hr = pUnStream->Commit(STGC_DEFAULT);
	pUnStream->Release();

	hr = pStream->Commit(STGC_DEFAULT); 
	pStream->Release();

	hr = lpStream->Commit(0);
	UlRelease(lpStream);
	lpStream = NULL;

cleanup:
	if (hLdifFile != NULL) {
		fclose(hLdifFile);
	}
	INI_LogWrite(_T("-->ProcessBody"));
}

int CLdifProc::GetNamedPropertyIdx(ULONG namedProperty)
{
	for (int i = 0; i<ulContactProps; i++) {
		if (aulContactProps[i].pID == namedProperty)
			return i;
	}
	return -1;
}
