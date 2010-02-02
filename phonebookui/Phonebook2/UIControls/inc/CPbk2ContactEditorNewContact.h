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
* Description:  A contact editor strategy for the new contact creation.
*
*/



#ifndef CPBK2CONTACTEDITORNEWCONTACT_H
#define CPBK2CONTACTEDITORNEWCONTACT_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include "MPbk2ContactEditorStrategy.h"

// FORWARD DECLARATIONS
class TPbk2ContactEditorParams;
class CPbk2PresentationContact;
class CPbk2InputAbsorber;

// CLASS DECLARATION

/**
 *  A contact editor strategy for the new contact creation.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorNewContact) : 
            public CActive,
            public MPbk2ContactEditorStrategy,
            private MVPbkContactObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *
         * @param aParams    The contact editor parameters.
         * @param aContact   Presentation level contact.
         * @return A new instance of this class.
         */
        static CPbk2ContactEditorNewContact* NewL(
            TPbk2ContactEditorParams& aParams,
            CPbk2PresentationContact* aContact);
        
        /**
         * Destructor.
         */
        ~CPbk2ContactEditorNewContact();
        
    private: // From CActive        
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    public: // From MPbk2ContactEditorStrategy
        void SaveContactL(
            MPbk2ContactEditorEventObserver& aObserver,
		    MPbk2ContactEditorEventObserver::TParams& aParams);
		void DeleteContactL(
		    MPbk2ContactEditorEventObserver& aObserver,
		    MPbk2ContactEditorEventObserver::TParams& aParams);
		const TDesC& DefaultTitle() const;
		void SwitchContactL(
            CPbk2PresentationContact* aContact);
        void GetHelpContext(TCoeHelpContext &aContext) const;
        TBool StopQuery();
        
    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
            MVPbkContactObserver::TContactOpResult aResult);
        void ContactOperationFailed(
            MVPbkContactObserver::TContactOp aOpCode,
            TInt aErrorCode, TBool aErrorNotified);

    private: // Implementation
        CPbk2ContactEditorNewContact(
            CPbk2PresentationContact* aContact);
        void ConstructL();
        static TInt IdleDeleteCallBack(
            TAny* aSelf);
        void HandleIdleDelete();

    private: // Data
        /// Ref: The new contact
        CPbk2PresentationContact* iContact;
        /// Own: For absorbing user input while saving the contact
        CPbk2InputAbsorber* iInputAbsorber;
        /// Ref: The current contact event observer
        MPbk2ContactEditorEventObserver* iObserver;
        /// Own: Parameters for the event observer
        MPbk2ContactEditorEventObserver::TParams iEditorParams;
        /// Own: For asynchronously completing DeleteContactL
        CIdle* iIdleDelete;
        /// Own: "New contact" title for the editor
        HBufC* iDefaultTitle;
        /// Own: Last result of completed event
        MVPbkContactObserver::TContactOpResult iLatestResult;
    };

#endif  // CPBK2CONTACTEDITORNEWCONTACT_H
            
// End of File
