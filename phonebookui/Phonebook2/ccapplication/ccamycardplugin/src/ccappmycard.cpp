/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MyCard
*
*/


#include "ccappmycard.h"
#include "ccappmycardcommon.h"

// System
#include <coemain.h>
#include <f32file.h>
#include <avkon.hrh>
#include <eikenv.h>
#include <aknViewAppUi.h>

// Virtual phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore2.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact2.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <VPbkEng.rsg>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>

// Phonebook 2
#include <CPbk2StorePropertyArray.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2StoreSpecificFieldPropertyArray.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2ApplicationServices.h>
#include <CPbk2StoreManager.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2ContactEditorDlg.h>
#include <MVPbkBaseContactField.h>

// internal
#include "ccappmycardplugin.h"
#include <ccappmycardpluginrsc.rsg>

/**
 * Helper class for making delayed callbacks
 * @see public methods of CTimer for help
 */
class CTimerCallBack : public CTimer
    {
public:
    /**
     * @param aCallBack called when CTimer is due to run
     * @param aPriority priority of CTimer
     */
    static CTimerCallBack* NewL(
        const TCallBack& aCallBack,
        CActive::TPriority aPriority = CActive::EPriorityIdle );

protected: // From CActive
    void RunL();
    TInt RunError( TInt /*aError*/ );

protected:
    CTimerCallBack( const TCallBack& aCallBack, CActive::TPriority aPriority );

private: // data
    TCallBack iCallBack;
    };


