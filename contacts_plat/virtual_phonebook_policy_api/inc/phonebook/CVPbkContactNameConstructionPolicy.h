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
* Description:  An ECOM interface for the virtual phonebook name 
*                construction policy.
*                The name construction policy implementation must have an UID
*                KVPbkContactNameConstructionPolicyImplementationUID
*
*/


#ifndef CVPBKCONTACTNAMECONSTRUCTIONPOLICY_H
#define CVPBKCONTACTNAMECONSTRUCTIONPOLICY_H

#include <e32std.h>
#include <ecom/ecom.h>
#include <vpbkpolicyuid.h>
#include <mvpbkcontactnameconstructionpolicy.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;

/**
 * Virtual Phonebook contact name construction policy
 */
class CVPbkContactNameConstructionPolicy : 
        public CBase,
        public MVPbkContactNameConstructionPolicy
    {
    public: // Types
        struct TParam
            {
            /**
             * @param aMasterFieldTypeList Field type list.
             */
            inline TParam( const MVPbkFieldTypeList& aMasterFieldTypeList );

            /// Ref: Master field type list of vpbk
            const MVPbkFieldTypeList& iMasterFieldTypeList;
            ///Own: Reserve for future extension
            TAny* iSpare;
            };
            
    public: // Interface
        /**
         * Creates a new object of this class.
         *
         * @param aParam parameters for the name construction policy
         * @return A new object of this class.
         */
        static CVPbkContactNameConstructionPolicy* NewL( TParam& aParam );

        /**
         * Destructor.
         */
        ~CVPbkContactNameConstructionPolicy();
        
    private:
        /// Own: ID key for destructor
        TUid iDtorIDKey;
    };

// INLINE FUNCTIONS
inline CVPbkContactNameConstructionPolicy::TParam::TParam(
        const MVPbkFieldTypeList& aMasterFieldTypeList ) :
    iMasterFieldTypeList( aMasterFieldTypeList )
    {
    }

inline CVPbkContactNameConstructionPolicy* CVPbkContactNameConstructionPolicy::NewL(
        TParam& aParam )
    {
    // Instantiate the correct ECom plugin
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
            TUid::Uid( KVPbkContactNameConstructionPolicyImplementationUID ),
            _FOFF( CVPbkContactNameConstructionPolicy, iDtorIDKey), &aParam );

    return reinterpret_cast<CVPbkContactNameConstructionPolicy*>( ptr );
    }
    
inline CVPbkContactNameConstructionPolicy::~CVPbkContactNameConstructionPolicy()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }   

#endif // CVPBKCONTACTNAMECONSTRUCTIONPOLICY_H

// End of File
