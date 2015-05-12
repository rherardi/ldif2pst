// PersonItem.cpp: implementation of the CPersonItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PersonItem.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPersonItem::CPersonItem()
{
	m_name[0] = _T('\0');
	m_value[0] = _T('\0');
	m_condition[0] = _T('\0');
	m_numeric = false;
}

CPersonItem::~CPersonItem()
{

}

// Get/Set m_name
_TCHAR const * CPersonItem::GetName()
{
	return &m_name[0];
}
void CPersonItem::SetName(const _TCHAR *name)
{
	if (name != NULL) {
		::_tcscpy(m_name, name);
	}
}

// Get/Set m_value
_TCHAR const * CPersonItem::GetValue()
{
	return &m_value[0];
}
void CPersonItem::SetValue(const _TCHAR *value)
{
	if (value != NULL) {
		::_tcscpy(m_value, value);
	}
}

// Get/Set m_valuenum
long const CPersonItem::GetValueNumeric()
{
	return m_valuenum;
}
void CPersonItem::SetValueNumeric(const long value)
{
	m_valuenum = value;
}

// Get/Set m_numeric
bool const CPersonItem::IsNumeric()
{
	return (m_numeric == true);
}
void CPersonItem::SetNumeric(const bool value)
{
	m_numeric = value;
}

// Get/Set m_condition
_TCHAR const * CPersonItem::GetCondition()
{
	return &m_condition[0];
}
void CPersonItem::SetCondition(const _TCHAR *value)
{
	if (value != NULL) {
		::_tcscpy(m_condition, value);
	}
}

// Get/Set m_namedProperty
bool const CPersonItem::IsNamedProperty()
{
	return m_namedProperty;
}
void CPersonItem::SetNamedProperty(const bool namedProperty)
{
	m_namedProperty = namedProperty;
}


