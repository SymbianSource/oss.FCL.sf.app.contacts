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
* Description:  An implementation of multioperation factory
*                that uses contact store list for creating suboperations
*                from stores
*
*/


#ifndef CVPBKCONTACTOPERATIONFACTORY_H
#define CVPBKCONTACTOPERATIONFACTORY_H

#include <e32base.h>
#include <mvpbkcontactoperationfactory.h>
#include <mvpbkcontactoperationfactory2.h>

class MVPbkContactStoreList;

/**
 * An multioperation factory for store lists
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactOperationFactory : 
        public CBase,
        public MVPbkContactOperationFactory,
        public MVPbkContactOperationFactory2 
    {
    public:
        /**
         * @param aStoreList a store list that is used for creating suboperations.
         */
        IMPORT_C static CVPbkContactOperationFactory* NewL( 
            MVPbkContactStoreList& aStoreList );
        virtual ~CVPbkContactOperationFactory();

    public: // from MVPbkContactOperationFactory
        MVPbkContactOperation* CreateContactRetrieverL(
            const MVPbkContactLink& aLink,
            MVPbkSingleContactOperationObserver& aObserver );
        MVPbkContactOperation* CreateDeleteContactsOperationL(
            const MVPbkContactLinkArray& aContactLinks, 
            MVPbkBatchOperationObserver& aObserver );
        MVPbkContactOperation* CreateCommitContactsOperationL(
            const TArray<MVPbkStoreContact*>& aContacts,
            MVPbkBatchOperationObserver& aObserver );
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
            const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver );
        MVPbkContactOperation* CreateFindOperationL(
            const TDesC& aSearchString, 
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindObserver& aObserver );
        MVPbkContactOperation* CreateFindOperationL(
            const MDesC16Array& aSearchStrings, 
            const MVPbkFieldTypeList& aFieldTypes, 
            MVPbkContactFindFromStoresObserver& aObserver, 
            const TCallBack& aWordParserCallBack );
        MVPbkContactOperation* CreateCompressStoresOperationL(
		    MVPbkBatchOperationObserver& aObserver );
		TAny* ContactOperationFactoryExtension( TUid aExtensionUid ); 
		    
	public: // from MVPbkContactOperationFactory2
		MVPbkContactOperation* CreateFindOperationL(
            const MDesC16Array& aSearchStrings, 
            const MVPbkFieldTypeList& aFieldTypes, 
            MVPbkContactFindFromStoresObserver& aObserver, 
            const TCallBack& aWordParserCallBack,
            const CDesC16ArrayFlat& aStoreEntriesArray );
    private:

        CVPbkContactOperationFactory( MVPbkContactStoreList& aStoreList );
        void ConstructL();

    private: // data
        /// Ref: A list of stores that used in operation creation
        MVPbkContactStoreList& iStores;
    };

#endif // CVPBKCONTACTOPERATIONFACTORY_H

// End of File
