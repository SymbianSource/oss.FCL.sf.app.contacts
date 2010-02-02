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
* Description:  A store that doesn't have a cache in the TSY side.
*                Uses ETel RMobilePhoneBookStore 
*
*/



#ifndef VPBKSIMSTOREIMPL_CBASICSTORE_H
#define VPBKSIMSTOREIMPL_CBASICSTORE_H

//  INCLUDES
#include "CStoreBase.h"
#include <etelmm.h>

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A store that doesn't have a cache in the TSY side. 
*  Uses ETel RMobilePhoneBookStore
*
*/
NONSHARABLE_CLASS(CBasicStore) : public CStoreBase
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CBasicStore();

    public: // New functions
        
        /**
        * Returns the ETel phonebook store
        * @return the ETel phonebook store
        */
        inline RMobilePhoneBookStore& ETelStore();

    public: // Functions from base classes
        
        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* SaveL( const TDesC8& aData, TInt& aSimIndex, 
            MVPbkSimContactObserver& aObserver );
        
        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* DeleteL( RVPbkStreamedIntArray& aSimIndexes, 
                      MVPbkSimContactObserver& aObserver );
         
        /**
        * From MVPbkSimCntStore
        */
        TInt GetGsmStoreProperties( 
            TVPbkGsmStoreProperty& aGsmProperties ) const;

        /**
        * From MVPbkSimCntStore
        */
        TInt GetUSimStoreProperties( 
            TVPbkUSimStoreProperty& aUSimProperties ) const;
        
        /**
        * From CStoreBase
        */
        RMobilePhoneStore& ETelStoreBase();
        
        /**
        * From CStoreBase
        */
        RMobilePhoneStore::TMobilePhoneStoreInfoV1& ETelStoreInfo();
        
        /**
        * From CStoreBase
        */
        TDes8& ETelStoreInfoPackge();
        
        /**
        * From CStoreBase
        */
        void OpenETelStoreL();

        /**
        * From CStoreBase
        */
        void CloseETelStore();

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
        
    protected:  // New functions
        
        /**
        * Adds a read command to the command chain.
        * @param aCmdChain the command chain.
        * @param aFromSimIndex the first sim index or KErrNotFound if
        *           the command should read all contacts.
        * @param aToSimIndex the last sim index or KErrNotFound if
        *           the command should read all contacts.
        */
        void AddMultipleReadCmdToChainL( CCompositeCmdBase& aCmdChain,
            TInt aFromSimIndex, TInt aToSimIndex );

    protected:

        /**
        * C++ constructor.
        * @param aPhone an open phone subsession
        * @param aIdentifier the store identifier
        */
        CBasicStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// The ETel store
        RMobilePhoneBookStore iETelStore;
        /// Multimode ETel phone book info structure.
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 iETelStoreInfo;
        /// A package for the ETel phonebook info
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg iETelInfoPckg;
    };

// INLINE FUNCTIONS
inline RMobilePhoneBookStore& CBasicStore::ETelStore()
    {
    return iETelStore;
    }
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CBASICSTORE_H
            
// End of File
