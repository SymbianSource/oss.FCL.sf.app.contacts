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
* Description:  Phonebook 2 create new group command object.
*
*/

// INCLUDE FILES
#include "CPguCreateNewGroupCmd.h"

// Phonebook 2
#include "CPguGroupNameQueryDlg.h"
#include "Pbk2GroupConsts.h"
#include <CPbk2InputAbsorber.h>
#include <pbk2groupuires.rsg>
#include <pbk2uicontrols.rsg>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <StringLoader.h>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::CPguCreateNewGroupCmd
// --------------------------------------------------------------------------
//
CPguCreateNewGroupCmd::CPguCreateNewGroupCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguCreateNewGroupCmd::CPguCreateNewGroupCmd(0x%x)"),this);
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::~CPguCreateNewGroupCmd
// --------------------------------------------------------------------------
//
CPguCreateNewGroupCmd::~CPguCreateNewGroupCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        (" CPguCreateNewGroupCmd::~CPguCreateNewGroupCmd(0x%x)"), this );

    delete iNewGroup;
    delete iInputAbsorber;
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::NewL
// --------------------------------------------------------------------------
//
CPguCreateNewGroupCmd* CPguCreateNewGroupCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPguCreateNewGroupCmd* self =
        new ( ELeave ) CPguCreateNewGroupCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguCreateNewGroupCmd::ConstructL()
    {
    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    iTargetStore = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().ContactStoresL().
            Find( storeConfig->DefaultSavingStoreL() );
    CleanupStack::PopAndDestroy( storeConfig );
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguCreateNewGroupCmd::ExecuteLD(0x%x)"), this);
    CleanupStack::PushL( this );

    CheckDefaultSavingStorageL();

    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::ContactOperationCompleted
        ( TContactOpResult /*aResult*/ )
    {
    // Will finish the command even if errors occur
    TRAP_IGNORE( FinishCommandL() );
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt /*aErrorCode*/,
          TBool /*aErrorNotified*/ )
    {
    // Will finish the command even if errors occur
    TRAP_IGNORE( FinishCommandL() );
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::AddNewGroupL
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::AddNewGroupL()
    {
    HBufC* textBuf = HBufC::NewLC( KGroupLabelLength );
    TPtr text = textBuf->Des();

    MVPbkContactViewBase* groupsView =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ViewSupplier().AllGroupsViewL();

    CPguGroupNameQueryDlg* dlg = 
        CPguGroupNameQueryDlg::NewL( 
            text,
            *groupsView,
            Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter(), 
            ETrue );
    dlg->SetMaxLength( KGroupLabelLength );

    if ( dlg->ExecuteLD( R_PHONEBOOK2_GROUPLIST_NEWGROUP_QUERY ) )
        {        
        // Insert new group into database
        iNewGroup = iTargetStore->CreateNewContactGroupLC();
        CleanupStack::Pop(); // iNewGroup
        iNewGroup->SetGroupLabelL( text );
        iNewGroup->CommitL( *this );
        CleanupStack::PopAndDestroy( textBuf );

        // Use input absorber to block key events until
        // FinishCommandL is executed
        delete iInputAbsorber;
        iInputAbsorber = NULL;
        iInputAbsorber = CPbk2InputAbsorber::NewL( R_AVKON_SOFTKEYS_OPTIONS_EXIT );
        iInputAbsorber->Wait();
        }
    else
        {
        CleanupStack::PopAndDestroy( textBuf );
        FinishCommandL();
        }
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::CheckDefaultSavingStorageL
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::CheckDefaultSavingStorageL()
    {
    TVPbkContactStoreUriPtr defaultSavingStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().DefaultSavingStoreL();
    MVPbkContactStoreList& stores =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().ContactStoresL();

    MVPbkContactStore* store = stores.Find( defaultSavingStore.UriDes() );
    if ( store && store->StoreProperties().SupportsContactGroups() )
        {
        AddNewGroupL();
        }
    else
        {
        MVPbkContactStoreList& stores = Phonebook2::Pbk2AppUi()->ApplicationServices().
               ContactManager().ContactStoresL();

        // Change target store to Contacts Model
        TVPbkContactStoreUriPtr ptr
            ( VPbkContactStoreUris::DefaultCntDbUri() );
        iTargetStore = stores.Find( ptr );
        
        AddNewGroupL();
        }
    }

// --------------------------------------------------------------------------
// CPguCreateNewGroupCmd::FinishCommandL
// --------------------------------------------------------------------------
//
void CPguCreateNewGroupCmd::FinishCommandL()
    {
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }

    if (iUiControl)
        {
        // Control focuses created group by reacting to contact view event,
        // do not set focus to created group here
        iUiControl->ResetFindL();
        iUiControl->UpdateAfterCommandExecution();
        }

    if ( iInputAbsorber )
        {
        iInputAbsorber->StopWait();
        }
    }

// End of File
