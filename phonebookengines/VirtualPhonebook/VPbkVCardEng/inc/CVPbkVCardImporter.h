/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Importing the CParserProperty field from CParserVCard to 
*                contact field.
*
*/


#ifndef CVPBKVCARDIMPORTER_H
#define CVPBKVCARDIMPORTER_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "MVPbkImportOperationImpl.h"
#include <MVPbkContactFieldCopyObserver.h>
#include <s32strm.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactStore;
class CVPbkVCardData;
class CParserProperty;
class CVPbkVCardContactFieldData;
class TVersitDateTime;
class CParserVCard;
class CVPbkVCardAttributeHandler;
class CVPbkVCardContactFieldIterator;
class CVPbkVCardPropertyParser;
class MVPbkContactCopyPolicy;

// CLASS DECLARATIONS
/**
 * Class is importing the CParserProperty field from CParserVCard to 
 * contact field.
 */
NONSHARABLE_CLASS( CVPbkVCardImporter ): 
        public CActive,
        public MVPbkImportOperationImpl,
        private MVPbkContactFieldCopyObserver,
        private MVPbkContactFieldsCopyObserver,
        public MVPbkContactStoreObserver
    {
    public:
        static CVPbkVCardImporter* NewL(
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            RReadStream& aSourceStream,
            MVPbkContactStore& aTargetStore, 
            CVPbkVCardData& aData );
            
        ~CVPbkVCardImporter();
            
    public: // From MVPbkImportOperationImpl
        void StartL();
        void SetObserver( MVPbkImportOperationObserver& aObserver );
        TBool IsOwncard();
        
    public:
        //return true, if X-CATEGORIES field is present in Vcard
        TBool IsGroupcard();
        //return, X-CATEGORIES field Value
        TPtr  GetGroupcardvalue();
        
    private: // From MVPbkContactFieldCopyObserver
        void FieldAddedToContact(
                MVPbkContactOperationBase& aOperation);
        void FieldAddingFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);
                
    private: // From MVPbkContactFieldsCopyObserver
        void FieldsCopiedToContact(
                MVPbkContactOperationBase& aOperation);
        void FieldsCopyFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);

    public: // from MVPbkContactStoreObserver
        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable(
        		MVPbkContactStore& aContactStore, 
        		TInt aReason);
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);
                
    protected: // CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();        
                
    private: // types
       enum TState
            {
            EParseNext,
            ESaveField,
            ESaveContact,
            EStop
            };        

    private:
        CVPbkVCardImporter( 
                RPointerArray<MVPbkStoreContact>& aImportedContacts,
                MVPbkContactStore& aTargetStore, 
                CVPbkVCardData& aData );
        void ConstructL(RReadStream& aSourceStream);
            
        void IssueRequest();
        CParserProperty* NextProperty();
        CParserProperty* GetCurrentProperty();
        void ParseNextL();
        TInt FindContactFieldL( 
                MVPbkStoreContact* aContact, 
                TInt aFieldTypeResId );
        void SaveFieldL();
        TBool SaveDataL( CVPbkVCardContactFieldData& aData );
        void SaveL( 
            CVPbkVCardContactFieldData& aData, const TVersitDateTime& aValue );
        void SaveL( 
            CVPbkVCardContactFieldData& aData, const TDesC& aValue );
        void SaveL( 
            CVPbkVCardContactFieldData& aData, const TDesC8& aValue );
        void SaveContactL();
        void HandleFailedFieldAdditionL( TInt aError );
        void RemoveEmptyFieldsAndContacts();
        void CopyContactTitlesL();
        void ContinueCopyingToNewContactL();
        void HandleError( TInt aError );  
        void StartAsync( TState aNextState );

    private:
        /// Own: vcard parser
        CParserVCard* iParser;
        /// Own: contact attribute handler
        CVPbkVCardAttributeHandler* iAttributeHandler;
        /// Own: vcard field iterator
        CVPbkVCardContactFieldIterator* iContactFieldIterator;
        /// Own: current contact
        MVPbkStoreContact* iCurContact;
        /// Own: vcard property parser
        CVPbkVCardPropertyParser* iPropertyParser;
        /// Ref: reference to client's imported contacts array
        RPointerArray<MVPbkStoreContact>& iImportedContacts;
        /// Ref: target store
        MVPbkContactStore& iTargetStore;
        /// Ref: vcard engine data
        CVPbkVCardData& iData;
        /// Ref: observer interface
        MVPbkImportOperationObserver* iObserver;
        /// Own: parser's property array loop index
        TInt iArrayElementIndex;
        /// Own: save field operation
        MVPbkContactOperationBase* iAddFieldOperation;
        /// Ref: copy policy
        MVPbkContactCopyPolicy* iCopyPolicy;
        /// Own: import process state
        TState iState;
        // Own: Buffer to hold copy of VCard data to be imported 
        HBufC8* iStreamBuffer;
        // Own: Stream of VCard data to be imported 
        RDesReadStream iSourceStream;
        TBool iOwnCard;
        TBool iGroupCard;
        //Own: X-CATEGORIES field Value
        HBufC16* iGroupCardValue;
        /// Ref: cntmodel store
        MVPbkContactStore* iCntModelStore;
        /// Own: cntmodel store contact for getting contact title
        MVPbkStoreContact* iCntContact;
        /// Own: save cntmodel contact field operation
        MVPbkContactOperationBase* iAddCntContactFieldOperation;
        /// Ref: Copy cntmodel contact field operation
        MVPbkContactCopyPolicy* iCntCopyPolicy;
        /// Own: allow being overwriten
        RArray<TInt> iErasableFields;
    };

#endif // CVPBKVCARDIMPORTER_H
// End of file
