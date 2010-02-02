/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 textual contact data assigner.
*
*/


#ifndef CPBK2CONTACTIMPPDATAASSIGNER_H
#define CPBK2CONTACTIMPPDATAASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"

// FORWARD DECLARATIONS
class MPbk2ContactAssignerObserver;
class TVPbkFieldVersitProperty;

// CLASS DECLARATION

/**
 * Phonebook 2 textual contact data assigner.
 */
class CPbk2ContactImppDataAssigner : public CActive,
                                     public MPbk2ContactAssigner
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @return  A new instance of this class.
         */
        static CPbk2ContactImppDataAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactImppDataAssigner();

    public: // From MPbk2ContactAssigner
        void AssignDataL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                const MVPbkFieldType* aFieldType,
                const HBufC* aDataBuffer );
        void AssignAttributeL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                TPbk2AttributeAssignData aAttributeAssignData );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Implementation
        CPbk2ContactImppDataAssigner(
                MPbk2ContactAssignerObserver& aObserver );
        void IssueRequest();
        void ParseDataBuffer(TPtrC* axSP, TPtrC* aFirstName, TPtrC* aLastName,
                TPtrC* aNickname, TPtrC aDataBuffer);
        void UpdateField(const TVPbkFieldVersitProperty& aVersitProp,
                const TDesC& aValue, MVPbkStoreContact* aStoreContact);
        inline TBool GetDataBufferPart(TPtrC* aBuffer, TPtrC* aPart);

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Index of the added field
        TInt iIndex;
    };

TBool CPbk2ContactImppDataAssigner::GetDataBufferPart(
        TPtrC* aBuffer, TPtrC* aPart)
    {
    const TChar KSeparator = '\n';

    TInt pos = aBuffer->Locate(KSeparator);
    if (pos == KErrNotFound)
        {
        aPart->Set(*aBuffer);
        return EFalse;
        }
    else
        {
        aPart->Set(aBuffer->Left(pos));
        if (pos == aBuffer->Length()-1)
            {
            return EFalse;
            }
        }
    aBuffer->Set(aBuffer->Mid(++pos));
    return ETrue;
    }

#endif // CPBK2CONTACTIMPPDATAASSIGNER_H

// End of File
