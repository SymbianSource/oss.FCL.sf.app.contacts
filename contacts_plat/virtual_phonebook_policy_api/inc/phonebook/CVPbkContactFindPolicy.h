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
* Description:  An ECOM interface for the virtual phonebook find policy.
*                The find policy implementation must have an UID
*                KVPbkFindPolicyImplementationUID
*
*/


#ifndef CVPBKCONTACTFINDPOLICY_H
#define CVPBKCONTACTFINDPOLICY_H

#include <e32std.h>
#include <ecom/ecom.h>
#include <VPbkPolicyUid.h>
#include <MVPbkContactFindPolicy.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class RFs;

/**
 * Virtual Phonebook contact find policy.
 */
class CVPbkContactFindPolicy :
        public CBase,
        public MVPbkContactFindPolicy
    {
    public: // Interface
    
        /**
         * Paramters for NewL
         */
        struct TParam
            {
            inline TParam( const MVPbkFieldTypeList& aFieldTypeList,
                           RFs& aRFs );
            /// Ref: The master field type list.
            const MVPbkFieldTypeList& iFieldTypeList;
            /// Ref: An open file session.
            RFs& iRFs;
            /// Own: Reserve for future extension
            TAny* iSpare;
            };

        /**
         * Creates a new object of this class.
         * MatchContactNameL service is not supported.
         *
         * @return A new object of this class.
         */
        static CVPbkContactFindPolicy* NewL();

        /**
         * Creates a new object of this class.
         * MatchContactNameL service is supported if using this
         * constructor.
         *
         * @param aContactManager Contact manager
         * @return A new object of this class.
         */
        static CVPbkContactFindPolicy* NewL( TParam& aParam );

        /**
         * Destructor.
         */
        ~CVPbkContactFindPolicy();

    private:
        /// Own: ID key for destructor
        TUid iDtorIDKey;
    };


// INLINE FUNCTIONS
inline CVPbkContactFindPolicy::TParam::TParam(
        const MVPbkFieldTypeList& aFieldTypeList,
        RFs& aRFs ) :
    iFieldTypeList( aFieldTypeList ),
    iRFs( aRFs )
    {
    }


inline CVPbkContactFindPolicy* CVPbkContactFindPolicy::NewL()
    {
    // Instantiate the correct ECom plugin
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
            TUid::Uid( KVPbkFindPolicyImplementationUID ),
            _FOFF( CVPbkContactFindPolicy, iDtorIDKey),
            NULL);

    return reinterpret_cast<CVPbkContactFindPolicy*>( ptr );
    }

inline CVPbkContactFindPolicy* CVPbkContactFindPolicy::NewL( TParam& aParam )
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
        TUid::Uid( KVPbkFindPolicyImplementationUID ),
        _FOFF( CVPbkContactFindPolicy, iDtorIDKey ),
        &aParam );

    return reinterpret_cast<CVPbkContactFindPolicy*>( ptr );
    }

inline CVPbkContactFindPolicy::~CVPbkContactFindPolicy()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

#endif // CVPBKCONTACTFINDPOLICY_H

// End of File
