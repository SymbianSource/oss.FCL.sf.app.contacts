/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact editor strategy for the new own contact creation.
*
*/



#ifndef CPBK2CONTACTEDITORNEWOWNCONTACT_H
#define CPBK2CONTACTEDITORNEWOWNCONTACT_H

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
 *  A contact editor strategy for the new own contact creation.
 */
NONSHARABLE_CLASS( CPbk2ContactEditorNewOwnContact ) : 
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
        static CPbk2ContactEditorNewOwnContact* NewL(
            TPbk2ContactEditorParams& aParams,
            CPbk2PresentationContact* aContact);
        
        /**
         * Destructor.
         */
        ~CPbk2ContactEditorNewOwnContact();
        
    private: // From CActive        
        void RunL();
        void DoCancel();

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
        CPbk2ContactEditorNewOwnContact( CPbk2PresentationContact* aContact );
        void ConstructL();
        static TInt IdleDeleteCallBack( TAny* aSelf );
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
        /// Own: Last result of completed event
        MVPbkContactObserver::TContactOpResult iLatestResult;
    };

#endif  // CPBK2CONTACTEDITORNEWOWNCONTACT_H
            
// End of File
