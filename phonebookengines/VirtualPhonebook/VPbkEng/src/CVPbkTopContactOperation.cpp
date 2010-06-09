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
* Description:  Top Contact opeartion
*
*/


// INCLUDES
#include "CVPbkTopContactOperation.h"

#include <barsc.h>
#include <barsread.h>

#include <MVPbkContactViewBase.h>
#include <VPbkStoreUriLiterals.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <VPbkContactViewFilterBuilder.h>
#include <CVPbkSortOrder.h>
#include <VPbkEng.rsg>
#include <VPbkDataCaging.hrh>
#include <RLocalizedResourceFile.h>
#include <TVPbkFieldVersitProperty.h>

#include "CVPbkContactEasyManager.h"
#include "VPbkDebug.h"

#include <CVPbkSortOrderAcquirer.h>
#include <CVPbkEComImplementationsList.h>
#include <VPbkSortOrderAcquirerUid.h>

namespace
	{
	const TInt KFirstTopOrderIndex = 1;
    const TInt KTcFieldLength = 10;
    
    _LIT( KAllContactsSortOrderDisplayName, "AllContacts" );

    /**
     * Custom cleanup function.
     *
     * @param aObj  Object to clean.
     */
    void CleanupResetAndDestroy( TAny* aObj )
        {
        if ( aObj )
            {
            static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
            }
        }    
    
#ifdef _DEBUG	
	enum TTopErrors
	    {
		ETopErrorWrongLogic1,
		ETopErrorWrongLogic2,
		ETopErrorWrongLogic3,
		EInvalidViewOperation,
		EInvalidLinksOperation,
		EInvalidTopOperation,
		ETopErrorAlreadyHaveAStore,
		ETopErrorAlreadyHaveAView,
		ETopErrorNoView,
		ETopErrorNoFieldData,
		ETopErrorNoLinks,
		ETopViewMissing,
		ETopErrorBadIndex,
		ETopErrorOldArrayExists,
		ETopErrorNoContacts,
        ETopErrorWrongState,
        ETopErrorWrongStateRun,
	    };
	
	_LIT( KPanicStrTopManagement, "VPbk_topManag");
	
	void Panic(TTopErrors aReason)
	    {
	    User::Panic(KPanicStrTopManagement,aReason);
	    }
#endif	
	}

///////////////////////////////////////////////////////////////////////////////

