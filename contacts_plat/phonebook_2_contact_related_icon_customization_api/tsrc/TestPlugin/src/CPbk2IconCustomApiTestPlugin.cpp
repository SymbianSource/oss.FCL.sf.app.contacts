/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*/

// INCLUDE FILES
#include <coemain.h>
#include <gulicon.h>
#include <e32math.h>
#include <fbs.h>
#include <cntdef.h>
#include <e32property.h>

#include <CCustomIconDecisionData.h>
#include <CCustomIconIdMap.h>
#include <MCustomIconChangeObserver.h>

#include "CPbk2IconCustomApiTestPlugin.h"

#if defined(__WINS__) || defined(WINS)
_LIT( KIconFile, "Z:\\Data\\Test\\CustIconApiTestIcons.mbm" );
#else
_LIT( KIconFile, "C:\\Data\\Test\\CustIconApiTestIcons.mbm" );
#endif

// Publish/Subscribe category UID of used for configuring test environment.
// See SetPsProperty() implementation.
const TUint KStiffPsCategoryUid = 0x1000007A;
const TUint KIconApiTestPSPropertyCategoryUID = KStiffPsCategoryUid;


// Publish/Subscribe keys under KIconApiTestPSPropertyCategoryUID category.
// Make sure these are same as in Pbk2IconCustomApiTestModule.cfg
const TInt KPsKeyIconUpdaterEnabled = 1;
const TInt KPsKeyIconUpdaterIntervalMs = 2;
const TInt KPsKeyNotificationEventType = 3;
const TInt KPsKeyNotificationEmptyUidArray = 4;


// ----------------------------------------------------------------------------
// GetPsTestProperty
// Read property value from Publish/Subscribe subsystem.
// The values are set by test module dll, contacts_dom\
//      phonebook_2_contact_related_icon_customization_api\Internal\Test
// and are meant to configure this plug-in at runtime.
// ----------------------------------------------------------------------------
//
LOCAL_C TInt GetPsTestProperty( TInt aKey, TInt& aResult )
    {
    TInt value;
    TInt err = RProperty::Get(
        TUid::Uid( KIconApiTestPSPropertyCategoryUID ),
        aKey,
        value );
    if( !err )
        {
        aResult = value;
        }
    return err;
    }

// ----------------------------------------------------------------------------
// ValidateHintL
// ----------------------------------------------------------------------------
//
LOCAL_C void ValidateHintL( const CCustomIconDecisionData& aHint )
    {
    // At least one of the members should have been filled by the caller
    if( aHint.ContactLinkRef() == NULL &&
        aHint.PhoneNumber() == KNullDesC &&
        aHint.Time() == NULL)
        {
        User::Leave( KErrArgument );
        }

    }

// ----------------------------------------------------------------------------
// AppendIconToMapL
// ----------------------------------------------------------------------------
//
LOCAL_C void AppendIconToMapL( const TDesC& aMbmFileName, TInt aMbmIndex,
    TInt aId, CCustomIconIdMap* aIconMap )
    {
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError( bitmap->Load( aMbmFileName, aMbmIndex ) );
    CGulIcon* icon = CGulIcon::NewL( bitmap ); // takes ownership of bitmap
    CleanupStack::Pop( bitmap );
    CleanupStack::PushL( icon );
    aIconMap->AppendL( icon, aId );            // takes ownership of icon
    CleanupStack::Pop( ); // icon
    }

