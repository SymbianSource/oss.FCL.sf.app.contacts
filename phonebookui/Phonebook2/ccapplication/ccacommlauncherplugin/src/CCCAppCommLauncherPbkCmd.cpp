/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 command execution handler
*
*/


#include "CCCAppCommLauncherPbkCmd.h"
#include "ccappcommlauncherplugin.h"

#include <CPbk2ApplicationServices.h>
#include <Pbk2DataCaging.hrh>
#include <CPbk2CommandHandler.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MPbk2Command.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <coemain.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>

_LIT(KPbk2CommandsDllResFileName,   "Pbk2Commands.rsc");
_LIT(KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc");
_LIT(KPbk2CommonUiDllResFileName,   "Pbk2CommonUi.rsc"  );

/// Unnamed namespace for local definitions
namespace
    {
    const TInt KNumberOfContacts = 1;
    } // namespace

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::CCCAppCommLauncherPbkCmd
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPbkCmd::CCCAppCommLauncherPbkCmd( CCCAppCommLauncherPlugin& aPlugin ):
    iCommandsResourceFile( *CCoeEnv::Static() ),
    iUiControlsResourceFile( *CCoeEnv::Static() ),
    iCommonUiResourceFile( *CCoeEnv::Static() ),
    iPlugin( aPlugin ),
    iCommandInProgress( EFalse )
    {
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ConstructL()
    {
    PreparePbk2ApplicationServicesL();
    iCommandHandler = CPbk2CommandHandler::NewL();
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::NewL
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPbkCmd* CCCAppCommLauncherPbkCmd::NewL( CCCAppCommLauncherPlugin& aPlugin )
    {
    CCCAppCommLauncherPbkCmd* self = CCCAppCommLauncherPbkCmd::NewLC( aPlugin );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::NewLC
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPbkCmd* CCCAppCommLauncherPbkCmd::NewLC( CCCAppCommLauncherPlugin& aPlugin )
    {
    CCCAppCommLauncherPbkCmd* self = new( ELeave ) CCCAppCommLauncherPbkCmd( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::~CCCAppCommLauncherPbkCmd
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPbkCmd::~CCCAppCommLauncherPbkCmd()
    {
    if ( iCommand )
        {
        iCommand->ResetUiControl( *this );
        }

    delete iStoreContact;
    delete iCommandHandler;
    delete iLinks;
    delete iOperation;

    // Close contact store
    if( iContactStore )
        {
        iContactStore->Close( *this );
        }

    Release( iAppServices );

    iCommandsResourceFile.Close();
    iUiControlsResourceFile.Close();
    iCommonUiResourceFile.Close();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::PreparePbk2ApplicationServicesL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::PreparePbk2ApplicationServicesL()
    {
    iCommandsResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommandsDllResFileName );
    iUiControlsResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName );
    iCommonUiResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommonUiDllResFileName );

    iAppServices = CPbk2ApplicationServices::InstanceL();
    };

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdAssignDefault
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdAssignDefaultL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
    iPbk2CommandId = EPbk2CmdDefaultSettings;

    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }

    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdViewImageL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdViewImageL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
    iPbk2CommandId = EPbk2CmdViewImage;

    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }

    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdChangeImageL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdChangeImageL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
    iPbk2CommandId = EPbk2CmdChangeImage;

    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }

    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdRemoveImageL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdRemoveImageL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
	iPbk2CommandId = EPbk2CmdRemoveImage;

    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }

    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdAddImageL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdAddImageL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
    iPbk2CommandId = EPbk2CmdAddImage;

    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }

    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdShowOnMapL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdShowMapL( const HBufC8* aContact,
		TPbk2CommandId aCommandId )
    {
    iCommandInProgress = ETrue;
	
	iPbk2CommandId = aCommandId;
    
    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }
    
    iLinks = iAppServices->ContactManager().CreateLinksLC( *aContact );   
    
    if ( iLinks->Count() > 0 )
        {       
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }
    
    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ExecutePbk2CmdDeleteL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ExecutePbk2CmdDeleteL( const TDesC8& aContact )
    {
    iCommandInProgress = ETrue;
    
    iPbk2CommandId = EPbk2CmdDeleteMe;
    
    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }
    
    iLinks = iAppServices->ContactManager().CreateLinksLC( aContact );   
    
    if ( iLinks->Count() > 0 )
        {       
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        ( iLinks->At( 0 ) ).ContactStore().OpenL( *this );
        }
    
    CleanupStack::Pop( 1 ); // iLinks (deleted in destructor)
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::HandleError
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::HandleError( TInt aError )
    {
    if( KErrNotSupported == aError && 
        (TPbk2CommandId)EPbk2ExtensionShowOnMap == iPbk2CommandId )
        {
        // Show maps specifix error note
        HBufC* prompt = StringLoader::LoadLC( R_QTN_CCA_ERROR_NOTE_MAPS_NOT_FOUND );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( *prompt );
        CleanupStack::PopAndDestroy( prompt );        
        }
    else
        {
        CCoeEnv::Static()->HandleError( aError );
        }
    }
// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::StoreReady
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::StoreReady(
    MVPbkContactStore& aContactStore )
    {
    iContactStore = &aContactStore;
    
    // operation completes by VPbkSingleContactOperationComplete
    //   or VPbkSingleContactOperationFailed
    if( iOperation  )
        {
        delete iOperation;
        iOperation = NULL;
        }

    TRAPD( err,iOperation = iAppServices->ContactManager().RetrieveContactL( iLinks->At( 0 ), *this ) ;)
    if( err != KErrNone )
        {
        HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/,
    TInt /*aReason*/)
    {
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::HandleStoreEventL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::HandleStoreEventL(
    MVPbkContactStore& /*aContactStore*/,
    TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    delete iStoreContact;
    iStoreContact = aContact;

    if( !iCommandHandler )
        {
        TRAPD(err, iCommandHandler = CPbk2CommandHandler::NewL();)
        if( err != KErrNone )
            {
            HandleError( err );
            return;
            }
        }

    TRAPD(err, iCommandHandler->HandleCommandL(
            iPbk2CommandId, *this, NULL );)
    if( err != KErrNone )
        {
        HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aError*/ )
    {

    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CCCAppCommLauncherPbkCmd::ParentControl() const
{
    // Contact info control has no parent control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::NumberOfContacts() const
{
    return KNumberOfContacts;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CCCAppCommLauncherPbkCmd::FocusedContactL() const
{
    return STATIC_CAST(MVPbkBaseContact*, iStoreContact);
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CCCAppCommLauncherPbkCmd::FocusedViewContactL() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CCCAppCommLauncherPbkCmd::FocusedStoreContact() const
{
    return iStoreContact;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetFocusedContactL(
    const MVPbkBaseContact& aContact)
{
    if (!iStoreContact->IsSame(aContact))
        {
        MVPbkContactLink* link = aContact.CreateLinkLC();
        HBufC8* packed = link->PackLC();
    
        // Update the CCA parameter contact data
        HBufC16* link16 = HBufC16::NewLC(packed->Length());
        link16->Des().Copy(packed->Des());
        iPlugin.AppEngine()->Parameter().SetContactDataL(link16->Des());
        iPlugin.AppEngine()->CCAppContactEventL();
        CleanupStack::PopAndDestroy(3); // link16, packed, link
    
        // Update the commlauncher the UI contact data
        iPlugin.ContactHandler().RefetchContactL();
        iPlugin.Container().ContactsChangedL();    
        }
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetFocusedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetFocusedContactL(
    const MVPbkContactLink& /*aContactLink*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::FocusedContactIndex() const
{
    return KErrNotFound;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetFocusedContactIndexL(TInt /*aIndex*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::NumberOfContactFields() const
{
    TInt cnt = KErrNotSupported;
    return cnt;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CCCAppCommLauncherPbkCmd::FocusedField() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::FocusedFieldIndex() const
{
    return 0;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetFocusedFieldIndex(TInt /*aIndex*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherPbkCmd::ContactsMarked() const
{
    // This control does not support contacts marking
    return EFalse;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CCCAppCommLauncherPbkCmd::SelectedContactsL() const
{
    // No selection support in contact info control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
CCCAppCommLauncherPbkCmd::SelectedContactsOrFocusedContactL() const
{
    CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();

    if ( iStoreContact )
    {
        MVPbkContactLink* link = iStoreContact->CreateLinkLC();
        array->AppendL(link);
        CleanupStack::Pop(); // link
    }
    CleanupStack::Pop(); // array

    return array;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
CCCAppCommLauncherPbkCmd::SelectedContactsIteratorL() const
{
    // No selection support in contact info control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
CCCAppCommLauncherPbkCmd::SelectedContactStoresL() const
{
    // This is not a contact store control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ClearMarks
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ClearMarks()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetSelectedContactL(
    TInt /*aIndex*/,
    TBool /*aSelected*/)
{
    // Not supported in Contact Info
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetSelectedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/,
    TBool /*aSelected*/)
{
    // Not supported
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetSelectedContactL(
    const MVPbkContactLink& /*aContactLink*/,
    TBool /*aSelected*/)
{
    // Not supported
}



// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::DynInitMenuPaneL(
    TInt /*aResourceId*/,
    CEikMenuPane* /*aMenuPane*/) const
{
    // Nothing to do
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ProcessCommandL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ProcessCommandL(TInt /*aCommandId*/) const
{
    // Nothing to do
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::UpdateAfterCommandExecution()
{
    if( iCommand )
    {
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
        iCommandInProgress = EFalse;
    }
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::GetMenuFilteringFlagsL() const
{
    TInt ret = KPbk2MenuFilteringFlagsNone;
    return ret;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CCCAppCommLauncherPbkCmd::ControlStateL() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::RestoreControlStateL(
    CPbk2ViewState* /*aState*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CCCAppCommLauncherPbkCmd::FindTextL()
{
    // No find box in this control
    return KNullDesC;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ResetFindL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ResetFindL()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ShowThumbnail
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::ShowThumbnail()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::HideThumbnail
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::HideThumbnail()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetBlank
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetBlank(TBool /*aBlank*/)
{
    // This control does not support blanking
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::RegisterCommand
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::RegisterCommand(MPbk2Command* aCommand)
{
    iCommand = aCommand;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::SetTextL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::SetTextL(const TDesC& /*aText*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CCCAppCommLauncherPbkCmd::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        {
		return static_cast<MPbk2ContactUiControl2*>( this );
		}

    return NULL;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::CommandItemCount
// --------------------------------------------------------------------------
//
TInt CCCAppCommLauncherPbkCmd::CommandItemCount() const
{
    return 0; // No command items.
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::CommandItemAt
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem& CCCAppCommLauncherPbkCmd::CommandItemAt(
    TInt /*aIndex*/) const
{
    // There are no command items in this control
    __ASSERT_ALWAYS(EFalse, User::Panic(_L("Pbk2"), 4));
    MPbk2UiControlCmdItem* item = NULL; // For compiler only
    return *item; // For compiler only
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::FocusedCommandItem
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem* CCCAppCommLauncherPbkCmd::FocusedCommandItem() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::DeleteCommandItemL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::DeleteCommandItemL(TInt /*aIndex*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::AddCommandItemL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPbkCmd::AddCommandItemL(
    MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
{
    //Do nothing, since there shouldn't be any command items in this state.
}

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::IsPbk2CommandRunning
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherPbkCmd::IsPbk2CommandRunning()
    {
    return iCommandInProgress;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherPbkCmd::ApplicationServices
// --------------------------------------------------------------------------
//
CPbk2ApplicationServices& CCCAppCommLauncherPbkCmd::ApplicationServices() const
    {
    return *iAppServices;
    }
