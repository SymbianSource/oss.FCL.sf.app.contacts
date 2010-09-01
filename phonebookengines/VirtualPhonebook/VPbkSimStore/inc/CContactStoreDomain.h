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
* Description:  Implements the sim store domain
*
*/



#ifndef VPBKSIMSTORE_CCONTACTSTOREDOMAIN_H
#define VPBKSIMSTORE_CCONTACTSTOREDOMAIN_H

//  INCLUDES
#include <CVPbkContactStoreDomain.h>
#include <f32file.h>

class CVPbkContactStoreList;
class CVPbkContactOperationFactory;

namespace VPbkEngUtils {
class CTextStore;
}
    
namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CFieldTypeMappings;

// CLASS DECLARATION

/**
*  Implements the sim store domain
*
*/
NONSHARABLE_CLASS( CContactStoreDomain ): 
        public CVPbkContactStoreDomain
    {
    public:  // Constructors and destructor
        
        /**
        * Creates a new instance of this class.
        *
        * @param aURIList A list of sim store name URIs.
        * @param aURIDomain the URI domain name
        * @param aMasterFieldTypeList the master field type list.
        * @param aSecurityInfo Security information of client
        * @return A new instance of this class.
        */
        static CContactStoreDomain* NewL
            ( const CVPbkContactStoreUriArray& aURIList, 
            const TDesC& aURIDomain,
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            const TSecurityInfo& aSecurityInfo );

        /**
        * Two-phased constructor. Only for ECom use.
        * @param aParam   ecom paramater.
        * @return a new instance of this class
        */
        static CContactStoreDomain* NewL(TAny* aParam);
        
        /**
        * Destructor.
        */
        virtual ~CContactStoreDomain();

    public: // New functions
        
        /**
        * Returns the field type mappings vpbk <-> sim
        * @return the field type mappings vpbk <-> sim
        */
        inline CFieldTypeMappings& FieldTypeMappings();

        /**
        * @return the master field type list
        */
        inline const MVPbkFieldTypeList& MasterFieldTypeList() const;
        
        /**
         * Reference to security information of client
         * @return Security information of client.
         */
        inline const TSecurityInfo& SecurityInformation() const;
        
        /**
         * Returns the file server session.
         */
        RFs& FsSession() const;

    public: // Functions from base classes

        /**
         * From CVPbkContactStoreDomain.
         */
        const TDesC& UriDomain() const;

        /**
         * From CVPbkContactStoreDomain.
         */
        void LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri);

        /**
         * From MVPbkContactStoreList
         */
        TInt Count() const;

        /**
         * From MVPbkContactStoreList
         */
        MVPbkContactStore& At( TInt aIndex ) const;

        /**
         * From MVPbkContactStoreList
         */
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const;

        /**
         * From MVPbkContactStoreList
         */
        void OpenAllL( MVPbkContactStoreListObserver& aObserver );

        /**
         * From MVPbkContactStoreList
         */
        void CloseAll( MVPbkContactStoreListObserver& aObserver );

        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateContactRetrieverL( 
            const MVPbkContactLink& aLink, 
            MVPbkSingleContactOperationObserver& aObserver );

        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateDeleteContactsOperationL(
            const MVPbkContactLinkArray& aContactLinks, 
            MVPbkBatchOperationObserver& aObserver );

        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateCommitContactsOperationL(
            const TArray<MVPbkStoreContact*>& aContacts,
            MVPbkBatchOperationObserver& aObserver );

        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
            const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateFindOperationL(
            const TDesC& aSearchString,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindObserver& aObserver);
        
        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack );
                
        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateCompressStoresOperationL(
        	MVPbkBatchOperationObserver& aObserver);
        /**
         * From MVPbkContactOperationFactory2
         */
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack,
                const CDesC16ArrayFlat& aStoreEntriesArray );
                
         TAny* ContactOperationFactoryExtension( TUid aExtensionUid );
 
    private:

        /**
        * C++ constructor.
        */
        CContactStoreDomain( const MVPbkFieldTypeList& aMasterFieldTypeList, 
                             const TSecurityInfo& aSecurityInfo );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CVPbkContactStoreUriArray& aURIList, 
                const TDesC& aURIDomain );

    private:    // Data
        ///Own: File server session
        RFs iFs;
        ///Own: this domains URI domain name
        HBufC* iDomainDescriptor;
        ///Own: VPbk <-> SIM type mappings
        CFieldTypeMappings* iFieldTypeMappings;
        ///Own: list of stores in this domain
        CVPbkContactStoreList* iContactStoreList;
        ///Own: a factory for creating multioperations
        CVPbkContactOperationFactory* iOperationFactory;
        ///Ref: Master field type list
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        ///Own: a text store for sharing extension names of X-field types
        VPbkEngUtils::CTextStore* iTextStore;
        ///Ref: Security information of client
        const TSecurityInfo& iSecurityInfo;
    };

// INLINE FUNCTIONS
inline CFieldTypeMappings& CContactStoreDomain::FieldTypeMappings()
    {
    return *iFieldTypeMappings;
    }

inline const MVPbkFieldTypeList& 
    CContactStoreDomain::MasterFieldTypeList() const
    {
    return iMasterFieldTypeList;
    }
    
inline const TSecurityInfo& CContactStoreDomain::SecurityInformation() const
    {
    return iSecurityInfo;
    }
 
inline RFs& CContactStoreDomain::FsSession() const
    {
    return const_cast<RFs&>(iFs);
    }

}
// namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CCONTACTSTOREDOMAIN_H
            
// End of File
