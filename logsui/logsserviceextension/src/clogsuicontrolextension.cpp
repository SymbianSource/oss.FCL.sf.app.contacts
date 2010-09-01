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
* Description:  Provides additional functionality to the Logs UI application
*
*/



// INCLUDE FILES
#include <gulicon.h>
#include <aknnotewrappers.h> 
#include <aknlists.h>

#include "clogsuicontrolextension.h"

#include "clogspresencetablemanager.h"
#include "clogseventinfomanager.h"
#include "clogsextbrandfetcher.h"
#include "clogsextpresentityidfetcher.h"
#include "tlogseventinfo.h"
#include "clogsiconfileprovider.h"
#include "clogsextservicehandler.h"

#include "LogsConstants.hrh"    // for KLogsBuff128
#include "LogsConsts.h"         // for KMaxNbrOfRecentTrailIcons, KTab                        
#include "logsextconsts.h"
#include "tlogsextutil.h"

#include "simpledebug.h"

// ================= MEMBER FUNCTIONS =======================



// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsUiControlExtension* CLogsUiControlExtension::NewL()
    {
    _LOG("CLogsUiControlExtension::NewL()" )
    CLogsUiControlExtension* self = CLogsUiControlExtension::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsUiControlExtension* CLogsUiControlExtension::NewLC()
    {
    _LOG("CLogsUiControlExtension::NewLC()" )
    CLogsUiControlExtension* self = new( ELeave ) CLogsUiControlExtension;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::ConstructL()
    {
    _LOG("CLogsUiControlExtension::ConstructL() begin" )
    iLogEntryInfoManager = CLogsEventInfoManager::NewL();
    iPresenceTableManager = CLogsPresenceTableManager::NewL();
    _LOG("CLogsUiControlExtension::ConstructL() end" )
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsUiControlExtension::CLogsUiControlExtension():    
    iListBox( NULL ),
    iLogsExtObserver( NULL ),
    iBrandFetcherArray( KBrandFetcherArrayGranularity ),    
    iPresentityIdFetcherArray( KPresentityIdFetcherArrayGranularity ),
    iServiceHandlerArray( KPresentityIdFetcherArrayGranularity ),
    iBitmap( NULL ),
    iBitmask( NULL )    
    {
    _LOG("CLogsUiControlExtension::CLogsUiControlExtension()" )
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsUiControlExtension::~CLogsUiControlExtension()
    {
    _LOG("CLogsUiControlExtension::~CLogsUiControlExtension() begin" )
    delete iLogEntryInfoManager;        
    delete iPresenceTableManager;
    iBrandFetcherArray.ResetAndDestroy();           
    iPresentityIdFetcherArray.ResetAndDestroy();
        
    delete iBitmap;
    delete iBitmask;
    
    _LOG("CLogsUiControlExtension::~CLogsUiControlExtension()..." )
    _LOG("...iServiceHandlerArray.ResetAndDestroy")
    iServiceHandlerArray.ResetAndDestroy();
    
    _LOG("CLogsUiControlExtension::~CLogsUiControlExtension() end" )
    }
    
    
// ---------------------------------------------------------------------------
// Delete member data.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::DoRelease()
    {
    _LOG("CLogsUiControlExtension::DoRelease() begin" )    
    delete this;
    _LOG("CLogsUiControlExtension::DoRelease() end" )
    }

// ---------------------------------------------------------------------------
// functionality not supported by this dll - function does nothing
// ---------------------------------------------------------------------------
//    
void CLogsUiControlExtension::AppendIconsL( CAknIconArray* /*aArray*/ )
    {
    _LOG("CLogsUiControlExtension::AppendIconsL begin" )
    // Reset brand fetcher array. Brand fetchers and icons are reconstructed
    // again when when HandleAdditionalData() gets called. We cannot load
    // icons here because we don't know yet which icons to load.
    iBrandFetcherArray.ResetAndDestroy();
    iPresenceTableManager->ResetState();
    iServiceHandlerArray.ResetAndDestroy();
    _LOG("CLogsUiControlExtension::AppendIconsL end" )
    }

// ---------------------------------------------------------------------------
// functionality not supported by this dll - function does nothing
// ---------------------------------------------------------------------------
//
TBool CLogsUiControlExtension::GetIconIndexL( TContactItemId /*aContactId*/,
                                              TInt& /*aIndex*/ )
    {
    _LOG("CLogsUiControlExtension::GetIconIndexL begin" )
    _LOG("CLogsUiControlExtension::GetIconIndexL end" )
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// Sets the observer of the extension (probably a CLogsRecentListView object)
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::SetObserver( MLogsExtObserver& aObserver )
    {
    _LOG("CLogsUiControlExtension::SetObserver begin" )
    iLogsExtObserver = &aObserver;
    _LOG("CLogsUiControlExtension::SetObserver end" )
    }


// ---------------------------------------------------------------------------
// Starts the additional data handling.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::HandleAdditionalData( 
    MLogsModel& aModel, 
    CAknDoubleGraphicStyleListBox& aListBox )
    {
    _LOG("CLogsUiControlExtension::HandleAdditionalDataL begin" )
    iListBox = &aListBox;    
        
    ResetPresentityIdFetcherArray();    
    
    TRAP_IGNORE( DoAdditionalDataHandlingL( aModel ) );    
    
    _LOG("CLogsUiControlExtension::HandleAdditionalDataL end" )        
    }
    
// ---------------------------------------------------------------------------
// Creates and starts all the icon fetching processes.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::DoAdditionalDataHandlingL( MLogsModel& aModel )
    {
    _LOG("CLogsUiControlExtension::DoAdditionalDataHandlingL begin" )
    
    CreateAndStartBrandFetchersL( aModel );    
    CreateAndStartPresentityIdFetchersL( aModel );           
    
    _LOG("CLogsUiControlExtension::DoAdditionalDataHandlingL end" )        
    }    

// ---------------------------------------------------------------------------
// Creates a service handler for a certain service if it not already 
// exists and issues the binding request.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::CreateAndBindServiceHandlerL( TUint32 aServiceId )
    {
    _LOG("CLogsUiControlExtension::CreateAndBindServiceHandlerL: begin" )
        
    if ( aServiceId && !ServiceHandlerExists( aServiceId ) )
        {            
        // create and bind the service handler
        _LOG("CLogsUiControlExtension::CreateAndBindServiceHandlerL:..." )
        _LOG("... !ServiceHandlerExists")
        
        
        CLogsExtServiceHandler* serviceHandler = 
            CLogsExtServiceHandler::NewLC( aServiceId, *this );                
        
        _LOG("CLogsUiControlExtension::CreateAndBindServiceHandlerL:..." )
        _LOG("...iServiceHandlerArray.AppendL( serviceHandler )")
        
        iServiceHandlerArray.AppendL( serviceHandler );
        
        CleanupStack::Pop( serviceHandler );        
        }
    
    _LOG("CLogsUiControlExtension::CreateAndBindServiceHandlerL: end" )
    }

// ---------------------------------------------------------------------------
// Creates and starts all the brand icon fetching processes.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::CreateAndStartBrandFetchersL( 
        MLogsModel& aModel )
    {
            
    for ( TInt i = 0 ; i < aModel.Count() ; i++ )
        {        
        // Here we go through the list of events an check whether the events
        // contain a service id or not.Then a brand fetcher is created for 
        // each service id.
        
        // PC-lint errors: #1013, #1055, #64
        // Explanation: error message occurs because the 'DataField' function
        // is flagged by __VOIP and RD_VOIP_REL_2_2 flags; as long as the 
        // entire logsserviceextension is flagged by __VOIP and 
        // RD_VOIP_REL_2_2 as well this does not cause any problems
                
        // get the serviceId
        TUint32 serviceId ( aModel.At(i)->LogsEventData()->ServiceId() );
        
        if ( serviceId && !BrandFetcherExists( serviceId ) )
            {
            // the manager adds a eventInfo object only if it does not
            // contain an eventinfo object with the given service id
            // --> this acts as a duplicate-elimination
            TRAP_IGNORE( iLogEntryInfoManager->AddEventEntryL( serviceId ) );
            
            // create and start the fetcher
            CLogsExtBrandFetcher* brandFetcher = 
                CLogsExtBrandFetcher::NewLC( serviceId, *this );
                
            iBrandFetcherArray.AppendL( brandFetcher );
            CleanupStack::Pop( brandFetcher );
                
            brandFetcher->Fetch();            
            }        
        }    
    }


// ---------------------------------------------------------------------------
// Creates and starts all the presence icon fetching processes.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::CreateAndStartPresentityIdFetchersL( 
        MLogsModel& aModel )
    {    
    _LOG("CLogsUiControlExtension::CreateAndStartPresentityIdFetchersL begin " ) 
    // go through all the log events and create the status entries 
    // (iPresenceTableManager takes care for duplication elimination and returns
    //  KErrAlreadyExists if one tries to add an entry that already exists)
    // create one presence fetcher for each status table entry and start them.
    // note: entries to the index table will only be added after the callback 
    // from the presencefw
    
    // the presence table manager decides which mappings are not needed
    // anymore by comparing the log ids of the current view to the 
    // log ids in the array of mappings; mappings which contain a log id
    // that cannot be found from 'aModel' will be deleted.
    iPresenceTableManager->RemoveUnneededMappings( aModel );
      
    for ( TInt i = 0 ; i < aModel.Count() ; i++ )
        {

        // PC-lint errors: #1013, #1055
        // Explanation: error message occurs because the 'DataField' function
        // is flagged by __VOIP and RD_VOIP_REL_2_2 flags; as long as the 
        // entire logsserviceextension is flagged by __VOIP and RD_VOIP_REL_2_2
        // as well this does not cause any problems
        
        TUint32 serviceId( aModel.At(i)->LogsEventData()->ServiceId() );
        TPtrC8 contactLink;
        TBool contactLinkRetrievalSuccess( EFalse );
        if ( KErrNone == aModel.At(i)->
                            LogsEventData()->GetContactLink( contactLink ) )
            {
            contactLinkRetrievalSuccess = ETrue;
            }

        if ( serviceId && contactLinkRetrievalSuccess )
            {
            //create the presentity id fetcher
            DoPresentityIdFetcherCreationL( serviceId, 
                                            contactLink, 
                                            aModel.At(i)->LogId() );
            }
        
        }        
    _LOG("CLogsUiControlExtension::CreateAndStartPresentityIdFetchersL end " )
    }

// ---------------------------------------------------------------------------
// Modifies the given icon string.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::ModifyIconString ( 
        TDes& aDes, 
        const MLogsEventGetter& aLogsEventGetter )
    {
    _LOGP("CLogsUiControlExtension::ModifyIconString begin originalstring=%S",
        &aDes)    
    
    // only make changes to log entries that contain a service id and
    // dont start looking for fetching results before the fetching
    // process has been started. (Fetching is initiated through 
    // HandleAdditionalDataL function call)
    
    TInt result( KErrNone );
    TRAP( result, DoIconStringModificationL( aDes, aLogsEventGetter ) );
        
    _LOGP("CLogsUiControlExtension::ModifyIconString resulting string=%S",
    	&aDes )
    
    _LOG("CLogsUiControlExtension::ModifyIconString end" )
    }   
    
// ---------------------------------------------------------------------------
// Makes the actual modifications to the string.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::DoIconStringModificationL(  
    TDes& aDes, 
    const MLogsEventGetter& aLogsEventGetter )
    {   
    _LOG("CLogsUiControlExtension::DoIconStringModificationL begin" )
    
    // use the parser to extract e.g. the service id from the data field of a
    // log event. the information should be tagged like this -> "SI=5", 
    // whereas "SI" is the identifier for a service id and "5" is its value

    // PC-lint errors: #1013
    // Explanation: error message occurs because the 'DataField' function
    // is flagged by __VOIP and RD_VOIP_REL_2_2 flags; as long as the 
    // entire logsserviceextension is flagged by __VOIP and RD_VOIP_REL_2_2
    // as well this does not cause any problems       
    
    TUint32 serviceId( aLogsEventGetter.LogsEventData()->ServiceId() );
    if ( serviceId <= 0 )
        {
        // without service id we cannont proceed any further...
        User::Leave( KErrNotFound );
        }
    
    TBool brandIconIndexAvailable( 
        iLogEntryInfoManager->BrandIconIndexAvailable( serviceId ) );
    
    TBool presenceIconIndexAvailable( EFalse );

    const TDesC* presentityId = 
        presentityId = LookupPresentityId( aLogsEventGetter.LogId() ); 
    //don't leave here: if the presentity id does not exist it means that
    //we cannot check the availability of that presentity from the 
    //presencetablemanager - but we can still try to set the 
    //brandicon index.
    
    if ( presentityId )
        {        
        _LOG("CLogsUiControlExtension::DoIconStringModificationL ...")
        _LOGP("...presentityId=%S", presentityId )
                
        if ( iPresenceTableManager )
            {
            _LOG("CLogsUiControlExtension::DoIconStringModificationL..." )
            _LOG("...iPresenceTableManager available")
            presenceIconIndexAvailable = 
                iPresenceTableManager->PresenceIconIndexAvailable( 
                    serviceId, *presentityId );                
            }
        }    
    
    // only create the HBufC if either brand- or presence index is available
    if ( brandIconIndexAvailable || presenceIconIndexAvailable )
        {
        // e.g. aDes = '0\ttext1\ttext2' whereas '\t' is the delimiter
        //create an empty local buffer of the same size as aDes
        HBufC* temp = HBufC::NewLC( KLogsBuff128 );
        TPtr tempDes( temp->Des() );
        
        //create the new icon string:
        //if no error occurred while creating the local string data->
        //update aDes with the modified version of the icon string
        //otherwise the original data will not be modified
        
        if ( brandIconIndexAvailable 
            && AddBrandIconIndex( tempDes, serviceId ) 
            && AddNonIndexData( aDes, tempDes ) )
            {
            aDes.Copy( *temp );
            }
        
        // make a copy of the original string (that might contain  
        // the modified brandicon index already)
        tempDes.Copy( aDes );            
        
        if ( presenceIconIndexAvailable 
            && AddPresenceIconIndex( tempDes, serviceId, *presentityId ) )
            {
            aDes.Copy( *temp );
            }
        
        //cleanup the local string buffer
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;        
        }
            
    _LOG("CLogsUiControlExtension::DoIconStringModificationL end" )           
    }


