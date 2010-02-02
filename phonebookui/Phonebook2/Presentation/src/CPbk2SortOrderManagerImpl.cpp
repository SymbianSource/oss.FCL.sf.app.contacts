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
* Description:  Phonebook 2 sort order manager implementation.
*
*/


#include "CPbk2SortOrderManagerImpl.h"

// Phonebook 2
#include "Pbk2PresentationUtils.h"
#include "Pbk2DataCaging.hrh"
#include <RPbk2LocalizedResourceFile.h>
#include <Phonebook2PrivateCRKeys.h>
#include <Pbk2Presentation.rsg>
#include <Pbk2DataCaging.hrh>

// Virtual Phonebook
#include <MVPbkContactView.h>
#include <MVPbkFieldType.h>
#include <CVPbkSortOrder.h>

// System includes
#include <barsread.h>
#include <centralrepository.h>
#include <CPsRequestHandler.h>
#include <featmgr.h>


/// Unnamed namespace for local definitions
namespace {

/// Theoretical maximum of field types in sort order.
/// NOTE: Some of the underlying stores may not support
/// this many fields in sort order.
const TInt KPbk2MaxNumberOfFieldsInSortOrder = 10;
/// Maximum separator length
/// Defines the string length, which is used to read separator character 
/// between last and first name from central repository.
/// @see qtn_phob_name_separator_char 
const TInt KMaxSeparatorLength = 4;
/// White space
_LIT(KSpace, " ");

/// These enumerations must have same values as defined in loc-file
/// qtn_phob_name_separator_used
enum TPbk2NameSeparatorUsage
    {
    EPbk2NameSepartorNotUsed,
    EPbk2NameSepartorUsed
    };

/// These enumerations must have same values as in the specification
/// and in the qtn_phob_name_order. These are different compared to
/// TPbk2NameOrderInCenRep because TPbk2NameOrderInCenRep values
/// must be same as in old PbkEng TPbkNameOrder to keep data compatibility
/// of the CenRep.
enum TPbk2NameOrderInUiSpecification
    {
    EPbk2UiSpecFirstNameLastName,
    EPbk2UiSpecLastNameFirstName
    };

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicInvalidNameConfiguration
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2SortOrderManagerImpl");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG


/**
 * Gets a digit from a descriptor.
 *
 * @param aReaderToBuf  Resource reader pointed to a descriptor resource.
 * @return  The digit.
 */
TInt GetDigitFromDescriptorL( TResourceReader& aReaderToBuf )
    {
    HBufC* orderBuf = aReaderToBuf.ReadHBufCL();
    CleanupStack::PushL( orderBuf );

    // The setting should be 1 digit
    __ASSERT_DEBUG( orderBuf->Length() == 1,
        Panic( EPanicInvalidNameConfiguration ) );

    TInt result = KErrNotFound;
    TPtrC ptr( orderBuf->Des() );
    if ( ptr.Length() > 0 && TChar(ptr[0]).IsDigit() )
        {
        result = TChar(ptr[0]).GetNumericValue();
        }

    CleanupStack::PopAndDestroy( orderBuf );
    return result;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CPbk2SortOrderManagerImpl
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::CPbk2SortOrderManagerImpl
        (const MVPbkFieldTypeList& aMasterFieldTypeList) :
            iMasterFieldTypeList(aMasterFieldTypeList)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::~CPbk2SortOrderManagerImpl
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::~CPbk2SortOrderManagerImpl()
    {
    if (iContactView)
        {
        iContactView->RemoveObserver(*this);
        }
    iObservers.Reset();
    delete iSortOrderMonitor;
    delete iSeparatorMonitor;
    delete iSortOrder;
    delete iSeparator;
    delete iDefaultSeparator;
    delete iSortOrderSettings;
     if(iFeatureManagerInitilized)
        {
        FeatureManager::UnInitializeLib();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::NewL
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl* CPbk2SortOrderManagerImpl::NewL(
        const MVPbkFieldTypeList& aMasterFieldTypeList, RFs* aFs )
    {
    CPbk2SortOrderManagerImpl* self =
        new (ELeave) CPbk2SortOrderManagerImpl(aMasterFieldTypeList);
    CleanupStack::PushL(self);
    self->ConstructL( aFs );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SortOrderManagerImpl::ConstructL( RFs* aFs )
    {
    iSortOrderSettings = CRepository::NewL(TUid::Uid(KCRUidPhonebook));

    // Open resource file for reading language specific default values
    RPbk2LocalizedResourceFile resFile( aFs );
    resFile.OpenLC( KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile() );

    // Check and set separator usage

    // 1) Read language specific default separator
    iDefaultSeparator = DefaultSeparatorConfigurationL( resFile );

    // 2) Read separator from Central Repository
    HBufC* separator = PersistentSeparatorL();

    // 3) Check that is it defined
    if ( separator->Length() == 0 )
        {
        // Not defined
        delete separator;
        // 4) Check if language specific configuration is defined
        // and use that if it is. Otherwise use space as separator
        if ( IsDefaultSeparatorConfiguredL( resFile ) )
            {
            separator = iDefaultSeparator->AllocL();
            }
        else
            {
            separator = KSpace().AllocL();
            }
        }
    iSeparator = separator;

    // Get language specific default name order
    iDefaultNameOrder = DefaultNameDisplayOrderConfigurationL( resFile );
    // Get current name order
    TPbk2NameOrderInCenRep cenRepOrder = PersistentNameDisplayOrder();
    if ( cenRepOrder == EPbk2CenRepNameOrderUndefined )
        {
        // Order was undefined in Central Repository -> use language
        // specfic default configuration
        cenRepOrder = iDefaultNameOrder;
        }

    // Convert ordering from cenrep format to external format and
    // create sort order
    iSortOrder = CreateSortOrderL( ConvertNameDisplayOrder( cenRepOrder ),
         &resFile );    
    CleanupStack::PopAndDestroy(); // resFile

    // Start listen to CenRep changes because settings can be changed e.g
    // using Phonebook1 engine API
    iSortOrderMonitor = new ( ELeave ) CSortOrderMonitor
        ( *iSortOrderSettings, *this );
    iSeparatorMonitor = new ( ELeave ) CSeparatorMonitor
        ( *iSortOrderSettings, *this );
    iSortOrderMonitor->ActivateL();
    iSeparatorMonitor->ActivateL();
    FeatureManager::InitializeLibL();
    iFeatureManagerInitilized = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SetContactViewL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::SetContactViewL
        (MVPbkContactViewBase& aContactView)
    {
    if (iContactView)
        {
        iContactView->RemoveObserver(*this);
        }
    iContactView = &aContactView;
    iContactView->AddObserverL(*this);

    delete iSortOrder;
    iSortOrder = NULL;
    iSortOrder = CVPbkSortOrder::NewL(iContactView->SortOrder());
    iSortOrder->SetReserveL(KPbk2MaxNumberOfFieldsInSortOrder);
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::AddObserverL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::AddObserverL
        (MPbk2SortOrderObserver& aObserver)
    {
    User::LeaveIfError(iObservers.Append(&aObserver));
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::RemoveObserver
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::RemoveObserver
        (MPbk2SortOrderObserver& aObserver)
    {
    const TInt index = FindObserver(aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SetNameDisplayOrderL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::SetNameDisplayOrderL
        ( CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
          const TDesC& aSeparator )
    {
    // Create new sort order object
    CVPbkSortOrder* newSortOrder =
        CreateSortOrderLC( aNameDisplayOrder, NULL );
    newSortOrder->SetReserveL( KPbk2MaxNumberOfFieldsInSortOrder );

    TBool sortOrderChanged = 
            !VPbkFieldTypeList::IsSame( *iSortOrder, *newSortOrder );
    if ( sortOrderChanged )
        {
        // Set new sort order to cenrep
        TInt res = SetPersistentNameDisplayOrder(
            ConvertNameDisplayOrder( aNameDisplayOrder) );
        if ( res == KErrNone )
            {
            delete iSortOrder;
            iSortOrder = newSortOrder;
            CleanupStack::Pop( newSortOrder );
            newSortOrder = NULL;
            
            // If view is set then try change its sort order
            if ( iContactView )
                {
                iContactView->ChangeSortOrderL( *iSortOrder );
                }
            if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
                {
                NotifyPsEngineAboutSortOrderChangeL();
                }
            }
        else
            {
            User::Leave( res );
            }
        }
            
    if ( newSortOrder )
        {
        CleanupStack::PopAndDestroy( newSortOrder );
        }
    
    // Set separator
    SetSeparatorL( aSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::NameDisplayOrder
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager::TPbk2NameDisplayOrder
        CPbk2SortOrderManagerImpl::NameDisplayOrder() const
    {
    return ConvertNameDisplayOrder( PersistentNameDisplayOrder() );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CPbk2SortOrderManagerImpl::SortOrder() const
    {
    return *iSortOrder;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::DefaultSeparator
// --------------------------------------------------------------------------
//
const TDesC& CPbk2SortOrderManagerImpl::DefaultSeparator() const
    {
    return *iDefaultSeparator;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CurrentSeparator
// --------------------------------------------------------------------------
//
const TDesC& CPbk2SortOrderManagerImpl::CurrentSeparator() const
    {
    return *iSeparator;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::ContactViewReady(MVPbkContactViewBase& aView)
    {
    const MVPbkFieldTypeList& sortOrder = aView.SortOrder();
    if ( !VPbkFieldTypeList::IsSame( *iSortOrder, sortOrder ) )
        {
        // Set contact view's sort order if sort orders don't match, for
        // example due to a language change reboot
        iContactView->ChangeSortOrderL( *iSortOrder );
        SendEventToObservers( &MPbk2SortOrderObserver::SortOrderChanged );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ContactViewUnavailable
// We are not interested in other events than the ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::ContactViewUnavailable
        (MVPbkContactViewBase& /* aView */)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ContactAddedToView
// We are not interested in other events than the ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::ContactAddedToView
        (MVPbkContactViewBase& /* aView */, TInt /* aIndex */,
        const MVPbkContactLink& /* aContactLink */)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ContactRemovedFromView
// We are not interested in other events than the ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::ContactRemovedFromView
        (MVPbkContactViewBase& /* aView */, TInt /* aIndex */,
        const MVPbkContactLink& /* aContactLink */)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ContactViewError
// We are not interested in other events than the ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::ContactViewError
        (MVPbkContactViewBase& /* aView */, TInt /* aError */,
        TBool /* aErrorNotified */)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CenRepSortOrderChangedL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CenRepSortOrderChangedL()
    {
    // Sort order was changed in CenRep either by call to
    // SetNameDisplayOrderL or by call to Phonebook1 API.
    CVPbkSortOrder* newSortOrder =
        CreateSortOrderLC( NameDisplayOrder(), NULL );
    newSortOrder->SetReserveL( KPbk2MaxNumberOfFieldsInSortOrder );

    if ( iContactView )
        {
        // If order was changed e.g using Phonebook1 APIs
        // Phonebook2 must update all subviews for new sort order.
        iContactView->ChangeSortOrderL( *newSortOrder );
        }

    delete iSortOrder;
    iSortOrder = newSortOrder;
    CleanupStack::Pop( newSortOrder );
    SendEventToObservers( &MPbk2SortOrderObserver::SortOrderChanged );
    
    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))    
        {
        NotifyPsEngineAboutSortOrderChangeL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CenRepSortOrderChangeError
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CenRepSortOrderChangeError( TInt /*aError*/ )
    {
    // This is called if CenRepSortOrderChangedL leaves. Don't send message
    // because iSortOrder was not changed.
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CenRepSeparatorChangedL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CenRepSeparatorChangedL()
    {
    // Separator was changed in CenRep either by call to
    // SetNameDisplayOrderL or  Phonebook1 API.

    // Get separator from CenRep
    HBufC* cenRepSeparator = PersistentSeparatorL();
    CleanupStack::PushL( cenRepSeparator );
    SetSeparatorL( *cenRepSeparator );
    CleanupStack::PopAndDestroy( cenRepSeparator );
    SendEventToObservers(
        &MPbk2SortOrderObserver::SortOrderChanged );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CenRepSeparatorChangeError
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CenRepSeparatorChangeError( TInt /*aError*/ )
    {
    // This is called when CenRepSeparatorChangedL leaves. The error
    // is ignored because iSeparator is not changed if SetSeparatorL
    // leaves.
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SendEventToObservers
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::SendEventToObservers
        (Pbk2SortOrderObserverEvent aEvent)
    {
    const TInt count = iObservers.Count();
    for (TInt i = 0; i < count; ++i)
        {
        (iObservers[i]->*aEvent)();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::FindObserver
// --------------------------------------------------------------------------
//
TInt CPbk2SortOrderManagerImpl::FindObserver
        (MPbk2SortOrderObserver& aObserver)
    {
    TInt result = KErrNotFound;

    const TInt count = iObservers.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iObservers[i] == &aObserver)
            {
            result = i;
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CreateSortOrderL
// --------------------------------------------------------------------------
//
CVPbkSortOrder* CPbk2SortOrderManagerImpl::CreateSortOrderL(
        CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
        RResourceFile* aResFile )
    {
    TInt resId = R_DEFAULT_NAME_DISPLAY_ORDER_LASTNAME_FIRSTNAME;

	if ( aNameDisplayOrder ==
		 CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameFirstName ||
		 aNameDisplayOrder ==
		 CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameSeparatorFirstName )
		{
		resId = R_DEFAULT_NAME_DISPLAY_ORDER_LASTNAME_FIRSTNAME;
		}
	else
		{
		resId = R_DEFAULT_NAME_DISPLAY_ORDER_FIRSTNAME_LASTNAME;
		}

    TResourceReader reader;
    if ( !aResFile )
        {
        RPbk2LocalizedResourceFile resFile;
        resFile.OpenLC( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
            Pbk2PresentationUtils::PresentationResourceFile() );
        HBufC8* buffer = resFile.AllocReadL( resId );
        CleanupStack::PopAndDestroy(); // resFile
        CleanupStack::PushL( buffer );
        reader.SetBuffer( buffer );
        }
    else
        {
        reader.SetBuffer( aResFile->AllocReadLC( resId ) );
        }

    CVPbkSortOrder* result = CVPbkSortOrder::NewL( reader,
        iMasterFieldTypeList );

    CleanupStack::PopAndDestroy(); // AllocReadLC
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CreateSortOrderLC
// --------------------------------------------------------------------------
//
CVPbkSortOrder* CPbk2SortOrderManagerImpl::CreateSortOrderLC(
        CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
        RResourceFile* aResFile )
    {
    CVPbkSortOrder* result = CreateSortOrderL( aNameDisplayOrder, aResFile );
    CleanupStack::PushL( result );
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::PersistentNameDisplayOrder
// --------------------------------------------------------------------------
//
TPbk2NameOrderInCenRep
        CPbk2SortOrderManagerImpl::PersistentNameDisplayOrder() const
    {
    TInt result = EPbk2CenRepNameOrderUndefined;
    // Ignore error and return a default order in that case
    /*TInt err = */iSortOrderSettings->Get(KPhonebookNameOrdering, result);
    return TPbk2NameOrderInCenRep( result );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SetPersistentNameDisplayOrder
// --------------------------------------------------------------------------
//
TInt CPbk2SortOrderManagerImpl::SetPersistentNameDisplayOrder(
        TPbk2NameOrderInCenRep aNameDisplayOrder)
    {
    return iSortOrderSettings->Set( KPhonebookNameOrdering,
        TInt( aNameDisplayOrder ) );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::DefaultNameDisplayOrderConfigurationL
// --------------------------------------------------------------------------
//
TPbk2NameOrderInCenRep
        CPbk2SortOrderManagerImpl::DefaultNameDisplayOrderConfigurationL(
        RResourceFile& aResFile ) const
    {
    TResourceReader reader;
    reader.SetBuffer( aResFile.AllocReadLC( R_QTN_PHOB_NAME_ORDER ) );
    TInt order = GetDigitFromDescriptorL( reader );
    CleanupStack::PopAndDestroy(); // reader

    /// This function is responsibe of converting from
    /// TPbk2NameOrderInUiSpecification to TPbk2NameOrderInCenRep
    TPbk2NameOrderInCenRep result = EPbk2CenRepLastNameFirstName;
    if ( order != KErrNotFound && order == EPbk2UiSpecFirstNameLastName )
        {
        result = EPbk2CenRepFirstNameLastName;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ConvertNameDisplayOrder
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager::TPbk2NameDisplayOrder
        CPbk2SortOrderManagerImpl::ConvertNameDisplayOrder(
            TPbk2NameOrderInCenRep aCenRepOrder ) const
    {
    TPbk2NameOrderInCenRep order = aCenRepOrder;
    if ( order == EPbk2CenRepNameOrderUndefined )
        {
        // If not defined in CenRep then use the loc-file specified setting
        order = iDefaultNameOrder;
        }
        
    CPbk2SortOrderManager::TPbk2NameDisplayOrder result =
        CPbk2SortOrderManager::EPbk2NameDisplayOrderFirstNameLastName;
    if ( order == EPbk2CenRepLastNameFirstName )
        {
        if ( iSeparator->Compare( KSpace ) == 0 )
            {
            result =
                CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameFirstName;
            }
        else
            {
            result =
                CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameSeparatorFirstName;
            }
        }
    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::ConvertNameDisplayOrder
// --------------------------------------------------------------------------
//
TPbk2NameOrderInCenRep
        CPbk2SortOrderManagerImpl::ConvertNameDisplayOrder(
            CPbk2SortOrderManager::TPbk2NameDisplayOrder aDisplayOrder ) const
    {
    TPbk2NameOrderInCenRep result = EPbk2CenRepLastNameFirstName;
    if ( aDisplayOrder ==
         CPbk2SortOrderManager::EPbk2NameDisplayOrderFirstNameLastName )
        {
        result = EPbk2CenRepFirstNameLastName;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::PersistentSeparatorL
// --------------------------------------------------------------------------
//
HBufC* CPbk2SortOrderManagerImpl::PersistentSeparatorL() const
    {
    HBufC* separator = HBufC::NewL(KMaxSeparatorLength);
    TPtr separatorPtr = separator->Des();
    TInt err = iSortOrderSettings->Get
        (KPhonebookFormattingSeparator, separatorPtr);
    if (err != KErrNone)
        {
        // Return zero length descriptor if separator not defined in
        // Central Repository
        delete separator;
        separator = KNullDesC().AllocL();
        }

    return separator;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::IsDefaultSeparatorConfiguredL
// --------------------------------------------------------------------------
//
TBool CPbk2SortOrderManagerImpl::IsDefaultSeparatorConfiguredL(
        RResourceFile& aResFile )
    {
    TResourceReader reader;
    reader.SetBuffer( aResFile.AllocReadLC(
        R_QTN_PHOB_NAME_SEPARATOR_USED ) );
    TInt used = GetDigitFromDescriptorL( reader );
    CleanupStack::PopAndDestroy(); // reader

    if ( used == EPbk2NameSepartorUsed )
        {
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::DefaultSeparatorConfigurationL
// --------------------------------------------------------------------------
//
HBufC* CPbk2SortOrderManagerImpl::DefaultSeparatorConfigurationL(
        RResourceFile& aResFile ) const
    {
    TResourceReader reader;
    reader.SetBuffer( aResFile.AllocReadLC(
        R_QTN_PHOB_NAME_SEPARATOR_CHAR ) );
    HBufC* separator = reader.ReadHBufCL();
    CleanupStack::PopAndDestroy(); // AllocReadLC
    return separator;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SetSeparatorL
// --------------------------------------------------------------------------
//
TBool CPbk2SortOrderManagerImpl::SetSeparatorL
        ( const TDesC& aSeparator )
    {
    TBool separatorChanged = EFalse;

    HBufC* separator = NULL;
    if ( aSeparator.Length() == 0 )
        {
        separator = KSpace().AllocLC();
        }
    else
        {
        separator = aSeparator.AllocLC();
        }

    User::LeaveIfError( SetPersistentSeparator( *separator ) );
    CleanupStack::Pop(); //separator

    if ( iSeparator && iSeparator->CompareC( *separator ) != 0 )
        {
        separatorChanged = ETrue;
        }
    else if ( !iSeparator && separator )
        {
        separatorChanged = ETrue;
        }

    delete iSeparator;
    iSeparator = separator;

    return separatorChanged;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::SetPersistentSeparator
// --------------------------------------------------------------------------
//
TInt CPbk2SortOrderManagerImpl::SetPersistentSeparator
        ( const TDesC& aSeparator ) const
    {
    return iSortOrderSettings->Set
        ( KPhonebookFormattingSeparator, aSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::NotifyPsEngineAboutSortOrderChangeL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::NotifyPsEngineAboutSortOrderChangeL() const
    {
    CPSRequestHandler* psHandler = CPSRequestHandler::NewLC();
    RArray<TInt> psSortOrder;
    CleanupClosePushL( psSortOrder );
    TInt fieldsCount = iSortOrder->FieldTypeCount();
    for ( TInt i = 0; i < fieldsCount; i++ )
        {
    	psSortOrder.AppendL( iSortOrder->FieldTypeAt(i).FieldTypeResId() );
        }
    psHandler->ChangeSortOrderL( KVPbkDefaultCntDbURI, psSortOrder );
    CleanupStack::PopAndDestroy( 2 ); //psHandler, psSortOrder 
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::CSortOrderMonitor
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::CSortOrderMonitor::CSortOrderMonitor(
        CRepository& aRepository, MPbk2CenRepSortOrderObserver& aObserver )
        :   CActive( EPriorityStandard ),
            iRepository( aRepository ),
            iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::~CSortOrderMonitor
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::CSortOrderMonitor::~CSortOrderMonitor()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSortOrderMonitor::ActivateL()
    {
    User::LeaveIfError( iRepository.NotifyRequest( KPhonebookNameOrdering,
        iStatus ) );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSortOrderMonitor::RunL()
    {
    iObserver.CenRepSortOrderChangedL();
    ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSortOrderMonitor::DoCancel()
    {
    iRepository.NotifyCancel( KPhonebookNameOrdering );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSortOrderMonitor::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SortOrderManagerImpl::CSortOrderMonitor::RunError( TInt aError )
    {
    iObserver.CenRepSortOrderChangeError( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::CSeparatorMonitor
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::CSeparatorMonitor::CSeparatorMonitor(
        CRepository& aRepository, MPbk2CenRepSeparatorObserver& aObserver )
        :   CActive( EPriorityStandard ),
            iRepository( aRepository ),
            iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::~CSeparatorMonitor
// --------------------------------------------------------------------------
//
CPbk2SortOrderManagerImpl::CSeparatorMonitor::~CSeparatorMonitor()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSeparatorMonitor::ActivateL()
    {
    User::LeaveIfError( iRepository.NotifyRequest(
        KPhonebookFormattingSeparator, iStatus ) );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSeparatorMonitor::RunL()
    {
    iObserver.CenRepSeparatorChangedL();
    ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SortOrderManagerImpl::CSeparatorMonitor::DoCancel()
    {
    iRepository.NotifyCancel( KPhonebookFormattingSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManagerImpl::CSeparatorMonitor::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SortOrderManagerImpl::CSeparatorMonitor::RunError( TInt aError )
    {
    iObserver.CenRepSeparatorChangeError( aError );
    return KErrNone;
    }

// End of File
