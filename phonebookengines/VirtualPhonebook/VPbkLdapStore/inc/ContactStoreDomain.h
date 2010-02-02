/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactStoreDomain - Store domain.
*
*/


#ifndef __CONTACTSTOREDOMAIN_H__
#define __CONTACTSTOREDOMAIN_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cvpbkcontactstoredomain.h>

// FORWARD DECLARATIONS
class CLDAPFinderUI;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStoreList;
class CFieldTypeMappings;

/**
*  CContactStoreDomain - store domain.
*  This class implements Virtual Phonebook contact store domain.
*/
class CContactStoreDomain : public CVPbkContactStoreDomain
    {
    public:     // CContactStoreDomain public constructors and destructor

        /**
        @function   NewL
        @discussion Create CContactStoreDomain object
        @param      aParam ECom parameter
        @return     Pointer to instance of CContactStoreDomain
        */
        static CContactStoreDomain* NewL(TAny* aParam);

        /**
        @function   NewLC
        @discussion Create CContactStoreDomain object
        @return     Pointer to instance of CContactStoreDomain
        */
        static CContactStoreDomain* NewLC(
            const CVPbkContactStoreUriArray& aURIList, 
            const TDesC& aURIDomain,
            const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        @function  ~CContactStoreDomain
        @discussion CContactStoreDomain destructor
        */
        ~CContactStoreDomain();

    public:     // CContactStoreDomain public methods

        /**
        @function   FieldTypeMappings
        @discussion Returns the field type mappings.
        @return     Field type mappings between contact model and phonebook.
        */
        CFieldTypeMappings& FieldTypeMappings();

        /**
        @function   MasterFieldTypeList
        @discussion Returns the master field type list.
        @return     Master field type list.
        */
        const MVPbkFieldTypeList& MasterFieldTypeList() const;

        /**
        @function   FinderUI
        @discussion Returns this domain finder ui reference.
        @return     Finder ui
        */
        CLDAPFinderUI& FinderUI();

    public:     // Methods from CVPbkContactStoreDomain

        /**
        @function   UriDomain
        @discussion Return store domain uri
        @return     Domain name
        */
        const TDesC& UriDomain() const;

        /**
        @function   LoadContactStoreL
        @discussion Load store
        */
        void LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri);

    public:     // Methods from MVPbkContactStoreList

        /**
        @function   Count
        @discussion Returns number of stores in list
        @return     Number of stores in list
        */
        TInt Count() const;

        /**
        @function   At
        @discussion Returns contact store at index
        @param      aIndex Index to get store
        @return     Contact store
        */
        MVPbkContactStore& At(TInt aIndex) const;

        /**
        @function   Find
        @discussion Finds contact store by URI
        @param      aUri URI to find store
        @return     Contact store
        */
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const;

        /**
        @function   OpenAllL
        @discussion Opens all stores in this list asynchronously.
        @param      aObserver Observer for the open process.
        @exception  KErrInUse Store list is already opening.
        */
        void OpenAllL(MVPbkContactStoreListObserver& aObserver);

        /**
        @function   CloseAll
        @discussion Closes all stores in the list.
        @param      aObserver Observer for the close process.
        @exception  KErrInUse Store list is already closing.
        */
        void CloseAll(MVPbkContactStoreListObserver& aObserver);

    public:     // Methods from MVPbkContactOperationFactory

        /**
        @function   CloseAll
        @discussion Creates a command for asynchronous contact link retrieval.
        @param      aLink Link to retrieve the contact from.
        @param      aObserver Observer for the retrieval operation.
        @return     Created retrieval operation.
        */
        MVPbkContactOperation* CreateContactRetrieverL( 
            const MVPbkContactLink& aLink, 
            MVPbkSingleContactOperationObserver& aObserver);

        /**
        @function   CreateDeleteContactsOperationL
        @discussion Creates an operation for deleting multiple contacts.
        @param      aContactLinks Array of contact links to delete.
        @param      aObserver Observer for the deletion operation.
        @return     Created delete operation.
        */
        MVPbkContactOperation* CreateDeleteContactsOperationL(
            const MVPbkContactLinkArray& aContactLinks, 
            MVPbkBatchOperationObserver& aObserver);

        /**
        @function   CreateCommitContactsOperationL
        @discussion Creates an operation for committing multiple contacts.
        @param      aContacts Array of contact links to commit.
        @param      aObserver Observer for the commit operation.
        @return     Created commit operation.
        */
         MVPbkContactOperation* CreateCommitContactsOperationL(
            const TArray<MVPbkStoreContact*>& aContacts,
            MVPbkBatchOperationObserver& aObserver);

        /**
        @function   CreateMatchPhoneNumberOperationL
        @discussion Creates an operation for asynchronous phone number matching.
        @param      aPhoneNumber    Phone number to search for.
        @param      aMaxMatchDigits Number of digits to match from right.
        @param      aObserver       Observer for the find operation.
        @return     Created find operation.
        */        
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
            const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver );

        /**
        @function   CreateFindOperationL
        @discussion Creates an operation for asynchronous contact finding.
        @param      aSearchString String to search for.
        @param      aFieldTypes   List of field types that the search will include.
        @param      aObserver     Observer for the find operation.
        @return     Created find operation.
        */        
        MVPbkContactOperation* CreateFindOperationL(
            const TDesC& aSearchString,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindObserver& aObserver);

        /**
        @function   CreateFindOperationL
        @discussion Creates an operation for asynchronous contact finding.
        @param      aSearchString String to search for.
        @param      aFieldTypes   List of field types that the search will include.
        @param      aObserver     Observer for the find operation.
        @param      aWordParserCallBack a client implementation of word parser
                                        function that separates the field data
                                        into words. Parameter to function is
                                        TVPbkWordParserParam.
        @return     Created find operation.
        */        
        MVPbkContactOperation* CreateFindOperationL(
            const MDesC16Array& aSearchStrings, 
            const MVPbkFieldTypeList& aFieldTypes, 
            MVPbkContactFindFromStoresObserver& aObserver, 
            const TCallBack& aWordParserCallBack );
            
        /**
        @function   CreateCompressStoresOperationL
        @discussion Creates an operation for compressing all the stores.
        @param      aObserver Observer for the compress operation.
        @return     Created operation.
        */        
        MVPbkContactOperation* CreateCompressStoresOperationL(
                MVPbkBatchOperationObserver& aObserver);

    private:    // CContactStoreDomain private constructors

        /**
        @function   CContactStoreDomain
        @discussion CContactStoreDomain default contructor
        */
        CContactStoreDomain(const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactStoreDomain 
        */
        void ConstructL(const CVPbkContactStoreUriArray& aURIList,
                        const TDesC& aURIDomain);

    private:    // CContactStoreDomain private methods

        /**
        @function   InitializeFieldMappingL
        @discussion Initializes field mappings 
        */
        void InitializeFieldMappingL();

        /**
        @function   LoadFinderUIL
        @discussion Initializes finder
        */
        void LoadFinderUIL();

    private:    // CContactStoreDomain private member variables

        /// Domain uri
        HBufC* iDomainDescriptor;

        /// List of stores in this domain
        CContactStoreList* iContactStoreList;

        /// Contact model field map to Phonebook
        CFieldTypeMappings* iFieldTypeMappings;

        /// Master field type list reference
        const MVPbkFieldTypeList& iMasterFieldTypeList;

        /// Finder UI ECom object
        CLDAPFinderUI* iFinder;

    };

} // End of namespace LDAPStore
#endif // __CONTACTSTOREDOMAIN_H__
