/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The current extension does not provide a view.
*
*/


// INCLUDE FILES
#include <eikmenup.h>

#include <CMessageData.h>
#include <sendui.h> 
#include <SendUiConsts.h>
#include <TSendingCapabilities.h>
#include <CSendingServiceInfo.h>

#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include <logs.rsg>

#include "MLogsEventGetter.h"
#include "MLogsEvent.h"
#include "MLogsEventData.h"
#include "Logs.hrh"

#include "clogsviewextension.h"
#include "simpledebug.h"

const TInt KArrayGranularity = 5; 
const TInt KServiceIdNotAvailable = 0;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsViewExtension* CLogsViewExtension::NewL()
    {
    _LOG("CLogsViewExtension::NewL() - begin" )
    CLogsViewExtension* self = CLogsViewExtension::NewLC();
    CleanupStack::Pop( self );
    _LOG("CLogsViewExtension::NewL() - end" )
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsViewExtension* CLogsViewExtension::NewLC()
    {
    _LOG("CLogsViewExtension::NewLC() - begin" )
    CLogsViewExtension* self = new( ELeave ) CLogsViewExtension;
    CleanupStack::PushL( self );
    self->ConstructL();
    _LOG("CLogsViewExtension::NewLC() - end" )
    return self;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CLogsViewExtension::ConstructL()
    {    
    _LOG("CLogsViewExtension::ConstructL() - begin" )         
    iServiceList = 
        new (ELeave) RPointerArray<CSendingServiceInfo>( KArrayGranularity );
    iServiceProvidersToDim = 
        new (ELeave) CArrayFixFlat<TUid>( KArrayGranularity );    
    
    //SP settings
    iSettings = CSPSettings::NewL();
    //SP setting's entry
    iEntry = CSPEntry::NewL();

    _LOG("CLogsViewExtension::ConstructL() - end" )
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsViewExtension::CLogsViewExtension() : 
    iSettings( NULL ),
    iEntry( NULL ),
    iSendUi( NULL ),
    iSendUiText( KNullDesC() ),
    iServiceId( 0 ),
    iContactLink( KNullDesC8() )
    {
    _LOG("CLogsViewExtension::CLogsViewExtension() - begin" )
    _LOG("CLogsViewExtension::CLogsViewExtension() - end" )
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsViewExtension::~CLogsViewExtension()
    {
    _LOG("CLogsViewExtension::~CLogsViewExtension() - begin" )   
    
    if ( iServiceProvidersToDim->Count() )
        {
        iServiceProvidersToDim->Reset();
        }
    delete iServiceProvidersToDim;
            
    if ( iServiceList->Count() )
        {
        iServiceList->Close();    
        }    
    delete iServiceList;
        
    delete iEntry;    
    delete iSettings;
    _LOG("CLogsViewExtension::~CLogsViewExtension() - end" )
    }


// ---------------------------------------------------------------------------
// Releases this object and any resources it owns.
// ---------------------------------------------------------------------------
//
void CLogsViewExtension::DoRelease()
    {
    _LOG("CLogsViewExtension::DoRelease() - begin" )
    delete this;   
    _LOG("CLogsViewExtension::DoRelease() - end" )
    }


// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//        
void CLogsViewExtension::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane,
        const MLogsEventGetter* aEvent)
    {    
    _LOG("CLogsViewExtension::DynInitMenuPaneL() - begin")        
    _LOGP("CLogsViewExtension::aResourceId =0x%x",  aResourceId )
    
    if ( aEvent && aMenuPane && iSendUi )
        {                
        if ( R_COMMON_SEND_MESSAGE_SUBMENU == aResourceId &&
            aEvent->Event()->LogsEventData() )
            {                                            
            TPtrC8 tempPtr( KNullDesC8 );
            // get service id

            // PC-lint errors: #1013, #1055 (in lines: 180,182)
            // Explanation:error message occurs because the 'DataField'function
            // is flagged by __VOIP and RD_VOIP_REL_2_2 flags; as long as the 
            // entire logsserviceextension is flagged by __VOIP and 
            // RD_VOIP_REL_2_2 as well this does not cause any problems

            TUint32 serviceId =  aEvent->Event()->LogsEventData()->ServiceId();
            //get contact link
            aEvent->Event()->LogsEventData()->GetContactLink(
                tempPtr );
            //save contact link for later sending                    
            SetContactlink( tempPtr );
            //save service id for later sending
            SetServiceId( serviceId );
            //Validate
            if ( iServiceId && iContactLink.Length() )
                {
                _LOG("CLogsViewExtension::valid contact link & service id")
                TUid provideruid( KNullUid );
                //get sendui service provider uid from SP settings
                TBool providerExists = GetServicePluginUidL( iServiceId, 
                                                             provideruid );
                if ( providerExists )
                    {
                    //check if this provider is in availble sendui services
                    TRAPD( error, VerifyServiceProviderL( provideruid ,
                        providerExists ););
                    if ( KErrNone != error )
                        {
                        providerExists = EFalse;
                        }
                    }
                if ( providerExists )
                    {                            
                    _LOG("CLogsViewExtension::providerExists")
                    SetProviderUid( provideruid );                            
                    // Check if send menu item already there
                    TInt position( KErrNotFound );
                    if ( !aMenuPane->MenuItemExists( ELogsCmdMenuSendUi, position ) )
                        {
                        _LOG("CLogsViewExtension::Create send menu item")
                        position = 0;  
                        TSendingCapabilities capabilities( 0, 0, 0 );
                        TRAPD( error, iSendUi->AddSendMenuItemL( 
                                                    *aMenuPane, 
                                                    position, 
                                                    ELogsCmdMenuSendUi, 
                                                    capabilities ); );
                        
                        _LOGP("CLogsViewExtension::iSendUi->AddSendMenuItemL= %d",
                             error )
                             
                        if ( KErrNone == error )
                            {
                            //all ok, sent menu item text
                            aMenuPane->SetItemTextL( ELogsCmdMenuSendUi,
                                iSendUiText );
                            aMenuPane->SetItemSpecific(
                                    ELogsCmdMenuSendUi, ETrue );
                            _LOG("CLogsViewExtension::Create send menu item done")
                            }
                        }                                                        
                    }
                }                         
            _LOG("CLogsViewExtension::done")                
            }
        }            
    _LOG("CLogsViewExtension::DynInitMenuPaneL() - end-----------" )    
    }


// ---------------------------------------------------------------------------
// HandleCommandL
// ---------------------------------------------------------------------------
// 
TBool CLogsViewExtension::HandleCommandL( TInt aCommandId )
    {
    _LOG("CLogsViewExtension::HandleCommandL() - begin" )
    _LOGP("CLogsViewExtension::aCommandId =%x",  aCommandId)
    _LOGP("provider UID =0x%x", iServiceProviderUid.iUid)
    _LOGP("iServiceId =%d", iServiceId)
    _LOGP("iContactLink.Length() =%d", iContactLink.Length() )
    TBool handled( EFalse );
    if ( ELogsCmdMenuSendUi == aCommandId &&
        iServiceId &&
        iContactLink.Length() &&        
        KNullUid != iServiceProviderUid )    
            {        
            _LOG("CLogsViewExtension::SendMessageL()" )
            // start handling command            
            TRAP_IGNORE( handled = SendMessageL(); );
            
            //reset all 
            iServiceId = 0;
            iContactLink.Set( KNullDesC8() );
            iServiceProviderUid = KNullUid;
            iServiceList->Reset();
            iServiceProvidersToDim->Reset();            
            }
        
    _LOG("CLogsViewExtension::HandleCommandL() - end" )
    return handled;
    }

// ---------------------------------------------------------------------------
// VerifyServiceL
// ---------------------------------------------------------------------------
// 
void CLogsViewExtension::VerifyServiceProviderL( 
        const TUid& aServiceProviderUid ,
        TBool& aServiceProviderExists )
    {
     _LOG("CLogsViewExtension::VerifyServiceProviderL() - begin" )     
    aServiceProviderExists = EFalse;

    iServiceList->Reset();    
    iServiceProvidersToDim->Reset();
    if ( !iSendUi )
        {
        User::Leave( KErrNotFound );
        }

    // check if WLM Send IM plugin UID exists in service table
    // check if WLM Send IM plugin's service is found in available services

    //get available services
    iSendUi->AvailableServicesL( *iServiceList, KNullUid );
    _LOGP("iServiceList->Count() =%d", iServiceList->Count() )
    
    if ( iServiceList && iServiceList->Count() ) 
        {
        
        TUid serviceUid( KNullUid );       
        TUid serviceProviderUid( KNullUid );       
        
        for ( TInt i=0; i < iServiceList->Count(); i++ )
            {
            _LOGPP("service number =%d of %d",  
                ( i + 1 ), 
                iServiceList->Count() )
            
            serviceUid = (*iServiceList)[i]->ServiceId();        
            serviceProviderUid = (*iServiceList)[i]->ServiceProviderId();
            
            _LOGP("service UID =0x%x",  serviceUid.iUid)
            _LOGP("provider UID =0x%x", serviceProviderUid.iUid)
            _LOGDES( (*iServiceList)[i]->ServiceName() );
            _LOGDES( (*iServiceList)[i]->ServiceMenuName() );
            
            if ( aServiceProviderUid == serviceProviderUid )
                {
                _LOG("WLM SendUi provider exists")
                aServiceProviderExists = ETrue;
                }
            else                
                {
                _LOG("Append service to dimm list")
                iServiceProvidersToDim->AppendL( serviceProviderUid );            
                }            
            }
        
        }            
    
    _LOG("CLogsViewExtension::VerifyServiceProviderL() - end" )
    }

// ---------------------------------------------------------------------------
// GetServicePluginUidL
// ---------------------------------------------------------------------------
// 
TBool CLogsViewExtension::GetServicePluginUidL( TInt aServiceId, 
                                                TUid& aServiceProviderUid )
    {
    _LOG("CLogsViewExtension::GetServicePluginUidL() - begin" )
    
    aServiceProviderUid = KNullUid;        
    TBool serviceFound( EFalse );
    
    // we have to reset the entry to ensure that we dont accidently reuse
    // properties of formerly fetched entries
    iEntry->Reset();
    
    if ( iSettings && iEntry )
        {
        // find entry using service id
        TInt error( KErrNotFound );
        TRAP( error, error = iSettings->FindEntryL( aServiceId , *iEntry ) );
        if ( KErrNone == error )
            {
            _LOG("CLogsViewExtension::iSettings->FindEntryL" )
            TServicePropertyName propertyName( EPropertySendFwImPluginId );        
            const CSPProperty* property = NULL;
            error = iEntry->GetProperty( property, propertyName );
            if ( KErrNone == error && property )
                {            
                _LOG("CLogsViewExtension::entry->GetProperty" )
                TInt value( KErrNone );
                
                if ( KErrNone == property->GetValue( value ) )
                    {
                    _LOGP("CLogsViewExtension::property->GetValue =0x%x",
                        value )            
                    //save it
                    aServiceProviderUid.iUid = value;
                    serviceFound = ETrue;            
                    _LOGP("CLogsViewExtension::aServiceProviderUid.iUid =0x%x",
                        aServiceProviderUid.iUid)                             
                    }
                }                    
            }            
        }   
        
    _LOG("CLogsViewExtension::GetServicePluginUidL() - end" )        
    return serviceFound;
    }

// ---------------------------------------------------------------------------
// SetSendUi
// ---------------------------------------------------------------------------
// 
void CLogsViewExtension::SetSendUi( CSendUi& aSendUi )
    {
     _LOG("CLogsViewExtension::SetSendUi() - begin" )
    iSendUi = &aSendUi;
    _LOG("CLogsViewExtension::SetSendUi() - end" )
    }

    
// ---------------------------------------------------------------------------
// SetSendUiText
// ---------------------------------------------------------------------------
// 
void CLogsViewExtension::SetSendUiText( const TDesC& aSendUiText )
    {
    _LOG("CLogsViewExtension::SetSendUiText() - begin" )    
    iSendUiText.Set(aSendUiText );        
    _LOG("CLogsViewExtension::SetSendUiText() - end" )
    }

// ---------------------------------------------------------------------------
// SetContactlink
// ---------------------------------------------------------------------------
// 
void CLogsViewExtension::SetContactlink( const TDesC8& aContactLink )
    {
    _LOG("CLogsViewExtension::SetContactlink() - begin" )
    iContactLink.Set( KNullDesC8 );
    if ( aContactLink.Length() )
        {
        iContactLink.Set( aContactLink );    
        }    
    _LOG("CLogsViewExtension::SetContactlink() - end" )    
    }
    
// ---------------------------------------------------------------------------
// SetServiceId
// ---------------------------------------------------------------------------
// 
void CLogsViewExtension::SetServiceId( const TUint32& aServiceId )
    {
    _LOG("CLogsViewExtension::SetServiceId() - begin" )    
    iServiceId = KServiceIdNotAvailable;
    if ( aServiceId )
        {
        iServiceId = aServiceId;    
        }
    _LOG("CLogsViewExtension::SetServiceId() - end" )    
    }

// ---------------------------------------------------------------------------
// SetProviderUid
// ---------------------------------------------------------------------------
//     
void CLogsViewExtension::SetProviderUid( const TUid& aUid )
    {
    _LOG("CLogsViewExtension::SetProviderUid() - begin" )    
    iServiceProviderUid = aUid;
    _LOG("CLogsViewExtension::SetProviderUid() - end" )    
    }
    
// ----------------------------------------------------------------------------
// SendMessageL
// ----------------------------------------------------------------------------
//
TBool CLogsViewExtension::SendMessageL()
    {
    _LOG("CLogsViewExtension::SendMessageL - begin" )
    TBool commandHandled( ETrue );
    TInt error( KErrNone );
    TSendingCapabilities capabilities( 0, 0, 0 );
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );    
    _LOG("CLogsViewExtension::SendMessageL iSendUi->ShowTypedQueryL")             
    TUid selectedServiceUid( KNullUid );
    selectedServiceUid = iSendUi->ShowTypedQueryL( CSendUi::ESendMenu,
        NULL, capabilities, iServiceProvidersToDim );

    _LOGP("CLogsViewExtension::Selected service UID =0x%x",  
        selectedServiceUid.iUid )
    
    //check if user pressed cancel 
    if ( KNullUid != selectedServiceUid )     
        {      
        //set opaque data
        TUid dataType;
        dataType.iUid = iServiceId;
        TRAP( error, messageData->SetOpaqueDataL( &iContactLink , dataType ) );
        _LOGP("messageData->SetOpaqueDataL= %d", error)
        if ( KErrNone == error )
            {
            _LOG("CLogsViewExtension::SendMessageL...")
            _LOG("...iSendUi->CreateAndSendMessageL")
            iSendUi->CreateAndSendMessageL( selectedServiceUid,
                messageData, KNullUid, EFalse );
            _LOG("CLogsViewExtension::iSendUi->CreateAndSendMessageL")   
            }
        }
        
    CleanupStack::PopAndDestroy( messageData );
    messageData = NULL;

    _LOG("CLogsViewExtension::SendMessageL - end" ) 

    return commandHandled;
    }

// End of File
