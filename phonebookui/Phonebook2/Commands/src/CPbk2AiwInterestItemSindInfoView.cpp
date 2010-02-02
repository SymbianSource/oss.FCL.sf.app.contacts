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
* Description:  Phonebook 2 SIND info view AIW interest item.
*
*/


#include "CPbk2AiwInterestItemSindInfoView.h"

// Phonebook 2
#include "CPbk2SindInfoViewCmd.h"
#include "Pbk2InternalCommands.hrh"
#include <MPbk2Command.h>
#include <MPbk2ContactUiControl.h>

// Virtual Phonebook
#include <MVPbkBaseContact.h>
#include <MVPbkContactLink.h>

// System includes
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <eikmenup.h>

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::CPbk2AiwInterestItemSindInfoView
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestItemSindInfoView::CPbk2AiwInterestItemSindInfoView
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CPbk2AiwInterestItemBase( aInterestId, aServiceHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::~CPbk2AiwInterestItemSindInfoView
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemSindInfoView::~CPbk2AiwInterestItemSindInfoView()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemSindInfoView* CPbk2AiwInterestItemSindInfoView::NewL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemSindInfoView* self =
        new( ELeave ) CPbk2AiwInterestItemSindInfoView
            ( aInterestId, aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemSindInfoView::DynInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& /*aControl*/ )
    {
    TBool ret = EFalse;

    // Normal menu
    if( IsMenuPaneAttached( aResourceId ) )
        {
        ret = ETrue;

        // Let provider add its menu items to the menu
        iServiceHandler.InitializeMenuPaneL
            ( aMenuPane, aResourceId, KAiwCmdInfoView,
              iServiceHandler.InParamListL() );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemSindInfoView::HandleCommandL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& aControl,
          TInt aServiceCommandId )
    {
    TBool ret = EFalse;

    // Only handle this command if service command is SIND InfoView
    // AIW service command UID
    if( aServiceCommandId == KAiwCmdInfoView )
        {
        ret = ETrue;
        CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();

        const MVPbkBaseContact* contact = aControl.FocusedContactL();
        MVPbkContactLink* link = contact->CreateLinkLC();

        linkArray->AppendL( link );
        CleanupStack::Pop(); // link

        // Pack link array, then set it to CPbk2SindInfoViewCmd
        TDesC8* packedLinkArray = linkArray->PackLC();

        CPbk2SindInfoViewCmd* cmd =
            CPbk2SindInfoViewCmd::NewL( aMenuCommandId, iServiceHandler );

        cmd->SetPackedLinkArray( packedLinkArray ); // takes ownership
        CleanupStack::Pop( packedLinkArray );
        CleanupStack::PopAndDestroy( linkArray );

        // Execute the command
        cmd->ExecuteLD();
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemSindInfoView::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSindInfoView::RunL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemSindInfoView::RunL()
    {
    // Do nothing
    }

// End of File
