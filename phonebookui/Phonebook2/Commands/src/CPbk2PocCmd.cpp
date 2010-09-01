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
* Description:  Phonebook 2 PoC command object.
*
*/


#include "CPbk2PocCmd.h"

// Phonebook 2
#include "CPbk2CallTypeSelector.h"
#include <Pbk2UID.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2Commands.hrh>

// Virtual Phonebook
#include <CVPbkContactLinkArray.h>
#include <MVPbkStoreContact.h>

// System includes
#include <AiwServiceHandler.h>
#include <AiwPoCParameters.h>
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPbk2PocCmd::CPbk2PocCmd
// --------------------------------------------------------------------------
//
CPbk2PocCmd::CPbk2PocCmd( const TInt aCommandId,
        CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        MPbk2ContactUiControl& aControl,
        const MVPbkStoreContact& aContact,
        MVPbkStoreContactField* aSelectedField ) :
            iCommandId( aCommandId ),
            iServiceHandler( aServiceHandler ),
            iSelector( aSelector ),
            iControl( &aControl ),
            iContact( aContact ),
            iSelectedField( aSelectedField )
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2PocCmd::CPbk2PocCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2PocCmd::~CPbk2PocCmd
// --------------------------------------------------------------------------
//
CPbk2PocCmd::~CPbk2PocCmd()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2PocCmd::~CPbk2PocCmd(0x%x)"), this);
    delete iSelectedField;
    }

// --------------------------------------------------------------------------
// CPbk2PocCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2PocCmd* CPbk2PocCmd::NewL(
        const TInt aCommandId, CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        MPbk2ContactUiControl& aControl,
        const MVPbkStoreContact& aContact,
        MVPbkStoreContactField* aSelectedField )
    {
    CPbk2PocCmd* self = new ( ELeave ) CPbk2PocCmd( aCommandId,
            aServiceHandler, aSelector, aControl, aContact,
            aSelectedField );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PocCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2PocCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2PocCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );

    if (iControl)
        {        
        CAiwGenericParamList& inParamList = iServiceHandler.InParamListL();
        TAiwPocParameterData pocParameter;
        pocParameter.iConsumerAppUid = TUid::Uid( KPbk2UID3 );
        pocParameter.iConsumerWindowGroup =
            CCoeEnv::Static()->RootWin().Identifier();
        pocParameter.iCommandId = 0;

        // Select PoC call type
        if ( iCommandId == EPbk2CmdPoC || iCommandId == EPbk2CmdCall )
            {
            // We must select one call type when using PoC key
            pocParameter.iCommandId =
                iSelector.SelectPocCallTypeForPocKeyPress( *iControl );
            }
        else
            {
            // Use call type selector to deduct what call type to use
            pocParameter.iPreferredMenu =
                iSelector.SelectPocCallType( *iControl );
            }

        // Append PoC parameter data to parameter list
        TAiwGenericParam param = TAiwGenericParam(
            EGenericParamPoCData,
            TAiwVariant( TAiwPocParameterDataPckg( pocParameter ) ) );
        inParamList.AppendL( param );

        // Append contact links to parameter list
        MVPbkContactLinkArray* linkArray = NULL;
        if ( iSelectedField )
            {
            CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();

            // If selected field is set create field link from it
            MVPbkContactLink* contactLink = iSelectedField->CreateLinkLC();
            CleanupStack::Pop(); // contactLink
            array->AppendL( contactLink );
            linkArray = array;
            }
        else
            {
            linkArray = iControl->SelectedContactsOrFocusedContactL();
            CleanupDeletePushL( linkArray );
            }

        if ( linkArray )
            {
            HBufC8* packedLinks = linkArray->PackLC();
            inParamList.AppendL( TAiwGenericParam(
                EGenericParamContactLinkArray, TAiwVariant( *packedLinks ) ) );
            CleanupStack::PopAndDestroy(); // packedLinks
            }

        CleanupStack::PopAndDestroy(); // linkArray

        // If the command id is EPbk2CmdPoC it means the call was launched
        // with PoC key and we therefore must use different AIW command
        // than when the call was selected from the menu. The same applies
        // when the call is made with Send key (EPbk2CmdCall).
       if ( iCommandId == EPbk2CmdPoC || iCommandId == EPbk2CmdCall)
           {
            iServiceHandler.ExecuteServiceCmdL(
                KAiwCmdPoC,
                inParamList,
                iServiceHandler.OutParamListL(),
                0,      // no options used.
                NULL);  // no need for callback
           }
       else // the call was launched from menu
           {
            // Relay the command to AIW for handling
            iServiceHandler.ExecuteMenuCmdL(
                iCommandId,
                inParamList,
                iServiceHandler.OutParamListL(),
                0,      // no options used.
                NULL);  // no need for callback
           }

        // Update UI control
        TInt fieldIndex = iControl->FocusedFieldIndex();
        iControl->ClearMarks();
        iControl->ResetFindL();
        iControl->SetFocusedContactL( iContact );
        iControl->SetFocusedFieldIndex( fieldIndex );
        }
        
    // Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

// --------------------------------------------------------------------------
// CPbk2PocCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2PocCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iControl == &aUiControl)
        {
        iControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PocCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2PocCmd::AddObserver( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }

// End of File
