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
* Description:  An API for sim contacts
*
*/



#ifndef CVPBKSIMCONTACTBASE_H
#define CVPBKSIMCONTACTBASE_H

//  INCLUDES
#include <e32base.h>
#include "MVPbkSimContact.h"

// FORWARD DECLARATIONS
class MVPbkSimContactObserver;
class CVPbkSimCntField;
class MVPbkSimCntStore;

// CLASS DECLARATION

/**
*  An API for sim contacts
*
*/
class CVPbkSimContactBase : public CBase,
                            public MVPbkSimContact
    {
    public: // Functions from base classes
        
        /**
        * From MVPbkSimContact
        */
        MVPbkSimCntStore& ParentStore() const;
        
        /**
        * From MVPbkSimContact
        */
        MVPbkSimStoreOperation* DeleteL( MVPbkSimContactObserver& aObserver );

        /**
        * From MVPbkSimContact
        */
        MVPbkSimContact::TFieldLookup FindField( 
            TVPbkSimCntFieldType aType ) const;

        /**
        * From MVPbkSimContact
        */
        void FindNextField( MVPbkSimContact::TFieldLookup& aLookup ) const;

    protected: // Construction

        /**
        * C++ constructor
        */
        CVPbkSimContactBase( MVPbkSimCntStore& aStore );

    protected:  // Data
        /// The the store in which this contact belongs
        MVPbkSimCntStore& iStore;
    };




#endif      // CVPBKSIMCONTACTBASE_H
            
// End of File
