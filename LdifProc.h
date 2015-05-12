// NotesProc.h: interface for the CLdifProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTESPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_)
#define AFX_NOTESPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFFER_SIZE 8192
#define ENTRY_LEN 256

#define LDIFMASK _T("*.ldif")
#define CONDITION_SENDER				_T("Sender")

#define PR_DEF_POST_DISPLAYNAME                          PROP_TAG( PT_TSTRING,   0x36E6)
#define PR_DEF_POST_DISPLAYNAME_W                        PROP_TAG( PT_UNICODE,   0x36E6)
#define PR_DEF_POST_DISPLAYNAME_A                        PROP_TAG( PT_STRING8,   0x36E6)

#define PR_DEF_POST_MSGCLASS	                          PROP_TAG( PT_TSTRING,   0x36E5)
#define PR_DEF_POST_MSGCLASS_W		                      PROP_TAG( PT_UNICODE,   0x36E5)
#define PR_DEF_POST_MSGCLASS_A			                  PROP_TAG( PT_STRING8,   0x36E5)

#define PR_VIEW_SELECTION								PROP_TAG( PT_BINARY,   0x36E0)

// See here:
// http://msdn.microsoft.com/en-us/library/cc678348.aspx

// PSETID_Address GUID {00062004-0000-0000-c000-000000000046}
// Propset used for contact items
DEFINE_OLEGUID(PSETID_Address, MAKELONG(0x2000+(0x04),0x0006),0,0);

#define PidLidContactItemData              0x8007
#define PidLidFileUnderList                0x8026
#define PidLidAutoLog                      0x8025
#define PidLidAddressBookProviderEmailList 0x8028
#define PidLidAddressBookProviderArrayType 0x8029
#define PidLidFileUnder                    0x8005
#define PidLidFileUnderId                  0x8006
#define PidLidContactCharSet               0x8023
#define PidLidEmail1DisplayName            0x8080
#define PidLidEmail1AddressType            0x8082
#define PidLidEmail1EmailAddress           0x8083
#define PidLidEmail1OriginalDisplayName    0x8084
#define PidLidEmail1OriginalEntryID        0x8085
#define PidLidEmail2DisplayName            0x8090
#define PidLidEmail2AddressType            0x8092
#define PidLidEmail2EmailAddress           0x8093
#define PidLidEmail2OriginalDisplayName    0x8094
#define PidLidEmail2OriginalEntryID        0x8095
#define PidLidWorkAddressStreet            0x8045
#define PidLidWorkAddressCity              0x8046
#define PidLidWorkAddressState             0x8047
#define PidLidWorkAddressPostalCode        0x8048
#define PidLidWorkAddressCountry           0x8049
#define PidLidWorkAddressCountryCode       0x80DB
#define PidLidHomeAddress				   0x801A
#define PidMyLidUserField1		           0x804F
#define PidMyLidUserField2		           0x8050
#define PidMyLidUserField3		           0x8051
#define PidMyLidUserField4		           0x8052
#define PidLidHtml                         0x802B
#define PidMyInstMsg			           0x8062
//#define PidLidPostalAddressId              0x8022
//#define PidLidAddressCountryCode           0x80DD


//#define PR_CARD_NAME	0x808D001F
//#define PR_CARD_MAIL	0x808E001F
//#define PR_CARD_ENTRYID	0x808F0102

//#define PR_CARD_NAME		0x8048001F
//#define PR_CARD_MAIL		0x804B001F
//#define PR_CARD_ENTRYID	0x804D0102
//#define PR_CARD_NAME2		0x8050001F
//#define PR_CARD_MAIL2		0x8053001F
//#define PR_CARD_ENTRYID2	0x80550102