// ---------------------------------------------------------------------------
// Adds the branding icon index.
// ---------------------------------------------------------------------------
//
TBool CLogsUiControlExtension::AddBrandIconIndex( TDes& aTempDes, 
                                                  const TUint32 aServiceId )
    {
    _LOG("CLogsUiControlExtension::AddBrandIconIndex begin" )   
    TBool result( EFalse );        
    
    TInt brandIndex( KErrNotFound );
    // Check if the brand icon for the specified service id can be found
    if ( iLogEntryInfoManager->GetBrandIconIndex( aServiceId, brandIndex ) )
        {
        //Add the brand icon index to the temp string
        aTempDes.Num( brandIndex );
        _LOGP("CLogsUiControlExtension::AddBrandIconIndex BrandIndex=%d",
            brandIndex )
        // e.g. aTempDes = '7'
        result = ETrue;
        }        
    
    _LOG("CLogsUiControlExtension::AddBrandIconIndex end" )  
    return result;
    }


// ---------------------------------------------------------------------------
// Copies that part of the global icon string to the local copy which contains
// the non-icon index related data.
// ---------------------------------------------------------------------------
//
TBool CLogsUiControlExtension::AddNonIndexData( 
        const TDesC& aDes, 
        TDes& aTempDes )
    {
    _LOG("CLogsUiControlExtension::AddNonIndexData begin" )  
    TBool result( EFalse );
    
    // find the first occurrence of the delimiter in the original
    // icon string
    TInt Position( aDes.Find( KTab ) );
    if ( KErrNotFound != Position )
        {
        // position has been found:
        // now copy 
        aTempDes.Append( aDes.Mid( Position ) );
        // e.g. aTempDes = '7\ttext1\ttext2'
        result = ETrue;
        }        
    
    _LOG("CLogsUiControlExtension::AddNonIndexData end" )  
    return result;    
    }


