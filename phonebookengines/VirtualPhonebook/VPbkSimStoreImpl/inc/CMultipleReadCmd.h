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
* Description:  A class that can read multiple contacts from ETel store.
*                Uses ETel RMobilePhoneBookStore Read
*
*/



#ifndef VPBKSIMSTOREIMPL_CMULTIPLEREADCMD_H
#define VPBKSIMSTOREIMPL_CMULTIPLEREADCMD_H

//  INCLUDES
#include <MVPbkSimCommand.h>
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class CVPbkETelCntConverter;
class CVPbkSimContactBuf;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CBasicStore;

// CLASS DECLARATION

/**
*  A class that can read multiple contacts from ETel store.
*
*/
NONSHARABLE_CLASS(CMultipleReadCmd) : public CActive,
                                      public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor. Reads all contacts from ETel.
        * @param aBasicStore the parent store for the contacts
        * @return a new instance of this class
        */
        static CMultipleReadCmd* NewLC( CBasicStore& aBasicStore );
        
        /**
        * Two-phased constructor.
        * @param aBasicStore the parent store for the contacts
        * @param aFirstIndexToRead the first ETel index to read ( >=1 )
        * @param aLastIndexToRead the last ETel index to read 
        * @return a new instance of this class
        */
        static CMultipleReadCmd* NewLC( CBasicStore& aBasicStore, 
                TInt aFirstIndexToRead, TInt aLastIndexToRead );
        
        /**
        * Two-phased constructor.
        * 
        * This constructor is for the case that command is created before
        * the index is known.
        * @param aBasicStore the parent store for the contacts
        * @param aIndexToRead a reference to the index to read
        * @return a new instance of this class
        */
        static CMultipleReadCmd* NewLC( CBasicStore& aBasicStore, 
                TInt& aIndexToRead );

        /**
        * Destructor.
        */
        virtual ~CMultipleReadCmd();

    public: // Functions from base classes

        /**
        * From MVPbkSimCommand
        */
        void Execute();

        /**
        * From MVPbkSimCommand
        */
        void AddObserverL( MVPbkSimCommandObserver& aObserver );

        /**
        * From MVPbkSimCommand
        */
        void CancelCmd();
        
    private: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );
        
    private:    // Construction

        /**
        * C++ constructor.
        */
        CMultipleReadCmd( CBasicStore& aBasicStore,
            TInt aFirstIndexToRead, TInt aLastIndexToRead );
        
        /**
        * C++ constructor.
        */
        CMultipleReadCmd::CMultipleReadCmd( CBasicStore& aBasicStore, 
                TInt& aIndexToRead );
            
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// Completes the active object with given result
        void DoDummyRequest( TInt aResult );
        /// Does the ETel request
        void DoRequest();
        /// Checks that is buffer growing without control
        TBool IsBufferSizeInSane();
        /// Enlarges the buffer if it wasn't big enough
        void EnlargeBufferL();
        /// Converts contact from buffer
        void ConvertContactsL();
        /// Returns ETrue if buffer was big enough
        TBool WasBufferBigEnough();
        /// Inserts contacts to contact array
        void UpdateContactArrayL();
        /// Fills buffer with zeros and sets the length to zero
        void ResetBuffer();

    private:    // Data
        /// The parent store
        CBasicStore& iBasicStore;
        /// The first ETel index to read, minimum is 1, not zero.
        TInt iFirstIndexToRead;
        /// The amount contacts that is tried to read at once
        TInt iLastIndexToRead;
        /// The index to read in the case the index is not known when
        /// the command is created.
        const TInt& iIndexToRead;
        /// Own: a buffer for ETel contacts
        HBufC8* iBuffer;
        /// A pointer to the buffer
        TPtr8 iBufferPtr;
        /// Own: converts ETel contacts to own format
        CVPbkETelCntConverter* iCntConverter;
        /// an array for the contacts. Owns contact until contact array
        /// takes the ownership
        RPointerArray<CVPbkSimContactBuf> iCntArray;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CMULTIPLEREADCMD_H
            
// End of File