CVPbkTopContactOperation* CVPbkTopContactOperation::NewLC(
        CVPbkContactManager& aContactManager,
        MVPbkOperationErrorObserver& aErrorObserver,
        TTopOperation aOperation)
    {
    CVPbkTopContactOperation* self = new (ELeave) CVPbkTopContactOperation(
            aContactManager,
            aErrorObserver,
            aOperation );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CVPbkTopContactOperation::ConstructL()
    {
    iContactEasyManager = CVPbkContactEasyManager::NewL( iContactManager );
    }

CVPbkTopContactOperation::CVPbkTopContactOperation(
        CVPbkContactManager& aContactManager,
        MVPbkOperationErrorObserver& aErrorObserver,
        TTopOperation aOperation )
    : CActive( EPriorityStandard ),
    iCurrentOperation(aOperation),
    iNextState(EStateOpenStore),
    iContactManager(aContactManager),
    iErrorObserver(&aErrorObserver)
    {
    CActiveScheduler::Add( this );
    CompleteOurself(); //start execution right away
    }

CVPbkTopContactOperation::~CVPbkTopContactOperation()
    {
    Cancel();
    delete iContactOperation;
    delete iContactEasyManager;
    iContacts.ResetAndDestroy();
    delete iInputLinks;
    delete iView;
    if ( iContactStore )
        {
        // Store is not owned by us.
        iContactStore->Close( *this );
        }
    }

MVPbkContactOperationBase* CVPbkTopContactOperation::NewGetViewOperationL(
        CVPbkContactManager& aContactManager,
        MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver,
        TTopOperation aOperation )
    {
    __ASSERT_DEBUG( aOperation == EGetTopContacts || 
                    aOperation == EGetNonTopContacts,
                    Panic(EInvalidViewOperation) );
    
    CVPbkTopContactOperation* self = NewLC(
            aContactManager,
            aErrorObserver,
            aOperation );
    self->iViewObserver = &aObserver;

    CleanupStack::Pop(self);
    return self;
    }

MVPbkContactOperationBase* CVPbkTopContactOperation::NewGetViewLinksOperationL(
        CVPbkContactManager& aContactManager,
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver,
        TTopOperation aOperation )
    {
    __ASSERT_DEBUG( aOperation == EGetTopContactLinks || 
                    aOperation == EGetNonTopContactLinks,
                    Panic(EInvalidLinksOperation) );
    
    CVPbkTopContactOperation* self = NewLC(
            aContactManager,
            aErrorObserver,
            aOperation );
    self->iLinksObserver = &aObserver;

    CleanupStack::Pop(self);
    return self;
    }

MVPbkContactOperationBase* CVPbkTopContactOperation::NewTopOperationL(
        CVPbkContactManager& aContactManager,
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver,
        TTopOperation aOperation,
        MVPbkContactViewBase* aViewRef)
    {
    __ASSERT_DEBUG( aOperation == EAddToTop || aOperation == ERemoveFromTop ||
            aOperation == EReorderTop,
            Panic(EInvalidTopOperation) );
    
    CVPbkTopContactOperation* self = NewLC(
            aContactManager,
            aErrorObserver,
            aOperation );
    self->iObserver = &aObserver;
    self->iInputLinks = CloneArrayL( aContactLinks ); 
    self->iViewRef = aViewRef;
    
    CleanupStack::Pop(self);
    return self;
    }

void CVPbkTopContactOperation::DoCancel()
	{
	delete iContactOperation;
	iContactOperation = NULL;
	iContactEasyManager->Cancel();
	}

void CVPbkTopContactOperation::RunL()
	{
	if ( iNextState == EStateAbortWithError )
	    {
	    NotifyError( iAbortError );
	    }
	else
	    {
    	switch ( iCurrentOperation )
    	    {
    	    case EAddToTop:
    	    case ERemoveFromTop:
    	    case EReorderTop:
        		{
        		Top_RunL();
        		break;
        		}
    	    case EGetNonTopContacts:
    	    case EGetTopContacts:
    	    case EGetTopContactLinks:
    	    case EGetNonTopContactLinks:
                {
        		GetView_RunL();
        		break;
                }
    	    default:
    	        __ASSERT_DEBUG( EFalse,
    	                Panic( ETopErrorWrongStateRun ) );
            }
	    }
	}

TInt CVPbkTopContactOperation::RunError( TInt aError )
	{
	NotifyError( aError );
	return KErrNone;
	}

//---------------------------------------------------------------
// These are from MVPbkContactViewObserver

void CVPbkTopContactOperation::ContactViewReady(
    MVPbkContactViewBase& aView )
    {
    aView.RemoveObserver(*this);  
	CompleteOurself();
    }

void CVPbkTopContactOperation::ContactViewUnavailable(
    MVPbkContactViewBase& aView )
    {
    aView.RemoveObserver(*this);
	AbortWithError( KErrGeneral );
    }

void CVPbkTopContactOperation::ContactAddedToView(
    MVPbkContactViewBase& /*aView*/, 
    TInt /*aIndex*/, 
    const MVPbkContactLink& /*aContactLink*/ )
    {
    }

void CVPbkTopContactOperation::ContactRemovedFromView(
    MVPbkContactViewBase& /*aView*/, 
    TInt /*aIndex*/, 
    const MVPbkContactLink& /*aContactLink*/ )
    {
    }

void CVPbkTopContactOperation::ContactViewError(
        MVPbkContactViewBase& /*aView*/, 
        TInt aError, 
        TBool /*aErrorNotified*/ )
    {
    AbortWithError( aError );
    }


//---------------------------------------------------------------
// These are from MVPbkContactStoreObserver

void CVPbkTopContactOperation::StoreReady(MVPbkContactStore& /*aContactStore*/)
	{
	CompleteOurself();
	}

void CVPbkTopContactOperation::StoreUnavailable(
	MVPbkContactStore& /*aContactStore*/, TInt aReason)
	{
	AbortWithError( aReason );
	}

void CVPbkTopContactOperation::HandleStoreEventL(
    MVPbkContactStore& /*aContactStore*/, 
    TVPbkContactStoreEvent /*aStoreEvent*/)
	{
	}

//---------------------------------------------------------------
// These are from MVPbkBatchOperationObserver

void CVPbkTopContactOperation::StepComplete( 
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aStepSize*/ )
    {
    // Ignore.
    }

TBool CVPbkTopContactOperation::StepFailed(
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aStepSize*/, TInt aError )
    {
    AbortWithError( aError );
    delete iContactOperation;
    iContactOperation = NULL;
    return EFalse; // do not continue
    }

void CVPbkTopContactOperation::OperationComplete( 
    MVPbkContactOperationBase& /*aOperation*/ )
    {
    // Committing of contacts has completed.
    delete iContactOperation;
    iContactOperation = NULL;
    iContacts.ResetAndDestroy();
    //can leave only when returning links 
    TRAP_IGNORE(NotifyResultL());
    }

//---------------------------------------------------------------
// From MVPbkContactEasyManagerObserver
void CVPbkTopContactOperation::VPbkOperationCompleted(
        MVPbkContactOperationBase* /*aOperation*/)
    {

    // Retrieving and locking of contacts is now complete.
    CompleteOurself();
    // Continue processing in RunL()
    }

void CVPbkTopContactOperation::VPbkOperationFailed(
        MVPbkContactOperationBase* /*aOperation*/,
        TInt aError )
    {
    AbortWithError( aError );
    }

// --------------------------------------------------------------------------
// Creates contact link array from a contact view.
// --------------------------------------------------------------------------
MVPbkContactLinkArray* CVPbkTopContactOperation::ViewToLinkArrayL(
        const MVPbkContactViewBase& aView )
    {
    CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(); 
    TInt count = aView.ContactCountL();
    for ( TInt n = 0; n  < count; ++n )
        {
        MVPbkContactLink* link = aView.CreateLinkLC( n );
        links->AppendL( link );
        CleanupStack::Pop(); // link
        }
    CleanupStack::Pop( links );
    return links;
    }

// --------------------------------------------------------------------------
// Loads and opens contact store
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::OpenStoreL( const TVPbkContactStoreUriPtr& aUri )
    {
    // First need to open the store.
    iContactManager.LoadContactStoreL( aUri );

    iContactStore = iContactManager.ContactStoresL().Find( aUri );
    // Asynchronous opening call.
    iContactStore->OpenL(*this);
    }

// --------------------------------------------------------------------------
// Loads and opens default contact store
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::OpenDefaultStoreL()
    {
    TVPbkContactStoreUriPtr uri( KVPbkDefaultCntDbURI );
    OpenStoreL( uri );
	}

// --------------------------------------------------------------------------
// Requests a top contact view. The completion is signalled to this
// object through MVPbkContactViewObserver. The observer callback will pass
// the event on in a certain way that is dependent on the current iState
// value.
// -------------------------------------------------------------------------- 
void CVPbkTopContactOperation::RequestTopContactsViewL()
	{
	__ASSERT_DEBUG( !iView, Panic(ETopErrorAlreadyHaveAView) );
	
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewLC();
    viewDef->SetUriL( KVPbkDefaultCntDbURI );
    
    CVPbkFieldTypeSelector* filter = CVPbkFieldTypeSelector::NewL(
        iContactManager.FieldTypes());
    CleanupStack::PushL(filter);         
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( *filter, 
        TVPbkContactViewFilter(EVPbkContactViewFilterTopContact), iContactManager );

    viewDef->SetFieldTypeFilterL( filter ); 
    CleanupStack::PopAndDestroy( filter );  

    CVPbkSortOrderAcquirer* soa = AllContactsSortOrderL();
    CleanupStack::PushL(soa);

    iView = iContactManager.CreateContactViewLC(
        *this, *viewDef, soa->SortOrder() );

    CleanupStack::Pop(); // iView
    CleanupStack::PopAndDestroy( soa );
    CleanupStack::PopAndDestroy( viewDef );
	}

// --------------------------------------------------------------------------
// Requests a top contact view. The completion is signalled to this
// object through MVPbkContactViewObserver. The observer callback will pass
// the event on in a certain way that is dependent on the current iState
// value.
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::RequestNonTopContactsViewL()
	{
	__ASSERT_DEBUG( !iView, Panic(ETopErrorAlreadyHaveAView) );
	
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewLC();
    viewDef->SetUriL( KVPbkDefaultCntDbURI );
    
    CVPbkSortOrderAcquirer* soa = AllContactsSortOrderL();
    CleanupStack::PushL(soa);

    // Add the Non-Top Contacts selector           
    // No filter (i.e. with a field x) is used
    // but a contact selector
    viewDef->SetContactSelector(this);   // ownership not transferred     

    iView = iContactManager.CreateContactViewLC(
        *this, *viewDef, soa->SortOrder() );

    CleanupStack::Pop(); // iView
    CleanupStack::PopAndDestroy( soa );
    CleanupStack::PopAndDestroy( viewDef );
	}

// --------------------------------------------------------------------------
// The RunL handler for top operations.
// --------------------------------------------------------------------------
inline void CVPbkTopContactOperation::Top_RunL()
	{
	if ( iNextState == EStateOpenStore )
        {
        if ( iInputLinks->Count() > 0 )
            {
            // If the top view was not passed as a reference, we needed to construct
            // own instance for AddToTop operation to get next top index
            if ( iCurrentOperation == EAddToTop && !iViewRef )
                {
                iNextState = EStateCreateView;
                }
            else
                {
                iNextState = EStateRetrieveLock;
                }
            OpenStoreL( iInputLinks->At(0).ContactStore().StoreProperties().Uri() );
            }
        else
            {
            // Empty input array is ok do nothing
            // just signal completion right away
            iNextState = EStateNone;
            NotifyResultL();
            }
        }
	else if ( iNextState == EStateCreateView )
		{
		iNextState = EStateRetrieveLock;
		RequestTopContactsViewL();
		// When view is ready, we come back to this function.
		}
	else if ( iNextState == EStateRetrieveLock )
		{
        iNextState = EStateModifyCommit;
        iContactEasyManager->RetrieveAndLockContactsL(
            *iInputLinks, iContacts, *this, *this );
        // The contacts are placed into iContacts.
        // Completion is signalled to our callback, which
        // will complete ourself so we come back here to the next state.
		}
    else if ( iNextState == EStateModifyCommit )
        {
        DoTopOperationL();
        
        iNextState = EStateNone;
        iContactOperation = iContactManager.CommitContactsL(
            iContacts.Array(), *this );
        // Completion is signalled to our callback, where our observer is
        // called for informing that everything is done.
        }
	}

// --------------------------------------------------------------------------
// Perform actual contact modification, add, remove or change toppnes
// depending on the operation
// --------------------------------------------------------------------------
inline void CVPbkTopContactOperation::DoTopOperationL()
    {
    switch( iCurrentOperation )
        {
        case EAddToTop:
            {
            // Get next top index, either from the provided top view or
            // from the self contsructed one. Delete the owned view right away.
            __ASSERT_DEBUG( iView || iViewRef, Panic(ETopViewMissing) );
            TInt nextTopIndex =  iViewRef ? NextTopOrderIndexL( *iViewRef ) : 
                                            NextTopOrderIndexL( *iView );
            __ASSERT_DEBUG( nextTopIndex >= 0, Panic(ETopErrorBadIndex) );
            delete iView;
            iView = NULL;

            SetTopOrderToContactsL( iContacts, nextTopIndex );
            break;
            }
        case ERemoveFromTop:
            {
            RemoveTopnessFromContacts( iContacts );
            break;
            }
        case EReorderTop:
            {
            ReorderContactsL( iContacts );
            break;
            }
        default:
            __ASSERT_DEBUG( EFalse, Panic(ETopErrorWrongState) );
        }   
    }

// --------------------------------------------------------------------------
// The RunL handler for get view/links operation.
// --------------------------------------------------------------------------
inline void CVPbkTopContactOperation::GetView_RunL()
	{
	if ( iNextState == EStateOpenStore )
        {
        iNextState = EStateCreateView;
        OpenDefaultStoreL();
        }
    else if ( iNextState == EStateCreateView )
	    {
	    iNextState = EStateNone;
	    switch( iCurrentOperation )
	        {
	        case EGetTopContacts:
	        case EGetTopContactLinks:
	            {
	            RequestTopContactsViewL();
	            break;
	            }
	        case EGetNonTopContacts:
	        case EGetNonTopContactLinks:
	            {
	            RequestNonTopContactsViewL();
	            break;
	            }
	        default:
	            __ASSERT_DEBUG( EFalse, Panic(ETopErrorWrongState) );
	        } 	    
	    }
	else
	    {
	    __ASSERT_DEBUG( iNextState == EStateNone, Panic(ETopErrorWrongState) );
    	// The view is ready.
    	NotifyResultL();
	    }
	}

// --------------------------------------------------------------------------
// Finds out what is the top ordering index for new top contacts.
// It looks at the collection of top contacts in iView which has been fetched
// earlier.
// @return The next top index, or KFirstTopOrderIndex if no valid data was found.
// --------------------------------------------------------------------------
TInt CVPbkTopContactOperation::NextTopOrderIndexL(
    const MVPbkContactViewBase& aView )
	{
	const TInt count = aView.ContactCountL();
	TInt result = KFirstTopOrderIndex; // The default if no tops exist yet.
	if ( count > 0 )
		{
		// Take the last contact.
		const MVPbkViewContact& contact = aView.ContactAtL( count - 1 );
		const TInt orderValue = TopOrder( contact );
		// Check the validity of the stored value.
		if ( orderValue >=  KFirstTopOrderIndex && orderValue < KMaxTInt )
		    {
		    // Yes the value is OK.
		    // Offset of one from existing last top contact.
		    result = orderValue + 1;
		    }
		}
	return result;
	}

// --------------------------------------------------------------------------
// Completes this active object.
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::CompleteOurself()
	{
	if ( !IsActive() )
		{
		TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrNone );
	    SetActive();
		}
	else
		{
		__ASSERT_DEBUG( EFalse, Panic(ETopErrorWrongLogic3) );
		}
	}

