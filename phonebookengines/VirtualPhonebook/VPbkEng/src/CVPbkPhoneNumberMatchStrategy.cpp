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
* Description:  A high level class for matching phone numbers from stores.
*
*/


// INCLUDES
#include <CVPbkPhoneNumberMatchStrategy.h>

#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <RLocalizedResourceFile.h>
#include <VPbkDataCaging.hrh>
#include <VPbkEng.rsg>
#include <VPbkFieldTypeSelectors.rsg>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldFilter.h>
#include <barsread.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkContactStoreUriArray.h>

#include "CVPbkPhoneNumberSequentialMatchStrategy.h"
#include "CVPbkPhoneNumberParallelMatchStrategy.h"

// CONSTANTS
const TInt KMagicNumber = -1;

NONSHARABLE_CLASS(CVPbkPhoneNumberMatchStrategyImpl) :
        public CActive,
        public MVPbkContactFindObserver,
        public MVPbkSingleContactOperationObserver
    {
    public: // Construction
        static CVPbkPhoneNumberMatchStrategyImpl* NewL(
                CVPbkPhoneNumberMatchStrategy& aParent,
                const CVPbkPhoneNumberMatchStrategy::TConfig& aConfig,
                CVPbkContactManager& aContactManager,
                MVPbkContactFindObserver& aObserver);
        ~CVPbkPhoneNumberMatchStrategyImpl();

    public: // Interface
        void MatchL(const TDesC& aPhoneNumber);
        TInt MaxMatchDigits() const;
        TArray<MVPbkContactStore*> StoresToMatch() const;

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // From MVPbkContactFindObserver
        void FindCompleteL(MVPbkContactLinkArray* aResults);
        void FindFailed(TInt aError);

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError);

    private: // Implementation
        CVPbkPhoneNumberMatchStrategyImpl(CVPbkPhoneNumberMatchStrategy& aParent);
        void ConstructL(
                const CVPbkPhoneNumberMatchStrategy::TConfig& aConfig,
                CVPbkContactManager& aContactManager,
                MVPbkContactFindObserver& aObserver);

        /**
         * Searches for a store that has given URI from the list of
         * stores that the contact manager has.
         * @param aUriPtr URI of the store to search for.
         * @return The store with aUriPtr, or NULL if store was not found.
         */
        MVPbkContactStore* FindStoreL(
                const TVPbkContactStoreUriPtr& aUriPtr);

        /**
         * Issues new request to be handled in RunL.
         */
        void IssueRequest();

        MVPbkContactLink* IsValidResultLC(MVPbkStoreContact* aContact);

        /**
         * Creates name tokens array
         * @param aContact which is checking, RPointerArray reference
         */
        void CreateNameTokensArrayL( MVPbkStoreContact* aContact, RPointerArray <HBufC>& aNameTokensArray );
        
        /**
         * Check if contact already exists in results array
         * @param aContact which is checking
         * @return True if contact exist or if array was empty.
         */
        TBool CheckContactDuplicationL( MVPbkStoreContact* aContact );
        
        /**
         * Gets contact's field value
         * @param aContact to get field's value from
         * @param aFieldType: EVPbkVersitNameN for Last Name 
         * EVPbkVersitNameFN for First Name
         * @return Pointer descriptor with field's value.
         */
        TPtrC NameFieldValueL( MVPbkStoreContact* aContact, TVPbkFieldTypeName aFieldType );
        
    private: // Data
        CVPbkPhoneNumberMatchStrategy& iParent;
        /// Ref: The contact manager instance to be used for searching.
        CVPbkContactManager* iContactManager;
        /// Ref: Observer for the searching process.
        MVPbkContactFindObserver* iObserver;
        /// Own: The find operation that is currently ongoing.
        MVPbkContactOperationBase* iOperation;
        /// Maximum number of matched digits.
        TInt iMaxMatchDigits;
        /// Flags to configure matching process, @see TVPbkPhoneNumberMatchFlags
        TUint32 iMatchFlags;

        /// Own: Array of stores that are used in matching.
        RPointerArray<MVPbkContactStore> iStoresToMatch;
        /// Own: Phone number that is being matched.
        HBufC* iPhoneNumber;
        /// Own: Intermediate results of the matching process.
        CVPbkContactLinkArray* iWorkingResults;
        /// Own: Final results of the matching process.
        CVPbkContactLinkArray* iResults;
        /// iWorkingResults index of current contact retrieval
        TInt iCurrentContact;

        /// Own: Currently retrieved contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Field type selector
        CVPbkFieldTypeSelector* iFieldTypeSelector;
        /// Own: A filtered and sorted collection of Virtual Phonebook contact fields.
        CVPbkFieldFilter* iFieldFilter;

        /// Active object states
        enum TState { EMatch, ERemoveDuplicates, ERefineSearch, EComplete };
        /// Active object current state
        TState iState;
        
        /// Own: First Name field type selector
        CVPbkFieldTypeSelector* iFirstNameSelector;
        /// Own: Last Name field type selector
        CVPbkFieldTypeSelector* iLastNameSelector;
        /// Own: Array of tokens gotten from first and last name fields of first matched contact
        RPointerArray <HBufC> iNameTokensArray;
        /// Own: Array of tokens gotten from first and last name fields of contact
        RPointerArray <HBufC> iTempNameTokensArray;
        /// Indicates if all contact in iResult are the same
        TBool iDoubledContacts;
    };