// ---------------------------------------------------------------------------
// CTimerCallBack::NewL
// ---------------------------------------------------------------------------
//
CTimerCallBack* CTimerCallBack::NewL(
    const TCallBack& aCallBack,
    CActive::TPriority aPriority )
    {
    CTimerCallBack* self = new(ELeave) CTimerCallBack( aCallBack, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CTimerCallBack::RunL
// ---------------------------------------------------------------------------
//
CTimerCallBack::CTimerCallBack( const TCallBack& aCallBack, CActive::TPriority aPriority ) :
    CTimer( aPriority ), iCallBack( aCallBack )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CTimerCallBack::RunL
// ---------------------------------------------------------------------------
//
void CTimerCallBack::RunL()
    {
    iCallBack.CallBack();
    }

// ---------------------------------------------------------------------------
// CTimerCallBack::RunError
// ---------------------------------------------------------------------------
//
TInt CTimerCallBack::RunError( TInt /*aError*/ )
    {
    // Leaves in RunL are ignored
    return KErrNone;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppMyCard::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCard* CCCAppMyCard::NewL( CCCAppMyCardPlugin& aPlugin, RFs* aFs )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCard::NewL()"));

    CCCAppMyCard* self = new ( ELeave ) CCCAppMyCard( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL(aFs);
    CleanupStack::Pop( self );

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::NewL()"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::~CCCAppMyCard
// ---------------------------------------------------------------------------
//
CCCAppMyCard::~CCCAppMyCard()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCard::~CCCAppMyCard()"));

    delete iOperation;
    delete iFetchOperation;
    delete iMyCard;
    delete iPresentationContact;
    delete iMyCardContact;
    delete iFieldProperties;
    delete iSpecificFieldProperties;

    iObservers.Reset();

    if( iAppServices )
	   {
	   iAppServices->StoreManager().DeregisterStoreEvents(*this);
	   }
	Release(iAppServices);

	delete iCloseCallBack;
    delete iCreateCallBack;
    delete iDlgCloseCallBack;
    delete iStoreCallBack;

	CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::~CCCAppMyCard()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::CCCAppMyCard()
// ---------------------------------------------------------------------------
//
inline CCCAppMyCard::CCCAppMyCard( CCCAppMyCardPlugin& aPlugin )
: iPlugin( aPlugin ), iEvent( MMyCardObserver::EEventContactLoaded )

	{
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::ConstructL
// ---------------------------------------------------------------------------
//
inline void CCCAppMyCard::ConstructL( RFs* /*aFs*/ )
    {
    // Get the AppServices instance
	iAppServices = CPbk2ApplicationServices::InstanceL();

	CPbk2StoreManager& storeManager = iAppServices->StoreManager();
	iVPbkContactManager = &iAppServices->ContactManager();
	storeManager.RegisterStoreEventsL( *this );
	storeManager.EnsureDefaultSavingStoreIncludedL();

	iCloseCallBack = new(ELeave) CAsyncCallBack(
        TCallBack( CloseCcaL, this ), CActive::EPriorityIdle );
    iCreateCallBack = new(ELeave) CAsyncCallBack(
        TCallBack( CreateMyCardContact, this ), CActive::EPriorityHigh );
    iDlgCloseCallBack = new(ELeave) CAsyncCallBack(
        TCallBack( ExitDlgL, this ), CActive::EPriorityHigh );
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::PresentationContactL
// ---------------------------------------------------------------------------
//
CPbk2PresentationContact& CCCAppMyCard::PresentationContactL()
    {
    if( !iPresentationContact )
        {
        if( !iMyCardContact )
            {
            // own contact not loaded
            User::Leave( KErrNotReady );
            }

        const MVPbkContactStoreProperties& storeProperties =
            iMyCardContact->ParentStore().StoreProperties();
        const MVPbkFieldTypeList& supportedFieldTypes =
            storeProperties.SupportedFields();

        if( !iFieldProperties )
            {
            iFieldProperties = CPbk2FieldPropertyArray::NewL(
                supportedFieldTypes, iVPbkContactManager->FsSession() );
            }

        // Create a field property list of the supported
        // field types of the used store
        CPbk2StorePropertyArray* pbk2StoreProperties = CPbk2StorePropertyArray::NewL();
        CleanupStack::PushL( pbk2StoreProperties );

        if( !iSpecificFieldProperties )
            {
            iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL(
                *iFieldProperties,
                *pbk2StoreProperties,
                supportedFieldTypes,
                iMyCardContact->ParentStore() );
            }

        iPresentationContact = CPbk2PresentationContact::NewL(
            *iMyCardContact, *iSpecificFieldProperties );

        CleanupStack::PopAndDestroy( pbk2StoreProperties );
        }

    return *iPresentationContact;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::StoreContact
// ---------------------------------------------------------------------------
//
MVPbkStoreContact& CCCAppMyCard::StoreContact()
    {
    __ASSERT_ALWAYS( iMyCardContact, Panic( ECCAppMyCardPanicContactNotReady ) );
    return *iMyCardContact;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactManager
// ---------------------------------------------------------------------------
//
CVPbkContactManager& CCCAppMyCard::ContactManager()
    {
    return *iVPbkContactManager;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::IsContactLinkReady
// ---------------------------------------------------------------------------
//
TBool CCCAppMyCard::IsContactLinkReady() const
    {
    return iMyCard != NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactLink
// ---------------------------------------------------------------------------
//
MVPbkContactLink& CCCAppMyCard::ContactLink()
    {
    __ASSERT_ALWAYS( iMyCard, Panic( ECCAppMyCardPanicLinkNotReady ) );
    return *iMyCard;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::AddObserverL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::AddObserverL( MMyCardObserver* aObserver )
    {
    if( aObserver )
        {
        TInt index = iObservers.Find( aObserver );
        if( index == KErrNotFound )
            {
            iObservers.AppendL( aObserver );
            if( iMyCardContact )
                {
                aObserver->MyCardEventL( MMyCardObserver::EEventContactLoaded );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::RemoveObserver
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::RemoveObserver( MMyCardObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::SetLinkL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::SetLinkL( const MVPbkContactLink& aLink )
    {
    MVPbkContactLink* link = aLink.CloneLC();
    CleanupStack::Pop(); // link
    delete iMyCard;
    iMyCard = link;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::FetchMyCardL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::FetchMyCardL()
    {
    if( !iStoreCallBack )
        {
        iStoreCallBack = CTimerCallBack::NewL(
            TCallBack( &CCCAppMyCard::OpenStoresL, this ) );

        // 200 ms delay before open. Used to prevent system jamming before the
        // UI is drawn, so that launching of mycard feels a lot faster.
        iStoreCallBack->After( 200e3 );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::ForceCreateMyCard
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ForceCreateMyCard()
    {
    iForceCreateMyCard = ETrue;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::NotifyObservers
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::NotifyObservers( MMyCardObserver::TEvent aEvent ) const
    {
    TInt count = iObservers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        TRAPD( err, iObservers[i]->MyCardEventL( aEvent ) );
        if( err )
            {
            CCA_DP(KMyCardLogFile,
                CCA_L("<-CCCAppMyCard::NotifyObservers notify error (%d)"), err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::LoadContact
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::LoadContact()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCard::LoadContact()"));

    if( !iFetchOperation && iMyCard )
        {
        TRAPD( err, iFetchOperation =
            iVPbkContactManager->RetrieveContactL( *iMyCard, *this ) );
        if( err )
            {
            // Cannot load own contact from VPbk
            CCA_DP(KMyCardLogFile,
                CCA_L("  CCCAppMyCard::LoadContact load error = %d"), err );

            iPlugin.HandleError( err );
            }
        }

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::LoadContact()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::StoreReady
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::StoreReady( MVPbkContactStore& aContactStore )
	{
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCard::StoreReady()"));

	const MVPbkContactStoreProperties& storeProperties =
		aContactStore.StoreProperties();
	TVPbkContactStoreUriPtr uri = storeProperties.Uri();
	TInt isSame = uri.Compare( VPbkContactStoreUris::DefaultCntDbUri(),
		TVPbkContactStoreUriPtr::EContactStoreUriAllComponents );

	if( isSame != 0 )
		{
		return;
		}

    if( iMyCard )
        {
        // if link is already available then use that
        LoadContact();
        }
    else if( iForceCreateMyCard )
        {
        // launch editor because we don't have mycard
        iCreateCallBack->Call();
        }
    else
        {
        // MyCard is always created to contact model. So we check that event was
        // from the correct store.
		delete iOperation;
		iOperation = NULL;

		MVPbkContactStore2* phoneStoreExtension =
			static_cast<MVPbkContactStore2*>(aContactStore.ContactStoreExtension(KMVPbkContactStoreExtension2Uid));
		if ( phoneStoreExtension )
			{
			TRAPD( err, iOperation = phoneStoreExtension->OwnContactLinkL(*this) );
			if( err )
				{
                iPlugin.HandleError( err );
				}
			}
        }
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::StoreReady()"));
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::StoreUnavailable(
		MVPbkContactStore& /*aContactStore*/,
        TInt /*aReason*/)
	{
    CCA_DP(KMyCardLogFile, CCA_L("  CCCAppMyCard::StoreUnavailable()"));
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::HandleStoreEventL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent aStoreEvent )
	{
    CCA_DP(KMyCardLogFile,
        CCA_L("->CCCAppMyCard::HandleStoreEventL Event = %d"),
        aStoreEvent.iEventType );

    if( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged )
        {
        if( iMyCard && aStoreEvent.iContactLink->IsSame( *iMyCard ) )
            {
            // Own contact has changed. Reload contact to update content
            iEvent = MMyCardObserver::EEventContactChanged;
            LoadContact();
            }
        }
    else if( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactDeleted )
        {
        if( iMyCard && aStoreEvent.iContactLink->IsSame( *iMyCard ) )
            {
            if( !iDialogIsRunning )
                {
                CEikAppUi* aEikAppUi = CEikonEnv::Static()->EikAppUi();
                CAknViewAppUi* appUi = static_cast<CAknViewAppUi*> ( aEikAppUi );
                appUi->RunAppShutter();
                }
            }
        }

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::HandleStoreEventL()") );
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::VPbkSingleContactLinkOperationComplete
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::VPbkSingleContactLinkOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkContactLink* aLink )
	{
    CCA_DP(KMyCardLogFile,
        CCA_L("->CCCAppMyCard::VPbkSingleContactLinkOperationComplete()") );

    delete iMyCard;
	iMyCard = aLink;

	LoadContact();

	CCA_DP(KMyCardLogFile,
        CCA_L("<-CCCAppMyCard::VPbkSingleContactLinkOperationComplete()") );
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::VPbkSingleContactLinkOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::VPbkSingleContactLinkOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError )
	{
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCard::VPbkSingleContactLinkOperationFailed error = %d"),
        aError );    
    
	delete iMyCard;
	iMyCard = NULL;
	TInt error(aError);
	// if link is not found
	if( KErrNotFound == error )
	    {
        // launch contact editor
        iCreateCallBack->Call();
        }
	if( error != KErrNone && error != KErrNotFound )
	    {
        iPlugin.HandleError( error );
	    }
	
	CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCard::VPbkSingleContactLinkOperationFailed()"));    
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::CreateMyCardContact
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCard::CreateMyCardContact( TAny* aPtr )
	{
	CCCAppMyCard* self = static_cast<CCCAppMyCard*>( aPtr );
	TRAPD( err, self->LaunchContactEditorL( TPbk2ContactEditorParams::ENewContact | 
        TPbk2ContactEditorParams::EOwnContact ) );
	
	if( err != KErrNone )
        {        
        self->iPlugin.HandleError( err );
        
        if( !self->iMyCard )
            {
            self->iCloseCallBack->Call();
            }
        else
            {
            self->LoadContact();
            }
        }
	
	return err;
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::LaunchContactEditorL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::LaunchContactEditorL( TUint32 aFlags )
	{
	// default contact store
	MVPbkContactStore* store = iVPbkContactManager->ContactStoresL().Find( 
        VPbkContactStoreUris::DefaultCntDbUri() );
	
	MVPbkBaseContactField* field = NULL;
	MVPbkStoreContact* contact = NULL;
	if( aFlags & TPbk2ContactEditorParams::ENewContact )
		{
		// create empty store contact and launch editor
		contact = store->CreateNewContactLC();
		CleanupStack::Pop(); // contact, whose ownership is taken away
		}
	else if( aFlags & TPbk2ContactEditorParams::EModified )
		{
        if( iFocusedFieldIndex >= KErrNone )
            {
            // focused field
            field = iMyCardContact->Fields().FieldAtLC( iFocusedFieldIndex ); 
            }
        				
		// pass current store contact, ownership is taken
		contact = iMyCardContact;
		iMyCardContact = NULL;		
		delete iPresentationContact;
		iPresentationContact = NULL;		
		}
	// params for the editor
	TPbk2ContactEditorParams params( aFlags, field, NULL, this );
	
	// pass custom title text
    if ( contact )
        {
        HBufC* title = CCoeEnv::Static()->AllocReadResourceLC( 
            R_QTN_CCA_MC_TITLE_EDIT_MY_CARD );
        // ownership of the title is passed
        CPbk2ContactEditorDlg* dlg = CPbk2ContactEditorDlg::NewL( params, contact, 
            *this, iAppServices, title, R_SOCIAL_PHONEBOOK_FIELD_PROPERTIES );
        CleanupStack::Pop( title );
        iEditorEliminator = dlg;
        dlg->ResetWhenDestroyed( &iEditorEliminator );
        
        iDialogIsRunning = ETrue;
        dlg->ExecuteLD();
        }
	
	// if field was created, destroy it
	if( field )
		{
		CleanupStack::PopAndDestroy( field );
		}
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::EditContactL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::EditContactL( TInt aFocusedFieldIndex )
	{
	// if exists
	if( iMyCardContact )
		{
		// store focused index
		iFocusedFieldIndex = aFocusedFieldIndex;
		iMyCardContact->LockL( *this );
		}
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactEditingComplete
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ContactEditingComplete( MVPbkStoreContact* aEditedContact )
	{
    // create link of mycard
    MVPbkContactLink* link = NULL;       
    
    TRAPD( err,
        {
        link = aEditedContact->CreateLinkLC(); 
        
        if( link )
            {
            CleanupStack::Pop(); //link
            }
        } );
        
    delete aEditedContact; // ignore given contact
    
    if( link && err == KErrNone )
        {
        
        delete iMyCard;
        iMyCard = link;
        
        // reload mycard to get rid of the empty template fields
        LoadContact();
        }
    else if( !iMyCard )
        {
        iCloseCallBack->Call();
        }               
    
    iDialogIsRunning = EFalse;
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactEditingDeletedContact
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ContactEditingDeletedContact( 
    MVPbkStoreContact* aEditedContact )
	{
	// Editing was cancelled, go back to phonebook
	delete aEditedContact;	
	iCloseCallBack->Call();
	iDialogIsRunning = EFalse;
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactEditingAborted
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ContactEditingAborted()
	{
    iDialogIsRunning = EFalse;
	// Editing was aborted -> move to pb2
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CCCAppMyCard::OkToExitL( TInt /*aCommandId*/ )
    {
    // Handle exit via callback. Calls CCCAppMyCard::ExitDlg.
    // Dialog's exit is handled this way to make the address selector's 
    // exit work as it should.
    iDlgCloseCallBack->Call();
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::CloseCcaL
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCard::CloseCcaL( TAny* aPtr )
	{
	CCCAppMyCard* self = static_cast<CCCAppMyCard*>( aPtr );
	// if fields are empty, and user pushes "Done" or "Exit" this function is 
	// called. Card is not created -> goto pbk2
	self->DoCloseCCaL();
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::CloseCcaL
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCard::ExitDlgL( TAny* aPtr )
    {
    CCCAppMyCard* self = static_cast<CCCAppMyCard*>( aPtr );
    if( self->iEditorEliminator )
        {
        self->iEditorEliminator->RequestExitL( EAknCmdExit );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::OpenStoresL
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCard::OpenStoresL( TAny* aPtr )
    {
    CCCAppMyCard* self = static_cast<CCCAppMyCard*>( aPtr );
    self->iAppServices->StoreManager().OpenStoresL();
    return 0;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::DoCloseCCaL
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::DoCloseCCaL()
	{
	// close the plugin
	iPlugin.HandleCommandL( EAknSoftkeyBack );
	}

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactOperationCompleted
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ContactOperationCompleted( TContactOpResult aResult )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCard::ContactOperationCompleted()"));   
    
    // contact locked complete for editing
    if( aResult.iOpCode == MVPbkContactObserver::EContactLock )
    	{
    	TRAPD( err, LaunchContactEditorL( TPbk2ContactEditorParams::EModified | 
            TPbk2ContactEditorParams::EOwnContact) );
    	
    	if( err == KLeaveExit )
    	    {                                                                           
            iCloseCallBack->Call();
            }    	    
    	else if( err != KErrNone )
			{
            LoadContact();
            iPlugin.HandleError( err );
            if( !iMyCard )
                {
                iCloseCallBack->Call();
                }                
			}
    	}    
    else if( aResult.iOpCode == MVPbkContactObserver::EContactDelete )
    	{
		iCloseCallBack->Call();
    	}   
        
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCard::ContactOperationCompleted()")); 
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::ContactOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::ContactOperationFailed(
    TContactOp aOpCode, TInt aErrorCode, TBool /*aErrorNotified*/ )    
    {
    if ( aErrorCode != KErrNone )          
        {                                  
        iPlugin.HandleError( aErrorCode );    
        }        
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCard::VPbkSingleContactOperationComplete()") );    
    
    delete iFetchOperation;
    iFetchOperation = NULL;
    
    delete iMyCardContact;
    iMyCardContact = aContact;
    
    // also delete presentation contact, since it points to old store contact
    delete iPresentationContact;
    iPresentationContact = NULL;
    
    NotifyObservers( iEvent );

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCard::VPbkSingleContactOperationComplete()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppMyCard::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppMyCard::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt aError )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCard::VPbkSingleContactOperationFailed error = %d"),
        aError );    
    
    delete iFetchOperation;
    iFetchOperation = NULL;

    delete iOperation;
    iOperation = NULL;
    
    if( aError != KErrNone )
        {
        iPlugin.HandleError( aError );
        }

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCard::VPbkSingleContactOperationFailed()") );    
    }

// End of File