// --------------------------------------------------------------------------
// Clone contact link array, used to copy input link array
// --------------------------------------------------------------------------
CVPbkContactLinkArray* CVPbkTopContactOperation::CloneArrayL(
	const MVPbkContactLinkArray& aArray )
	{
	CVPbkContactLinkArray* newArray = CVPbkContactLinkArray::NewLC();
	const TInt count = aArray.Count();
	for ( TInt n = 0; n < count; ++n )
		{
        MVPbkContactLink* link = aArray.At(n).CloneLC();
        newArray->AppendL( link );
        CleanupStack::Pop(); // link
		}
	CleanupStack::Pop( newArray );
	return newArray;
	}

// --------------------------------------------------------------------------
// Sets the top contact data to the contacts.
// Does not commit the contacts.
// The contacts might already have some top data. If not then
// the data holder is created.
// @param aContacts The contacts to be modified.
// @param aHighestOrderIndex The value from which ordering starts. The order
// value increases in steps of one.
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::SetTopOrderToContactsL(
    RPointerArray<MVPbkStoreContact>& aContacts,
    TInt aHighestOrderIndex )
    {
    TInt topOrderIndex = aHighestOrderIndex; 
    const TInt count = aContacts.Count();
    for ( TInt n = 0; n < count; ++ n )
        {
        MVPbkStoreContact& contact = *aContacts[ n ];
        SetTopOrderL( contact, topOrderIndex );
        ++topOrderIndex;
        }
    }