// ---------------------------------------------------------------------------
// Appends the trailing delimiters (if they do not already exist) and the 
// presence icon index.
// ---------------------------------------------------------------------------
//
TBool CLogsUiControlExtension::AddPresenceIconIndex( 
        TDes& aTempDes, 
        const TUint32 aServiceId,
        const TDesC16& aPresentityId )
    {
    _LOGP("CLogsUiControlExtension::AddPresenceIconIndex begin %d" , 
        aServiceId)     
    TBool result( EFalse );
    
    /// determine the number of trailing delimiters and add missing ones
    //  e.g. Given a) aTempDes = '7\ttext1\ttext2' and 
    //             b) KMaxNbrOfRecentTrailIcons == 2 
    //       --> Add two empty trailing icon indices
    //       Result: aTempDes = '7\ttext1\ttext2\t\t'
    
    // determine the number of delimiters
    TInt numberDelimiters = DetermineNumberDelimiters( aTempDes );
    
    if ( numberDelimiters >= KNumberDelimitersUnmodifiedIconStringMin && 
         numberDelimiters <= KNumberDelimitersUnmodifiedIconStringMax )
        {
        //add the trailing delimiters to the end of the string
        TInt returnValue( 
            AppendTrailingDelimiters( 
                aTempDes, 
                KNumberDelimitersModifiedIconString - numberDelimiters ) );
        result = ( KErrNone == returnValue ) ? ETrue : EFalse;
        }
    else
        {
        result = EFalse;        
        }
    
    //if adding the 
    if ( result ) 
        {
        // finally append the presence icon index
        TInt presenceIndex( KErrNotFound );
        /// Check, if the presence icon could be retrieved
        if( KErrNone == iPresenceTableManager->GetIndex( aServiceId, 
                                                         aPresentityId, 
                                                         presenceIndex ) )
            {
            // Add the presence icon index to the temp string            
            aTempDes.AppendNum( presenceIndex );       
            // e.g. aTempDes = '7\ttext1\ttext2\t\t\t8'
            _LOG("CLogsUiControlExtension::AddPresenceIconIndex ...")
            _LOGP("...PresenceIndex=%d", presenceIndex )
            result = ETrue;
            }
        else
            {
            result = EFalse;
            }
        }
    _LOG("CLogsUiControlExtension::AddPresenceIconIndex end" )     
    return result;    
    }

