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

#include <ecom\ecom.h>
#include <e32property.h>
#include <e32cmn.h>

#include <StifParser.h>
#include <StifTestInterface.h>

#include <CContactCustomIconPluginBase.h>
#include <CCustomIconIdMap.h>
#include <CCustomIconDecisionData.h>

#include "CIconCustomApiTestModule.h"

_LIT( KTestPluginDisplayName, "Custom Icon test plugin" );


const TInt KExecutableUidIndex = 0;

// -----------------------------------------------------------------------------
// CleanUpEComInfoArray
// CleanupStack support for RImplInfoPtrArray.
// See LoadTestPluginL how it is used
// -----------------------------------------------------------------------------
//
LOCAL_C void CleanUpEComInfoArray( TAny* aInfoArray )
    {
    RImplInfoPtrArray* infoArray = (static_cast<RImplInfoPtrArray*>(aInfoArray));
    infoArray->ResetAndDestroy();
    infoArray->Close();
    }

// ----------------------------------------------------------------------------
// ProcessSID
// Figure out current process SID (this should be STIFF SID)
// ----------------------------------------------------------------------------
//
LOCAL_C TUid ProcessSID()
    {
    RProcess process;
    TUidType type = process.Type();
    // current process SID (STIFF executable SID = 0x1000007A)
    TUid uid = type[KExecutableUidIndex];
    return uid;
    }

// -----------------------------------------------------------------------------
// BuildDecisionDataLC
// Utility to build decision data object.
// See LoadTestPluginL how it is used
// -----------------------------------------------------------------------------
//
LOCAL_C CCustomIconDecisionData* BuildDecisionDataLC(  CStifItemParser& aItem )
    {
    CCustomIconDecisionData* decisionData =
        new ( ELeave ) CCustomIconDecisionData();
    CleanupStack::PushL( decisionData );

    TUid contextParam = ProcessSID();

    TTime* time = new (ELeave) TTime;
    decisionData->SetTime( time ); // ownership taken
    time->UniversalTime();

    TPtrC phoneNumber;
    aItem.GetNextString( phoneNumber );
    decisionData->SetPhoneNumberL( phoneNumber );

    decisionData->SetContactLinkRef( NULL );
    decisionData->SetContext( contextParam );

    return decisionData;
    }


// ----------------------------------------------------------------------------
// ReplaceIconL
// Find an icon and replace it
// ----------------------------------------------------------------------------
//
LOCAL_C void ReplaceIconL( CCustomIconIdMap* aIcons,
    CGulIcon* aIcon, TCustomIconId aIconId )
    {
    TInt i;
    TInt count = aIcons->Count();
    TInt err( KErrNotFound );
    for( i=0; i<count; i++ )
        {
        TCustomIconIdMapItem& item( (*aIcons)[i] );
        if( item.iId == aIconId )
            {
            // Change the icon
            delete item.iIconPtr;
            item.iIconPtr = aIcon;
            err = KErrNone;
            break;
            }
        }
    User::LeaveIfError( err );
    }


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::CleanupPsKeys
// Remove all key definitions and/or values, which have been
// defined/set by this test module.
// ----------------------------------------------------------------------------
//
void CIconCustomApiTestModule::CleanupPsKeys()
    {
    TInt count = iPublishedPsKeys.Count();
    TInt i;
    TUid sid = ProcessSID();
    for( i=0; i<count; i++ )
        {
        // Ignore return value, because key may not exist
        RProperty::Delete( sid, iPublishedPsKeys[i] );
        }
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "LoadTestPluginL", CIconCustomApiTestModule::LoadTestPluginL ),
        ENTRY( "LoadIconsL", CIconCustomApiTestModule::LoadIconsL ),
        ENTRY( "LoadIconL", CIconCustomApiTestModule::LoadIconL ),
        ENTRY( "UpdateIconsWithLastIconIdsL", CIconCustomApiTestModule::UpdateIconsWithLastIconIdsL ),
        ENTRY( "DeleteIconsL", CIconCustomApiTestModule::DeleteIconsL ),
        ENTRY( "DeleteIconL", CIconCustomApiTestModule::DeleteIconL ),
        ENTRY( "IconChoiceL", CIconCustomApiTestModule::IconChoiceL ),
        ENTRY( "SetPsProperty", CIconCustomApiTestModule::SetPsProperty ),
        ENTRY( "DeletePsProperty", CIconCustomApiTestModule::DeletePsProperty ),
        ENTRY( "SetTestCaseAsObserverL", CIconCustomApiTestModule::SetTestCaseAsObserverL ),
        ENTRY( "CheckIfHasExtensionL", CIconCustomApiTestModule::CheckIfHasExtensionL ),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::CustomIconsChangedL