// --------------------------------------------------------------------------
// Remove "top" field from contacts
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::RemoveTopnessFromContacts(
    RPointerArray<MVPbkStoreContact>& aContacts )
    {
    const TInt count = aContacts.Count();
    for ( TInt n = 0; n < count; ++ n )
        {
        MVPbkStoreContact& contact = *aContacts[ n ];
        RemoveTopnessFromContact( contact );
        }
    }

// --------------------------------------------------------------------------
// Find and remove the "top" field from a contact
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::RemoveTopnessFromContact(
    MVPbkStoreContact& aContact )
    {
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    const TInt count = fields.FieldCount();
    for ( TInt n = 0; n < count; ++n )
        {
        TVPbkFieldVersitProperty versitProp;
        versitProp.SetName(EVPbkVersitNameTopContact);
        const MVPbkFieldType* fieldType = fields.FieldAt(n).BestMatchingFieldType();
        if ( fieldType && fieldType->Matches(versitProp, 0) )
            {
            aContact.RemoveField( n );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// Change contacts "top" field according to their relative order
// in the input array. Reuses top values the contacts
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::ReorderContactsL(
    RPointerArray<MVPbkStoreContact>& aContacts )
    {
    const TInt count = aContacts.Count();
    RArray<TInt> topOrders;
    RPointerArray<MVPbkStoreContact> contactsToBeReordered; // does not own
    CleanupClosePushL(topOrders);
    CleanupClosePushL(contactsToBeReordered);
    
    for ( TInt n = 0; n < count; ++n )
        {
        const MVPbkStoreContact& contact = *aContacts[ n ];
        const TInt topOrder = TopOrder( contact );
        if ( topOrder >= 0 )
            {
            // Yes it is a top contact.
            User::LeaveIfError( contactsToBeReordered.Append( &contact ) );
            topOrders.AppendL( topOrder );
            }
        }
    topOrders.Sort();

    // Now set the order values. Reuse the old order values, but assign them
    // now to different contacts.
    const TInt reorderCount = topOrders.Count();
    for ( TInt i = 0; i < reorderCount; ++i )
        {
        MVPbkStoreContact& contact = *contactsToBeReordered[ i ];
        SetTopOrderL( contact, topOrders[ i ] );
        }
    CleanupStack::PopAndDestroy(); //contactsToBeReordered
    CleanupStack::PopAndDestroy(); // topOrders
    }

// --------------------------------------------------------------------------
// Returns field type for the top field type
// --------------------------------------------------------------------------
const MVPbkFieldType& CVPbkTopContactOperation::TopContactFieldTypeL()
    {
    const MVPbkFieldType* fieldType;
    TVPbkFieldTypeMapping mapping;
    TVPbkFieldVersitProperty versitProp;
    versitProp.SetName(EVPbkVersitNameTopContact);
    mapping.SetVersitProperty(versitProp);
    fieldType = mapping.FindMatch(iContactManager.FieldTypes());
    if (!fieldType)
        {
        User::Leave(KErrNotFound);
        }
    return *fieldType;
    }


// --------------------------------------------------------------------------
// Gives the top contact order value.
// If there is no valid Top Contact order value, then returns
// KErrNotFound.
//
// @param aTopContact The top contact.
// @return Order value, or KErrNotFound if there is no
//         Top Contact data.
// --------------------------------------------------------------------------
TInt CVPbkTopContactOperation::TopOrder( const MVPbkBaseContact& aContact )
    {
    const MVPbkContactFieldTextData* textData = FindTopFieldTextData( aContact );
    TInt result = KErrNotFound;
    if ( textData )
        {
        TPtrC text(textData->Text());
        const TBool isEmpty = textData->IsEmpty();
        TLex lexer(text);
        
        if ( !isEmpty )
            {
            TInt topOrder;
            if(!lexer.Val(topOrder))
	            {
	             result=topOrder;	
	            }
            }
        }          
    return result;
    }

// --------------------------------------------------------------------------
// From MVPbkContactSelector
// --------------------------------------------------------------------------
TBool CVPbkTopContactOperation::IsContactIncluded( const MVPbkBaseContact& aContact )
    {
    return (TopOrder(aContact) == KErrNotFound);
    }

// --------------------------------------------------------------------------
// Saves top index to the contact field
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::SetTopOrderL(
    MVPbkContactFieldTextData& aTextData, TInt aTopOrderIndex )
    {
    TBuf<KTcFieldLength> text;
    text.NumFixedWidth(aTopOrderIndex, EDecimal, KTcFieldLength);
    aTextData.SetTextL( text );
    }

// --------------------------------------------------------------------------
// Adds given top index to a store contact
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::SetTopOrderL(
    MVPbkStoreContact& aContact, TInt aTopOrderIndex )
    {
    MVPbkContactFieldTextData* textData = FindTopFieldTextData( aContact );
    if ( textData )
        {
        // The field already exists
        SetTopOrderL( *textData, aTopOrderIndex );
        }
    else
        {
        // Need to create the field.
        MVPbkStoreContactField* field = aContact.CreateFieldLC(
            TopContactFieldTypeL() );        
        MVPbkContactFieldTextData& fieldData = 
            MVPbkContactFieldTextData::Cast( field->FieldData() );        
        SetTopOrderL( fieldData, aTopOrderIndex );
        // Add VPbk Field to the contact
        aContact.AddFieldL(field);                    
        CleanupStack::Pop(); // field
        }    
    }

// --------------------------------------------------------------------------
// Retruns top field text data of a view contact
// or NULL if aContact doesn't have top field
// --------------------------------------------------------------------------
const MVPbkContactFieldTextData* CVPbkTopContactOperation::FindTopFieldTextData(
    const MVPbkBaseContact& aContact )
    {
    // Loop through all VPbk fields in the contact 
    // check if a field contains top contact type.
    const MVPbkBaseContactFieldCollection& fields = aContact.Fields();
    const TInt count = fields.FieldCount();
    const MVPbkContactFieldTextData* result = NULL;
    for ( TUint i = 0; i < count && !result; ++i )
        {
        const MVPbkBaseContactField& field = fields.FieldAt(i);
        TVPbkFieldVersitProperty versitProp;
        versitProp.SetName(EVPbkVersitNameTopContact);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && fieldType->Matches(versitProp, 0) )
            {
            // Contact Template has Top Contact Field, therefore
            // a newly created contact will have an empty TC field
            // So, remember to make an additional check to see if the field
            // has data or not by calling IsEmpty().
            result = &MVPbkContactFieldTextData::Cast( field.FieldData() );        
            }                   
        }
    return result;
    }

// --------------------------------------------------------------------------
// Retruns top field text data of a store contact
// or NULL if aContact doesn't have top field
// --------------------------------------------------------------------------
MVPbkContactFieldTextData* CVPbkTopContactOperation::FindTopFieldTextData(
    MVPbkStoreContact& aContact )
    {
    // Loop through all VPbk fields in the contact 
    // check if a field contains top contact type.
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    const TInt count = fields.FieldCount();
    MVPbkContactFieldTextData* result = NULL;
    for ( TUint i = 0; i < count && !result; ++i )
        {
        MVPbkStoreContactField& field = fields.FieldAt(i);
        TVPbkFieldVersitProperty versitProp;
        versitProp.SetName(EVPbkVersitNameTopContact);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && fieldType->Matches(versitProp, 0) )
            {
            // Contact Template has Top Contact Field, therefore
            // a newly created contact will have an empty TC field
            // So, remember to make an additional check to see if the field
            // has data or not by calling IsEmpty().
            result = &MVPbkContactFieldTextData::Cast( field.FieldData() );        
            }                   
        }
    return result;
    }

// --------------------------------------------------------------------------
// Notifies operation observer of results
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::NotifyResultL()
    {
    switch( iCurrentOperation )
        {
        case EGetTopContacts:
        case EGetNonTopContacts:
            {
            __ASSERT_DEBUG( iView, Panic(ETopErrorNoView) );
            MVPbkContactViewBase* view = iView;
            iView = NULL; //transfer ownership
            iViewObserver->VPbkOperationResultCompleted( this, view );
            break;
            }
        case EGetTopContactLinks:
        case EGetNonTopContactLinks:
            {
            __ASSERT_DEBUG( iView, Panic(ETopErrorNoView) );
            MVPbkContactLinkArray* links = ViewToLinkArrayL( *iView );
            iLinksObserver->VPbkOperationResultCompleted( this, links );
            break;
            }
        case EAddToTop:
        case ERemoveFromTop:
        case EReorderTop:
            {
            iObserver->VPbkOperationCompleted( this );
            break;
            }
        default:
            __ASSERT_DEBUG( EFalse, Panic(ETopErrorWrongState) );
        }
    }

// --------------------------------------------------------------------------
// Sends error notification to the error observer
// --------------------------------------------------------------------------
inline void CVPbkTopContactOperation::NotifyError(TInt aErr)
    {
    iErrorObserver->VPbkOperationFailed( this, aErr );
    }

// --------------------------------------------------------------------------
// Notify error async
// --------------------------------------------------------------------------
void CVPbkTopContactOperation::AbortWithError(TInt aErr)
    {
    iAbortError = aErr;
    iNextState = EStateAbortWithError;
    CompleteOurself();
    }

// --------------------------------------------------------------------------
// Aquire sort order from presentation layer
// --------------------------------------------------------------------------
CVPbkSortOrderAcquirer* CVPbkTopContactOperation::AllContactsSortOrderL() const
    {
    CVPbkSortOrderAcquirer* result = NULL;
    // Acquire group sort order
    RImplInfoPtrArray implementations;
    REComSession::ListImplementationsL
        ( TUid::Uid( KVPbkSortOrderAcquirerInterfaceUID ), implementations );
    CleanupStack::PushL( TCleanupItem
            ( CleanupResetAndDestroy, &implementations ) );

    TBool found = EFalse;
    const TInt count = implementations.Count();
    for ( TInt i = count - 1; i >= 0 && !found ; --i )
        {
        CImplementationInformation* implInfo = implementations[i];

        if ( implInfo->DisplayName().
                CompareC( KAllContactsSortOrderDisplayName ) == 0 )
            {
            TUid implUid = implInfo->ImplementationUid();

            CVPbkSortOrderAcquirer::TSortOrderAcquirerParam param
                ( iContactManager.FieldTypes() );

            result = CVPbkSortOrderAcquirer::NewL
                ( implUid, param );
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(); // implementations
    return result;
    }

// end of file
