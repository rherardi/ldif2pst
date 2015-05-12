// XMLMailProc.cpp: implementation of the CXMLPersonProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <xercesc/util/XercesDefs.hpp>

#include <xalanc/Include/PlatformDefinitions.hpp>

#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/LocalFileInputSource.hpp>
//#include <xercesc/framework/MemBufInputSource.hpp>

#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanElement.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XalanDOM/XalanNamedNodeMap.hpp>
#include <xalanc/XalanDOM/XalanNodeList.hpp>
#include <xalanc/XalanDOM/XalanDOMException.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>

#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMException.hpp>

//#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
//#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp>
//#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp>
#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/MutableNodeRefList.hpp>

#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>

//#define XML_INTERNAL

#include "SAXErrorHandler.hpp"

#include "XMLProc.hpp"
#include "XMLPersonProc.hpp"

#include "PersonItem.hpp"
#include "EntryTransformer.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLPersonProc::CXMLPersonProc()
{

}

CXMLPersonProc::~CXMLPersonProc()
{

}

void CXMLPersonProc::InitializePersonItemList()
{
	getEntryList(PERSON_PERSON_ITEM_SELECTOR);
	m_currentEntryIndex = 0;
	m_entryListInitialized = true;
}

CPersonItem* CXMLPersonProc::GetNextPersonItem(_TCHAR *partCondition)
{
	CPersonItem *entry = NULL;
	if (partCondition != NULL && !m_entryListInitialized) {
		_TCHAR selectorFilled[256];

//		if (_tcscmp(partCondition, _T("Sender")) == 0)
//			_tprintf(_T("-->10 Sender\n"));

		_stprintf(selectorFilled, PERSON_PERSON_ITEM_CSELECTOR, partCondition);
//		if (_tcscmp(partCondition, _T("Sender")) == 0)
//			_tprintf(_T("-->11 Sender\n"));
		getEntryList(selectorFilled);
//		if (_tcscmp(partCondition, _T("Sender")) == 0)
//			_tprintf(_T("-->12 Sender\n"));
		m_currentEntryIndex = 0;
		m_entryListInitialized = true;
	}

	if (m_entryList != NULL && (m_currentEntryIndex < m_entryList->getLength())) {
		entry = new CPersonItem();
		const XalanNode* const	theNode = m_entryList->item(m_currentEntryIndex);
		const XalanNamedNodeMap* attrs = theNode->getAttributes();
		XalanNode* nameNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_NAME));
		XalanNode* valueNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_VALUE));
		XalanNode* conditionNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_CONDITION));
		XalanNode* numericNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_NUMERIC));
		XalanNode* namedPropertyNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_NAMEDPROPERTY));
		entry->SetName(nameNode->getNodeValue().c_str());
		entry->SetValue(valueNode->getNodeValue().c_str());
		if (conditionNode != NULL) {
			entry->SetCondition(conditionNode->getNodeValue().c_str());
		}
		if (numericNode != NULL) {
			entry->SetNumeric(_tcsicmp(numericNode->getNodeValue().c_str(), _T("true")) == 0);
			entry->SetValueNumeric(_ttoi(entry->GetValue()));
		}
		if (namedPropertyNode != NULL) {
			entry->SetNamedProperty(_tcsicmp(namedPropertyNode->getNodeValue().c_str(), _T("true")) == 0);
		}
		m_currentEntryIndex++;
	}
	else {
		m_currentEntryIndex = 0;
		m_entryListInitialized=false;
		entry = NULL;
	}
	return entry;
}

void CXMLPersonProc::resetEntryListIndex()
{
	m_currentEntryIndex = 0;
	m_entryListInitialized = false;
	if (m_entryList != NULL) {
		delete(m_entryList);
		m_entryList = NULL;
	}
}

void CXMLPersonProc::resetTransformerListIndex()
{
	m_currentTransformerIndex = 0;
	m_transformerListInitialized = false;
	if (m_transformerList != NULL) {
		delete(m_transformerList);
		m_transformerList = NULL;
	}
}

