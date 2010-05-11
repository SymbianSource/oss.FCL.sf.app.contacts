/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An ECOM class for loading implementation of the policy
*
*/


#ifndef CVPBKCONTACTDUPLICATEPOLICY_H
#define CVPBKCONTACTDUPLICATEPOLICY_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <MVPbkContactDuplicatePolicy.h>
#include <VPbkPolicyUid.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkFieldTypeList;


/**
 * An ECOM class for loading implementation of the policy
 */
class CVPbkContactDuplicatePolicy : 
        public CBase,
        public MVPbkContactDuplicatePolicy
    {
    public:
        struct TParam
            {
            inline TParam( CVPbkContactManager& aContactManager );

            ///Ref: A contact manager containing a list of available field types.
            CVPbkContactManager& iContactManager;
            /// Ref: An optional list of types that should be used to
            /// find duplicates. If NULL then the policy defines the types.
            const MVPbkFieldTypeList* iFieldTypesForFind;
            ///Own: Reserve for future extension
            TAny* iSpare;
            };

        /**
         * Returns the policy implementation.
         * @return the policy implementation.
         */
        static CVPbkContactDuplicatePolicy* NewL( TParam& aParam );
    
        /**
         * Destructor
         */ 
        virtual ~CVPbkContactDuplicatePolicy();

    private: // data
        /// Own: ID key for destructor
        TUid iDtorIDKey;
    };


//INLINE FUNCTIONS

inline CVPbkContactDuplicatePolicy::TParam::TParam(
        CVPbkContactManager& aContactManager) :   
    iContactManager(aContactManager),
    iFieldTypesForFind( NULL ),
    iSpare( NULL )
    {
    }

inline CVPbkContactDuplicatePolicy* CVPbkContactDuplicatePolicy::NewL(
        TParam& aParam )
    {
    // Instantiate the correct ECom plugin
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
            TUid::Uid( KVPbkContactDuplicatePolicyImplementationUID ), 
            _FOFF( CVPbkContactDuplicatePolicy, iDtorIDKey ),
            &aParam );

    return reinterpret_cast<CVPbkContactDuplicatePolicy*>( ptr );
    }
    
inline CVPbkContactDuplicatePolicy::~CVPbkContactDuplicatePolicy()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

#endif // CVPBKCONTACTDUPLICATEPOLICY_H

// End of file
