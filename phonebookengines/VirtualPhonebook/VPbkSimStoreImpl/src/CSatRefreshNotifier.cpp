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
* Description:  A notifier that gets refresh events from the SAT server
*
*/



// INCLUDE FILES
#include "CSatRefreshNotifier.h"

#include <RSatSession.h>
#include <RSatRefresh.h>
#include <VPbkDebug.h>
#include "MSimRefreshObject.h"

namespace VPbkSimStoreImpl {


// ============================= LOCAL FUNCTIONS ===============================

namespace {
// -----------------------------------------------------------------------------
// ConvertFromSatFormat() Converts from SAT server format to own format
// -----------------------------------------------------------------------------
//
TUint32 ConvertFromSatFormat(
    const TSatRefreshFiles& aFiles )  // List of files which has been refreshed
    {
    FUNC_ENTRY();
    TUint32 result = 0;
		
    // KAdnEf -> KADNRefresh
    TChar ef( KAdnEf );
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 1"));    
        result |= KADNRefresh;
        }
        
    // KExt1Ef -> KADNExtRefresh
    ef = KExt1Ef;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 2"));    
        result |= KADNExtRefresh;
        }
    
    // KFdnEf -> KFDNRefresh
    ef = KFdnEf;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 3"));    
        result |= KFDNRefresh;
        }
    
    // KExt2Ef -> KFDNExtRefresh
    ef = KExt2Ef;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 4"));    
        result |= KFDNExtRefresh;
        }
    
    // KSdnEf -> KSDNRefresh
    ef = KSdnEf;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 5"));    
        result |= KSDNRefresh;
        }
    
    // KExt3Ef -> KSDNExtRefresh
    ef = KExt3Ef;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 6"));    
        result |= KSDNExtRefresh;
        }
    
    // KSstEf -> KSTRefresh
    ef = KSstEf;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 7"));    
        result |= KSTRefresh;
        }
        
    // KMsisdnEf -> KONRefresh
    ef = KMsisdnEf;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 8"));    
        result |= KONRefresh;
        }
        
    // KExt5Ef -> KONExtRefresh
    ef = KExt5Ef;
    if ( aFiles.Locate( ef ) != KErrNotFound )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 9"));    
        result |= KONExtRefresh;
        }
        
    // Check all 0x4FXX files     
    const TInt count( aFiles.Length() );
    for( TInt i(0); i < count; ++i )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat 10"));    
        if ( !(0x4F ^ aFiles[i] >> 8 ) )
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
                "VPbkSimStoreImpl: ConvertFromSatFormat 11"));    
            // USIM ADN file updated
            result |= KADNRefresh;
            result |= KADNExtRefresh;
            }
        }        
    FUNC_EXIT();
    return result;    
    }

TUint32 ConvertFromSatFormat(
    const TSatRefreshType& aType )  // List of files which has been refreshed
    {
    FUNC_ENTRY();
    TUint result(0);

    if( aType == ESimReset )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat ESimReset"));
        result |= KSimReset;
        }

    if( aType == ESimInitFullFileChangeNotification )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat ESimInitFullFileChangeNotification"));
        result |= KSimInitFullFileChangeNotification;
        }

    if( aType == EUsimApplicationReset )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat EUsimApplicationReset"));
        result |= KUsimApplicationReset;
        }

    if( aType == E3GSessionReset )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat E3GSessionReset"));
        result |= K3GSessionReset;
        }

    if( aType == ESimInit )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: ConvertFromSatFormat ESimInit"));
        result |= KSimInit;
        }

    FUNC_EXIT();
    return result;
    }


} // unnamed namespace

// ============================ MEMBER FUNCTIONS ===============================

NONSHARABLE_CLASS(CSatRefreshNotifier::CSatSession) :    public CBase
    {
    public:  // Constructors and destructor
        CSatSession( MSatRefreshObserver& aSatObserver );
        ~CSatSession();

    public: // New functions
        /**
        * Opens SAT session
        */
        void OpenL();
        
        /**
        * Returns ETrue if session has been opened
        */
        TBool IsOpen();
        
        /**
        * @return the refresh subsession
        */
        RSatRefresh& Refresh();
        
    private:    // Data
        /// The main session to SAT server
        RSatSession iSatSession;
        /// The subsession that implements SAT refresh notifying
        RSatRefresh iSatRefresh;
        TBool iIsOpen;
    };

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::CSatSession::CSatSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatRefreshNotifier::CSatSession::CSatSession( MSatRefreshObserver& aSatObserver )
:   iSatRefresh( aSatObserver )
    {
    }

