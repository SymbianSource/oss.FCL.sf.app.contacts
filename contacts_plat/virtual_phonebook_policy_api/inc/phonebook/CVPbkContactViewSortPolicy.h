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
* Description:  an interface for sort policy for the Virtual Phonebook.
*                The implementation uses UID: KVPbkSortPolicyImplementationUID
*
*/


#ifndef CVPBKCONTACTVIEWSORTPOLICY_H
#define CVPBKCONTACTVIEWSORTPOLICY_H

#include <ecom/ecom.h>
#include <vpbkpolicyuid.h>
#include <mvpbkcontactviewsortpolicy.h>

class CVPbkContactManager;

/**
 * ECom plugin for contact sort policy.
 */
class CVPbkContactViewSortPolicy : public CBase,
                                   public MVPbkContactViewSortPolicy
    {
    public: // Constructor and destructor
        struct TParam
            {
            inline TParam(const MVPbkFieldTypeList& aFieldTypes,
                          const MVPbkFieldTypeList& aSortOrder,
                          RFs* aFs = NULL);

            ///Ref: Global list of available field types.
            const MVPbkFieldTypeList& iFieldTypes;
            ///Ref: Initial sort order for the sort policy.
            const MVPbkFieldTypeList& iSortOrder;
            ///Ref: (optional) handle to file server
            RFs* iFs;
            ///Own: Reserve for future extension
            TAny* iSpare;
            };

        /**
         * Returns new instance of this class.
         */
        static CVPbkContactViewSortPolicy* NewL(TParam& aParam);

        static CVPbkContactViewSortPolicy* NewL();

        ~CVPbkContactViewSortPolicy();

    private:
        ///Own: Destructor ID key
        TUid iDtorIDKey;
    };


// INLINE FUNCTIONS

inline CVPbkContactViewSortPolicy::TParam::TParam(
        const MVPbkFieldTypeList& aFieldTypes, 
        const MVPbkFieldTypeList& aSortOrder,
        RFs* aFs) :
    iFieldTypes(aFieldTypes),
    iSortOrder(aSortOrder),
    iFs(NULL)
    {
    if (aFs)
        {
        iFs = aFs;
        }
    }


inline CVPbkContactViewSortPolicy* CVPbkContactViewSortPolicy::NewL(TParam& aParam)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
        TUid::Uid(KVPbkSortPolicyImplementationUID), 
        _FOFF(CVPbkContactViewSortPolicy, iDtorIDKey), 
        &aParam);

    return reinterpret_cast<CVPbkContactViewSortPolicy*>(ptr);
    } 

inline CVPbkContactViewSortPolicy* CVPbkContactViewSortPolicy::NewL()
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL
        (TUid::Uid(KVPbkSortPolicyImplementationUID), 
         _FOFF(CVPbkContactViewSortPolicy, iDtorIDKey), 
         NULL);

    return reinterpret_cast<CVPbkContactViewSortPolicy*>(ptr);
    } 
    
inline CVPbkContactViewSortPolicy::~CVPbkContactViewSortPolicy()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }   


#endif // CVPBKCONTACTVIEWSORTPOLICY_H

//End of file
