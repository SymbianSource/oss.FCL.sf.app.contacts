/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Maps a Contact Model databases to a virtual Phonebook contact.
*
*/


#ifndef VPBKCNTMODEL_CCONTACTSTOREDOMAIN_H
#define VPBKCNTMODEL_CCONTACTSTOREDOMAIN_H


// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "CVPbkContactStoreDomain.h"


// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class CVPbkContactStoreList;
class CVPbkContactOperationFactory;
class RSharedDataClient;

namespace VPbkEngUtils
    {
    class CTextStore;
    }

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CFieldTypeMap;
class CContact;
class CFieldFactory;
class CAsyncContactOperation;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model databases to a virtual Phonebook contact.
 */
NONSHARABLE_CLASS( CContactStoreDomain ): 
        public CVPbkContactStoreDomain
    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aURIList              List of filename URI of the Contact Model database to map.
         * @param aURIDomain            URI domain names
         * @param aMasterFieldTypeList  Master field type list of vpbk.
         * @param aFieldTypeMap         Field type mapper. Ownership is transferred to 
         *                              this object in the end of this function.
         * @return A new instance of this class.
         */
        static CContactStoreDomain* NewL
            (const CVPbkContactStoreUriArray& aURIList, 
            const TDesC& aURIDomain,
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            CFieldTypeMap* aFieldTypeMap);

        /**
         * Creates a new instance of this class. 
         * Only for ECom use.
         *
         * @param aParam   ecom paramater.
         */
        static CContactStoreDomain* NewL(TAny* aParam);

        ~CContactStoreDomain();

    public:  // New functions
        /**
         * Returns the field type map to use for mapping Contact Model field
         * types to Virtual Phonebook field types.
         */
        const CFieldTypeMap& FieldTypeMap() const;

        /**
         * Returns the file server session.
         */
        RFs& FsSession() const;

        /**
         * Returns the master field type list.
         */
        const MVPbkFieldTypeList& MasterFieldTypeList() const;

        /**
         * Return shared data client.
         * @return shared data client pointer or NULL.
         */
        RSharedDataClient* SharedDataClient() const;        

    public: // from CVPbkContactStoreDomain
        const TDesC& UriDomain() const;
        void LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri);
        MVPbkContactOperation* CreateContactRetrieverL(
                const MVPbkContactLink& aLink,
                MVPbkSingleContactOperationObserver& aObserver);
        MVPbkContactOperation* CreateDeleteContactsOperationL(
                const MVPbkContactLinkArray& aContactLinks, 
                MVPbkBatchOperationObserver& aObserver);
        MVPbkContactOperation* CreateCommitContactsOperationL(
                const TArray<MVPbkStoreContact*>& aContacts,
                MVPbkBatchOperationObserver& aObserver);
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
                const TDesC& aPhoneNumber, 
                TInt aMaxMatchDigits,
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperation* CreateFindOperationL(
                const TDesC& aSearchString, 
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack );
        
        TAny* ContactOperationFactoryExtension( TUid aExtensionUid );
        
        MVPbkContactOperation* CreateFindOperationL(
		        const MDesC16Array& aSearchStrings, 
		        const MVPbkFieldTypeList& aFieldTypes, 
		        MVPbkContactFindFromStoresObserver& aObserver, 
		        const TCallBack& aWordParserCallBack,
		        const CDesC16ArrayFlat& aStoreEntriesArray );

        MVPbkContactOperation* CreateCompressStoresOperationL(
        		MVPbkBatchOperationObserver& aObserver);

        TInt Count() const;
        MVPbkContactStore& At(TInt aIndex) const;
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const;
        void OpenAllL(MVPbkContactStoreListObserver& aObserver);
        void CloseAll(MVPbkContactStoreListObserver& aObserver);

    private:  // Implementation
        CContactStoreDomain(const MVPbkFieldTypeList& aMasterFieldTypeList);
        void ConstructL(const CVPbkContactStoreUriArray& aURIList, 
                const TDesC& aURIDomain,
                CFieldTypeMap* aFieldTypeMap);

    private:  // Data
        ///Own: this domains URI domain name
        HBufC* iDomainDescriptor;
        ///Own: Contact model to VPbk field type mapping
        CFieldTypeMap* iFieldTypeMap;
        ///Own: list of stores in this domain
        CVPbkContactStoreList* iContactStoreList;
        ///Own: a factory for creating multioperations
        CVPbkContactOperationFactory* iOperationFactory;
        ///Ref: Master field type list
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        ///Own: File server session shared among stores
        RFs iFs;
        ///Own: Text storage for extension names
        VPbkEngUtils::CTextStore* iTextStore;        
        ///Own: Shared data client
        RSharedDataClient* iSharedDataClient;

    };


// INLINE FUNCTIONS

inline const CFieldTypeMap& CContactStoreDomain::FieldTypeMap() const
    {
    return *iFieldTypeMap;
    }

inline RFs& CContactStoreDomain::FsSession() const
    {
    return const_cast<RFs&>(iFs);
    }

inline const MVPbkFieldTypeList& CContactStoreDomain::MasterFieldTypeList() const
    {
    return iMasterFieldTypeList;
    }

inline RSharedDataClient* CContactStoreDomain::SharedDataClient() const
    {
    return iSharedDataClient;
    }

} // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_CCONTACTSTOREDOMAIN_H

//End of file