// ---------------------------------------------------------------------------
// Appends 'aNumber' delimiters to the end of aTempDes.
// ---------------------------------------------------------------------------
//
TInt CLogsUiControlExtension::AppendTrailingDelimiters( TDes& aTempDes, 
                                                        TInt  aNumber )
    {
    _LOG("CLogsUiControlExtension::AppendTrailingDelimiters begin" )     
    TInt error( KErrNoMemory );
    
    //make sure the size of descriptor is big enough, so that appending
    //won't panic
    TInt len( aNumber * KTab().Length() );
    if ( aTempDes.Length() + len <= aTempDes.MaxLength() )
        {
        for ( TInt i( 0 ); i < aNumber ; i++ )
            {
            aTempDes.Append( KTab );
            }
        error = KErrNone;    
        }
    else
        {
        error = KErrNoMemory;
        }

    _LOG("CLogsUiControlExtension::AppendTrailingDelimiters end" )
    return error;    
    }


// ---------------------------------------------------------------------------
// Determines the number of delimiters in the specified string
// ---------------------------------------------------------------------------
//
TInt CLogsUiControlExtension::DetermineNumberDelimiters( 
        const TDesC& aTempDes )
    {
    _LOG("CLogsUiControlExtension::DetermineNumberDelimiters begin" )     

    TInt result( 0 );    
    TPtrC buffer = aTempDes.Left( aTempDes.Length() );    
    TInt nextTokenStart( KErrNone );
    
    while ( buffer.Length() > 0 && nextTokenStart >= 0 )
        {            
        nextTokenStart = buffer.Find( KTab );
        if ( KErrNotFound != nextTokenStart )
            {
            result++;
            buffer.Set( buffer.Mid( nextTokenStart + 1 ));
            }

        }    

    _LOG("CLogsUiControlExtension::DetermineNumberDelimiters end" )     
    return result;
    }


// ---------------------------------------------------------------------------
// Callback function that is called by the fetcher objects after they have 
// successfully retrieved the bitmaps of the icon.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::BrandIconFetchDoneL( const TUint32 aServiceId,
                                                   RFile& aBitmapFile,
                                                   const TInt aBitmapId,   
                                                   const TInt aBitmapMaskId )
    {    
    _LOGP("CLogsUiControlExtension::BrandIconFetchDoneL begin %d",aServiceId )     
    
    // This is the fetcher-callback; handle the fetched data now:
    //    - take ownership of the file handle
    //    - create icon
    //    - add the data to the to the listbox's icon array (in AppIconL)
    //    - store the icon's index to the event info manager    
    
    // take ownership of file handle
    CleanupClosePushL( aBitmapFile );
    
    // File provider duplicates the file handle and close the original.
    // Duplication is needed because lifetime of image is different than
    // this plugin's file handle's.
    CLogsIconFileProvider* ifp = CLogsIconFileProvider::NewL( aBitmapFile );    
    _LOG("CLogsUiControlExtension::BrandIconFetchDoneL close bitmapfile" );
    CleanupStack::PopAndDestroy( &aBitmapFile );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmask = NULL;
    
    // create icons            
    AknIconUtils::CreateIconL ( bitmap,
                                bitmask,
                                *ifp,
                                aBitmapId, 
                                aBitmapMaskId );                                          

    // in case a new icon is added to the icon array of the listbox
    // 'indexOfAppendedIcon' will contain the index of the added icon
    // in the icon array
    TInt indexOfAppendedIcon( 0 );             
    
    // ownership of the bitmaps is transferred to AddIconL
    TRAPD( err, AddIconL( bitmap, bitmask, indexOfAppendedIcon ) );    
    bitmap = NULL;                              
    bitmask = NULL;
    
    if ( KErrNone == err && indexOfAppendedIcon >= 0 )
        {        
        // store index information into the event info manager
        err = SetBrandIndex( aServiceId, indexOfAppendedIcon );
        
        // if setting the index failed, the last icon that was added to the
        // icon array must be removed again
        if ( KErrNone != err )
            {
            CArrayPtr<CGulIcon>* iconList = 
                iListBox->ItemDrawer()->ColumnData()->IconArray();
            iconList->Delete( indexOfAppendedIcon );
            iconList->Compress();
            }        
        }
        
    if ( KErrNone == err )
        {
        NotifyObserverL(); // notify the observer (to update the view)
        }
    
    _LOG("CLogsUiControlExtension::BrandIconFetchDoneL end" )     
   
    }
    

