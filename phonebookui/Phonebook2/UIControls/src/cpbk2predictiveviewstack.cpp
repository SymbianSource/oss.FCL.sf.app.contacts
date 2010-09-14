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
* Description:  Phonebook 2 Predictive search view stack
*
*/

#include "CPbk2ContactPositionInfo.h"
#include "cpbk2predictiveviewstack.h"

// Phonebook 2 / Virtual Phonebook
#include <MVPbkContactViewFiltering.h>
#include <CVPbkContactFindPolicy.h>
#include <vpbkeng.rsg>
#include <MVPbkViewContact.h>
#include <CVPbkContactIdConverter.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <VPbkContactStoreUris.h>
#include <centralrepository.h>
#include <Phonebook2PrivateCRKeys.h>
#include <CVPbkTopContactManager.h>
#include "cpbk2filteredviewstack.h"
#include <CPbk2StoreConfiguration.h>
#include <MVPbkContactBookmarkCollection.h>
#include <CVPbkContactIdConverter.h>
#include "CPbk2PredictiveSearchFilter.h"
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>

// Predictive seard engine headers 
#include <CPsSettings.h>
#include <CPsQueryItem.h>
#include <CPsData.h>
#include <CPsRequestHandler.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <bidivisual.h>
#include <fepbase.h>
#include "Phonebook2PrivateCRKeys.h"


// System includes
#include <aknsfld.h>
#include <featmgr.h>
// CONSTANTS
const TInt Kspace = ' ';
_LIT(KPsGroupDBSuffix, "?id=%d");

/// Unnamed namespace for local definitions
namespace {




// --------------------------------------------------------------------------
// SendEventToObservers
// Sends events to the array of observes that take MVPbkContactViewBase.
// as a parameter.
//
// @param aView         The parameter for the NotifyFunc.
// @param aObservers    An array of MVPbkContactViewObservers.
// @param aNotifyFunc   A member function pointer of the
//                      MVPbkContactViewObserver.
// --------------------------------------------------------------------------
//
template <class Observer, class NotifyFunc>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<Observer>& aObservers,
        NotifyFunc aNotifyFunc )
    {
    const TInt count = aObservers.Count();
    for (TInt i = count-1; i >= 0 ; --i)
        {
        Observer* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView);
        }
    }

