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
* Description:  An ECOM class for store plugin implementations.
*                Store implementations derives from this class.
*
*                Implementation set default data as:
*                default_data  KVPbkStorePluginDataString
*                Implementation sets opaque data as the domain part of the URIs.
*                E.g. a plugin that implements contact model store uses:
*                opaque_data  "cntdb"
*
*/



#ifndef CVPBKCONTACTSTOREDOMAIN_H
#define CVPBKCONTACTSTOREDOMAIN_H

// INCLUDES
#include <ecom/ecom.h>
#include "mvpbkcontactstorelist.h"
#include "mvpbkcontactoperationfactory.h"
#include "mvpbkcontactoperationfactory2.h"

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class MVPbkContactStoreListObserver;
class MVPbkContactStore;
class MVPbkStoreContact;
class MVPbkContactLink;
class MVPbkFieldTypeList;
class MVPbkContactOperation;
class MVPbkSingleContactOperationObserver;
class CVPbkContactStoreUriArray;

/**
 * Virtual contact storage domain class. 
 * Virtual Phonebook Store plugins must derive from this class,
 * so that ecom can instantiate the plugin component. A domain
 * can contains stores.
 */
class CVPbkContactStoreDomain : 
        public CBase,
        public MVPbkContactStoreList,
        public MVPbkContactOperationFactory,
        public MVPbkContactOperationFactory2 
    {
    public: // Constructor and desctructor
        class TStoreDomainParam
            {
            public:
                TStoreDomainParam( const CVPbkContactStoreUriArray& aURIList, 
                                   const TSecurityInfo& iSecurityInfo );
            public: // data
                ///Ref: Contact store URI array
                const CVPbkContactStoreUriArray& iURIList;
                ///Ref: URI domain
                const TDesC* iURIDomain;
                ///Ref: master field type list
                const MVPbkFieldTypeList* iFieldTypeList;
                ///Own: Security information
                const TSecurityInfo& iSecurityInfo;
                ///Own: Spare for future extension
                TAny* iSpare;
                ///Own: Spare for future extension
                TAny* iSpare2;
                ///Own: Spare for future extension
                TAny* iSpare3;

            };

        /**
         * ECom constructor.
         */
        static CVPbkContactStoreDomain* NewL(TUid aUid, TStoreDomainParam aParam);
        
        ~CVPbkContactStoreDomain();
        
    public: // Internal store API 
        /**
         * Returns the URI domain.
         */
        virtual const TDesC& UriDomain() const =0;
        
        /**
         * Loads a new contact store implementation to this domain.
         * If the contact store with given URI already exists, this function
         * does nothing.
         */
        virtual void LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri) =0;

    public: // From MVPbkContactStoreList
        TInt Count() const =0;
        MVPbkContactStore& At(TInt aIndex) const =0;
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const =0;
        void OpenAllL(MVPbkContactStoreListObserver& aObserver) =0;
        void CloseAll(MVPbkContactStoreListObserver& aObserver) =0;

    private: // data members
        /// Own: Destructor ID key
        TUid iDtorIDKey;
    };


// INLINE FUNCTIONS

inline CVPbkContactStoreDomain::TStoreDomainParam::TStoreDomainParam(
        const CVPbkContactStoreUriArray& aURIList, const TSecurityInfo& aSecurityInfo ) :
    iURIList(aURIList), iSecurityInfo( aSecurityInfo )
    {
    }

inline CVPbkContactStoreDomain* CVPbkContactStoreDomain::NewL(
        TUid aUid, 
        TStoreDomainParam aParam)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL
        (aUid, _FOFF(CVPbkContactStoreDomain, iDtorIDKey),
        static_cast<TAny*>(&aParam));

    return reinterpret_cast<CVPbkContactStoreDomain*>(ptr);
    } 
    
inline CVPbkContactStoreDomain::~CVPbkContactStoreDomain()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }   

#endif // CVPBKCONTACTSTOREDOMAIN_H

//End of file