// ---------------------------------------------------------------------------
// Add the fetched icon to the listbox' icon array
// Returns the index of the appended icon in the icon array.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::AddIconL( CFbsBitmap* aBitmap, 
                                        CFbsBitmap* aBitmask,
                                        TInt& aIndex )
    {
    _LOG("CLogsUiControlExtension::AddIconL begin" )     

    // delete bitmaps before saving new bitmaps
    DeleteBitmaps();
    
    // take ownership of the bitmaps       
    iBitmap = aBitmap;      // Ownership here
    iBitmask = aBitmask;    // Ownership here
    CArrayPtr<CGulIcon>* iconList = 
        iListBox->ItemDrawer()->ColumnData()->IconArray();
    User::LeaveIfNull( iconList );    
    iBitmap = NULL;     // ownership will be transferred to CGulIcon
    iBitmask = NULL;    // ownership will be transferred to CGulIcon
    CGulIcon* icon = CGulIcon::NewL( aBitmap, aBitmask );
    CleanupStack::PushL( icon );     
    iconList->AppendL( icon ); // iconList owns icon now
    CleanupStack::Pop( icon ); 
    
    aIndex = iconList->Count() - 1;
    _LOG("CLogsUiControlExtension::AddIconL end... ")
    _LOGP("...index of the appended icon in the icon array==%d",
        ( iconList->Count() - 1 ) )   
    }
    
// ---------------------------------------------------------------------------
// Sets the index of a brand icon.
// ---------------------------------------------------------------------------
//
TInt CLogsUiControlExtension::SetBrandIndex( const TUint32 aServiceId, 
                                             const TInt aIndex )
    {
    _LOG("CLogsUiControlExtension::SetBrandIndex" )     
    
    return iLogEntryInfoManager->SetBrandIndex( aServiceId , aIndex );
    }


// ---------------------------------------------------------------------------
// Notifies the observer that an icon was added to the listbox's icon array.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::NotifyObserverL()
    {
    _LOG("CLogsUiControlExtension::NotifyObserverL begin" )     

    iLogsExtObserver->ExtStateChangedL();

    _LOG("CLogsUiControlExtension::NotifyObserverL end" )     
    }


// ---------------------------------------------------------------------------
// Sets the index of a presence icon.
// ---------------------------------------------------------------------------
//
TInt CLogsUiControlExtension::SetPresenceIndex( const TUint32 aServiceId, 
                                                const TDesC& aPresenceStatus,
                                                const TInt aIndex )
    {
    _LOG("CLogsUiControlExtension::SetPresenceIndex" )
    
    return iPresenceTableManager->SetIndexInIndexTable( aServiceId, 
                                                        aPresenceStatus, 
                                                        aIndex );
    }


   
// ---------------------------------------------------------------------------
// Deletes the icon bitmaps.
// ---------------------------------------------------------------------------
//   
void CLogsUiControlExtension::DeleteBitmaps()
    {
    _LOG("CLogsUiControlExtension::DeleteBitmaps begin" )     
    delete iBitmap;
    delete iBitmask;
    iBitmap = NULL;
    iBitmask = NULL;
    _LOG("CLogsUiControlExtension::DeleteBitmaps end" )         
    }

// ---------------------------------------------------------------------------
// Checks if service handler exists already
// ---------------------------------------------------------------------------
//  
TBool CLogsUiControlExtension::ServiceHandlerExists( const TUint32 aServiceId )
    {
    TBool handlerFound( EFalse ); 
    _LOG("CLogsUiControlExtension::ServiceHandlerExists begin" )     
    
    // check if there is already a fetcher for the brand    
    CLogsExtServiceHandler* serviceHandler = NULL;    
    if ( KErrNone == GetServiceHandler( aServiceId, serviceHandler ) )
        {
        handlerFound = ETrue;
        }
    
#ifdef _DEBUG
    if ( handlerFound )
        {
        _LOG("CLogsUiControlExtension::ServiceHandlerExists YES" )
        }
    else
        {
        _LOG("CLogsUiControlExtension::ServiceHandlerExists NO" )
        }
#endif

    _LOG("CLogsUiControlExtension::ServiceHandlerExists end" )         
    return handlerFound;    
    }


