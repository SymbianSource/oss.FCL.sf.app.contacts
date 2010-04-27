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
* Description:  Phonebook 2 group name query dialog.
*
*/


#include "CPguGroupNameQueryDlg.h"

// Phonebook 2
#include "Pbk2GroupConsts.h"
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2GroupUIRes.rsg>
#include <Pbk2PresentationUtils.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2StartupMonitor.h>

// Virtual Phonebook
#include <MVPbkContactGroup.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactViewBase.h>

// System includes
#include <aknnotewrappers.h>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Strips directionality markers from given text.
 *
 * @param aText     The text to strip.
 */
void StripCharacters( TDes& aText)
    {
    // Strip any directionality markers to get pure text
    const TUint32 KPbk2LeftToRightMarker = 0x200F;
    const TUint32 KPbk2RightToLeftMarker = 0x200E;
    const TInt markersLength( 2 );
    TBuf<markersLength> bufMarkers;
    bufMarkers.Append( KPbk2LeftToRightMarker );
    bufMarkers.Append( KPbk2RightToLeftMarker );
    AknTextUtils::StripCharacters( aText, bufMarkers );
    }

} /// namespace

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::CPguGroupNameQueryDlg
// --------------------------------------------------------------------------
//
 CPguGroupNameQueryDlg::CPguGroupNameQueryDlg
        ( TDes& aDataText, MVPbkContactViewBase& aGroupsView,
          MPbk2ContactNameFormatter& aNameFormatter ) :
            CAknTextQueryDialog( aDataText ),
            iGroupsListView( aGroupsView ),
            iNameFormatter( aNameFormatter )
    {
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::~CPguGroupNameQueryDlg
// --------------------------------------------------------------------------
//
CPguGroupNameQueryDlg::~CPguGroupNameQueryDlg()
    {
    iCoeEnv->RemoveForegroundObserver( *this );
    delete iOriginalName;
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguGroupNameQueryDlg::ConstructL
        ( TDes& aDataText, TBool aNameGeneration )
    {
    // Take a copy of the original name
    iOriginalName = HBufC::NewL( KGroupLabelLength );
    TPtr originalTextPtr = iOriginalName->Des();
    originalTextPtr.Append( aDataText );

    if ( aNameGeneration )
        {
        UpdateGroupTitleL();
        }
    iCoeEnv->AddForegroundObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::NewL
// --------------------------------------------------------------------------
//
CPguGroupNameQueryDlg* CPguGroupNameQueryDlg::NewL
        ( TDes& aDataText, MVPbkContactViewBase& aGroupsView,
          MPbk2ContactNameFormatter& aNameFormatter,
          TBool aNameGeneration )
    {
    CPguGroupNameQueryDlg* self = new ( ELeave ) CPguGroupNameQueryDlg
        ( aDataText, aGroupsView, aNameFormatter );
    CleanupStack::PushL( self );
    self->ConstructL( aDataText, aNameGeneration );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPguGroupNameQueryDlg::OkToExitL( TInt aButtonId )
    {
    TBool result = CAknTextQueryDialog::OkToExitL( aButtonId );

    HBufC* text = Text().AllocLC();
    TPtr textPtr( text->Des() );
    StripCharacters( textPtr );

    // Format text
    HBufC* formattedText = HBufC::NewLC( text->Length() );
    TPtr formattedTextPtr( formattedText->Des() );
    Pbk2PresentationUtils::TrimRightAppend( *text, formattedTextPtr );

    TBool nameAlreadyExists = ContainsL( *formattedText );

    if ( nameAlreadyExists )
        {
        if (iOriginalName->CompareC( *formattedText ) != 0 )
        	{
            // Display information note
            HBufC* prompt = StringLoader::LoadLC(
                    R_QTN_FLDR_NAME_ALREADY_USED, *formattedText );
    
            CAknInformationNote* dlg = new(ELeave) CAknInformationNote( ETrue );
            dlg->ExecuteLD( *prompt );
            CleanupStack::PopAndDestroy(); // prompt
    
            CAknQueryControl* queryControl = QueryControl();
            if ( queryControl )
                {
                CEikEdwin* edwin = static_cast<CEikEdwin*>(
                    queryControl->ControlByLayoutOrNull( EDataLayout ) );
                if ( edwin )
                    {
                    edwin->SetSelectionL( edwin->TextLength(), 0 );
                    }
                }
            result = EFalse;
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // formattedText, text
    return result;
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::UpdateGroupTitleL
// --------------------------------------------------------------------------
//
void CPguGroupNameQueryDlg::UpdateGroupTitleL()
    {
    HBufC* groupTitle;
    TInt newGroupNumber = iGroupsListView.ContactCountL() + 1;
    groupTitle = StringLoader::LoadL( R_PHONEBOOK2_QTN_FLDR_DEFAULT_GROUP_NAME, newGroupNumber );
    Text().Copy( *groupTitle );
    delete groupTitle;
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::ContainsL
// Checks if the group view contains a group named similarly as the given
// name.
// --------------------------------------------------------------------------
//
TBool CPguGroupNameQueryDlg::ContainsL( const TDesC& aText )
    {
    TBool ret = EFalse;

    const TInt count( iGroupsListView.ContactCountL() );
    for ( TInt i(0); i < count; ++i )
        {
        const MVPbkViewContact& contact = iGroupsListView.ContactAtL( i );
        HBufC* groupName = iNameFormatter.GetContactTitleOrNullL
            ( contact.Fields(),
              MPbk2ContactNameFormatter::EPreserveLeadingSpaces );

        if ( groupName )
            {
            TPtr groupNamePtr( groupName->Des() );
            StripCharacters( groupNamePtr );

            if ( groupNamePtr.Compare( aText ) == 0 )
                {
                ret = ETrue;
                }
            }

        delete groupName;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::HandleLosingForeground
// --------------------------------------------------------------------------
//
void CPguGroupNameQueryDlg::HandleLosingForeground() 
    {           
    }

// --------------------------------------------------------------------------
// CPguGroupNameQueryDlg::HandleGainingForeground
// --------------------------------------------------------------------------
//
void CPguGroupNameQueryDlg::HandleGainingForeground() 
    {
    MPbk2AppUi* pbk2AppUI = NULL;
    pbk2AppUI = Phonebook2::Pbk2AppUi();
    
    if ( pbk2AppUI && pbk2AppUI->Pbk2StartupMonitor() )
        {
        TAny* extension = pbk2AppUI->Pbk2StartupMonitor()
                ->StartupMonitorExtension( KPbk2StartupMonitorExtensionUid );

        if( extension )
            {
            MPbk2StartupMonitorExtension* startupMonitorExtension =
                    static_cast<MPbk2StartupMonitorExtension*>( extension );

            if( startupMonitorExtension )
                {
                startupMonitorExtension->DisableMonitoring();
                }
            }
        }
    }

// End of File