CEntryTransformer* CXMLPersonProc::getNextEntryTransformer(_TCHAR *nodeSelector)
{
//	_TCHAR usage[256];
//	HINSTANCE hInst = ::GetModuleHandle(NULL);
//	_TCHAR current_method[] = _TEXT("CXMLPersonProc::getNextEntryTransformer");
//	if (m_verbose){
//		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
//		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
//	}

	CEntryTransformer *transformer;
	if (!m_transformerListInitialized) {
		XalanNode* theNode = NULL;
		XPathEvaluator	*theEvaluator = new XPathEvaluator();
		if (nodeSelector == NULL)
			theNode = m_entryList->item(m_currentEntryIndex-1);
		else {
			theNode = theEvaluator->selectSingleNode(
				*theDOMSupport,
				theDocument,
				XalanDOMString(nodeSelector).c_str(),
				*thePrefixResolver);
		}
		NodeRefList	theResult;
		theEvaluator->selectNodeList(
								theResult,
								*theDOMSupport,
								theNode,
								XalanDOMString(TRANSFORMER_SELECTOR).c_str(),
								*thePrefixResolver);
		m_transformerList = new MutableNodeRefList(theResult, theResult.getMemoryManager());
		theEvaluator->~XPathEvaluator();
		m_transformerListInitialized = true;
	}
#ifdef DEBUG_LOG
	_tprintf(_T("m_currentTransformerIndex: %d, m_transformerList->getLength(): %d, m_currentEntryIndex: %d\n"),
		m_currentTransformerIndex, m_transformerList->getLength(), m_currentEntryIndex);
#endif
	if (m_currentTransformerIndex < m_transformerList->getLength()) {
		transformer = new CEntryTransformer();
		const XalanNode* const	theTransformerNode = m_transformerList->item(m_currentTransformerIndex);
		const XalanNamedNodeMap* attrs = theTransformerNode->getAttributes();
		XalanNode* nameNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_NAME));
		XalanNode* placeholderNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_PLACEHOLDER));
//		XalanNode* orgHierarchyNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_ORG_HIERARCHY));
//		XalanNode* resolveDocFieldNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_RESOLVE_DOC_FIELD));
//		XalanNode* resolveDocFormNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_RESOLVE_DOC_FORM));
//		XalanNode* resolveDocLookupFieldNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_RESOLVE_DOC_LOOKUP_FIELD));
		XalanNode* valueModifierNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_VALUEMODIFIER));
		
		transformer->SetName(nameNode->getNodeValue().c_str());
		transformer->SetPlaceholder(placeholderNode->getNodeValue().c_str());
//		if (orgHierarchyNode == NULL) {
//			transformer->SetOrgHierarchy(false);
//		}
//		else {
//			transformer->SetOrgHierarchy(_tcscmp(orgHierarchyNode->getNodeValue().c_str(), _T("true")) == 0 ? true : false);
//		}
//		if (resolveDocFieldNode != NULL) {
//			_tprintf(_T("resolveDocFieldNode: %s\n"), resolveDocFieldNode->getNodeValue().c_str());
//			transformer->SetResolveDocField(resolveDocFieldNode->getNodeValue().c_str());
//		}
//		if (resolveDocFormNode != NULL) {
//			_tprintf(_T("resolveDocFormNode: %s\n"), resolveDocFormNode->getNodeValue().c_str());
//			transformer->SetResolveDocForm(resolveDocFormNode->getNodeValue().c_str());
//		}
//		if (resolveDocLookupFieldNode != NULL) {
//			_tprintf(_T("resolveDocLookupFieldNode: %s\n"), resolveDocLookupFieldNode->getNodeValue().c_str());
//			transformer->SetResolveDocLookupField(resolveDocLookupFieldNode->getNodeValue().c_str());
//		}

		if (_tcscmp(theTransformerNode->getNodeName().c_str(), XMLELEMENT_LDIFATTRIBUTE) == 0) {
			transformer->SetLdifAttribute(true);
		}
		else {
			transformer->SetLdifAttribute(false);
		}
		if (valueModifierNode != NULL) {
			transformer->SetValueModifier(valueModifierNode->getNodeValue().c_str());
		}

		m_currentTransformerIndex++;
	}
	else {
		m_currentTransformerIndex = 0;
		m_transformerListInitialized = false;
		transformer = NULL;
	}

//	if (m_verbose){
//		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
//		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
//	}
	return transformer;
}

void CXMLPersonProc::GetMessageClassAttr(_TCHAR *attrValue)
{
	getEntryList(PERSON_MESSAGE_CLASS_ATTR_SELECTOR);
	 if (m_entryList == NULL || m_entryList->getLength() == 0)
		 return;

	const XalanNode* const	theNode = m_entryList->item(0);
	const XalanNamedNodeMap* attrs = theNode->getAttributes();
	XalanNode* nameNode = attrs->getNamedItem(XalanDOMString(XMLATTRIBUTE_NAME));
	_tcscpy(attrValue, nameNode->getNodeValue().c_str());
}
