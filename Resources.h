#ifdef JESCOMMON_EXPORTS
#define JESCOMMON_API __declspec(dllexport)
#else
#define JESCOMMON_API __declspec(dllimport)
#endif

class CResources {
public:
	static JESCOMMON_API void ShowTitle(unsigned int titleStrID, unsigned int versionStrID);
};
