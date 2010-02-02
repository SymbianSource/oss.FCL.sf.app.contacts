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
* Description:  Phonebook 2 contact editor strategy for editing contact.
*
*/


#ifndef CPBK2CONTACTEDITOREDITCONTACT_H
#define CPBK2CONTACTEDITOREDITCONTACT_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include "MPbk2ContactEditorStrategy.h"

// FORWARD DECLARATIONS
class TPbk2ContactEditorParams;
class CPbk2PresentationContact;
class CPbk2InputAbsorber;
class MPbk2ContactNameFormatter;
class CPbk2ApplicationServices;
class CPbk2GeneralConfirmationQuery;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor strategy for editing contact.
 * Responsible for implementing contact editing strategy interface
 * for a existing contact being edited.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorEditContact) :
        public CActive,
        public MPbk2ContactEditorStrategy,
        private MVPbkContactObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   The contact editor parameters.
         * @param aContact  Presentation level contact.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorEditContact* NewL(
                TPbk2ContactEditorParams& aParams,
                CPbk2PresentationContact* aContact );

        /**
         * Destructor.
         */
        ~CPbk2ContactEditorEditContact();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public: // From MPbk2ContactEditorStrategy
        void SaveContactL(
                MPbk2ContactEditorEventObserver& aObserver,
                MPbk2ContactEditorEventObserver::TParams& aParams );
        void DeleteContactL(
                MPbk2ContactEditorEventObserver& aObserver,
                MPbk2ContactEditorEventObserver::TParams& aParams );
        const TDesC& DefaultTitle() const;
        void SwitchContactL(
                CPbk2PresentationContact* aContact );
        void GetHelpContext(
                TCoeHelpContext &aContext ) const;
        TBool StopQuery();

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                MVPbkContactObserver::TContactOpResult aResult );
        void ContactOperationFailed(
                MVPbkContactObserver::TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Implementation
        CPbk2ContactEditorEditContact(
                CPbk2PresentationContact* aContact );
        void ConstructL();
        static TInt IdleDeleteCallBack(
                TAny* aSelf );
        void HandleIdleDelete();
        void CancelDeleteL();

    private: // Data
        TBool IsActualContactEmpty();
        
    private: // Data
        /// Ref: The edited contact
        CPbk2PresentationContact* iContact;
        /// Own: For absorbing user input while saving the contact
        CPbk2InputAbsorber* iInputAbsorber;
        /// Ref: The current contact event observer
        MPbk2ContactEditorEventObserver* iObserver;
        /// Own: Parameters for the event observer
        MPbk2ContactEditorEventObserver::TParams iEditorParams;
        /// Ref: For unnamed default title
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: For asynchronously completing DeleteContactL
        CIdle* iIdleDelete;
        /// Own: Last result of completed event
        MVPbkContactObserver::TContactOpResult iLatestResult;
        /// Ref: Pbk2 application services
        CPbk2ApplicationServices* iAppServices;
        /// Own: Query dialog
        CPbk2GeneralConfirmationQuery* iQuery;
        /// Own: Check if the contact is real empty when creating edit dialog
        TBool iIsEmpty;
    };

#endif // CPBK2CONTACTEDITOREDITCONTACT_H

// End of File