CVPbkPhoneNumberMatchStrategyImpl::CVPbkPhoneNumberMatchStrategyImpl(
        CVPbkPhoneNumberMatchStrategy& aParent) :
    CActive(CActive::EPriorityIdle),
    iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }

inline void CVPbkPhoneNumberMatchStrategyImpl::ConstructL(
        const CVPbkPhoneNumberMatchStrategy::TConfig& aConfig,
        CVPbkContactManager& aContactManager,
        MVPbkContactFindObserver& aObserver)
    {
    iContactManager = &aContactManager;
    iObserver = &aObserver;

    iMaxMatchDigits = aConfig.iMaxMatchDigits;
    iMatchFlags = aConfig.iMatchFlags;

    const TInt uriCount = aConfig.iUriPriorities.Count();
    for (TInt i = 0; i < uriCount; ++i)
        {
        MVPbkContactStore* store = FindStoreL(aConfig.iUriPriorities[i]);
        if (store)
            {
            iStoresToMatch.AppendL(store);
            }
        }

    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC(iContactManager->FsSession(),
                   KVPbkRomFileDrive,
                   KDC_RESOURCE_FILES_DIR,
                   KVPbkFieldTypeSelectorsResFileName);
    HBufC8* selectorBuf = resFile.AllocReadLC(R_VPBK_PHONE_NUMBER_SELECTOR);
    TResourceReader resReader;
    resReader.SetBuffer(selectorBuf);

    iFieldTypeSelector = CVPbkFieldTypeSelector::NewL(resReader, iContactManager->FieldTypes());

    CleanupStack::PopAndDestroy( selectorBuf );
    
    if ( iMatchFlags & CVPbkPhoneNumberMatchStrategy::EVPbkDuplicatedContactsMatchFlag )
    	{ 
	    HBufC8* firstNameSelectorBuf = resFile.AllocReadLC( R_VPBK_FIRST_NAME_SELECTOR );
	    resReader.SetBuffer( firstNameSelectorBuf );
	    iFirstNameSelector = CVPbkFieldTypeSelector::NewL( resReader, iContactManager->FieldTypes() );
	    CleanupStack::PopAndDestroy( firstNameSelectorBuf );
	
	    HBufC8* lastNameSelectorBuf = resFile.AllocReadLC( R_VPBK_LAST_NAME_SELECTOR );
	    resReader.SetBuffer( lastNameSelectorBuf );
	    iLastNameSelector = CVPbkFieldTypeSelector::NewL( resReader, iContactManager->FieldTypes() );
	    CleanupStack::PopAndDestroy( lastNameSelectorBuf );
    	}
    
    CleanupStack::PopAndDestroy( &resFile );
    }

