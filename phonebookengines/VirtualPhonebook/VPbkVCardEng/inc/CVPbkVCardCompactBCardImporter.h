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
* Description:  Virtual Phonebook VCard compact BCard importer
*
*/



#ifndef CVPBKVCARDCOMPACTBCARDIMPORTER_H
#define CVPBKVCARDCOMPACTBCARDIMPORTER_H

// INCLUDES
#include "MVPbkImportOperationImpl.h"
#include <e32base.h>
#include <MVPbkContactFieldCopyObserver.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkFieldType;
class MVPbkContactStore;
class CVPbkVCardData;
class MVPbkContactCopyPolicy;

// CLASS DECLARATIONS
NONSHARABLE_CLASS(CVPbkVCardCompactBCardImporter)
    :	public CActive,
        public MVPbkImportOperationImpl,
        private MVPbkContactFieldCopyObserver,
        private MVPbkContactFieldsCopyObserver
    {
    public: // Static constructors and destructor
        static CVPbkVCardCompactBCardImporter* NewL(
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            MVPbkContactStore& aTargetStore, 
            RReadStream &aSourceStream,
            CVPbkVCardData& aData );

        ~CVPbkVCardCompactBCardImporter();
    
    public: // From MVPbkImportOperationImpl

        void StartL();
        void SetObserver( MVPbkImportOperationObserver& aObserver );
        TBool IsOwncard();
    
    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
    
    private: // From MVPbkContactFieldCopyObserver 
        void FieldAddedToContact( MVPbkContactOperationBase& aOperation );
        void FieldAddingFailed( MVPbkContactOperationBase& aOperation,
            TInt aError);
    
    private: // From MVPbkContactFieldsCopyObserver
        void FieldsCopiedToContact(
                MVPbkContactOperationBase& aOperation );
        void FieldsCopyFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);
                
    private: 
        void IssueRequest();
        TBool GetLineL();
        void ParseLineL();
        void ValueToBufL( const TDesC& aBuffer, TInt aResId );
        void ValueToArrayL( const TDesC& aBuffer );
        void ValueAndLabelToBufL( const TDesC& aBuffer, TInt aResId );
        void AddAddressL();
        const MVPbkFieldType* FindFieldTypeL( TInt aFieldIndex );
        void AddFieldWithValueL( 
            const MVPbkFieldType& aFieldType, const TDesC& aValue, HBufC* aLabel );
        void StartTitleCopyL();
        TInt GetLineFromStreamL( TDes& aBuf, RReadStream& aSourceStream );
        TInt GetByteFromStream( TInt8& aByte, RReadStream& aSourceStream );
        void HandleContactSwitchL();
        void CopyTitleL();
        void MoveCurrentContactToImportsL();
        void HandleError( TInt aError );
        TInt ResolveFieldTypeResIdL( const TDesC& aBuffer );

    private: // Construction

        CVPbkVCardCompactBCardImporter(
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            MVPbkContactStore& aTargetStore,
            RReadStream &aSourceStream,
            CVPbkVCardData& aData );

        void ConstructL();

    private: // Data
        /// Ref: to the clients result array
        RPointerArray<MVPbkStoreContact>& iImportedContacts;
        /// Ref: the store to use as a target for importing
        MVPbkContactStore& iTargetStore;
        /// Ref:
        CVPbkVCardData& iData;
        /// Ref: the copy policy for the target store
        MVPbkContactCopyPolicy* iCopyPolicy;
        /// Own:
        CDesCArray*         iAddressArray;
        /// Own:
        HBufC*              iLabel;
        /// Own:
        HBufC*              iValue;
        /// Own:
        HBufC*              iLine;
        /// Ref:
        RReadStream&        iSource;
        /// Own: the current destination contact
        MVPbkStoreContact*  iCurContact;
        /// Own:
        TInt                iLineCount;
        /// Own:
        TInt                iValueLength;
        /// Own:
        TBool               iEMailFetchedFlag;
        /// Own: an operation that adds an field to the contact
        MVPbkContactOperationBase* iAddToFieldOperation;
        /// Internal state of the import operation
        TInt                iState;
        /// Ref: the observer of the import
        MVPbkImportOperationObserver* iObserver;
        /// An index of target contact when title fields are copied
        /// to split contacts
        TInt iTargetContactIndex;
    };

#endif // CVPBKVCARDCOMPACTBCARDIMPORTER_H
//End of file

