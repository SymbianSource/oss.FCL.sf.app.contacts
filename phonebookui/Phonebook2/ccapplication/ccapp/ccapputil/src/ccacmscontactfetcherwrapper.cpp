/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper for CMS contact fetching
*
*/


#include "ccapputilheaders.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::CCCAppCmsContactFetcherWrapper
// --------------------------------------------------------------------------
//
CCCAppCmsContactFetcherWrapper::CCCAppCmsContactFetcherWrapper( 
    CCCAParameter* aParameter, TWrapperParam aWrapperParam ):
    CActive( EPriorityStandard ),
    iParameter( aParameter ),
    iHandlerState( EInitial ),
    iErrorsOccured( KErrNone ),
    iRefCount( 1 ),
    iWrapperParam( aWrapperParam )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppCmsContactFetcherWrapper()"));
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::~CCCAppCmsContactFetcherWrapper
// --------------------------------------------------------------------------
//
CCCAppCmsContactFetcherWrapper::~CCCAppCmsContactFetcherWrapper()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("~CCCAppCmsContactFetcherWrapper()"));
    Cancel();
    iCmsContactDataFetcher.Close();
    iCmsSession.Close();
    iContactFieldsArray.ResetAndDestroy();
    iContactFieldsArray.Close();
    iObservers.Close();
    delete iContactInfo;
    //iParameter, iObservers taken care elsewhere
    }
    
 // ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::Release()
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppCmsContactFetcherWrapper::Release()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::Release"));
    
    TAny* tlsPtr = Dll::Tls();
 
    CCCAppCmsContactFetcherWrapper* self = static_cast<CCCAppCmsContactFetcherWrapper*>( tlsPtr );
    if ( 0 == --self->iRefCount )
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::Release - last instance, will be deleted"));
        Dll::FreeTls();
        delete self;
        }
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::Release"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::CreateInstanceL
// --------------------------------------------------------------------------
//
EXPORT_C CCCAppCmsContactFetcherWrapper* CCCAppCmsContactFetcherWrapper::CreateInstanceL(
    CCCAParameter* aParameter, TWrapperParam aWrapperParam )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::CreateInstanceL"));

    CCCAppCmsContactFetcherWrapper* self = NULL;
    TAny* tlsPtr = Dll::Tls();
    if ( NULL == tlsPtr )
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::CreateInstanceL - not found, create new"));
        self = new( ELeave ) CCCAppCmsContactFetcherWrapper( aParameter, aWrapperParam );
        CleanupStack::PushL( self );
        self->ConstructL( );
        User::LeaveIfError( Dll::SetTls( self ) );
        CleanupStack::Pop( self );
        }
    else
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::CreateInstanceL - already existing"));
        User::Leave( KErrAlreadyExists );
        }

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::CreateInstanceL"));
    return self;
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::ConstructL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::ConstructL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::ConstructL()"));

    TRAPD( err, StartFetcherL());
    // problem can either be with connecting to CMS or with opening the contact
    iErrorsOccured = err;

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::ConstructL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::InstanceL
// --------------------------------------------------------------------------
//  
EXPORT_C CCCAppCmsContactFetcherWrapper* CCCAppCmsContactFetcherWrapper::InstanceL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::InstanceL()"));

    CCCAppCmsContactFetcherWrapper* self = NULL;
    TAny* tlsPtr = Dll::Tls();
    if ( NULL == tlsPtr )
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::HandleL() - singleton not created yet!"));
        User::Leave( KErrNotFound );
        }
    else
        {
        self = static_cast<CCCAppCmsContactFetcherWrapper*>( tlsPtr );
        ++self->iRefCount;
        }

    CCA_DP(KCCAppUtilLogFile, CCA_L("::InstanceL - iRefCount: %d"), self->iRefCount );
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::InstanceL()"));
    return self;
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::StartFetcherL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::StartFetcherL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::StartFetcherL()"));
    User::LeaveIfError( iCmsSession.Connect() );
    OpenContactL();
    
    StartAsyncFetchingL();
    
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::StartFetcherL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::StartAsyncFetchingL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::StartAsyncFetchingL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::StartAsyncFetchingL()"));

    if ( iWrapperParam & EFindContactFromOtherStores )
        FindFromOtherStoresL();
    else
        FetchContactInfoL();

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::StartAsyncFetchingL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::AddObserverL
// --------------------------------------------------------------------------
//  
EXPORT_C void CCCAppCmsContactFetcherWrapper::AddObserverL( 
    MCCAppContactFieldDataObserver& aObserver )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::AddObserverL()"));
    const TInt index = iObservers.Find( &aObserver );
    if ( KErrNotFound == index )
        {
        iObservers.AppendL( &aObserver );
        }

    if ( iErrorsOccured )
        {// errors happened already
        aObserver.ContactFieldDataObserverHandleErrorL( 
            iHandlerState, iErrorsOccured );
        }
        
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::AddObserverL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::RemoveObserver
// --------------------------------------------------------------------------
//  
EXPORT_C void CCCAppCmsContactFetcherWrapper::RemoveObserver(
    MCCAppContactFieldDataObserver& aObserver )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::RemoveObserverL()"));
    CCA_DP(KCCAppUtilLogFile, CCA_L("::RemoveObserverL - amount of observers: %d"), iObservers.Count() );
    const TInt index = iObservers.Find( &aObserver );
    if ( KErrNotFound != index )
        {
        iObservers.Remove( index );
        CCA_DP( KCCAppUtilLogFile, CCA_L("::RemoveObserverL - observer found & removed: %d"), index );
        }
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::RemoveObserverL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::IsServiceAvailable
// --------------------------------------------------------------------------
// 
EXPORT_C TInt CCCAppCmsContactFetcherWrapper::IsServiceAvailable(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    return iCmsContactDataFetcher.IsServiceAvailable( aContactAction );
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::ContactStore
// --------------------------------------------------------------------------
// 
EXPORT_C TCmsContactStore CCCAppCmsContactFetcherWrapper::ContactStore() const
    {
    return iCmsContactDataFetcher.ContactStore();
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::GetContactActionFieldCount
// --------------------------------------------------------------------------
// 
EXPORT_C TInt CCCAppCmsContactFetcherWrapper::GetContactActionFieldCount(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction)
    {
    return iCmsContactDataFetcher.GetContactActionFieldCount(aContactAction);
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::OpenContactL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::OpenContactL()
    {                    
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::OpenContactL()"));
    iHandlerState = EOpeningContact;
    switch( iParameter->ContactDataFlag())
        {
        case MCCAParameter::EContactId:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::OpenContactL() EContactId"));
            User::LeaveIfError( iCmsContactDataFetcher.Open(
                iCmsSession, iParameter->ContactIdL()));
            }
            break;
        case MCCAParameter::EContactLink:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::OpenContactL() EContactLink"));
            HBufC8* contactlink = iParameter->ContactLinkL();
            CleanupStack::PushL( contactlink );
            User::LeaveIfError( iCmsContactDataFetcher.Open( 
                iCmsSession, *contactlink));
            CleanupStack::PopAndDestroy( contactlink );            
            }
            break;
        case MCCAParameter::EContactMSISDN:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::OpenContactL() EContactMSISDN"));
            User::LeaveIfError( iCmsContactDataFetcher.Open( 
                iCmsSession, iParameter->ContactDataL()));                                    
            }
            break;
        //case MCCAParameter::EContactEmail:// fallthrough
        default:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::OpenContactL() unknown type, will leave.."));
            User::Leave( KErrNotSupported );
            }
            break;
        }
    iCmsContactDataFetcher.OrderNotificationsL(this, CCmsContactFieldItem::ECmsPhonebookNotification);
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::OpenContactL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::RunL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::RunL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::RunL()"));
    CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL: iStatus: %d"), iStatus.Int() );
           
    switch( iHandlerState )
        {
        case EFindingFromOtherStores:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() iHandlerState == EFindingFromOtherStores"));
            FetchContactInfoL();
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() EFindingFromOtherStores completed"));
            }
            break;        
        case ELoadingEnabledFields:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() iHandlerState == ELoadingEnabledFields"));
            HandleContactInfoUpdateL();
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() ELoadingEnabledFields completed"));
            }
            break;
        case ELoadingFields:
            {
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() iHandlerState == ELoadingField"));
            HandleContactDataFieldUpdateL();
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() ELoadingField completed"));
            }
            break;
        default:
            {
            // error situation - should not come here
            CCA_DP(KCCAppUtilLogFile, CCA_L("::RunL() iHandlerState not found, will leave.."));
            // error notification done in RunError
            User::Leave( KErrGeneral );
            }
            break;
        }
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::RunL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::RunError
// --------------------------------------------------------------------------
//
TInt CCCAppCmsContactFetcherWrapper::RunError( TInt aError )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppCmsContactFetcherWrapper::RunError: aError: %d"), aError );
    iErrorsOccured = aError;
    TRAP_IGNORE( NotifyErrorL() );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::NotifyErrorL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::NotifyErrorL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::NotifyErrorL()"));
    const TInt count = iObservers.Count();
    CCA_DP( KCCAppUtilLogFile, CCA_L("::NotifyErrorL - amount of observers: %d"), count );
    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverHandleErrorL( 
            iHandlerState, iErrorsOccured ));
        } 

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::NotifyErrorL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::DoCancel
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::DoCancel()
    {
    if( IsActive() )
        {// CMS Client doesn't know the operation to be cancelled
        if ( EFindingFromOtherStores == iHandlerState )
            {
            iCmsContactDataFetcher.CancelExternalContactFind();
            }        
        else if ( ELoadingEnabledFields == iHandlerState && iContactInfo )
            {
            iCmsContactDataFetcher.Cancel( *iContactInfo );
            delete iContactInfo;
            iContactInfo = NULL;
            }
        else if ( ELoadingFields == iHandlerState && iContactField )
            {
            iCmsContactDataFetcher.Cancel( *iContactField );
            delete iContactField;
            iContactField = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::FindFromOtherStoresL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::FindFromOtherStoresL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::FindFromOtherStoresL()"));

    iHandlerState = EFindingFromOtherStores;
    iStatus = KRequestPending;
    
    iCmsContactDataFetcher.FindExternalContact( iStatus );
    SetActive();
    
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::FindFromOtherStoresL()"));
    };

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::FetchContactInfoL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::FetchContactInfoL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::FetchEnabledFieldsL()"));

    iHandlerState = ELoadingEnabledFields;
    iStatus = KRequestPending;
    
    iContactInfo = iCmsContactDataFetcher.EnabledFieldsL( iStatus );
    SetActive();
    
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::FetchEnabledFieldsL()"));
    };

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::HandleContactInfoUpdateL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::HandleContactInfoUpdateL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::HandleContactInfoUpdateL()"));

    //error check
    const TInt err = iStatus.Int();
    if ( KErrNone != err )
        {
        // cleanup
        delete iContactInfo;
        iContactInfo = NULL;
        User::Leave( err );    
        }

    const TInt count = iObservers.Count();
    CCA_DP( KCCAppUtilLogFile, CCA_L("::HandleContactInfoUpdateL - amount of observers: %d"), count );
    MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
    param.iType = MCCAppContactFieldDataObserver::TParameter::EContactInfoAvailable;
    param.iContactInfo = iContactInfo;
    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverNotifyL( param ));
        }    
    
    iHandlerState = ELoadingFields;
    iContactDataFetchingIndex = 0;
    StartContactDataFieldFetchingL();

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::HandleContactInfoUpdateL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::StartContactDataFieldFetchingL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::StartContactDataFieldFetchingL()
    {                    
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::StartContactDataFieldFetchingL()"));
    TInt dataFieldCount = iContactInfo->Fields().Count();
    CCA_DP(KCCAppUtilLogFile, CCA_L("::StartContactDataFieldFetchingL amount of fields to be fetched = %d"), dataFieldCount );

    if ( dataFieldCount )
        {// start fetching
        FetchContactDataFieldL();
        }
    else
        {// nothing to fetch
        iHandlerState = EOperationsCompleted;
        //todo; do we need a error-callback to notify the error cases?
        }    

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::StartContactDataFieldFetchingL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::FetchContactDataFieldL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::FetchContactDataFieldL()
    {                    
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::FetchContactDataFieldL()"));
    iStatus = KRequestPending;

    const RArray<CCmsContactFieldItem::TCmsContactField>& enabledFields =
         iContactInfo->Fields();//not owned

    iContactField = NULL;
    iContactField = iCmsContactDataFetcher.FieldDataL( 
        iStatus, enabledFields[ iContactDataFetchingIndex ]);
  
    SetActive();
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::FetchContactDataFieldL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::HandleContactDataFieldUpdateL
// --------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::HandleContactDataFieldUpdateL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppCmsContactFetcherWrapper::HandleContactDataFieldUpdateL()"));
    CCA_DP(KCCAppUtilLogFile, CCA_L("::HandleContactDataFieldUpdateL fetched field type: %d"), (TInt)iContactField->Type() );
    
    TBool moreToFetch = iContactDataFetchingIndex < iContactInfo->Fields().Count() - 1;
    
    // error check
    if ( KErrNone == iStatus.Int() )
        {// update observers only if fetch was succesful
        iContactFieldsArray.AppendL( iContactField );
        const TInt count = iObservers.Count();
        CCA_DP( KCCAppUtilLogFile, CCA_L("::HandleContactDataFieldUpdateL - amount of observers: %d"), count );

        MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
        param.iType = MCCAppContactFieldDataObserver::TParameter::EContactDataFieldAvailable;
        param.iContactField = iContactField;
        param.iStatusFlag = ( moreToFetch ? 0 : CCCAppCmsContactFetcherWrapper::EOperationsCompleted );

        for ( TInt i = 0; i < count; i++ )
            {
            TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverNotifyL( param ));
            }
        }
    else
        {// ..otherwise cleanup
        delete iContactField;
        iContactField = NULL;
        // Notify error, but do not leave
        // since there might be other fetchable
        // fields..
        TRAP_IGNORE( NotifyErrorL() );
        }

    // more fields to fetch or all already fetched?
    if ( moreToFetch )
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::HandleContactDataFieldUpdateL() continue, fetch next field"));
        iContactDataFetchingIndex++;
        FetchContactDataFieldL();
        }
    else
        {
        CCA_DP(KCCAppUtilLogFile, CCA_L("::HandleContactDataFieldUpdateL() complete, all fields fetched"));
        iContactDataFetchingIndex = KErrNotFound;
        iHandlerState = EOperationsCompleted;
        iCmsContactDataFetcher.OrderNotificationsL(this, CCmsContactFieldItem::ECmsPresenceAllNotification);
        }        
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppCmsContactFetcherWrapper::HandleContactDataFieldUpdateL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::ContactFieldDataArray
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CCmsContactField>& CCCAppCmsContactFetcherWrapper::ContactFieldDataArray()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppCmsContactFetcherWrapper::ContactFieldDataArray()"));    
    return iContactFieldsArray;
    }
    
// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::ContactInfo
// ---------------------------------------------------------------------------
//
EXPORT_C const CCmsContactFieldInfo* CCCAppCmsContactFetcherWrapper::ContactInfo()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppCmsContactFetcherWrapper::ContactInfo()"));    
    
    if ( ELoadingEnabledFields == iHandlerState || EInitial  == iHandlerState ) 
        return NULL;
    else
        return iContactInfo;
    }

// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::ContactIdentifierLC
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CCCAppCmsContactFetcherWrapper::ContactIdentifierLC()
    {
    HBufC8* buf8 = iCmsContactDataFetcher.GetContactIdentifierL();
    CleanupStack::PushL( buf8 );
    return buf8;
    }

// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::HandlePresenceNotificationL
// ---------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::HandlePresenceNotificationL( CCmsContactField* aField )
    {
    CleanupStack::PushL(aField);
    MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
    param.iType = MCCAppContactFieldDataObserver::TParameter::EContactPresenceChanged;
    param.iContactField = aField;
    const TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverNotifyL( param ));
        }
    if (iContactFieldsArray.Find(aField) == KErrNotFound)
        {
        iContactFieldsArray.Append(aField);
        }
    CleanupStack::Pop(aField);
    }

// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::HandlePhonebookNotificationL
// ---------------------------------------------------------------------------
//
void CCCAppCmsContactFetcherWrapper::HandlePhonebookNotificationL( TCmsPhonebookEvent aNotificationType )
    {
    if (aNotificationType == ECmsContactModified)
        {
        MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
        param.iType = MCCAppContactFieldDataObserver::TParameter::EContactsChanged;
        const TInt count = iObservers.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverNotifyL( param ));
            }
        Cancel();
        iCmsContactDataFetcher.CancelNotifications(CCmsContactFieldItem::ECmsPresenceAllNotification);
        iContactFieldsArray.ResetAndDestroy();
        delete iContactInfo;
        iContactInfo = NULL;
        StartAsyncFetchingL();
        }
    else if (aNotificationType == ECmsContactDeleted)
        {
        MCCAppContactFieldDataObserver::TParameter param = MCCAppContactFieldDataObserver::TParameter();
        param.iType = MCCAppContactFieldDataObserver::TParameter::EContactDeleted;
        const TInt count = iObservers.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TRAP_IGNORE( iObservers[ i ]->ContactFieldDataObserverNotifyL( param ));
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper::CmsNotificationTerminatedL
// ---------------------------------------------------------------------------
// 
void CCCAppCmsContactFetcherWrapper::CmsNotificationTerminatedL( TInt /*aStatus*/, 
    CCmsContactFieldItem::TCmsContactNotification /*aNotificationType*/ )
    {
    
    }

void CCCAppCmsContactFetcherWrapper::RefetchContactL()
    {
    Cancel();
    iContactFieldsArray.ResetAndDestroy();
    if (iContactInfo)
        {
        delete iContactInfo;
        iContactInfo = NULL;
        }
    iCmsContactDataFetcher.Close();
    iWrapperParam = EFindContactFromOtherStores;
        
    StartFetcherL();
    }

// End of file
