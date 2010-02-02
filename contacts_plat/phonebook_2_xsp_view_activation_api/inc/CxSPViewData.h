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
* Description:  
*
*/



#ifndef CXSPVIEWDATA_H
#define CXSPVIEWDATA_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION

/**
 * 
 *
 */
class CxSPViewData : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instace of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CxSPViewData* NewL();        

        /**
         * Creates a new instace of this class initialized from a buffer.
         * @see CxSPViewData::PackL for constructing the buffer.
         *
         * @param aBuf  Buffer to initialize this instance from.
         * @return  A new instance of this class.
         */
        IMPORT_C static CxSPViewData* NewL( const TDesC8& aBuf );        

        /**
         * Destructor.
         */
        ~CxSPViewData();

    public: // Getters

        /**
         * Returns ECom Id.
         * 0 if not set.
         *
         * @return  ECom Id.
         */
        IMPORT_C TUint32 EComId() const;

        /**
         * Returns original view Id.
         * KErrNotFound if not set.
         *
         * @return  original view Id.
         */
        IMPORT_C TInt32 OriginalViewId() const;

        /**
         * Returns icon Id.
         * KErrNotFound if not set.
         *
         * @return  icon Id.
         */
        IMPORT_C TInt32 IconId() const;

        /**
         * Returns mask Id.
         * KErrNotFound if not set.
         *
         * @return  mask Id.
         */
        IMPORT_C TInt32 MaskId() const;

        /**
         * Returns icon file.
         * KNullDesC if not set.
         *
         * @return  icon file.
         */
        IMPORT_C const TDesC& IconFile() const;

        /**
         * Returns view name.
         * KNullDesC if not set.
         *
         * @return  view name.
         */
        IMPORT_C const TDesC& ViewName() const;       

    public: // Setters

        /**
         * Sets ECom Id.
         *
         * @param aEComId  The ECom Id to set.
         */
        IMPORT_C void SetEComId( TUint32 aEComId );

        /**
         * Sets original view Id.
         *
         * @param aOriginalViewId  The original view Id to set.
         */
        IMPORT_C void SetOriginalViewId( TInt32 aOriginalViewId );

        /**
         * Sets icon Id.
         *
         * @param aIconId  The icon Id to set.
         */
        IMPORT_C void SetIconId( TInt32 aIconId );

        /**
         * Sets mask Id.
         *
         * @param aMaskId  The mask Id to set.
         */
        IMPORT_C void SetMaskId( TInt32 aMaskId );

        /**
         * Sets icon file.
         *
         * @param aIconFile    The icon file to set.
         */
        IMPORT_C void SetIconFileL( const TDesC& aIconFile );

        /**
         * Sets view name.
         *
         * @param aViewName    The view name to set.
         */
        IMPORT_C void SetViewNameL( const TDesC& aViewName );
                
        /**
         * Reset data to an empty state.
         */
        IMPORT_C void Reset();
        

    public: // Client-server support

        /**
         * Packages and returns this object in a buffer.
         * Caller is responsible for deleting the buffer.
         *
         * @return  This view data instance packaged into a buffer.
         */
        IMPORT_C HBufC8* PackL() const;
        
        /**
         * Sets this view data from given packaged buffer.
         *
         * @param aPack     Packaged view data buffer.
         */
        IMPORT_C void UnpackL( const TDesC8& aPack );   

    private: // Implementation
    
        CxSPViewData();
        void ConstructL( const TDesC8& aBuf );
        void ExternalizeL( RWriteStream& aStream ) const;
        void InternalizeL( RReadStream& aStream );

    private: // Data
    
        /// Own: ECom plugin id
    	TUint32 iEComId;
    	/// Own: Original/unmapped view id
    	TInt32 iOriginalViewId;
    	/// Own: icon id
    	TInt32 iIconId;
    	/// Own: mask id
    	TInt32 iMaskId;
    	/// Own: icon file
    	HBufC* iIconFile;
    	/// Own: view name
    	HBufC* iViewName;
    };

#endif // CXSPVIEWDATA_H

// End of File