// --------------------------------------------------------------------------
// SendEventToObservers
// Sends events to the array of MVPbkContactViewObserver.
// Used for MVPbkContactViewObserver functions that have two extra parameters
// in addition to MVPbkContactViewBase.
//
// @param aView         The first parameter for the NotifyFunc.
// @param aObservers    An array of MVPbkContactViewObserver.
// @param aNotifyFunc   A member function pointer of the
//                      MVPbkContactViewObserver.
// @param aParam1       The second parameter for the aNotifyFunc.
// @param aParam2       The third parameter for the aNotifyFunc.
// --------------------------------------------------------------------------
//
template <class Observer, class FuncPtr, class ParamType1, class ParamType2>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<Observer>& aObservers,
        FuncPtr aNotifyFunc,
        ParamType1 aParam1, ParamType2& aParam2)
    {
    const TInt count = aObservers.Count();
    for (TInt i = count-1; i >= 0 ; --i)
        {
        Observer* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

} /// namespace

TInt CPbk2ContactPositionInfo::CompareByPosition( 
        const CPbk2ContactPositionInfo& aFirst, 
        const CPbk2ContactPositionInfo& aSecond )
    {
    if( aFirst.iPos < aSecond.iPos )
        {
        return -1;
        }
    else if( aFirst.iPos == aSecond.iPos )
        {
        return 0;
        }
    else
        {
        return 1;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CPbk2PredictiveViewStack
// --------------------------------------------------------------------------
CPbk2PredictiveViewStack::CPbk2PredictiveViewStack( CPbk2PredictiveSearchFilter& aSearchFilter,
        MPbk2ContactNameFormatter& aNameformatter ):
    CActive( EPriorityStandard ), iSearchFilter( aSearchFilter ),
    iNameformatter(aNameformatter),
    iNonMatchedMarkedContactStartIndex(KErrNotFound),
    iNonMatchedMarkedContactEndIndex(KErrNotFound)
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::~CPbk2PredictiveViewStack
// --------------------------------------------------------------------------
//
CPbk2PredictiveViewStack::~CPbk2PredictiveViewStack()
    {
    Cancel();
    iBidiPatterns.ResetAndDestroy();
    iPatternsCollection.ResetAndDestroy();
    iStackObservers.Reset();
    iViewObservers.Reset();
    delete iConverterDefaultStore;
    delete iPsQuery;
    delete iPsHandler;
    delete iCurrentGroupLink;
    if ( iBaseView )
        {
        iBaseView->RemoveObserver( *this );
        }
  
    if( iPredictiveSearchResultContactLinkArrray )
        {
        iPredictiveSearchResultContactLinkArrray->ResetAndDestroy();
        delete iPredictiveSearchResultContactLinkArrray;
        }
  
    iTopContactPositionInfoArray.ResetAndDestroy();
    iMarkedContactsPositionInfoArray.ResetAndDestroy();
    
    delete iTopContactManager;
    iSearchText.Close();
    if(iFeatureManagerInitilized)
        {
        FeatureManager::UnInitializeLib();
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::NewL
// --------------------------------------------------------------------------
//
CPbk2PredictiveViewStack* CPbk2PredictiveViewStack::NewL(
        MVPbkContactViewBase& aBaseView,
        CPbk2PredictiveSearchFilter& aSearchFilter,
        MPbk2ContactNameFormatter& aNameformatter )
    {
    CPbk2PredictiveViewStack* self = new ( ELeave ) CPbk2PredictiveViewStack( aSearchFilter,
            aNameformatter );
    CleanupStack::PushL( self );
    self->ConstructL( aBaseView );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ConstructL( MVPbkContactViewBase& aBaseView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::ConstructL"));

    iSearchedState = EFalse;
    iViewReady = EFalse;
    iBaseView = &aBaseView;
    
    // Initialize feature manager
    FeatureManager::InitializeLibL();
    iFeatureManagerInitilized = ETrue;
    // Initilize the PCS engine 
    InitializePCSEngineL();

    CVPbkContactManager& manager = Phonebook2::Pbk2AppUi()->
        ApplicationServices().ContactManager();
		
    iTopContactManager = CVPbkTopContactManager::NewL( manager );
	
    // Arrary for holding the search result links
    iPredictiveSearchResultContactLinkArrray = CVPbkContactLinkArray::NewL();

    // Start listening view events
    iBaseView->AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::UpdateFilterL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::UpdateFilterL( const MDesCArray& aFindStrings,
        const MVPbkContactBookmarkCollection* aAlwaysincluded,
		        TBool aAlwaysIncludedChanged )
    {
    if ( aAlwaysIncludedChanged )
        {
        // iBookMarkCollection is not owned
        iBookMarkCollection = 
            const_cast<MVPbkContactBookmarkCollection*>( aAlwaysincluded );
        }
    
    iPredictiveSearch = iSearchFilter.IsPredictiveActivated();
    
    TKeyboardModes pcsKeyboardMode( EItut );
    if ( iPredictiveSearch )
        {
        pcsKeyboardMode = EItut;
        }
    else
        {
        pcsKeyboardMode = EQwerty;
        }
    
    // During filtering if there were multitaps, Fep transacton is broken 
    // and search box sends canceletion of last tap, we ignore this.
    if( iPredictiveSearch && iSearchFilter.IsFiltering() )
        {
        return;
        }   
  
    RBuf searchText;
    iSearchFilter.GetSearchTextL( searchText );
    CleanupClosePushL( searchText );
    // Events go on up and down key, it is not good do search always twice 
    TInt compareSearchText = iSearchText.Compare(searchText);
    
    iSearchText.Close();
    iSearchText.CreateL(searchText.Length());
    iSearchText = searchText;
    
    CleanupStack::PopAndDestroy(&searchText);
    
    if( compareSearchText == 0 )
        {
        return;
        }
    
    if( !aFindStrings.MdcaCount() )
        {
        if( compareSearchText == 1 )
            {
            if( !iSearchText.Length() )
                {
                Reset();
                }
            else
                {
                ClearSearch();
                }
            }
        return;
        }
    
    // Delete the previous query
    delete iPsQuery;
    iPsQuery = NULL;
    
    // Create the new search query
    iPsQuery = CPsQuery::NewL(); 
    
    // Combine the search strings and create the PCS query
    for ( TInt j = 0; j < aFindStrings.MdcaCount(); j++ )
        {
        HBufC* searchString = aFindStrings.MdcaPoint(j).AllocL();
        TPtrC searchStringPtr(*searchString);    
        CleanupStack::PushL( searchString );
        
        for ( TInt i = 0; i < searchStringPtr.Length(); i++ )
            {    
            // Add a query item
            CPsQueryItem* item = CPsQueryItem::NewL();
            item->SetCharacter(searchStringPtr[i]); 
            
            TInt qwertyNumber = KErrCancel;
            if( iPredictiveSearch )
                {
                TLex numberVal( searchStringPtr.Mid(i, 1) );
                TInt num = 0;
                qwertyNumber = numberVal.Val(num);
                }
            if( qwertyNumber == KErrNone )
                {
                // Set qwerty search mode (in predictive search we can tap numbers only by long tap)
                item->SetMode( EQwerty );
                }
            else
                {
                // Set current search mode
                item->SetMode( pcsKeyboardMode );
                }
            
            iPsQuery->AppendL(*item);      
            }
        
        //Add a space in between two words
        if(aFindStrings.MdcaCount() > 1)
            {
            CPsQueryItem* item = CPsQueryItem::NewL();
            item->SetCharacter(Kspace);   
            item->SetMode(EItut);
            iPsQuery->AppendL(*item);    
            }
       
        //Cleanup
        CleanupStack::PopAndDestroy( searchString);
        }
    
    //Send the search query to the  PCS engine
    iPsHandler->SearchL(*iPsQuery);
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::Reset
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::Reset()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::Reset: topview(0x%x), current stack level = %d"));  
    TRAP_IGNORE(iSearchFilter.ResetL());
    iPsHandler->CancelSearch();
    ClearSearch();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ClearSearch
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ClearSearch()
    {
    iNonMatchedMarkedContactStartIndex = KErrNotFound;
    iNonMatchedMarkedContactEndIndex = KErrNotFound;
    iSearchedState = EFalse;
    iPredictiveSearchResultContactLinkArrray->ResetAndDestroy();
    iTopContactPositionInfoArray.ResetAndDestroy();
    iSearchText.Close();
    SendTopViewChangedEvent( *iBaseView );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::BaseView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2PredictiveViewStack::BaseView() const
    {
    return *iBaseView;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SetNewBaseViewL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::SetNewBaseViewL(
        MVPbkContactViewBase& aBaseView )
    {
    Reset();
    
    // Remove observering the old base view
    if ( iBaseView )
        {
        iBaseView->RemoveObserver( *this );
        }

    iBaseView = &aBaseView;

    // Start listening to new base view
    iBaseView->AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::Level
// --------------------------------------------------------------------------
//
TInt CPbk2PredictiveViewStack::Level() const
    {
    if ( iSearchedState )
        {
        return 1;
        }
        
    return 0;
    }


// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::AddStackObserverL
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::AddStackObserverL(
        MPbk2FilteredViewStackObserver& aStackObserver )
    {
    iStackObservers.AppendL( &aStackObserver );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::RemoveStackObserver
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::RemoveStackObserver(
        MPbk2FilteredViewStackObserver& aStackObserver )
    {
    TInt index = iStackObservers.Find( &aStackObserver );
    if ( index != KErrNotFound )
        {
        iStackObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CPbk2PredictiveViewStack::Type() const
    {
    return iBaseView->Type();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ChangeSortOrderL(
        const MVPbkFieldTypeList& aSortOrder )
    {
    return iBaseView->ChangeSortOrderL( aSortOrder );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CPbk2PredictiveViewStack::SortOrder() const
    {
    return iBaseView->SortOrder();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::RefreshL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::RefreshL()
    {
    return iBaseView->RefreshL();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactCountL
// --------------------------------------------------------------------------
//
TInt CPbk2PredictiveViewStack::ContactCountL() const
    {
    TInt count;
    if ( iSearchedState )
        {
        count = iPredictiveSearchResultContactLinkArrray->Count();
        }
    else
        {
        count = iBaseView->ContactCountL();
        }
    return count;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CPbk2PredictiveViewStack::ContactAtL
        ( TInt aIndex ) const
    {
    TInt ret = BaseViewIndex( aIndex);
    User::LeaveIfError( ret );
    return iBaseView->ContactAtL( ret  );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2PredictiveViewStack::CreateLinkLC( TInt aIndex ) const
    {
    TInt ret = BaseViewIndex( aIndex);
    User::LeaveIfError( ret );
    return iBaseView->CreateLinkLC( ret );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::IndexOfLinkL
// --------------------------------------------------------------------------
//
TInt CPbk2PredictiveViewStack::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    TInt ret = KErrNotFound;
    
    if ( iSearchedState )
        {
        ret = iPredictiveSearchResultContactLinkArrray->Find( aContactLink );
        }
    else
        {
        // Just return the index in the baseview
        ret = iBaseView->IndexOfLinkL( aContactLink );
        }
    
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::AddObserverL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::AddObserverL
        ( MVPbkContactViewObserver& aObserver )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::AddObserverL - IN"));

    if ( IsActive() )
        {
        Cancel();
        }
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();

    // Events are sent in reverse order so insert to first position.
    iViewObservers.InsertL( &aObserver, 0 );
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::AddObserverL - OUT"));
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::RemoveObserver
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::RemoveObserver(
        MVPbkContactViewObserver& aObserver )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::RemoveObserver - IN"));
    
    TInt index = iViewObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iViewObservers.Remove( index );
        }
        
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::RemoveObserver - OUT"));
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CPbk2PredictiveViewStack::MatchContactStore(
        const TDesC& aContactStoreUri ) const
    {
    return iBaseView->MatchContactStore( aContactStoreUri );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CPbk2PredictiveViewStack::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const
    {
    return iBaseView->MatchContactStoreDomain( aContactStoreDomain );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CPbk2PredictiveViewStack::CreateBookmarkLC(
        TInt aIndex ) const
    {
    return iBaseView->CreateBookmarkLC( BaseViewIndex( aIndex ) );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::IndexOfBookmarkL
// --------------------------------------------------------------------------
//
TInt CPbk2PredictiveViewStack::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    TInt ret = KErrNotFound;
    
    if ( iSearchedState )
        {
        TInt indexInBaseView = iBaseView->IndexOfBookmarkL( aContactBookmark );
        const MVPbkViewContact& contact = iBaseView->ContactAtL( indexInBaseView );
        
        TInt countInSearchedResult = iPredictiveSearchResultContactLinkArrray->Count();
        
        for ( TInt i = 0; i < countInSearchedResult && ret == KErrNotFound; i++ )
            {
            const MVPbkContactLink& link = iPredictiveSearchResultContactLinkArrray->At( i );
            if ( link.RefersTo( contact ) )
                {
                ret = i;
                }
            }
        }
    else
        {
        ret = iBaseView->IndexOfBookmarkL( aContactBookmark );
        }
    
    return ret;  
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ViewFiltering
// --------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CPbk2PredictiveViewStack::ViewFiltering()
    {
    // The stack itself doesn't support filtering
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ContactViewReady( MVPbkContactViewBase& /*aView*/ )
    {
    iViewReady = ETrue;
    SendBaseViewChangedEvent();

    SendEventToObservers( *this, iViewObservers,
        &MVPbkContactViewObserver::ContactViewReady );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    iViewReady = EFalse;
    SendEventToObservers( *this, iViewObservers,
        &MVPbkContactViewObserver::ContactViewUnavailable );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ContactAddedToView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    // Reset since we need to return to base view
    // when a new contact is added
   Reset();

   //Send observer events   
   if ( iBaseView == &aView )
        {
        SendEventToObservers( *this, iStackObservers,
            &MPbk2FilteredViewStackObserver::ContactAddedToBaseView, aIndex,
            aContactLink );
        
        // Always forward only top view events to clients
        SendEventToObservers( *this, iViewObservers,
            &MVPbkContactViewObserver::ContactAddedToView, aIndex,
            aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    TRAP_IGNORE ( HandleContactDeletionL( aView, aIndex,aContactLink ) )
    
    //Send observer events  
    if ( iBaseView == &aView )
        {
        // Always forward top view events to clients
        SendEventToObservers( *this, iViewObservers,
            &MVPbkContactViewObserver::ContactRemovedFromView, aIndex,
            aContactLink );
         }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::ContactViewError
        ( MVPbkContactViewBase& aView, TInt aError, TBool aErrorNotified )
    {
    iViewReady = EFalse;
    //Send observer events   
    if ( iBaseView == &aView )
        {
        // Always forward only top view events to clients
        SendEventToObservers( *this, iViewObservers,
            &MVPbkContactViewObserver::ContactViewError, aError,
            aErrorNotified );
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::HandlePsResultsUpdate
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::HandlePsResultsUpdate(
    RPointerArray<CPsClientData>& aSearchResults,
    RPointerArray<CPsPattern>& searchSeqs )
    {
    //set the searched state to indicate that
    // predictive search view is now active
    iSearchedState = ETrue;
    
    // update pattern array
    TRAP_IGNORE( CreatePatternsL(searchSeqs) );
        
    //Clean up the data stored during previous search
    iPredictiveSearchResultContactLinkArrray->ResetAndDestroy();
    iTopContactPositionInfoArray.ResetAndDestroy();
	
    //Calculate the search result indexes
    TRAPD( err, CalculateSearchResultIndexesL( aSearchResults ) );
    if ( err != KErrNone )
        {
        Reset();       
        }  
    else
        {
        if ( !aSearchResults.Count() )
            {
            TRAP_IGNORE( SendPSNoResultsEventL() );
            }
        }
    
    SendTopViewChangedEvent( *this );
    }
    
// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::HandlePsError
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::HandlePsError( TInt /*aErrorCode*/ )
    {
    Reset();
    TRAP_IGNORE( InitializePCSEngineL() );
    }
    
// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CachingStatus
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::CachingStatus( TCachingStatus& /*aStatus*/,
    TInt& /*aError*/)
    {
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::RunL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::RunL - IN"));
    
    if ( iViewReady )
        {
        iViewObservers[0]->ContactViewReady( *this );
        }
        
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2PredictiveViewStack::RunL - OUT"));
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::DoCancel()
    {
    // Nothing to cancel
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2PredictiveViewStack::RunError( TInt aError )
    {
    iViewObservers[0]->ContactViewError( *this, aError, ETrue );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CalculateSearchResultIndexesL
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::CalculateSearchResultIndexesL(
    RPointerArray<CPsClientData>& aSearchResults )
    {
	__ASSERT_ALWAYS( iTopContactManager, User::Leave( KErrGeneral ) );
	
	//Holds the Matched Char Seq of the First Item in the 
	//Nameslist View w.r.t the initiated Predictive Search
	TBuf<KSearchFieldLength> matchSeqChr; 
	
	CVPbkContactManager& cntManager = Phonebook2::Pbk2AppUi()->
        ApplicationServices().ContactManager();
    
    const TInt resultCount = aSearchResults.Count();
    for( TInt n = 0 ; n < resultCount ; ++n )
        {         
        // Store the view index of search results
        CPsClientData* result = aSearchResults[n];                
        MVPbkContactLink* contactLink = iPsHandler->ConvertToVpbkLinkLC(*result,cntManager);
        
        //Get the index in the base view
        // We need to check if base view contains this contact
        TInt posInBaseView = iBaseView->IndexOfLinkL( *contactLink );
        if ( posInBaseView != KErrNotFound )
            {
            const MVPbkViewContact& contact = iBaseView->ContactAtL( posInBaseView );
            TBuf< KPsQueryMaxLen > result;
            iSearchFilter.LookupL( contact, *this, iNameformatter, result );
            
            if( result.Length() <= 0 )
                {
                CleanupStack::Pop(); //contactLink
                continue;
                }
            
            TBool topcontact = iTopContactManager->IsTopContact( contact );
            if ( topcontact )
                {
                // ownership of contactLink is transfered, CPbk2TopContactPositionInfo
                // also keeps the contactLink' position from iBaseView
                CPbk2ContactPositionInfo* topContactPosInfo = 
                    CPbk2ContactPositionInfo::NewLC( contactLink, posInBaseView );
                   
                iTopContactPositionInfoArray.AppendL( topContactPosInfo ); 
                CleanupStack::Pop( topContactPosInfo );
                }
            else
                {
                iPredictiveSearchResultContactLinkArrray->AppendL( contactLink );     
                }
            CleanupStack::Pop(); //contactLink
            }
        else
            {
            CleanupStack::PopAndDestroy(); //contactLink
            }
        }
 
    iNonMatchedMarkedContactStartIndex = KErrNotFound;
    iNonMatchedMarkedContactEndIndex = KErrNotFound;
    
    //save marked contacts info to iMarkedContactsPositionInfoArray
    if ( iBookMarkCollection )
        {
        SaveBookmarkContatsInfoL();
        SortbyPositionInMainView( iMarkedContactsPositionInfoArray );
        iNonMatchedMarkedContactStartIndex = 0;
        iNonMatchedMarkedContactEndIndex = iMarkedContactsPositionInfoArray.Count() - 1;
        MoveArrayToSearchedResultTopL( iMarkedContactsPositionInfoArray );        
        }
    
    // add those top contacts to top of the iPredictiveSearchResultContactLinkArrray,
    // the order follows top contact orders in name list
    SortbyPositionInMainView( iTopContactPositionInfoArray );
    if ( iNonMatchedMarkedContactStartIndex != KErrNotFound )
        {
        iNonMatchedMarkedContactStartIndex += iTopContactPositionInfoArray.Count();
        iNonMatchedMarkedContactEndIndex += iTopContactPositionInfoArray.Count();
        }
    MoveArrayToSearchedResultTopL( iTopContactPositionInfoArray );
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::InitializePCSEngineL
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::InitializePCSEngineL()
    {
    	if(iPsHandler)
    	  {
    		delete iPsHandler;
    		iPsHandler = NULL;
    	  }	
    // Create async request handle for predictive search engine
    iPsHandler = CPSRequestHandler::NewL();
    iPsHandler->AddObserverL( this );
    
    // If the iCurrentGroupLink is set, then no need to re-do the settings.It is already done.
     if(iCurrentGroupLink)
         {
         return;  
         }
       
    RPointerArray<TDesC> databases;
    if ( iBaseView->MatchContactStore( VPbkContactStoreUris::SimGlobalFdnUri() ) )
        {
        //FDN view, initialize PS with FDN contacts only
        databases.AppendL(VPbkContactStoreUris::SimGlobalFdnUri().AllocL());
        }
    else
        {
        CPbk2StoreConfiguration& config = Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration();
        CVPbkContactStoreUriArray* stores = config.SearchStoreConfigurationL();
        CleanupStack::PushL(stores);
        TInt count = stores->Count();
        for ( TInt i = 0; i < count; ++i)
            {
            TVPbkContactStoreUriPtr uriPtr = stores->operator[](i);
            databases.AppendL(uriPtr.UriDes().AllocL());
            }
        CleanupStack::PopAndDestroy(); //stores
        }
    
    
    SetPsSettingL(databases);
   
    databases.ResetAndDestroy();
    
    }

// --------------------------------------------------------------------------
// Maps from the search subset position index to base view index.
// @param aSearchResultIndex Index within scope of search result set.
// @return Index of the contact in base view.
// --------------------------------------------------------------------------
TInt CPbk2PredictiveViewStack::BaseViewIndex( TInt aSearchResultIndex ) const
    {
    // If search is not active, the argument is actually already a base view index.
    TInt index = aSearchResultIndex;
    if ( iSearchedState )
        {
        TRAP_IGNORE( index = 
            iBaseView->IndexOfLinkL(
                iPredictiveSearchResultContactLinkArrray->At( aSearchResultIndex ) ) );
        }
        
    return index;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::HandleContactDeletionL
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::HandleContactDeletionL( MVPbkContactViewBase& /*aView*/,
    TInt /*aIndex*/, const MVPbkContactLink& aContactLink )
    {
    
    TInt removedContactPos = iPredictiveSearchResultContactLinkArrray->Find(aContactLink);
    if(removedContactPos != KErrNotFound)
        {
        //Remove the link from iPredictiveSearchResultContactLinkArrray
        iPredictiveSearchResultContactLinkArrray->Remove(removedContactPos);                   
        }
    
    if (iPredictiveSearchResultContactLinkArrray->Count() == 0)
        {
        //clear patterns collection
        iPatternsCollection.ResetAndDestroy();
        // back to name list view
        iSearchFilter.FindPaneResetL();
        // Reset() must be called after iSearchFilter.FindPaneResetL()
        // as it will make some judgment by the content of FindBox.
        Reset();
        }
    
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SendTopViewChangedEvent
// --------------------------------------------------------------------------
void CPbk2PredictiveViewStack::SendTopViewChangedEvent
        ( MVPbkContactViewBase& aOldTopView )
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->TopViewChangedL( aOldTopView ) );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendTopViewChangedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendTopViewUpdatedEvent
// --------------------------------------------------------------------------
//
void CPbk2PredictiveViewStack::SendTopViewUpdatedEvent()
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->TopViewUpdatedL() );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendTopViewUpdatedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendBaseViewChangedEvent
// --------------------------------------------------------------------------
//    
void CPbk2PredictiveViewStack::SendBaseViewChangedEvent()
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->BaseViewChangedL() );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendBaseViewChangedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }


// ---------------------------------------------------------------------------
// CPbk2PredictiveViewStack::LastPCSQuery
// ---------------------------------------------------------------------------
const CPsQuery* CPbk2PredictiveViewStack::LastPCSQuery() const
    {
    return iSearchedState?iPsQuery:NULL;
    }


// ---------------------------------------------------------------------------
// CPbk2PredictiveViewStack::PSHandler
// ---------------------------------------------------------------------------
CPSRequestHandler* CPbk2PredictiveViewStack::PSHandler() const
    {
    return iPsHandler;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveViewStack::CreatePatternsL()
// ---------------------------------------------------------------------------
void CPbk2PredictiveViewStack::CreatePatternsL(RPointerArray<CPsPattern>& searchSeqs)
    {
    iPatternsCollection.ResetAndDestroy();
    iBidiPatterns.ResetAndDestroy();
    TBidirectionalState::TRunInfo* runInfoArray = new (ELeave)TBidirectionalState::TRunInfo[4];

    for (TInt i = 0; i < searchSeqs.Count(); i++)
        {
        //create standard pattern
        TInt nIndex = searchSeqs[i]->FirstIndex();  
        TDesC& patternText = searchSeqs[i]->Pattern();
        CPsPattern* pattern = CPsPattern::NewL();
        iPatternsCollection.Append(pattern);
        pattern->SetFirstIndex( nIndex );
        pattern->SetPatternL( patternText );

        //create bidirectional pattern
        HBufC* bidiPattern = HBufC::NewLC( patternText.Length() + TBidiLogicalToVisual::KMinCharAvailable );
        TPtr tmpPtr2 = bidiPattern->Des();
        TBidiLogicalToVisual converter2( patternText, runInfoArray, /*runInfoArrayMaxLength*/4 ) ;
        converter2.Reorder();
        converter2.GetVisualLine( tmpPtr2, 0, patternText.Length(), TChar(0xffff) );
        iBidiPatterns.Append( bidiPattern );
        CleanupStack::Pop( bidiPattern );
        }
    delete runInfoArray; 
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveViewStack::GetMatchingPartsL()
// Main assumption to use this method only for bidirectional
// languages and patterns
// ---------------------------------------------------------------------------
void CPbk2PredictiveViewStack::GetMatchingPartsL( const TDesC& aSearchData,
                      RArray<TPsMatchLocation>& aMatchLocation) const
    {
    // If list of mathes empty just return
    if ( iPatternsCollection.Count() == 0 )
        {
        return;
        }
    //This method is implemented for bidirectional languages only
    RBuf parserText;
    parserText.CreateL(aSearchData);
    parserText.CleanupClosePushL();
    TLex lexParser(parserText);
    TPtrC ptr;
    //parse for all words in contact information
    TInt currentPos = 0;
    lexParser.SkipSpace();
    currentPos = lexParser.Offset();
    for (ptr.Set(lexParser.NextToken()); ptr.Length(); ptr.Set(lexParser.NextToken()) )
        {
        //loop by all patterns
        for (TInt j = iBidiPatterns.Count() - 1; j >= 0; j--)
            {
            HBufC* pattern = iBidiPatterns[j];
            TInt pos =  ptr.Find(pattern->Des());
            if ( pos != KErrNotFound)
                {
                //check if pattern is in th end of the string
                if ( (pos + pattern->Length()) == ptr.Length() ) 
                    {
                    // add math to output array
                    TPsMatchLocation location;
                    location.index = currentPos + pos;
                    location.length = pattern->Length();
                    location.direction = TBidiText::ELeftToRight;
                    aMatchLocation.Append(location);
                    break;
                    }
                }
            }
        lexParser.SkipSpace();
        currentPos = lexParser.Offset();
        }
    CleanupStack::PopAndDestroy();//parserText
    }
	
// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SetCurrentGroupLinkL
// --------------------------------------------------------------------------
//   
void CPbk2PredictiveViewStack::SetCurrentGroupLinkL( MVPbkContactLink* aGroupLinktoSet)
    {
    
    //Create contactid converter instance for default store if not already exists
    if(!iConverterDefaultStore)
        {
        CVPbkContactManager& manager = Phonebook2::Pbk2AppUi()->
            ApplicationServices().ContactManager();
        MVPbkContactStore* defaultStore = manager.ContactStoresL().Find(
            VPbkContactStoreUris::DefaultCntDbUri() );
        __ASSERT_DEBUG( defaultStore, User::Panic(_L("Pbk2 vstack"), 52));
        
        iConverterDefaultStore = CVPbkContactIdConverter::NewL( *defaultStore );
        }
    //Delete the existing group link 
    if(iCurrentGroupLink)
        {
        delete iCurrentGroupLink;
        iCurrentGroupLink = NULL;
        }
    
    
    iCurrentGroupLink = aGroupLinktoSet->CloneLC();
    CleanupStack::Pop();
        
    if(iCurrentGroupLink)
        {
        RPointerArray<TDesC> databases;
        RBuf storeUrl; 
        //CleanupResetAndDestroyPushL( databases );
        // Set max size for group URI
        TInt32 maxLength = sizeof(TInt32) + KVPbkDefaultGrpDbURI().Size() + 
                                 KPsGroupDBSuffix().Size() + 2;
        storeUrl.CreateL(KVPbkDefaultGrpDbURI(), maxLength);
        storeUrl.CleanupClosePushL();
        
        //get group id and create the url to be passed to ps engine
        TInt32 id = iConverterDefaultStore->LinkToIdentifier(*iCurrentGroupLink);
        storeUrl.AppendFormat(KPsGroupDBSuffix, NULL, id);
        databases.AppendL(storeUrl.AllocL());
        CleanupStack::PopAndDestroy(&storeUrl);
        
        // Perform the search settings
        SetPsSettingL(databases);
        
        databases.ResetAndDestroy();
        }
        
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SetPsSettingL
// --------------------------------------------------------------------------
//   
void CPbk2PredictiveViewStack::SetPsSettingL( RPointerArray<TDesC>& aDatabases) 
    {
    // Perform search settings
    CPsSettings* psSettings = CPsSettings::NewL();
    CleanupStack::PushL( psSettings );    
    psSettings->SetSearchUrisL(aDatabases);
        
    RArray<TInt> displayFields;
    displayFields.AppendL(R_VPBK_FIELD_TYPE_FIRSTNAME); // Firstname
    displayFields.AppendL(R_VPBK_FIELD_TYPE_LASTNAME); // Lastname
    displayFields.AppendL(R_VPBK_FIELD_TYPE_COMPANYNAME);// Companyname
   
    psSettings->SetDisplayFieldsL(displayFields);

    iPsHandler->SetSearchSettingsL(*psSettings);
    
    //Clean up
    CleanupStack::PopAndDestroy(psSettings);
    displayFields.Close();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SaveBookmarkContatsInfoL
// --------------------------------------------------------------------------
//  
void CPbk2PredictiveViewStack::SaveBookmarkContatsInfoL()
    {
    TInt count = iBookMarkCollection->Count();
    
    for ( TInt i = count-1; i >= 0; i-- )
        {
        const MVPbkContactBookmark& contactBookmark = 
            iBookMarkCollection->At( i );
        TInt indexInBaseView = iBaseView->IndexOfBookmarkL( contactBookmark );
        
        if ( indexInBaseView != KErrNotFound )
            {
            MVPbkContactLink* link = iBaseView->CreateLinkLC( indexInBaseView );
            
            // find from Searched result
            TInt indexInSearchedResult = iPredictiveSearchResultContactLinkArrray->Find( *link );
 
            TInt matchingTopContactIndex = MatchingTopContactFind( indexInBaseView );
            
            if ( indexInSearchedResult == KErrNotFound && 
                    matchingTopContactIndex == KErrNotFound )
                {
                const MVPbkViewContact& contact = iBaseView->ContactAtL( indexInBaseView );
                
                CPbk2ContactPositionInfo* positionInfo = CPbk2ContactPositionInfo::NewLC( link, indexInBaseView );
                CleanupStack::Pop( positionInfo );
                CleanupStack::Pop(); // link
                CleanupStack::PushL( positionInfo );
                
                // non matching marked top contacts
                if ( iTopContactManager->IsTopContact( contact ) )
                    {
                    iTopContactPositionInfoArray.AppendL( positionInfo );
                    }
                else
                    {
                    // non matching marked contacts(non top)
                    iMarkedContactsPositionInfoArray.AppendL( positionInfo );
                    }
                
                CleanupStack::Pop( positionInfo );
                }
            else
                {
                CleanupStack::PopAndDestroy(); // link
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::MoveArrayToSearchedResultTopL
// --------------------------------------------------------------------------
//  
void CPbk2PredictiveViewStack::MoveArrayToSearchedResultTopL( 
        RPointerArray<CPbk2ContactPositionInfo>& aContactInfoArray )
    {
    TInt posZero = 0;
    TInt count = aContactInfoArray.Count();
   
    for( TInt i = count-1; i >=0; i-- )
        {
        CPbk2ContactPositionInfo* positionInfo = aContactInfoArray[ i ];
 
        // ownership of contactLink is transferred
        MVPbkContactLink* contactLink = positionInfo->ContactLink();
        CleanupStack::PushL( contactLink );
        // always insert to the top of iPredictiveSearchResultContactLinkArrray
        iPredictiveSearchResultContactLinkArrray->InsertL( contactLink, posZero ); 
        CleanupStack::Pop( contactLink );
        // remove positionInfo from iTopContactPositionInfoArray
        aContactInfoArray.Remove( i );
        delete positionInfo;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::MatchingTopContactFind
// --------------------------------------------------------------------------
//  
TInt CPbk2PredictiveViewStack::MatchingTopContactFind( const TInt aIndexInBaseView )
    {
    TInt ret = KErrNotFound;
    
    TInt topContactCount = iTopContactPositionInfoArray.Count();
    for( TInt i = 0; i < topContactCount && ret == KErrNotFound; i++ )
        {
        CPbk2ContactPositionInfo* topContactPositionInfo = 
            iTopContactPositionInfoArray[i];
        
        if ( aIndexInBaseView == topContactPositionInfo->Position() )
            {
            ret = i;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveViewStack::SortbyPositionInMainView
// --------------------------------------------------------------------------
// 
void CPbk2PredictiveViewStack::SortbyPositionInMainView( 
                 RPointerArray<CPbk2ContactPositionInfo>& aContactInfoArray )
    {
    TLinearOrder< CPbk2ContactPositionInfo > 
    position( *CPbk2ContactPositionInfo::CompareByPosition );
    aContactInfoArray.Sort( position );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendPSNoResultsEventL
// --------------------------------------------------------------------------
//  
void CPbk2PredictiveViewStack::SendPSNoResultsEventL()
    {
    CPsQuery* lastQuery = const_cast<CPsQuery*> (LastPCSQuery());    
    if ( lastQuery )
        {        
        TDesC& strQuery = lastQuery->QueryAsStringLC();   
        //Send the matched char Sequence of the first item
        //to the Search Pane filter 
        iSearchFilter.HandlePSNoMatchL( strQuery, KNullDesC );
        CleanupStack::PopAndDestroy(); // QueryAsStringLC
        } 
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::IsNonMatchingMarkedContact
// --------------------------------------------------------------------------
//
TBool CPbk2PredictiveViewStack::IsNonMatchingMarkedContact( const TInt aIndex )
    {
    TBool ret = EFalse;
    
    if ( aIndex >= iNonMatchedMarkedContactStartIndex &&
           aIndex <= iNonMatchedMarkedContactEndIndex )
        {
        ret = ETrue;
        }
    return ret;
    }

// End of File