// ---------------------------------------------------------------------------
// Checks if brandfetcher exists already
// ---------------------------------------------------------------------------
//  
TBool CLogsUiControlExtension::BrandFetcherExists( const TUint32 aServiceId )
    {
    TBool fetcherFound( EFalse ); 
    _LOG("CLogsUiControlExtension::BrandFetcherExists begin" )     
    // check if there is already a fetcher for the brand
    CLogsExtBrandFetcher* fetcher = NULL;
    
    for ( TInt i = 0; i < iBrandFetcherArray.Count() 
        && fetcherFound == EFalse; i++)
        {
        fetcher = iBrandFetcherArray[i];
        if ( aServiceId == fetcher->ServiceId() )
            {            
            fetcherFound = ETrue;    
            }
        }

#ifdef _DEBUG
    if ( fetcherFound )
        {
        _LOG("CLogsUiControlExtension::BrandFetcherExists YES" )
        }
    else
        {
        _LOG("CLogsUiControlExtension::BrandFetcherExists NO" )
        }
#endif

    _LOG("CLogsUiControlExtension::BrandFetcherExists end" )         
    return fetcherFound;    
    }
   
    
// ---------------------------------------------------------------------------
// Adds a single presentity to a service handler.
// ---------------------------------------------------------------------------
// 
void CLogsUiControlExtension::AddPresentityToServiceHandlerL( 
        const TUint32 aServiceId, 
        const TDesC& aPresentityId )
    {
    _LOG("CLogsUiControlExtension::AddPresentityToServiceHandlerL begin" )
    
    CLogsExtServiceHandler* serviceHandler = NULL;
    TInt returnValue( GetServiceHandler( aServiceId, serviceHandler ) );
        
    if ( KErrNone == returnValue && serviceHandler )
            {
            TInt error( KErrNone );
            // add status entry to status table
            error = iPresenceTableManager->AddEntryToStatusTable(
                aServiceId,
                aPresentityId,
                EFalse );

            if ( KErrNone == error )
                {
                // adds the presentity to the service handler;
                // the service handler will start to observe the 
                // status of this presentity and will inform the
                // observer in case of status changes of the 
                // presentity
                serviceHandler->AddPresentityL( aPresentityId );
                }
            }
        
    _LOG("CLogsUiControlExtension::AddPresentityToServiceHandlerL end" )
    }
 
            
// ---------------------------------------------------------------------------
// Creates and starts a single presentity id fetcher.
// ---------------------------------------------------------------------------
// 
void CLogsUiControlExtension::DoPresentityIdFetcherCreationL( 
        const TUint32 aServiceId, 
        const TDesC8& aCntLink,
        const TLogId aLogId )
    {    
    _LOG("CLogsUiControlExtension::DoPresentityIdFetcherCreationL begin" )
    
    if ( aServiceId )
        {
        CLogsExtPresentityIdFetcher* presIdFetcher = 
            CLogsExtPresentityIdFetcher::NewLC( aServiceId, 
                                                aCntLink, 
                                                aLogId, 
                                                *this );
                                                
        _LOG("CLogsUiControlExtension::DoPresentityIdFetcherCreationL: ...")
        _LOGP("...presentity id fetcher created-> serviceid==%d ",aServiceId )
        iPresentityIdFetcherArray.AppendL( presIdFetcher );
        _LOG("CLogsUiControlExtension::DoPresentityIdFetcherCreationL:...")
        _LOGP("... #presIdFetcher=%d",iPresentityIdFetcherArray.Count() )
        CleanupStack::Pop( presIdFetcher );
        // start the fetcher
        presIdFetcher->Fetch();            
        }
    
    _LOG("CLogsUiControlExtension::DoPresentityIdFetcherCreationL end" )
    }
                        


// ---------------------------------------------------------------------------
// Callback for the presentity fetchers.
// ---------------------------------------------------------------------------
//  
void CLogsUiControlExtension::PresentityIdFetchDoneL( 
        const TUint32 aServiceId, 
        const TDesC& aPresentityId,
        const TLogId aLogId )
    {
    _LOG("CLogsUiControlExtension::PresentityIdFetchDoneL begin" )
    
    AddLogIdToPresentityIdMappingL( aLogId, aPresentityId );
    
    // create service handler if it not already exists    
    CreateAndBindServiceHandlerL( aServiceId );
    
    // handle the callback from the presentity fetcher:
    // it only creates a new fetcher if there is not already one fetcher with
    // the same service id / presentityid    
    
    // find the correct service handler and add this presentity
    AddPresentityToServiceHandlerL( aServiceId, aPresentityId );    
    
    _LOG("CLogsUiControlExtension::PresentityIdFetchDoneL end" )    
    }


// ---------------------------------------------------------------------------
// Adds another mapping to the collection of mappings.
// ---------------------------------------------------------------------------
// 
void CLogsUiControlExtension::AddLogIdToPresentityIdMappingL( 
        const TLogId aLogId,
        const TDesC& aPresentityId )
    {
    iPresenceTableManager->AddMappingL( aLogId, aPresentityId );
    NotifyObserverL();
    }

    
// ---------------------------------------------------------------------------
// Retrieves a presentity id using the specified log id.ownership is not 
// transferred.
// ---------------------------------------------------------------------------
// 
const TDesC* CLogsUiControlExtension::LookupPresentityId( 
        const TLogId aLogId )
    {   
    _LOGP("CLogsUiControlExtension::LookupPresentityId =%d",aLogId )
    return iPresenceTableManager->RetrievePresentityId( aLogId );
    }