//2009.04.15 10:47:25:624 -->tag p_PidLidContactItemData: 801c1003<--
//2009.04.15 10:47:25:624 -->tag p_PidLidFileUnderList: 800e1003<--
//2009.04.15 10:47:25:624 -->tag p_PidLidAddressBookProviderEmailList: 80101003<--
//2009.04.15 10:47:25:624 -->tag p_PidLidAddressBookProviderArrayType: 80110003<--
//2009.04.15 10:47:25:624 -->tag p_PidLidFileUnder: 8016001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidFileUnderId: 80170003<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail1DisplayName: 8048001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail1AddressType: 804a001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail1EmailAddress: 804b001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail1OriginalDisplayName: 804c001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail1OriginalEntryID: 804d0102<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail2DisplayName: 8050001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail2AddressType: 8052001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail2EmailAddress: 8053001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail2OriginalDisplayName: 8054001f<--
//2009.04.15 10:47:25:624 -->tag p_PidLidEmail2OriginalEntryID: 80550102<--

//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressStreet: 8021001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressCity: 8022001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressState: 8023001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressPostalCode: 8024001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressCountry: 8025001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidWorkAddressCountryCode: 8026001f<--
//2009.04.22 10:56:34:265 -->tag p_PidLidHomeAddress: 8027001f<--

//2009.04.22 22:49:47:265 -->tag p_PidMyLidUserField1: 8012001f<--
//2009.04.22 22:49:47:265 -->tag p_PidMyLidUserField2: 8013001f<--
//2009.04.22 22:49:47:265 -->tag p_PidMyLidUserField3: 8014001f<--
//2009.04.22 22:49:47:265 -->tag p_PidMyLidUserField4: 8015001f<--
//2009.04.22 22:49:47:265 -->tag p_PidLidHtml: 8031001f<--
//2009.04.22 22:49:47:265 -->tag p_PidMyInstMsg: 8018001f<--

//#define PR_ADDRESS_BOOK_PROVIDER_ARRAY_TYPE	0x80910003 //			PidLidAddressBookProviderArrayType
//#define PR_ADDRESS_BOOK_PROVIDER_EMAIL_LIST	0x80921003 // PidLidAddressBookProviderEmailList
//#define PR_FILE_UNDER_LIST					0x80931003 // should be obtained for PidLidFileUnderList                0x8026
//#define PR_FILE_UNDER						0x80941003 // should be obtained for PidLidFileUnder                0x8026
//#define PR_FILE_UNDER_ID					0x80950003 // PidLidFileUnderId
//#define PR_CONTACT_ITEM_DATA				0x80991003 // should be obtained for PidLidContactItemData              0x8007

//#define PR_ADDRESS_BOOK_PROVIDER_ARRAY_TYPE	0x80110003 //			PidLidAddressBookProviderArrayType
//#define PR_ADDRESS_BOOK_PROVIDER_EMAIL_LIST	0x80101003 // PidLidAddressBookProviderEmailList
//#define PR_FILE_UNDER_LIST					0x800e1003 // should be obtained for PidLidFileUnderList                0x8026
//#define PR_FILE_UNDER						0x8016001f // should be obtained for PidLidFileUnder                0x8026
//#define PR_FILE_UNDER_ID					0x80170003 // PidLidFileUnderId
//#define PR_CONTACT_ITEM_DATA				0x801c1003 // should be obtained for PidLidContactItemData              0x8007

#define CHARS_PER_LINE 80
#define DESCRIPTION_ATTR _T("description")

class CLDIF2PSTApp;
class CPersonItem;
class CLdifRecord;
class CList;

typedef struct {
	unsigned long totalBytesProcessed;
	FILE *outputFile;
} CallBackData;

class CLdifProc  
{
public:
	CLdifProc();
	virtual ~CLdifProc();