CVPbkPhoneNumberMatchStrategyImpl* CVPbkPhoneNumberMatchStrategyImpl::NewL(
        CVPbkPhoneNumberMatchStrategy& aParent,
        const CVPbkPhoneNumberMatchStrategy::TConfig& aConfig,
        CVPbkContactManager& aContactManager,
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkPhoneNumberMatchStrategyImpl* self =
            new(ELeave) CVPbkPhoneNumberMatchStrategyImpl(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aConfig, aContactManager, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

CVPbkPhoneNumberMatchStrategyImpl::~CVPbkPhoneNumberMatchStrategyImpl()
    {
    Cancel();
    
    delete iFieldFilter;
    delete iFieldTypeSelector;
    delete iWorkingResults;
    delete iResults;
    delete iStoreContact;
    delete iPhoneNumber;
    delete iOperation;
    delete iFirstNameSelector;
    delete iLastNameSelector;
    iNameTokensArray.ResetAndDestroy();
    iTempNameTokensArray.ResetAndDestroy();
    iStoresToMatch.Close();
    }

void CVPbkPhoneNumberMatchStrategyImpl::MatchL(const TDesC& aPhoneNumber)
    {
    HBufC* phoneNumber = aPhoneNumber.AllocL();
    delete iPhoneNumber;
    iPhoneNumber = phoneNumber;
    
    if ( iWorkingResults )
        {
        iWorkingResults->ResetAndDestroy();
        }                
    if ( iResults )
        {
        iResults->ResetAndDestroy();
        }                
    iCurrentContact = KMagicNumber;
    
    if ( iMatchFlags & CVPbkPhoneNumberMatchStrategy::EVPbkDuplicatedContactsMatchFlag )
    	{
    	iDoubledContacts = ETrue;
    	}
    else
    	{
    	iDoubledContacts = EFalse;
    	}
    iState = EMatch;
    IssueRequest();
    }


void CVPbkPhoneNumberMatchStrategyImpl::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CVPbkPhoneNumberMatchStrategyImpl::RunL()
    {
    switch (iState)
        {
        case EMatch:
            {
            MVPbkContactOperation* operation = iParent.CreateFindOperationLC(*iPhoneNumber);
            if (operation)
                {
                operation->StartL();
                CleanupStack::Pop(); // operation
                delete iOperation;
                iOperation = operation;
                }
            else
                {
                // all needed operations are done if any
                iState = ERemoveDuplicates;
                IssueRequest();
                }
            break;
            }
        case ERemoveDuplicates:
            {
            const TInt count = iWorkingResults ? iWorkingResults->Count() : 0;
            CVPbkContactLinkArray* results = CVPbkContactLinkArray::NewLC();
            for (TInt i = 0; i < count; ++i)
                {
                MVPbkContactLink* link = iWorkingResults->At(i).CloneLC();
                if (results->Find(*link) == KErrNotFound)
                    {
                    results->AppendL(link);
                    CleanupStack::Pop(); // link
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(); // link
                    }
                }
            CleanupStack::Pop(results);
            delete iWorkingResults;
            iWorkingResults = results;
            iState = ERefineSearch;
            iCurrentContact = 0;
            IssueRequest();
            break;
            }
        case ERefineSearch:
            {
            if (!iResults)
                {
                iResults = CVPbkContactLinkArray::NewL();
                }
            if ( iDoubledContacts )
                {
                iDoubledContacts = CheckContactDuplicationL( iStoreContact );
                }
            
            MVPbkContactLink* result = IsValidResultLC( iStoreContact );
            if ( result )
                {
                iResults->AppendL( result );
                CleanupStack::Pop(); // MVPbkContactLink
                }
            delete iStoreContact;
            iStoreContact = NULL;
                
            const TInt count = iWorkingResults ? iWorkingResults->Count() : 0;
            if (iCurrentContact < count &&                 
                !( iResults->Count() > 0 && 
                        (iMatchFlags & CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag)))
                {
                const MVPbkContactLink& link = iWorkingResults->At(iCurrentContact);
                delete iOperation;
                iOperation = NULL;
                iOperation = iContactManager->RetrieveContactL(link, *this);
                ++iCurrentContact;
                }
            else
                {
                iState = EComplete;
                IssueRequest();
                }
            break;
            }
        case EComplete:
            {
            iNameTokensArray.ResetAndDestroy();
            iTempNameTokensArray.ResetAndDestroy();
            if ( iDoubledContacts && iResults->Count() )
                {
                CVPbkContactLinkArray* results = CVPbkContactLinkArray::NewLC();
                MVPbkContactLink* link = iResults->At(0).CloneLC(); // first element
                results->AppendL( link );
                CleanupStack::Pop( 2, results ); // results, link
                delete iResults;
                iResults = results;
                }
            CVPbkContactLinkArray* results = iResults;
            iResults = NULL;            
            iObserver->FindCompleteL( results );
            break;
            }
        default:
            {
            // This case should not be possible
            User::Leave( KErrArgument );
            break;
            }
        }
    }

void CVPbkPhoneNumberMatchStrategyImpl::DoCancel()
    {
    }

TInt CVPbkPhoneNumberMatchStrategyImpl::RunError(TInt aError)
    {
    iObserver->FindFailed(aError);
    return KErrNone;
    }

inline MVPbkContactStore* CVPbkPhoneNumberMatchStrategyImpl::FindStoreL(
        const TVPbkContactStoreUriPtr& aUriPtr)
    {
    const TInt storeCount = iContactManager->ContactStoresL().Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        MVPbkContactStore& store = iContactManager->ContactStoresL().At(i);
        if (store.StoreProperties().Uri().Compare(
                    aUriPtr,
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            return &store;
            }
        }
    return NULL;
    }

void CVPbkPhoneNumberMatchStrategyImpl::FindCompleteL(MVPbkContactLinkArray* aResults)
    {
    // This function called by operation which is created in 
    // iParent.CreateFindOperationLC
    if (aResults)
        {
        CleanupDeletePushL( aResults ); // Take ownership
        
        if (!iWorkingResults)
            {
            iWorkingResults = CVPbkContactLinkArray::NewL();
            }
        const TInt count = aResults->Count();
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactLink* link = aResults->At(i).CloneLC();
            iWorkingResults->AppendL(link);
            CleanupStack::Pop(); // link
            }

        CleanupStack::PopAndDestroy(); // aResults
        }
    IssueRequest();
    }

void CVPbkPhoneNumberMatchStrategyImpl::FindFailed(TInt aError)
    {
    iObserver->FindFailed(aError);
    }

void CVPbkPhoneNumberMatchStrategyImpl::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact)
    {
    if (iOperation == &aOperation)
        {
        delete iOperation;
        iOperation = NULL;

        iStoreContact = aContact;
        IssueRequest();
        }
    }