// Notification handler, called by plugin.
// ----------------------------------------------------------------------------
//
void CIconCustomApiTestModule::CustomIconEvent(
            CContactCustomIconPluginBase& /*aPlugin*/,
            TEventType aEventType,
            const TArray<TCustomIconId>& aIconIds)
    {
    iLastEventType = aEventType;
    TInt signalCode( aEventType );

    TRAPD( err,
        iLastIconIds.Reset();
        TInt i;
        TInt count = aIconIds.Count();
        for( i=0; i<count; i++ )
            {
            iLastIconIds.AppendL( aIconIds[i] );
            }
        );
    if( err )
        {
        signalCode = err;
        }
    Signal( signalCode );
    }


// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::LoadTestPluginL
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::LoadTestPluginL( CStifItemParser& /*aItem*/ )
    {
    if( iTestPlugin )
        {
        return KErrAlreadyExists;
        }

    CContactCustomIconPluginBase* result = NULL;

    RImplInfoPtrArray implementationInfoArray;
    TCleanupItem cleanup( CleanUpEComInfoArray, &implementationInfoArray );
    CleanupStack::PushL( cleanup );

    TUid uid( KContactIconCustomizationInterfaceUid );

    REComSession::ListImplementationsL( uid,
        implementationInfoArray );

    TInt count = implementationInfoArray.Count();

    for( TInt i=0; i<count; i++)
        {
        CImplementationInformation* info = implementationInfoArray[i];
        if( !(info->DisplayName().Compare( KTestPluginDisplayName ) == 0) )
            {
            continue;
            }
        // Test plugin was found. Try to load it.
        TUid pluginUid = info->ImplementationUid();
        TRAPD( err, result = static_cast<CContactCustomIconPluginBase*>
            (REComSession::CreateImplementationL(pluginUid,
                                            _FOFF(CContactCustomIconPluginBase,
                                            iDtorKey)));
            );
        if( err )
            {
            result = NULL;
            }
        break;
        }

    CleanupStack::PopAndDestroy(&implementationInfoArray);

    iTestPlugin = result;

    if( !iTestPlugin )
        {
        return KErrNotFound;
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::LoadIconsL
// Load icon array
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::LoadIconsL( CStifItemParser& aItem )
    {
    User::LeaveIfNull( iTestPlugin );
    if( iIcons )
        {
        User::Leave( KErrAlreadyExists );
        }

    TInt sizeHint( 0 );
    aItem.GetNextInt( sizeHint );
    iIcons = iTestPlugin->IconsL(
        ProcessSID(), // use SID as context parameter
        (CContactCustomIconPluginBase::TIconSizeHint)sizeHint );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::LoadIconL
// Load individual icon
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::LoadIconL( CStifItemParser& aItem )
    {
    User::LeaveIfNull( iTestPlugin );
    if( iIcon )
        {
        User::Leave( KErrAlreadyExists );
        }
    TInt sizeHint( 0 );
    aItem.GetNextInt( sizeHint );

    CCustomIconDecisionData* decisionData = BuildDecisionDataLC( aItem );

    iIcon = iTestPlugin->IconL( *decisionData,
        (CContactCustomIconPluginBase::TIconSizeHint)sizeHint );

    CleanupStack::PopAndDestroy( decisionData );
    return KErrNone;

    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::UpdateIconsWithLastIconIdsL
// Replace current icons with new ones
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::UpdateIconsWithLastIconIdsL(
    CStifItemParser& aItem )
    {
    User::LeaveIfNull( iTestPlugin );
    if( !iIcons )
        {
        User::Leave( KErrNotFound );
        }
    TInt sizeHint( 0 );
    aItem.GetNextInt( sizeHint );

    TInt i;
    TInt count = iLastIconIds.Count();
    CGulIcon* icon;
    TCustomIconId iconId;
    TUid contextParam = ProcessSID(); // use process SID as context parameter
    for( i=0; i<count; i++ )
        {
        iconId = iLastIconIds[i];
        icon = iTestPlugin->IconL( contextParam,
            iconId,
            (CContactCustomIconPluginBase::TIconSizeHint)sizeHint );
        CleanupStack::PushL( icon );
        ReplaceIconL( iIcons, icon, iconId ); // Takes ownership
        CleanupStack::Pop( icon );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::DeleteIconsL
// Delete icon array
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::DeleteIconsL( CStifItemParser& /*aItem*/ )
    {
    if( !iIcons )
        {
        User::Leave( KErrNotFound );
        }
    delete iIcons;
    iIcons = NULL;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::DeleteIconL
// Delete individual icon instance
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::DeleteIconL( CStifItemParser& /*aItem*/ )
    {
    if( !iIcon )
        {
        User::Leave( KErrNotFound );
        }
    delete iIcon;
    iIcon = NULL;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::IconChoiceL
// Query plug-in for a icon choice to the previously given icon array
// Expect plug-in to provide a icon or otherwise fail
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::IconChoiceL( CStifItemParser& aItem )
    {
    User::LeaveIfNull( iTestPlugin );
    User::LeaveIfNull( iIcons );

    CCustomIconDecisionData* decisionData = BuildDecisionDataLC( aItem );

    TInt iconId( 0 );
    TBool iconFound = iTestPlugin->IconChoiceL( *decisionData, iconId );
    if( iconFound )
        {
        // Plug-in gave us an icon identifier. Now check if we have the
        // corresponding icon.
        CGulIcon* icon = NULL;
        TCustomIconIdMapItem item;
        for( TInt i=0; i<iIcons->Count(); i++ )
            {
            item = iIcons->At( i );
            if( item.iId == iconId )
                {
                icon = item.iIconPtr; // may return NULL
                break;
                }
            }
        if( !icon )
            {
            // icon in the icon array was NULL, or
            // icon was not found by given icon identifier
            //
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( decisionData );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::SetPsProperty
// Define and set publish/subscribe property. This is used for configuring
// testing environment. For example the test plug-in reads the P/S values
// on its initialization phase.
// ----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::SetPsProperty( CStifItemParser& aItem )
    {
    TInt key;
    TInt value;
    aItem.GetNextInt( key );
    aItem.GetNextInt( value );
    TSecurityPolicy readwritePolicy( TSecurityPolicy::EAlwaysPass );

    TUid sid = ProcessSID();
    TInt err = RProperty::Define(
        sid,
        key,
        RProperty::EInt,
        readwritePolicy,
        readwritePolicy );

    if(err == KErrNone || err == KErrAlreadyExists)
        {
        iPublishedPsKeys.Append( key );
        err = RProperty::Set( sid, key, value );
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CIconCustomApiTestModule::DeletePsProperty
//
// -----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::DeletePsProperty( CStifItemParser& aItem )
    {
    TInt key;
    aItem.GetNextInt( key );
    TInt err = RProperty::Delete( ProcessSID(), key );
    if( !err )
        {
        TInt index = iPublishedPsKeys.Find ( key );
        if( index >= 0 )
            {
            iPublishedPsKeys.Remove( index );
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CIconCustomApiTestModule::SetTestCaseAsObserver
//
// -----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::SetTestCaseAsObserverL( CStifItemParser& aItem )
    {
    TBool setAsObserver;
    aItem.GetNextInt( setAsObserver );
    if( setAsObserver )
        {
        iTestPlugin->AddObserverL( this );
        }
    else
        {
        iTestPlugin->RemoveObserver( this );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIconCustomApiTestModule::CheckIfHasExtensionL
//
// -----------------------------------------------------------------------------
//
TInt CIconCustomApiTestModule::CheckIfHasExtensionL( CStifItemParser& aItem )
    {
    TInt extensionUid;
    TBool expectedResult;
    User::LeaveIfError( aItem.GetNextInt( extensionUid ) );
    User::LeaveIfError( aItem.GetNextInt( expectedResult ) );
    TAny* extension =
        iTestPlugin->ContactCustomIconExtension( TUid::Uid( extensionUid ) );
    delete extension;
    if( expectedResult && extension ||
        !expectedResult && !extension )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }




//  End of File
