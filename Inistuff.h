/* INISTUFF.H */
#ifndef __INISTUFF_H
#define __INISTUFF_H

//#define LOG_C		1
//#define LOG_R		2
//#define LOG_S		3

#define MAX_STRING	256
#define DEFAULT_LOG_NAME _T("ldif2pst")
#define DEFAULT_LOG_EXT _T(".log")

char *INI_CurDir(char *path);
char *INI_WinDir(char *path);
int INI_GetIniFullName(char *fullname);
/*int INI_GetGlobalsSettings(char *filename);

void INI_GetDateTime(char *date, char *time, int points, int colons);
void INI_LogName(char *fname);
int INI_LogWrite(char *fname, int ids, char mode,
	char *fabo, char *tabo, char *sub);
void INI_FormLogName(char *fname);
int INI_WriteGlobalsSettings(char *filename);
void INI_Convert4(int intc, char *chc);

int INI_Mkdir(char *path);
int INI_Rmdir(char *path);
int INI_Chdir(char *path);
int INI_Remove(char *path, char *fname);
void INI_FormPath(char *aaaa, char *date, char *ver, char *work, char *rpath);
int INI_GetLastArclistFile(char *spath, char *arclist);
int INI_GetNextFile(char *spath, unsigned short int elem, char *fname);
char *INI_Alloc(unsigned int size);
void INI_Free(char *var);
void INI_AddProcessToPath(char *tmpPath, unsigned short int version);
void INI_ChMkDir(char *path);*/

extern char *glFileName;
extern int glLine;

void INI_LogInit(const _TCHAR *logPath, bool isFullName = false, bool isAppend = false);
void INI_LogWriteWithResourcePrefix(int prefix, _TCHAR *txt);
void INI_LogWriteWithPrefixNum(_TCHAR * prefix, long number, bool console = false);
void INI_LogWriteWithResourcePrefixA(int prefix, char *txt);
void INI_LogWriteWithResourcePrefixP(int prefix, _TCHAR *txt, _TCHAR *param);
void INI_LogWriteWithResourcePrefixNum(int prefix, long number, bool isUnsigned = false);
void INI_LogWriteWithResourcePrefixNumP(int prefix, long number, _TCHAR *param, bool console = false);
void INI_LogWriteWithResourcePrefixNumAP(int prefix, long number, char *param, bool console = false);
void INI_LogWrite(_TCHAR *txt);
void INI_LogClose();
void INI_SetLogSizeWrap(unsigned int logSizeWrap);

#endif /* __INISTUFF_H */
