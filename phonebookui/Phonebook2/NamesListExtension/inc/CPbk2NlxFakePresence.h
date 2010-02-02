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
* Description:
*
*/

#ifndef NLXFAKEPRESENCE_H
#define NLXFAKEPRESENCE_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <CVPbkContactManager.h>
#include <mcontactpresence.h>


// FORWARD DECLARATIONS
class MContactPresenceObs;
class CVPbkContactManager;

// CLASS DECLARATION


/**
 * A subscription event generator.
 */
NONSHARABLE_CLASS(CPbk2NlxFakePresenceGenerator) : public CTimer
    {
    public:  // Constructors and destructor
        
        static CPbk2NlxFakePresenceGenerator* NewLC(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager,
            const MVPbkContactLink& aContactLink );
        
        ~CPbk2NlxFakePresenceGenerator();

    private: // Construction & destruction
        CPbk2NlxFakePresenceGenerator(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager );
        void ConstructL( const MVPbkContactLink& aContactLink );
    public:
        void Start();
        TBool Equals( const MVPbkContactLink& aContactLink ) const;
    private: // from CActive
        void RunL();
    private: // new methods
        TPtrC8 NextIconElement();
        TInt Random( TInt aMax ) const;
    private: // Data
        MContactPresenceObs& iObserver;
        CVPbkContactManager& iContactManager;
        MVPbkContactLink* iContactLink;
        TInt iNextIconType;
        mutable TInt64 iRandomSeed;
    };

/**
 * An active icon request.
 */
NONSHARABLE_CLASS(CPbk2NlxFakeIconRequest) : public CTimer
    {
    public:  // Constructors and destructor
        
        static CPbk2NlxFakeIconRequest* NewLC(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager,
            const TDesC8& aBrandId,
            const TDesC8& aElementId );
        
        ~CPbk2NlxFakeIconRequest();

    private: // Construction & destruction
        CPbk2NlxFakeIconRequest(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager );
        void ConstructL(
            const TDesC8& aBrandId,
            const TDesC8& aElementId );
    public:
        void Start();
        TBool Equals( const TDesC8& aBrandId, const TDesC8& aElementId ) const;
    private: // from CActive
        void RunL();
    private: // new methods
        static TInt IconIndexForElementId( const TDesC8& aElementId );
    private: // Data
        MContactPresenceObs& iObserver;
        CVPbkContactManager& iContactManager;
        HBufC8* iBrandId;
        HBufC8* iElementId;
    };



/**
 * Fake presence client.
 */
NONSHARABLE_CLASS(CPbk2NlxFakePresence) :
        public CBase,
        public MContactPresence
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2NlxFakePresence* NewL(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager );
        
        ~CPbk2NlxFakePresence();

    private: // Construction & destruction
        CPbk2NlxFakePresence(
            MContactPresenceObs& aObserver,
            CVPbkContactManager& aContactManager );
        void ConstructL();

    public: // From MContactPresence

        void Close();

        void SubscribePresenceInfoL( const MDesC8Array& aLinkArray );

        void CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray );

        TInt GetPresenceIconFileL( const TDesC8& aBrandId, const TDesC8& aElementId );
        void CancelOperation( TInt aOpId ){};

        void CancelAll();
        
        void SetPresenceIconSize(const TSize aSize){};

    private: // new methods
        void CancelAndDeletePresenceGenerator( const MVPbkContactLink& aLink );
        CPbk2NlxFakePresenceGenerator* FindGenerator( const MVPbkContactLink& aLink );
        
    private: // Data
        MContactPresenceObs& iObserver;
        CVPbkContactManager& iContactManager;
        RPointerArray<CPbk2NlxFakePresenceGenerator> iGenerators; // owns
        RPointerArray<CPbk2NlxFakeIconRequest> iIconRequests; // owns
    };

#endif // NLXFAKEPRESENCE_H
           
// End of File