// Destructor
CSatRefreshNotifier::CSatSession::~CSatSession()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::~CSatSession,\
        Cancel refresh h%d"),
        iSatRefresh.SubSessionHandle() );
    iSatRefresh.Cancel();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::~CSatSession,\
        Close refresh h%d"), 
        iSatRefresh.SubSessionHandle() );
    iSatRefresh.Close();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::~CSatSession,\
        Close session h%d"), 
        iSatSession.Handle() );
    iSatSession.Close();
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::CSatSession::OpenL
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::CSatSession::OpenL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::OpenL"));
    iSatSession.ConnectL();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::OpenL, Session Connected h%d"), 
        iSatSession.Handle() );
    iSatRefresh.OpenL( iSatSession );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::CSatSession::OpenL, Refresh Opened h%d"), 
        iSatRefresh.SubSessionHandle() );
    iIsOpen = ETrue;
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::CSatSession::IsOpen
// -----------------------------------------------------------------------------
//
TBool CSatRefreshNotifier::CSatSession::IsOpen()
    {
    return iIsOpen;
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::CSatSession::Refresh
// -----------------------------------------------------------------------------
//
RSatRefresh& CSatRefreshNotifier::CSatSession::Refresh()
    {
    return iSatRefresh;
    }
        
// -----------------------------------------------------------------------------
// CSatRefreshNotifier::CSatRefreshNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatRefreshNotifier::CSatRefreshNotifier()
    {
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::ConstructL()
    {
    iSatSession = new( ELeave ) CSatSession( *this );
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatRefreshNotifier* CSatRefreshNotifier::NewL()
    {
    CSatRefreshNotifier* self = new( ELeave ) CSatRefreshNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// -----------------------------------------------------------------------------
// CSatRefreshNotifier::~CSatRefreshNotifier
// -----------------------------------------------------------------------------
//
CSatRefreshNotifier::~CSatRefreshNotifier()
    {
    delete iSatSession;
    iObjects.Close();
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::IsActive
// -----------------------------------------------------------------------------
//
TBool CSatRefreshNotifier::IsActive()
    {
    return iSatSession->IsOpen();
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::ActivateL
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::ActivateL( MSimRefreshObject& aSimRefreshObject )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::ActivateL"));
        
    // Open the session to SAT server if not opened
    if ( !iSatSession->IsOpen() )
        {
        iSatSession->OpenL();
        }
    
    // Listen to all changes
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::ActivateL: NotifyFileChangeL ALL"));
    iSatSession->Refresh().NotifyFileChangeL();

    if ( iObjects.Find( &aSimRefreshObject ) == KErrNotFound )
        {
        iObjects.AppendL( &aSimRefreshObject );
        }
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::DeActivate
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::DeActivate( MSimRefreshObject& aSimRefreshObject )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::DeActivate"));
        
    TInt index = iObjects.Find( &aSimRefreshObject );
    if ( index != KErrNotFound )
        {
        iObjects.Remove( index );
        }
        
    // Cancel the request if session is open and this was the last object
    if ( iSatSession->IsOpen() && iObjects.Count() == 0 )
        {
        iSatSession->Refresh().Cancel();        
        }
    }
    
// -----------------------------------------------------------------------------
// CSatRefreshNotifier::AllowRefresh
// -----------------------------------------------------------------------------
//
TBool CSatRefreshNotifier::AllowRefresh( TSatRefreshType aType, 
    const TSatRefreshFiles& aFiles )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::AllowRefresh:type=%d, aFiles=%S"), 
        aType,&aFiles);
    // First check files...
    TUint32 flags = ConvertFromSatFormat( aFiles );
    // ...and then refresh types
    flags |= ConvertFromSatFormat( aType );

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::AllowRefresh:Converted flags=%x"), 
        flags);
    
    TBool result = ETrue;
    const TInt count = iObjects.Count();
    for( TInt i = count - 1; i >= 0 && result; --i )
        {
        if ( !iObjects[i]->AllowRefresh( flags ) )
            {
            result = EFalse;
            }
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::AllowRefresh:return %d"), 
            result);
    // The refresh doesn't necessary start
    // because other clients or TSY can deny the refresh.
    return result;
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::Refresh
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::Refresh( TSatRefreshType aType,
    const TSatRefreshFiles& aFiles )
    {
    FUNC_ENTRY();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::Refresh:type=%d, aFiles=%S"), 
        aType,&aFiles);
        
    // Refresh has happened. Clients must update their content
    iRefreshedObjects = 0;
    iAdditionalEFsRead = EFalse;
    // First check files... 
    TUint32 flags = ConvertFromSatFormat( aFiles );
    // ...and then refresh types
    flags |= ConvertFromSatFormat( aType );

    const TInt count = iObjects.Count();
    for( TInt i = count - 1; i >= 0; --i )
        {
        iObjects[i]->SimRefreshed( flags, *this );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CSatRefreshNotifier::SatRefreshCompleted
// -----------------------------------------------------------------------------
//
void CSatRefreshNotifier::SatRefreshCompleted( 
        MSimRefreshObject& /*aSimRefreshObject*/,
        TBool aReadAdditionalFiles )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CSatRefreshNotifier::SatRefreshCompleted:total=%d,cur=%d"), 
        iObjects.Count(), iRefreshedObjects + 1);
        
    ++iRefreshedObjects;
    if ( aReadAdditionalFiles )
        {
        iAdditionalEFsRead = ETrue;
        }
    
    // Complete refresh to SAT server when all the objects have completed
    if ( iRefreshedObjects == iObjects.Count() )
        {
        iSatSession->Refresh().RefreshEFRead( iAdditionalEFsRead );
        }
    }
} // namespace VPbkSimStoreImpl
//  End of File  
