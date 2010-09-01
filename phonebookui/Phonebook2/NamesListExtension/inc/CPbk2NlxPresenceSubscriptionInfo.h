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
* Description:  Holds information about a presence subscription.
*
*/


#ifndef NLXSUBSCRIPTIONINFO_H
#define NLXSUBSCRIPTIONINFO_H

//  INCLUDES
#include <e32base.h>



// FORWARD DECLARATIONS
class CPbk2NlxPresenceIconInfo;
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Holds information about a presence subscription.
 */
NONSHARABLE_CLASS(CPbk2NlxPresenceSubscriptionInfo) :
        public CBase
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactLink Contact link. Takes ownership.
         * @return  A new instance of this class.
         */
        static CPbk2NlxPresenceSubscriptionInfo* NewL( MVPbkContactLink* aContactLink );
        
        ~CPbk2NlxPresenceSubscriptionInfo();

    private: // Construction & destruction
        CPbk2NlxPresenceSubscriptionInfo( );

    public: // New methods

        const MVPbkContactLink& ContactLink() const;

        MVPbkContactLink& ContactLink();

        /**
         * Set the icon info. Ownership is not taken!
         */
        void SetIconInfo( CPbk2NlxPresenceIconInfo* aIconInfo );

        /**
         * Accessor for icon info object. Might be NULL if not yet set.
         * @return Icon info or NULL.
         */
        const CPbk2NlxPresenceIconInfo* IconInfo() const;
        
        /**
         * Set TBool iSubscribed when contact link was subscribed
         */
        void Subscribed();
        
        /**
         * Check if contact link was subscribed
         */
        TBool IsSubscribed();
        
    private: // Data
        MVPbkContactLink* iContactLink; // owns
        CPbk2NlxPresenceIconInfo* iIconInfo; // does not own
        TBool iSubscribed;
    };

#endif // NLXSUBSCRIPTIONINFO_H
           
// End of File
