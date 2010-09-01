/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command for setting ringing tone.
*
*/


#ifndef CPGUSETTONECMD_H
#define CPGUSETTONECMD_H

//  INCLUDES
#include <e32base.h>
#include <CPbk2SetToneBase.h>
#include <MPbk2ContactRelocatorObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class MVPbkFieldType;
class CPbk2ContactRelocator;
class MVPbkContactLinkArray;
class MVPbkContactGroup;

// CLASS DECLARATION

/**
 *  A command for setting ringing tone.
 *
 */
NONSHARABLE_CLASS(CPguSetToneCmd) : 
        public CPbk2SetToneBase
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         * @param aUiControl    UI control.
         * @return A new instance of this class.
        */
        static CPguSetToneCmd* NewL(
                MPbk2ContactUiControl& aUiControl );
        
        /**
         * Destructor.
         */
        ~CPguSetToneCmd();

    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts );
        void RelocationProcessComplete();
        
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);
              
	private: // From MVPbkContactObserver
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed
            (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);    
            
    private: // Implementation
        CPguSetToneCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void SetToneL();
        void DoRelocateContactsL();
        void CreateLinkArrayL( MVPbkContactLinkArray* aArray );
        void ConfirmationL();
        
    private: // data
        /// Own: contacts which are belonging to group
        CVPbkContactLinkArray* iContacts;
        /// Own: current contact retrieved from link array
        MVPbkStoreContact* iCurrentContact;
        /// Ref: group
        MVPbkContactGroup* iGroup;
        /// Own: current contact index
        TInt iCurrentIndex;
        /// Own: flag indicating is process decorator already started
        TBool iIsStarted;
    };

#endif // CPGUSETTONECMD_H
            
// End of File