void CVPbkPhoneNumberMatchStrategyImpl::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError)
    {
    if (iOperation == &aOperation)
        {
        delete iOperation;
        iOperation = NULL;

        iObserver->FindFailed(aError);
        }
    }

TInt CVPbkPhoneNumberMatchStrategyImpl::MaxMatchDigits() const
    {
    return iMaxMatchDigits;
    }

TArray<MVPbkContactStore*> CVPbkPhoneNumberMatchStrategyImpl::StoresToMatch() const
    {
    return iStoresToMatch.Array();
    }

MVPbkContactLink* CVPbkPhoneNumberMatchStrategyImpl::IsValidResultLC(
        MVPbkStoreContact* aContact)
    {
    MVPbkContactLink* result = NULL;

    if (!(iMatchFlags & CVPbkPhoneNumberMatchStrategy::EVPbkExactMatchFlag))
        {
        // If exact match is not needed we will accept the contact as valid
        // if it is not NULL
        if ( aContact )
            {
            result = aContact->CreateLinkLC();
            }        
        }
    else if (aContact)
        {
        TPtrC matchedNumber = iPhoneNumber->Des().
                Right(Min(iPhoneNumber->Length(), iMaxMatchDigits));

        CVPbkFieldFilter::TConfig config(aContact->Fields(), iFieldTypeSelector);
        if (!iFieldFilter)
            {
            iFieldFilter = CVPbkFieldFilter::NewL(config);
            }
        else
            {
            iFieldFilter->ResetL(config);
            }
        const TInt count = iFieldFilter->FieldCount();
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactFieldData& data = iFieldFilter->FieldAt(i).FieldData();
            if (data.DataType() == EVPbkFieldStorageTypeText)
                {
                const TDesC& dataText = MVPbkContactFieldTextData::Cast(data).Text();
                TPtrC dataTextPtr = dataText.Right(Min(dataText.Length(), iMaxMatchDigits));
                if (dataTextPtr == matchedNumber)
                    {
                    result = iFieldFilter->FieldAt(i).CreateLinkLC();
                    break;
                    }
                }
            }
        }
    return result;
    }

TBool CVPbkPhoneNumberMatchStrategyImpl::CheckContactDuplicationL(
        MVPbkStoreContact* aContact )
    {

    if ( !aContact )
        {
        return ETrue;
        }
    
    if ( aContact && iCurrentContact == 1 )
        {
        CreateNameTokensArrayL( aContact, iNameTokensArray );
        return ETrue;
        }
    else 
        {
        iTempNameTokensArray.ResetAndDestroy();
        CreateNameTokensArrayL( aContact, iTempNameTokensArray );
        TInt count = iNameTokensArray.Count();
        if ( iNameTokensArray.Count() != iTempNameTokensArray.Count() )
        	{
        	return EFalse;
        	}
        else
        	{
        	TInt result = 0;
        	for ( TInt i = 0; i < count; i++ )
        		{
        		HBufC* token = iNameTokensArray[i];
        		TInt tempCount = iTempNameTokensArray.Count();
            	for ( TInt j = 0; j < tempCount; j++ )
            		{
            		result = token->Compare( *( iTempNameTokensArray[j] ) );
                	if ( !result )
                		{
                		HBufC* removedToken = iTempNameTokensArray[j];
                		iTempNameTokensArray.Remove( j );
                		delete removedToken;
                		break;
                		}
            		}
            	if ( result )
            		{
            		return EFalse;
            		}
        		}
        	}
        return ETrue;
        }
    }

