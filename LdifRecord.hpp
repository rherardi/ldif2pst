// LdifRecord.hpp: interface for the CLdifRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDIFRECORD_HPP__DEF8DED8_00BE_405C_A3A4_44E420B6F8CB__INCLUDED_)
#define AFX_LDIFRECORD_HPP__DEF8DED8_00BE_405C_A3A4_44E420B6F8CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.hpp"

class CLdifRecord  
{
private:
	CList *ldifRecord;

public:
	CLdifRecord();
	virtual ~CLdifRecord();

	_TCHAR *Add(_TCHAR *id, _TCHAR *value);
	void Release();
	_TCHAR *Lookup(_TCHAR *id, bool exact = true);
	void Remove(_TCHAR *id);
	_TCHAR *LookupAdd(_TCHAR *id, _TCHAR *value);
	_TCHAR *LookupAdd(_TCHAR *id, long valLong);
};

#endif // !defined(AFX_LDIFRECORD_HPP__DEF8DED8_00BE_405C_A3A4_44E420B6F8CB__INCLUDED_)
