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
* Description:  A command for setting ringing tone.
*
*/


#ifndef CPBK2SETTONECMD_H
#define CPBK2SETTONECMD_H

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
class MVPbkStoreContactField;
class CPbk2FieldFocusHelper;

// CLASS DECLARATION

/**
 *  A command for setting ringing tone.
 *
 */
NONSHARABLE_CLASS(CPbk2SetToneCmd) : 
        public CPbk2SetToneBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl    UI control.
         * @return A new instance of this class.
         */
        static CPbk2SetToneCmd* NewL(
                MPbk2ContactUiControl& aUiControl );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2SetToneCmd();

    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed( TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed( TInt aReason,
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
        void ContactOperationFailed(TContactOp aOpCode, 
                TInt aErrorCode, TBool aErrorNotified);          
                	
    private: // Implementation
        CPbk2SetToneCmd( MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void SetToneL();
        void DoRelocateContactsL();
        TBool ContactHasRingToneField( MVPbkStoreContact* aContact );
        
    private: // data
        /// Own: field focus preserver
        CPbk2FieldFocusHelper* iFieldFocusHelper;
    };

#endif // CPBK2SETTONECMD_H
            
// End of File
