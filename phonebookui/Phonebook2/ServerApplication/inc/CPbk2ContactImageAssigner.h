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
* Description:  Phonebook 2 contact image assigner.
*
*/


#ifndef CPBK2CONTACTIMAGEASSIGNER_H
#define CPBK2CONTACTIMAGEASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"
#include <MPbk2ImageOperationObservers.h>
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CPbk2ImageManager;
class MPbk2ImageOperation;
class MPbk2ContactAssignerObserver;
class CPbk2DrmManager;
class CPbk2FieldPropertyArray;

// CLASS DECLARATION

/**
 * Phonebook 2 contact image assigner.
 * Responsible for:
 * - Removing previous contact image (thumbnail)
 * - Saving new contact image (thumbnail)
 * - If call image feature is on,
 *   saves the name of the image file into a separate contact field
 */
class CPbk2ContactImageAssigner : public CActive,
                                  public MPbk2ContactAssigner,
                                  protected MPbk2ImageSetObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver         Observer.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aFieldProperties  Phonebook field properties.
         * @return  A new instance of this class.
         */
        static CPbk2ContactImageAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager,
                CPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Destructor.
         */
        ~CPbk2ContactImageAssigner();


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

    private: // From MPbk2ImageSetObserver
        void Pbk2ImageSetComplete(
                MPbk2ImageOperation& aOperation );
        void Pbk2ImageSetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2ContactImageAssigner(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager,
                CPbk2FieldPropertyArray& aFieldProperties );
        void ConstructL(
                CVPbkContactManager& aContactManager );
        TInt PassesDrmCheckL(
                const HBufC* aDataBuffer,
                TBool& aIsProtected );
        void AssignImageL();
        void AssignImageFileNameL();
        void InsertTextDataL(
                MVPbkStoreContactField& aField,
                const HBufC& aDataBuffer ) const;
        void IssueRequest();

    private: // Data structures

        /**
         * Process states.
         */
        enum TProcessState
            {
            EAssigningImage,
            EAssigningImageFileName,
            EStopping,
            EDrmProtected
            };

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Image operation
        MPbk2ImageOperation* iImageOperation;
        /// Own: Image manager
        CPbk2ImageManager* iImageManager;
        /// Own: DRM manager
        CPbk2DrmManager* iDrmManager;
        /// Own: Error code
        TInt iError;
        /// Ref: Contact to operate with
        MVPbkStoreContact* iContact;
        /// Ref: Contact field type of image field
        const MVPbkFieldType* iFieldType;
        /// Ref: Data to assign
        const HBufC* iDataBuffer;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Phonebook field properties
        CPbk2FieldPropertyArray& iFieldProperties;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Index of the added field
        TInt iIndex;
    };

#endif // CPBK2CONTACTIMAGEASSIGNER_H

// End of File
