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
* Description:  An ECOM interface for the virtual phonebook copy policies.
*                Stores have different demands on copying, like field type
*                mapping and other restrictions.
*
*                Implementation sets default data like this:
*                default_data  KVPbkCopyPolicyDataString;
*                Implemenation sets opaque data as the domain part of the URIs
                 it wants to handle e.g for contact model stores:
*                opaque_data  "cntdb";
*
*/


#ifndef CVPBKCONTACTCOPYPOLICY_H
#define CVPBKCONTACTCOPYPOLICY_H

// INCLUDES
#include <e32std.h>
#include <ecom/ecom.h>
#include <mvpbkcontactcopypolicy.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactStore;


/**
 * Virtual Phonebook contact copy policy.
 */
class CVPbkContactCopyPolicy : public CBase,
                               public MVPbkContactCopyPolicy
    {
    public: // Types
        struct TParam
            {
            /**
             * @param aContactManager A contact manager that has an open
             *                        target store for the copy.
             */
            inline TParam( CVPbkContactManager& aContactManager );

            ///Ref: A contact manager containing a list of available field types.
            CVPbkContactManager& iContactManager;
            ///Own: Reserve for future extension
            TAny* iSpare;
            };

    public: // Constructors and destructor
        /**
         * Two-phased constructor.
         *
         * @param aUid the implementation UID of the policy
         * @param aParam parameters for the copy policy
         * @return A new instance of this class
         */
        static CVPbkContactCopyPolicy* NewL( TUid aImplementationUid,
            TParam& aParam);

        /**
         * Destructor.
         */
        ~CVPbkContactCopyPolicy();

        /**
         * Returns the implementation UID of the policy.
         * @return The implementation UID of the policy.
         */
        TUid ImplementationUid() const;

    private:
        ///Own: Destructor ID key
        TUid iDtorIDKey;
        ///Own: implemenation uid
        TUid iImplementationUid;
    };


//INLINE FUNCTIONS

inline CVPbkContactCopyPolicy::TParam::TParam(
        CVPbkContactManager& aContactManager)
        :   iContactManager(aContactManager)
    {
    }

inline CVPbkContactCopyPolicy* CVPbkContactCopyPolicy::NewL(
        TUid aImplementationUid, TParam& aParam)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
            aImplementationUid,
            _FOFF(CVPbkContactCopyPolicy, iDtorIDKey),
            &aParam);
    CVPbkContactCopyPolicy* self =
        reinterpret_cast<CVPbkContactCopyPolicy*>(ptr);
    if (self)
        {
        self->iImplementationUid = aImplementationUid;
        }
    return self;
    }

inline CVPbkContactCopyPolicy::~CVPbkContactCopyPolicy()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }

inline TUid CVPbkContactCopyPolicy::ImplementationUid() const
    {
    return iImplementationUid;
    }

#endif // CVPBKCONTACTCOPYPOLICY_H

//End of file