// ---------------------------------------------------------------------------
// Resets the presentity id fetcher array and also deletes the objects whose
// pointers are stored in the array. 
// ---------------------------------------------------------------------------
// 
void CLogsUiControlExtension::ResetPresentityIdFetcherArray()
    {
    _LOG("CLogsUiControlExtension::ResetPresentityIdFetcherArray begin...") 
    _LOGP("...beforeDeletion=%d",iPresentityIdFetcherArray.Count() )            

    iPresentityIdFetcherArray.ResetAndDestroy();
    
    _LOG("CLogsUiControlExtension::ResetPresentityIdFetcherArray end...")
    _LOGP("...afterDeletion=%d",iPresentityIdFetcherArray.Count() )            
    }

// ---------------------------------------------------------------------------
// Resets the presence status of all entries in the status table. 
// ---------------------------------------------------------------------------
// 
void CLogsUiControlExtension::ResetPresenceStatuses()
    {
    _LOG("CLogsUiControlExtension::ResetPresenceStatuses begin" )
    
    iPresenceTableManager->ResetStatusTableEntries();
    
    _LOG("CLogsUiControlExtension::ResetPresenceStatuses begin" )
    }

// ---------------------------------------------------------------------------
// Gets the service handler of the specified service.
// ---------------------------------------------------------------------------
//
TInt CLogsUiControlExtension::GetServiceHandler( 
        TUint32 aServiceId, 
        CLogsExtServiceHandler*& aServiceHandler )
    {
    _LOGP("CLogsUiControlExtension::GetServiceHandler begin : aServiceId=%d", 
        aServiceId )
    
    TInt error( KErrNotFound );    
    TBool handlerFound( EFalse );
         
    // check if there is already a fetcher for the brand
    CLogsExtServiceHandler* handler = NULL;

    for ( TInt i = 0 ; i < iServiceHandlerArray.Count() && !handlerFound; i++ )
        {
        handler = iServiceHandlerArray[i];
        if ( handler && aServiceId == handler->ServiceId() )
            {            
            handlerFound = ETrue;
            aServiceHandler = handler;
            error = KErrNone;
            }
        }

    error = handlerFound ? KErrNone : KErrNotFound;    
    _LOGP("CLogsUiControlExtension::GetServiceHandler() error=%d",error )

    _LOG("CLogsUiControlExtension::GetServiceHandler end" )         
    return error;
    }

// ---------------------------------------------------------------------------
// Handles the service handler's callback.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::HandlePresentityPresenceStatusL( 
                                 const TUint32 aServiceId, 
                                 const TDesC& aPresentityId,
                                 const TDesC& aPresenceStatus,
                                 TBool aShowStatus )
    {
    _LOG("CLogsUiControlExtension::HandlePresentityPresenceStatusL begin" )
    

    TInt error( KErrNone );    
    // 1. find <serviceId,presentityId> entry in the status table and set the 
    //    status entry (that was retrieved by the presence fetcher earlier)
    error = 
        iPresenceTableManager->SetStatusInStatusTable( aServiceId, 
                                                       aPresentityId, 
                                                       aPresenceStatus,
                                                       aShowStatus );
    if( KErrNone == error )
        {
        // 2. check whether the presence icon index for 
        // <serviceId,PresenceStatus> entry from index table is already
        // available
        TInt presenceIndex ( KErrNotFound );        
        const TInt indexRetrievalErrorCode = 
            iPresenceTableManager->GetIndexFromIndexTable( aServiceId, 
                                                           aPresenceStatus,
                                                           presenceIndex );
        
        //case 0: ( KErrNone == indexRetrievalErrorCode ) && 
        //        ( presenceIndex >= 0 )
        // -> nothing to do in this case (the icon was added already earlier
        //    and the index information was already stored to the index table)
        
        //case 1: ( KErrNone == indexRetrievalErrorCode && 
        //          presenceIndex == KErrNotFound )
        //the index entry was found, but the index value in still unchanged.
        //needed actions: create icons, add icons to icon list array,determine
        //index of the added icon in the icon array,update the index entry in
        //the index table
    
        //case 2: ( KErrNone != indexRetrievalErrorCode )
        //the index entry was NOT found at all.
        //needed actions: create icons, add icons to icon list array,determine
        //index of the added icon in the icon array,CREATE an index entry in 
        // the index table,and update the index entry in the index table
                        
        if ( ( KErrNone == indexRetrievalErrorCode && 
               presenceIndex == KErrNotFound ) ||      // case 1
             ( KErrNone != indexRetrievalErrorCode ) ) // case 2
            {
            //get the filehandle and the bitmapIds
            RFile bitmapFile;
            TInt bitmapId;   
            TInt bitmapMaskId;
            
            FetchBitmapsFromBrandingServerL( aServiceId,
                                             aPresenceStatus,
                                             bitmapFile,
                                             bitmapId,
                                             bitmapMaskId );
            
            // take ownership of the filehandle
            CleanupClosePushL( bitmapFile );
            _LOG("CLogsUiControlExtension::HandlePresentityPresenceStatusL..")
            _LOG("...bitmapFile to cleanupstack" )
            
            // File provider duplicates the file handle, close the original.
            // Duplication is needed because lifetime of image is different 
            // than this plugin's file handle's.
            CLogsIconFileProvider* ifp = 
                CLogsIconFileProvider::NewL( bitmapFile );
            CFbsBitmap* bitmap;
            bitmap = NULL;
            CFbsBitmap* bitmask;
            bitmask = NULL;
            
            // create icons            
            AknIconUtils::CreateIconL ( bitmap,
                                        bitmask,
                                        *ifp,
                                        bitmapId, 
                                        bitmapMaskId );
    
            error = KErrNone;
            // in case a new icon is added to the icon array of the listbox
            // 'indexOfAppendedIcon' will contain the index of the added icon
            // in the icon array
            TInt indexOfAppendedIcon( 0 );
            
            // if AddIconL returns with KErrNone the ownership of the bitmaps
            // was successfully transferred
            TRAP( error, AddIconL( bitmap, bitmask, indexOfAppendedIcon ) );
            bitmap = NULL;
            bitmask = NULL;
            _LOGP("CLogsUiControlExtension::AddIconL returns==%d",
                error )
            
            TBool errorAfterIconAddition( EFalse );
            
            if ( KErrNone == error && KErrNone != indexRetrievalErrorCode )
                {
                // create the missing index entry for case 2
                error = 
                    iPresenceTableManager->AddEntryToIndexTable( 
                        aServiceId, 
                        aPresenceStatus );
                errorAfterIconAddition = 
                    ( KErrNone == error || KErrAlreadyExists == error ) ? 
                        EFalse : ETrue;
                }
            
            TInt numberIcons ( indexOfAppendedIcon + 1 );
            // update the index entry
            if ( KErrNone == error )
                {                
                // store index information into the presence table manager
                error = SetPresenceIndex( aServiceId,
                                          aPresenceStatus,
                                          indexOfAppendedIcon );
                errorAfterIconAddition = ( KErrNone == error ) ? EFalse:ETrue;
                }
            
            // if setting the index failed, the last icon that was added to 
            // the icon array must be removed again
            if ( errorAfterIconAddition && numberIcons > 0 )
                {
                CArrayPtr<CGulIcon>* iconList = 
                    iListBox->ItemDrawer()->ColumnData()->IconArray();
                iconList->Delete( indexOfAppendedIcon );
                iconList->Compress();
                }
                            
            if ( KErrNone == error )
                {
                NotifyObserverL(); // notify the observer (to update the view)
                }

            _LOG("CLogsUiControlExtension::HandlePresentityPresenceStatusL...")
            _LOG("...close bitmapfile" )
            
            CleanupStack::PopAndDestroy( &bitmapFile );
            
            }
        else if ( ( KErrNone == indexRetrievalErrorCode ) && 
                  ( presenceIndex >= 0 ) ) // case 0
            {
            NotifyObserverL(); // notify the observer (to update the view)
            }     
        }
    
    _LOG("CLogsUiControlExtension::HandlePresentityPresenceStatusL end" )
    }