// ----------------------------------------------------------------------------
// ESMRHelper::LocateResourceFile
// ----------------------------------------------------------------------------
//
LOCAL_C TInt LocateResourceFile(
        const TDesC& aResource,
        const TDesC& aPath,
        TFileName &aResourceFile)
    {
    RFs* fsSession;

    fsSession=&CCoeEnv::Static()->FsSession();//codescanner::eikonenvstatic

    TFindFile resourceFile( *fsSession );
    TInt err = resourceFile.FindByDir(
            aResource,
            aPath );

    if ( KErrNone == err )
        {
        aResourceFile.Copy( resourceFile.File() );
        }

    return err;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::NewL
// ----------------------------------------------------------------------------
//
CPbk2IconCustomApiTestPlugin* CPbk2IconCustomApiTestPlugin::NewL()
    {
    CPbk2IconCustomApiTestPlugin* self =
        new(ELeave)CPbk2IconCustomApiTestPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::CPbk2IconCustomApiTestPlugin
// ----------------------------------------------------------------------------
//
CPbk2IconCustomApiTestPlugin::CPbk2IconCustomApiTestPlugin()
    {
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::~CPbk2IconCustomApiTestPlugin
// ----------------------------------------------------------------------------
//
CPbk2IconCustomApiTestPlugin::~CPbk2IconCustomApiTestPlugin()
    {
    iObservers.Close();
    delete iTimer;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::ConstructL
// ----------------------------------------------------------------------------
//
void CPbk2IconCustomApiTestPlugin::ConstructL()
    {
    TTime now;
    now.HomeTime();
    iSeed = now.Int64();

    // If the test case has defined icon update property, initialize
    // periodical timer for generating icon update events
    TInt psResult( EFalse );
    if( GetPsTestProperty( KPsKeyIconUpdaterEnabled, psResult ) == KErrNone &&
        psResult )
        {
        if( GetPsTestProperty(
            KPsKeyIconUpdaterIntervalMs, psResult ) == KErrNone )
            {
            iTimer = CPeriodic::NewL( CActive:: EPriorityStandard );
            TInt tickIntevalMicroseconds = 1000 * psResult;
            iTimer->Start( tickIntevalMicroseconds,
                tickIntevalMicroseconds,
                TCallBack( Tick, this) );
            }
        }
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::AddObserverL
// ----------------------------------------------------------------------------
//
void CPbk2IconCustomApiTestPlugin::AddObserverL(
    MCustomIconChangeObserver* aObserver )
    {
    iObservers.AppendL( aObserver );
    // Test that observer interface returns NULL extension for NULL UID.
    if( aObserver->IconChangeNotificationExtension( KNullUid ) )
        {
        User::Leave( KErrGeneral );
        }
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::RemoveObserver
// ----------------------------------------------------------------------------
//
void CPbk2IconCustomApiTestPlugin::RemoveObserver(
    MCustomIconChangeObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if( index > 0 )
        {
        iObservers.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::IconsL
// ----------------------------------------------------------------------------
//
CCustomIconIdMap* CPbk2IconCustomApiTestPlugin::IconsL(
    TUid /*aContext*/,
    TIconSizeHint /*aSizeHint*/ )
    {

    CCustomIconIdMap* iconMap = new( ELeave )CCustomIconIdMap( 2 );
    CleanupStack::PushL( iconMap );
    
    AppendIconToMapL( KIconFile, 0, 653, iconMap );
    AppendIconToMapL( KIconFile, 1, 145, iconMap );

    CleanupStack::Pop( iconMap );
    return iconMap;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::IconChoiceL
// ----------------------------------------------------------------------------
//
TBool CPbk2IconCustomApiTestPlugin::IconChoiceL(
    const CCustomIconDecisionData& aHint, TInt& aIdResult )
    {
    ValidateHintL( aHint );
    TInt random = Math::Rand( iSeed ) % 2;
    if( random == 0)
        {
        aIdResult = 653;
        }
    else
        {
        aIdResult = 145;
        }
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::IconL
// ----------------------------------------------------------------------------
//
CGulIcon* CPbk2IconCustomApiTestPlugin::IconL(
    const CCustomIconDecisionData& aHint, TIconSizeHint /*aSizeHint*/ )
    {
    ValidateHintL( aHint );
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    TInt context = aHint.Context().iUid;
    const TTime* time = aHint.Time();
    // Icon depends on context param & time, here simply whether they
    // has bee defined..
    if( context || time )
        {
        User::LeaveIfError( bitmap->Load( KIconFile, 0 ) );
        }
    else
        {
        User::LeaveIfError( bitmap->Load( KIconFile, 1 ) );
        }
    CGulIcon* icon = CGulIcon::NewL( bitmap ); // takes ownership of bitmap
    CleanupStack::Pop( bitmap );
    return icon;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::IconL
// ----------------------------------------------------------------------------
//
CGulIcon* CPbk2IconCustomApiTestPlugin::IconL( TUid /*aContext*/,
    TCustomIconId aIconId,
    TIconSizeHint /*aSizeHint*/ )
    {
    TInt mbmIndex = KErrNotFound;
    if( aIconId == 653 )
        {
        mbmIndex = 0;
        }
    else if( aIconId == 145 )
        {
        mbmIndex = 1;
        }
    CGulIcon* icon = NULL;
    if( mbmIndex != KErrNotFound )
        {
        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
        CleanupStack::PushL( bitmap );
        
        User::LeaveIfError( bitmap->Load( KIconFile, 0 ) );
        icon = CGulIcon::NewL( bitmap ); // takes ownership of bitmap
        CleanupStack::Pop( bitmap );
        }
    return icon;
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::Tick
// ----------------------------------------------------------------------------
//
TInt CPbk2IconCustomApiTestPlugin::Tick( TAny* aObject )
    {
    // cast, and call non-static function
    TRAP_IGNORE(((CPbk2IconCustomApiTestPlugin*)aObject)->NotifyObserversL();)
    return ETrue; // continue ticking
    }

// ----------------------------------------------------------------------------
// CPbk2IconCustomApiTestPlugin::NotifyObserversL
// ----------------------------------------------------------------------------
//
void CPbk2IconCustomApiTestPlugin::NotifyObserversL()
    {
    TInt i;
    TInt count = iObservers.Count();
    MCustomIconChangeObserver* observer;
    for( i=0; i<count; i++ )
        {
        observer = iObservers[i];
        if( observer )
            {
            // Setup notification parameters from the values defined by the
            // Publish/Subscribe system (test case being the definer)
            // If values not found, use default values
            TInt eventType( MCustomIconChangeObserver::EIconsMajorChange );
            TBool emptyArray( ETrue );
            GetPsTestProperty( KPsKeyNotificationEventType, eventType );
            GetPsTestProperty( KPsKeyNotificationEmptyUidArray, emptyArray );

            RArray<TCustomIconId> array;
            CleanupClosePushL( array );
            if( !emptyArray )
                {
                array.AppendL( TCustomIconId( 653 ) );
                array.AppendL( TCustomIconId( 145 ) );
                }
            observer->CustomIconEvent( *this,
                (MCustomIconChangeObserver::TEventType)eventType,
                array.Array() );

            CleanupStack::PopAndDestroy( &array );
            }
        }
    }

// End of file