void CVPbkPhoneNumberMatchStrategyImpl::CreateNameTokensArrayL( MVPbkStoreContact* aContact,
										RPointerArray <HBufC>& aNameTokensArray )
	{
    TPtrC firstName = NameFieldValueL( aContact, EVPbkVersitNameFN );
    TLex lexer;
    lexer.Assign( firstName );
    
    TPtrC ptr;
    TInt len = 0;
    while( ETrue )
        {
        ptr.Set( lexer.NextToken() );
        
        len = ptr.Length();
        if ( len )
            {
            HBufC* token = ptr.AllocLC();
            aNameTokensArray.AppendL( token );
            CleanupStack::Pop( token );
            }
        else
            {
            break;
            }
        } 

    TPtrC lastName = NameFieldValueL( aContact, EVPbkVersitNameN );

    lexer.Assign( lastName );
    
    len = 0;
    while( ETrue )
        {
        ptr.Set( lexer.NextToken() );
        
        len = ptr.Length();
        if ( len )
            {
            HBufC* token = ptr.AllocLC();
            aNameTokensArray.AppendL( token );
            CleanupStack::Pop( token );
            }
        else
            {
            break;
            }
        }
	}

TPtrC CVPbkPhoneNumberMatchStrategyImpl::NameFieldValueL( 
        MVPbkStoreContact* aContact, TVPbkFieldTypeName aFieldType )
    {
    if ( aContact )
        {
        CVPbkFieldFilter::TConfig config( aContact->Fields() );
        if ( aFieldType == EVPbkVersitNameFN )
            {
            config.iFieldSelector = iFirstNameSelector;
            }
        else if ( aFieldType == EVPbkVersitNameN )
            {
            config.iFieldSelector = iLastNameSelector;
            }
        else
            {
            return KNullDesC();
            }
        
        if (!iFieldFilter)
            {
            iFieldFilter = CVPbkFieldFilter::NewL(config);
            }
        else
            {
            iFieldFilter->ResetL(config);
            }
        const TInt count = iFieldFilter->FieldCount();
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactFieldData& data = iFieldFilter->FieldAt(i).FieldData();
            if (data.DataType() == EVPbkFieldStorageTypeText)
                {
                return  MVPbkContactFieldTextData::Cast(data).Text();
                }
            }
        return KNullDesC();
        }
    else
        {
        return KNullDesC();
        }
    }

CVPbkPhoneNumberMatchStrategy::CVPbkPhoneNumberMatchStrategy()
    {
    }

EXPORT_C CVPbkPhoneNumberMatchStrategy* CVPbkPhoneNumberMatchStrategy::NewL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager,
        MVPbkContactFindObserver& aObserver)
    {
    if (aConfig.iMatchMode == EVPbkSequentialMatch)
        {
        return CVPbkPhoneNumberSequentialMatchStrategy::NewL(aConfig, aContactManager, aObserver);
        }
    else
        {
        return CVPbkPhoneNumberParallelMatchStrategy::NewL(aConfig, aContactManager, aObserver);
        }
   }

CVPbkPhoneNumberMatchStrategy::~CVPbkPhoneNumberMatchStrategy()
    {
    delete iImpl;
    }

EXPORT_C void CVPbkPhoneNumberMatchStrategy::MatchL(const TDesC& aPhoneNumber)
    {
    InitMatchingL();

    iImpl->MatchL(aPhoneNumber);
    }

void CVPbkPhoneNumberMatchStrategy::BaseConstructL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager,
        MVPbkContactFindObserver& aObserver)
    {
    iImpl = CVPbkPhoneNumberMatchStrategyImpl::NewL(
            *this, aConfig, aContactManager, aObserver);
    }

MVPbkContactFindObserver& CVPbkPhoneNumberMatchStrategy::FindObserver() const
    {
    return *iImpl;
    }

TInt CVPbkPhoneNumberMatchStrategy::MaxMatchDigits() const
    {
    return iImpl->MaxMatchDigits();
    }

TArray<MVPbkContactStore*> CVPbkPhoneNumberMatchStrategy::StoresToMatch() const
    {
    return iImpl->StoresToMatch();
    }

// End of File
