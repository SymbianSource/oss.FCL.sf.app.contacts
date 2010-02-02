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
* Description:  An own number store, contains the contacts in the MSISDN file
*                in (U)SIM
*
*/



#ifndef VPBKSIMSTOREIMPL_COWNNUMBERSTORE_H
#define VPBKSIMSTOREIMPL_COWNNUMBERSTORE_H

//  INCLUDES
#include "CStoreBase.h"

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  An own number store, contains the contacts in the MSISDN file in (U)SIM
*
*/
NONSHARABLE_CLASS(COwnNumberStore) : public CStoreBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParams the store parameters
        * @return a new instance of this class
        */
        static COwnNumberStore* NewL( TStoreParams& aParams );
        
        /**
        * Destructor.
        */
        virtual ~COwnNumberStore();

    public: // Functions from base classes
        
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
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
        
    private:  // Functions from base classes
        
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
        
    private:

        /**
        * C++ default constructor.
        */
        COwnNumberStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// Converts contact to ETel own number contact
        void ConvertToOwnNumberContactL( const TDesC8& aData, 
            RMobileONStore::TMobileONEntryV1& aONContact );

    private:    // Data
        /// The ETel own number store
        RMobileONStore iETelStore;
        /// Multimode ETel ON phone book info structure.
        RMobileONStore::TMobileONStoreInfoV1 iETelStoreInfo;
        /// A package for the ETel phonebook info
        RMobileONStore::TMobileONStoreInfoV1Pckg iETelInfoPckg;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_COWNNUMBERSTORE_H
            
// End of File
