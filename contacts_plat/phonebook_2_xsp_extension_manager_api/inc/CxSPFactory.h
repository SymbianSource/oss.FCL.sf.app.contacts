/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Phonebook xSP extension factory ECom interface
*
*/


#ifndef __CXSPFACTORY_H__
#define __CXSPFACTORY_H__

//  INCLUDES

#include <ecom/ecom.h>
#include "MxSPFactory.h"


// CLASS DECLARATION

/**
 * Phonebook xSP extension factory ECom interface. See MxSPFactory.
 */
class CxSPFactory : public CBase, public MxSPFactory
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aUid ECom plugin implementation uid
         *
         * @return Newly created CxSPFactory.
         */    
        static CxSPFactory* NewL( TUid aUid );
        
        /**
         * Destructor
         */
        ~CxSPFactory();
        
    public: // from MxSPFactory
        /**
         * Called in an application start-up and when the store
         * configuration changes. The extension can update the store
         * property array by adding or removing properties.
         *
         * @param aPropertyArray    Phonebook 2 store property array.
         */
        void UpdateStorePropertiesL( CPbk2StorePropertyArray& aPropertyArray );
            
    private:
        TUid iDtorIDKey;
    };

inline CxSPFactory* CxSPFactory::NewL( TUid aUid )
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL
        ( aUid, _FOFF( CxSPFactory, iDtorIDKey ));

    return reinterpret_cast<CxSPFactory*>( ptr );
    }

inline CxSPFactory::~CxSPFactory()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

inline void CxSPFactory::UpdateStorePropertiesL( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {   
    }

#endif // __CXSPFACTORY_H__

// End of File
