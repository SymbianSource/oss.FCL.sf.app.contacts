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
* Description:  An USIM ADN store that supports ADN specific store information
*
*/



#ifndef VPBKSIMSTOREIMPL_CUSIMADNSTORE_H
#define VPBKSIMSTOREIMPL_CUSIMADNSTORE_H

//  INCLUDES
#include "CCustomStore.h"

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  An USIM ADN store that supports ADN specific store information
*
*/
NONSHARABLE_CLASS(CUsimAdnStore) :
    public CCustomStore,
    private MVPbkSimPhoneObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParams the store parameters
        * @return a new instance of this class
        */
        static CUsimAdnStore* NewL( TStoreParams& aParams );
        
        /**
        * Destructor.
        */
        virtual ~CUsimAdnStore();

    public: // New functions
        
        /**
        * Returns the 3G ADN store information
        * @return the 3G ADN store information
        */
        inline RMmCustomAPI::T3GPBInfo& Get3GPBInfo();

    protected: // Functions from base classes
        
        /**
        * From CStoreBase
        */
        const TDesC& ETelName() const;
        
        /**
        * From CStoreBase
        */
        void CompleteSATRefresh( 
            MSimRefreshCompletion& aSimRefreshCompletion );
            
        /**
        * From MVPbkSimCntStore
        */
        TInt GetUSimStoreProperties( 
            TVPbkUSimStoreProperty& aUSimProperties ) const;

        /**
        * From CStoreBase
        */
        MVPbkSimCommand* CreateInitCommandL( 
            MVPbkSimCommandObserver& aObserver );

        /**
        * From CStoreBase
        */
        MVPbkSimCommand* CreateUpdateCommandL( TInt aSimIndex );
        
        /**
        * From CStoreBase
        */
        MVPbkSimCommand* CreateUpdateCommandUsingRefL( TInt& aSimIndexRef );
        
        /**
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
        
    private:    // Functions from base classes
    
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void PhoneOpened( MVPbkSimPhone& aPhone );

        /**
        * From MVPbkSimPhoneObserver
        */ 
        void PhoneError( MVPbkSimPhone& aPhone, 
            TErrorIdentifier aIdentifier, TInt aError );
        
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void ServiceTableUpdated( TUint32 aServiceTable );
        
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void FixedDiallingStatusChanged(
            TInt aFDNStatus );
        
    private:

        /**
        * C++ default constructor.
        */
        CUsimAdnStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// adds 3G adn info request command to the chain
        void Add3GPBInfoCmdToChainL( CCompositeCmdBase& aCmdChain );

    private:    // Data
        /// 3G phonebook information struct for ADN
        RMmCustomAPI::T3GPBInfo i3GPBInfo;
    };

// INLINE FUNCTIONS
inline RMmCustomAPI::T3GPBInfo& CUsimAdnStore::Get3GPBInfo()
    {
    return i3GPBInfo;
    }

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CUSIMADNSTORE_H
            
// End of File
