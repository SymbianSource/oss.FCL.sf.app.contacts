/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    XML content handler, constructs contacts entrys based upon
*                callbacks from the Xml parser. 
*
*/

#ifndef C_PDCXMLCONTENTHANDLER_H
#define C_PDCXMLCONTENTHANDLER_H

// System includes
#include <e32base.h>                // CBase
#include <contenthandler.h>         // MContentHandler
#include <MVPbkContactObserver.h>   // MVPbkContactObserver
#include <CPbkContactItem.h> // CPbkContactItem

// User includes
#include "CVPbkContactManager.h"
#include "pdcstringconstants.h"
#include "pdclogger.h"

// Forward declarations
class MVPbkStoreContact;
class MVPbkContactStore;
class Xml::RTagInfo;
class CVPbkContactLinkArray;
class MVPbkContactFieldData;

const TInt phoneNumberCount = 10;
/**
 *  This class is an implementation of the Xml:MContentHandler interface,
 *  it is reponsible for creating contact items and and adding them to 
 *  the contacts engine based upon the callbacks from the XML parser.
 */
NONSHARABLE_CLASS( CPdcXmlContentHandler ): public CBase,
    public Xml::MContentHandler, public MVPbkContactObserver
    {
public:
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcXmlContentHandler
    * @param    aContactStore      contacts store
    * @param    aLinkArray         links to the contacts added.
    * @return   a CPdcXmlContentHandler object.
    */
    static CPdcXmlContentHandler* NewL( MVPbkContactStore& aContactStore,
                              CVPbkContactLinkArray& aLinkArray);
    
    /**
    * Destructor.
    */
    virtual ~CPdcXmlContentHandler();
    
    /**
     * Sets the string pool
     * @param aStringPool   parser's string pool
     */
     void SetStringPoolL( RStringPool& aStringPool );
     
     /**
     * Sets the contactmanager 
     * @param aContactManager   engine's contactmanager
     */
     void SetContactManager(CVPbkContactManager* aContactManager);

private: // C++ constructor and the Symbian second-phase constructor 
    CPdcXmlContentHandler( MVPbkContactStore& aContactStore,
                                           CVPbkContactLinkArray& aLinkArray);
    void ConstructL();
    
private: // private member functions
    TInt GetTagIndexL( const Xml::RTagInfo& aElement ) const;
    
    void AddFieldL( TInt aFieldResId, const TDesC8& aBuffer );
    void SetTextFieldL( MVPbkContactFieldData& aFieldData, 
                                                    const TDesC8& aBuffer );
    void SetBinaryFieldL( MVPbkContactFieldData& aFieldData,
                                                    const TDesC8& aBuffer ); 
    void SetDateFieldL( MVPbkContactFieldData& aFieldData,
                                                    const TDesC8& aBuffer );
    void AddContactL();
    TInt GetSpeeddialAssignedFieldIndexL(CPbkContactItem* aItem);
    TInt ContactItemFieldIndexL( CPbkContactItem* aItem );
    TInt GetDefaultSpeeddialAssignedFieldIndexL();
    TBool CheckValidityOfSpeeddialAssignedFieldL( TInt aFieldIndex );
    
private: // from MContentHandler
   	void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam,
   	                                             TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode); 
	void OnStartElementL(const Xml::RTagInfo& aElement, 
	                 const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode); 
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    void OnStartPrefixMappingL( const RString& aPrefix,
            const RString& aUri, TInt aErrorCode );
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);   
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);   
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);	  
    void OnProcessingInstructionL( const TDesC8& aTarget,
            const TDesC8& aData, TInt aErrorCode );
	void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	
private: // from MVPbkContactObserver
    void ContactOperationCompleted(TContactOpResult aResult);
    void ContactOperationFailed
            (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);
	
private: // data
    /// Ref: Contact store
    MVPbkContactStore& iContactStore;
    /// Ref: Ids of the contacts that have been added
    CVPbkContactLinkArray& iLinkArray;
    /// Ref: String pool
    RStringPool* iStringPool;
    /// Own: Content buffer
    RBuf8 iContentBuffer;
    /// The current tag can be handled.
    TBool iKnownTag;
    /// Own: Contact item being created.
    MVPbkStoreContact* iContactItem;
    /// Own
    CActiveSchedulerWait* iWait;
    /// Error reported to contact observer
    TInt iContactObserverError;
    TInt iSpeeddialValue;
    CVPbkContactManager* iContactManager;
    /// The content of speeddial number
    TInt iSpeeddialIndex;
    /// Own: Speeddial assigned phone number field
    TInt iSpeeddialAssignedFieldIndex;
    RFs iFs;     
    static const 
        TInt iSpeeddailSupportedAssignedFieldsIndexSet[phoneNumberCount];

    };

#endif // C_PDCXMLCONTENTHANDLER_H