// ---------------------------------------------------------------------------
// This function is used to notify the observer of a service handler
// if the presence status of a certain presentity should be displayed
// or not.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::SetDisplayStatusFlag( 
        const TUint32 aServiceId, 
        const TDesC& aPresentityId,
        TBool aDisplayStatus )
    {
    _LOG("CLogsUiControlExtension::SetDisplayStatusFlag begin" )
    
    _LOGP("CLogsUiControlExtension::SetDisplayStatusFlag : aServiceId=%d ",
            aServiceId)
    
    _LOGP("CLogsUiControlExtension::SetDisplayStatusFlag : aPresentityId=%S",
            &aPresentityId )
    
    TInt error = 
        iPresenceTableManager->UpdateStatusTableEntry( aServiceId, 
                                                      aPresentityId,
                                                      aDisplayStatus );
    
    _LOG("CLogsUiControlExtension::SetDisplayStatusFlag : ...")
    _LOGP("...UpdateStatusTableEntry error = %d ", error )
    
    //refresh the view, to make changes visible
    if ( KErrNone == error )
        {
        TRAP_IGNORE( NotifyObserverL(); );
        }
    
    _LOG("CLogsUiControlExtension::SetDisplayStatusFlag end" )
    }

// ---------------------------------------------------------------------------
// Get the icon bitmaps from the branding server.
// ---------------------------------------------------------------------------
//
void CLogsUiControlExtension::FetchBitmapsFromBrandingServerL( 
        const TUint aServiceId,
        const TDesC& aPresenceStatus,
        RFile& aBitmapFile,
        TInt& aBitmapId,
        TInt& aBitmapMaskId )
    {
    _LOG("CLogsUiControlExtension::FetchBitmapsFromBrandingServerL(): begin" )
    
    // create the elementId string; e.g.: "person:offline:image"  
    HBufC* elementId = HBufC::NewLC( KPersonPresTmpl().Length() + 
                                     aPresenceStatus.Length() );            
    TPtr ptr( elementId->Des() );    
    ptr.AppendFormat( KPersonPresTmpl(), &aPresenceStatus );
    _LOG( "CLogsUiControlExtension::FetchBitmapsFromBrandingServerL():...")
    _LOGP("...elementId string=%S", elementId )
    
    HBufC8* elementId8 = HBufC8::NewLC( ptr.Length() );
    TPtr8 elementIdPtr8( elementId8->Des() );
    elementIdPtr8.Copy( ptr );
    
    _LOG("CLogsUiControlExtension::FetchBitmapsFromBrandingServerL(): ...")
    _LOGP("...elementId=%S", &ptr )
    
    CleanupClosePushL( aBitmapFile );        
    TLogsExtUtil::GetFileHandleL( aServiceId, 
                                  elementIdPtr8, 
                                  aBitmapFile, 
                                  aBitmapId, 
                                  aBitmapMaskId );    
    CleanupStack::Pop( &aBitmapFile );

    CleanupStack::PopAndDestroy( elementId8 );
    elementId8 = NULL;
    CleanupStack::PopAndDestroy( elementId );
    elementId = NULL;

    _LOG("CLogsUiControlExtension::FetchBitmapsFromBrandingServerL() end" )
    }


// End of File