	int Start(_TCHAR* exec, CLDIF2PSTApp *theApp);
	int PerformMigrationLocal(_TCHAR *inputDir);
	int PerformOneLdifMigration(const _TCHAR *outputdir, const _TCHAR *ldifFile = NULL, 
		UINT *totalFiles = NULL, UINT *totalFilesError = NULL);
	int ProcessOnePerson(CLdifRecord *ldifRecord, LPMAPIFOLDER lpfOutF, 
		unsigned int *totalPersonsSkipped, unsigned int *totalPersonsDone);
	void PutMessageSender(CLdifRecord *ldifRecord, 
		LPMAPIFOLDER lpFolder, LPMESSAGE lpMessage, LPSPropValue pMessagePropValues, UINT *c);
	int ApplyTransformers(CLdifRecord *ldifRecord, CPersonItem *item, 
		LPMAPIFOLDER lpFolder, LPMESSAGE lpMessage, LPSPropValue pMessagePropValues, UINT *c);
	int ProcessLdifFile(FILE *hLdifFile, LPMAPIFOLDER lpfRootF, unsigned int *totalPersons, unsigned int *totalPersonsDone, 
		unsigned int *totalPersonsSkipped, unsigned int *totalPersonsError);
	void ProcessBody(LPMESSAGE lpMessage, long offset);

protected:
	bool m_verbose;
	_TCHAR m_language[3];
	_TCHAR m_filter[256];
	_TCHAR m_inputdir[512];
	_TCHAR m_logFile[512];
	unsigned int m_logSizeWrap;
	_TCHAR m_outputdir[512];
	_TCHAR m_outlookFile[512];
	_TCHAR m_onlyLdifFile[512];
	_TCHAR m_defaultName[256];
	_TCHAR m_defaultEmail[256];
	bool m_appendLogFile;

	CLDIF2PSTApp *theParentApp;
	CList *listSuppl;

public:
	// Get/Set m_language
	_TCHAR const * GetLanguage();
	void SetLanguage(const _TCHAR *language);

	// Get/Set m_filter
	_TCHAR const * GetFilter();
	void SetFilter(const _TCHAR *filter);

	// Get/Set m_inputdir
	_TCHAR const * GetInputDir();
	void SetInputDir(const _TCHAR *inputDir);

	// Get/Set m_logFile
	_TCHAR const * GetLogFile();
	void SetLogFile(const _TCHAR *logFile);

	// Get/Set m_logSizeWrap
	unsigned int GetLogSizeWrap();
	void SetLogSizeWrap(unsigned int logSizeWrap);

	// Get/Set m_outputdir
	_TCHAR const * GetOutputDir();
	void SetOutputDir(const _TCHAR *outputDir);

	// Get/Set m_onlyLdifFile
	_TCHAR const * GetOnlyLdifFile();
	void SetOnlyLdifFile(const _TCHAR *onlyLdifFile);

	// Get/Set m_defaultName
	_TCHAR const * GetDefaultName();
	void SetDefaultName(const _TCHAR *defaultName);

	// Get/Set m_defaultEmail
	_TCHAR const * GetDefaultEmail();
	void SetDefaultEmail(const _TCHAR *defaultEmail);

	// Get/Set m_outlookFile
	_TCHAR const * GetOutlookFile();
	void SetOutlookFile(const _TCHAR *outlookFile);

	// Get/Set m_verbose
	bool const GetVerbose();
	void SetVerbose(const bool verbose);

	// Get/Set m_appendLogFile
	bool const GetAppendLogFile();
	void SetAppendLogFile(const bool appendLogFile);

	int CreateOutlookFile(_TCHAR *targetdir, const _TCHAR *name, int how, bool splitName = TRUE, _TCHAR *nsfFile = NULL);

#ifdef LOTUS_INTERNAL
	// Get m_foundNoteID
	NOTEID const GetFoundNoteID();
	void SetFoundNoteID(const NOTEID noteID);
#endif

private:
	void GetPSTMask(_TCHAR *pstmask);
	ULONG GetXMLPropTagValue(_TCHAR *propTag);
	int GetNamedPropertyIdx(ULONG namedProperty);
#ifdef LOTUS_INTERNAL
	void SetRecipient(CMessageItem *item, LPMESSAGE lpMessage, char *field_text, char *email, LPSPropValue pMessagePropValues, UINT *c);
#endif

};

#endif // !defined(AFX_NOTESPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_)
