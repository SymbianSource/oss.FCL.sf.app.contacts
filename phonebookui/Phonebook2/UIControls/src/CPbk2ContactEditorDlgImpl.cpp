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
* Description:  Phonebook 2 contact editor dialog implementation.
*
*/


// INCLUDE FILES
#include "CPbk2ContactEditorDlgImpl.h"

// Phonebook 2
#include "Pbk2UIControls.hrh"
#include "MPbk2ContactEditorStrategy.h"
#include "CPbk2ContactEditorFieldArray.h"
#include "MPbk2ContactEditorField.h"
#include <MPbk2ContactEditorField2.h>
#include "CPbk2AddItemManager.h"
#include "Pbk2TitlePanePictureFactory.h"
#include "TPbk2DeleteItemManager.h"
#include <MPbk2StoreObservationRegister.h>
#include <MPbk2ApplicationServices.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
#include <CPbk2FieldPropertyArray.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactEditorExtension.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2ContactEditorEventObserver.h>
#include <MPbk2EditedContactObserver.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2ContactEditorContactRelocator.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <MPbk2ExitCallback.h>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconFactory.h>
#include "Pbk2RingtoneCommands.h" 
#include "Pbk2ImageCommands.h"
#include "CPbk2ContactEditorUIAddressField.h"
#include "MPbk2ContactEditorUIField.h"
#include "Pbk2ContactEditorFieldFactory.h"
#include "Pbk2AddressTools.h"
#include "MPbk2UIField.h"
#include "CPbk2ContactEditorRingtoneField.h"
#include "CPbk2ContactEditorImageField.h"
#include "CPbk2ContactEditorReadonlyField.h" 
#include "Pbk2EditorLineIds.hrh" 
#include <Pbk2Config.hrh>
#include <Pbk2Commands.hrh>
#include "Pbk2FieldProperty.hrh"
#include "Pbk2IconId.hrh"
#include <Pbk2UID.h>
#include <pbk2mapcommands.hrh>
#include <MPbk2StartupMonitor.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>

#include <VPbkEng.rsg> 
#include <VPbkFieldType.hrh>
#include <VPbkContactStoreUris.h>

// System includes
#include <akntitle.h>
#include <aknnavi.h>
#include <StringLoader.h>
#include <eikcapc.h>
#include <barsread.h>
#include <aknnotewrappers.h>
#include <hlplch.h>
#include <aknnavide.h>
#include <akninputblock.h>
#include <charconv.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KEditorNameFormatFlags = MPbk2ContactNameFormatter::EUseSeparator;
const TInt KTwoBytes = 2;
const TInt KExtraByte = 1;

#ifdef _DEBUG
enum TPanicCode
    {
    EHandleControlStateChangeL_NullField = 1,
    EPanic_IsControlEmptyL_OOB,
    EPanic_EditorField_OOB
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2ContactEditorDlgImpl");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Displays a store not available note.
 *
 * @param aName     Store name.
 */
void ShowStoreNotAvailableNoteL( const TDesC& aName )
    {
    if ( aName.Length() > 0 )
        {
        HBufC* text = StringLoader::LoadLC( R_QTN_PHOB_STORE_NOT_AVAILABLE,
            aName );
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }
    }

/**
 * Decides whether to leave or not in case of an error.
 * Performs some filtering of errors.
 *
 * @param aError    Error code.
 */
void DecideToLeaveL( TInt aError )
    {
    if ( aError != KErrInUse && aError != KErrNone )
        {
        User::Leave( aError );
        }
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CPbk2ContactEditorDlgImpl
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlgImpl::CPbk2ContactEditorDlgImpl
    ( TPbk2ContactEditorParams& aParams,
      CPbk2PresentationContact& aContact,
      CPbk2FieldPropertyArray& aFieldProperties,
      MPbk2EditedContactObserver& aContactObserver,
      MPbk2ContactEditorStrategy& aEditorStrategy,
      MPbk2ContactEditorContactRelocator& aRelocator,
      MPbk2ApplicationServices* aAppServices,
      HBufC* aTitleText ) :
        iParams( aParams ),
        iEditorStrategy( aEditorStrategy ),
        iFieldProperties( aFieldProperties ),
        iContact( aContact ),
        iContactObserver( aContactObserver ),
        iRelocator( aRelocator ),
        iEndKeyWasPressed( EFalse ),
        iAddressViewStandalone( EFalse ),
        iAppServices( aAppServices ),
        iTitleText( aTitleText )
    {
    // Exit is approved by default
    iExitRecord.Set( EExitApproved );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::~CPbk2ContactEditorDlgImpl
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlgImpl::~CPbk2ContactEditorDlgImpl()
    {
    iCoeEnv->RemoveForegroundObserver( *this );

    if ( iWaitFinish && iWaitFinish->IsStarted() )
        {
        iWaitFinish->AsyncStop();
        }
    delete iWaitFinish;
    delete iInputBlock;
    
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }
    // Make the variable in CloseDialog() to ETrue.
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    
    if( iAppServices )
    	{
    	iAppServices->StoreObservationRegister().DeregisterStoreEvents( *this );
    	}
    else
    	{
    	Phonebook2::Pbk2AppUi()->ApplicationServices().
    	        StoreObservationRegister().DeregisterStoreEvents( *this );
    	}
              
    if ( iEikonEnv && iContextMenuBar )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( iContextMenuBar );
        delete iContextMenuBar;
        }

    // Restore the titlepane text
    if (iTitlePane && iStoredTitlePaneText)
        {
        iTitlePane->SetTextL(*iStoredTitlePaneText);
        }

    // Reset title pane picture
    if( iTitlePane )
        {
        iTitlePane->SetSmallPicture( NULL, NULL, EFalse );
        }

    // Remove empty status pane
    if (iNaviContainer)
        {
        iNaviContainer->Pop();
        TRAP_IGNORE(RestorePrevNaviDecoratorL());        
        }

    delete iStoreContact;
    delete iFieldFactory;
    delete iPbk2BaseCommand;
    delete iEditorExtension;
    delete iUiFieldArray;
    Release(iExtensionManager);
	delete iTitleText;    
    iTitleText = NULL;
    delete iStoredTitlePaneText;
    iStoredTitlePaneText = NULL;

    delete iConverter;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlgImpl* CPbk2ContactEditorDlgImpl::NewL(
        TPbk2ContactEditorParams& aParams,
        CPbk2PresentationContact& aContact,
        CPbk2FieldPropertyArray& aFieldProperties,
        MPbk2EditedContactObserver& aContactObserver,
        MPbk2ContactEditorStrategy& aEditorStrategy,
        MPbk2ContactEditorContactRelocator& aRelocator)
    {	
    CPbk2ContactEditorDlgImpl* self =
        new ( ELeave ) CPbk2ContactEditorDlgImpl(aParams, aContact,
            aFieldProperties, aContactObserver,
            aEditorStrategy, aRelocator);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDlgImpl* CPbk2ContactEditorDlgImpl::NewL(
		TPbk2ContactEditorParams& aParams,
		CPbk2PresentationContact& aContact,
		CPbk2FieldPropertyArray& aFieldProperties,
		MPbk2EditedContactObserver& aContactObserver,
		MPbk2ContactEditorStrategy& aEditorStrategy,
		MPbk2ContactEditorContactRelocator& aRelocator,
		MPbk2ApplicationServices* aAppServices,
		HBufC* aTitleText )
	{
	CPbk2ContactEditorDlgImpl* self =
		new ( ELeave ) CPbk2ContactEditorDlgImpl(aParams, aContact,
			aFieldProperties, aContactObserver,
			aEditorStrategy, aRelocator, aAppServices, aTitleText );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ConstructL()
    {
    CAknDialog::ConstructL(R_PBK2_CONTACTEDITOR_MENUBAR);
    iCoeEnv->AddForegroundObserverL( *this );
    
    if(iParams.iActiveView != TPbk2ContactEditorParams::EEditorView)
        {
        iParams.iFocusedContactField = NULL;
        }
    if(iParams.iFocusedContactField)
        {
        const MVPbkFieldType* type = iParams.iFocusedContactField->BestMatchingFieldType();
        const TArray<TVPbkFieldVersitProperty> propArr = type->VersitProperties();
        if(propArr.Count())
            {
            const TVPbkFieldVersitProperty prop = propArr[0];
            if(prop.Name() == EVPbkVersitNameADR)
                {
                if(prop.Parameters().Contains(EVPbkVersitParamHOME))
                    {
                    iParams.iActiveView = TPbk2ContactEditorParams::EEditorAddressHomeView;
                    }
                else if(prop.Parameters().Contains(EVPbkVersitParamWORK))
                    {
                    iParams.iActiveView = TPbk2ContactEditorParams::EEditorAddressOfficeView;
                    }
                else
                    {
                    iParams.iActiveView = TPbk2ContactEditorParams::EEditorAddressView;
                    }
                iAddressViewStandalone = ETrue;
                iParams.iFocusedContactField = NULL;
                }
            }
        }
    
    // Register to listen store events, so that we can close editor if
    // iContact's store becomes unavailable.
    
    /*
	 * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
	 */
    if( iAppServices )
    	{
    	iAppServices->StoreObservationRegister().RegisterStoreEventsL( *this );
    	iNameFormatter = &iAppServices->NameFormatter();
    	}
    else
    	{
    	Phonebook2::Pbk2AppUi()->ApplicationServices().
			StoreObservationRegister().RegisterStoreEventsL( *this );
    	iNameFormatter =
    	        &Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter();
    	}

   
    StoreTitlePaneTextL();
    ConstructContextMenuL();

    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    
    /*
	 * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
	 */
    CVPbkContactManager* manager = NULL;
    if( iAppServices )
    	{
    	manager = &iAppServices->ContactManager();
    	}
    else
    	{
    	manager = &Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager();
    	}
    
    iEditorExtension = iExtensionManager->FactoryL()->
        CreatePbk2ContactEditorExtensionL( *manager, iContact, *this );

    iFieldFactory = CPbk2ContactEditorFieldFactory::NewL(*this, *iEditorExtension);
    iUiFieldArray = CPbk2ContactEditorFieldArray::NewL(
    *manager, iContact, *this, *iFieldFactory, iAppServices );	
    
    iUseState = EUseReady;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ExecuteLD()
    {
    CleanupStack::PushL( this );

    // Set status pane layout to the Phonebook 2 one
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
        {
        statusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }

    CleanupStack::Pop( this );
    CAknForm::ExecuteLD(R_PBK2_EDIT_MEMORY_ENTRY_DIALOG);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CreateCustomControlL
// --------------------------------------------------------------------------
//
SEikControlInfo CPbk2ContactEditorDlgImpl::CreateCustomControlL(
    TInt /*aControlType*/ )
    {
    iLastCreatedCustomControl = NULL;
    SEikControlInfo control = {NULL, 0, 0};
    iLastCreatedCustomControl = new (ELeave) CPbk2ContactEditorReadonlyField();  
    control.iControl = iLastCreatedCustomControl; 
    control.iTrailerTextId = 0;
    control.iFlags = 0;
    return control;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ContactEditorUiBuilderExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactEditorDlgImpl::ContactEditorUiBuilderExtension( 
    TUid /*aExtensionUid*/ )
    {
    //return extension point of MPbk2ContactEditorUiBuilderExtension
    return static_cast<MPbk2ContactEditorUiBuilderExtension*>(this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::AddCustomFieldToFormL
// --------------------------------------------------------------------------
//   
CCoeControl* CPbk2ContactEditorDlgImpl::AddCustomFieldToFormL(
    TInt aIndex,TInt aResourceId)
    {
    InsertLineL(aIndex, aResourceId);
    // Return value is updated in CreateCustomControlL()
    return iLastCreatedCustomControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleCustomFieldCommandL
// --------------------------------------------------------------------------
//   
TBool CPbk2ContactEditorDlgImpl::HandleCustomFieldCommandL(TInt aCommand )
    {
    ProcessCommandL( aCommand );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ConvertCustomControlTypeToBaseControlType()
// Provide correct base type for custom contols
// -----------------------------------------------------------------------------
//
MEikDialogPageObserver::TFormControlTypes CPbk2ContactEditorDlgImpl::
    ConvertCustomControlTypeToBaseControlType(TInt aControlType) const
    {
    MEikDialogPageObserver::TFormControlTypes ret = 
        MEikDialogPageObserver::EUnknownType;
    
    switch(aControlType)
        {
        case EPbk2FieldCtrlTypeImageEditor:
        case EPbk2FieldCtrlTypeRingtoneEditor:
        case EPbk2FieldCtrlTypeExtAddressEditor:
        case EPbk2FieldCtrlTypeExtAddressHomeEditor:
        case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
        case EPbk2FieldCtrlTypeExtAssignFromMapsEditor:
        {
            ret = MEikDialogPageObserver::EEdwinDerived;
            }
            break;
        default:
            {
            ret = MEikDialogPageObserver::EUnknownType;
            }
            break;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandlePointerEventL( 
    const TPointerEvent& aPointerEvent )
    {
    //Here we can prevent recursive execution of ExecuteBaseCommandL if needed 
    //as touch events are handled in it too. So it may case multiple callbacks to here if 
    //user repeats the same pointer event very quickly using options menu.
    // This check is needed, otherwise, when touch quickly on Address field and 
    // quickly on Image field in the Editor Dialog, the address editor will be 
    // opened, but touch input can not open in any of its editors.
    if(!iCommandPending) 
        {
        CAknForm::HandlePointerEventL( aPointerEvent );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandleResourceChange( TInt aType )
    {
    // Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Set status pane layout to the Phonebook 2 one
        CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
        if ( statusPane )
            {
            TRAP_IGNORE(
                    statusPane->SwitchLayoutL(
                            R_AVKON_STATUS_PANE_LAYOUT_USUAL
                            )
            );
            }
        }
    
    CAknForm::HandleResourceChange( aType );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::AddItemToContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::AddItemToContactL
      ( TInt aFieldTypeResourceId,
        const TDesC& aFieldTypeXspName, 
        TBool aSetFocus )
    {
    // Store the original parameter value, since it is
    // changed by AddFieldsL
    TInt origParam = aFieldTypeResourceId;
    TPtrC xSpName = aFieldTypeXspName;

    CPbk2AddItemManager* addItemManager = CPbk2AddItemManager::NewL
            (iContact, *iUiFieldArray, iParams, iAppServices );
    CleanupStack::PushL(addItemManager);
    CPbk2AddItemManager::TReturnValue ret = addItemManager->AddFieldsL(
            aFieldTypeResourceId, xSpName);

    if (ret.iControlId == KErrAlreadyExists)
        {
        // cannot add more than one of this field type
        // find and focus that
        TInt fieldArrayCount( iUiFieldArray->Count() );
        for (TInt i(0); i < fieldArrayCount && ret.iControlId
                == KErrAlreadyExists; ++i)
            {
            if (!iUiFieldArray->At(i).ContactEditorField())
                {
                continue;
                }
            const MVPbkFieldType* fieldType =
                iUiFieldArray->At(i).ContactEditorField()->ContactField().
                    BestMatchingFieldType();
            if(fieldType && fieldType->FieldTypeResId() == aFieldTypeResourceId)
                {
                ret.iControlId = iUiFieldArray->At(i).ControlId();
                }
            }
        }

    if (ret.iControlId > 0)
        {
        if (aSetFocus)
            {
            TryChangeFocusL(ret.iControlId);
            MPbk2ContactEditorField* editorField = iUiFieldArray->Find( ret.iControlId )->ContactEditorField();
            MVPbkStoreContactField& storeContactField = editorField->ContactField();
            iParams.iFocusedContactField = &storeContactField;
            // Call this after adding or deleting lines
            UpdatePageL( ETrue );
            // Must be DrawNow and not DrawDeferred otherwise field label
            // and content of the following field will be incorrect
            DrawNow();
            }
        }
    else if (ret.iControlId == KErrNotSupported && origParam == KErrNotFound)
        {
        iUiFieldArray->SaveFieldsL();
        iRelocator.RelocateContactL(aFieldTypeResourceId, xSpName,
            Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery);
        }
    else if ( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView )
        {
        switch( ret.iGruopId )
            {
            case EPbk2FieldGroupIdPostalAddress:
                if(iUiFieldArray->Count())
                    {
                    iUiFieldArray->AddNewFieldL( ret.iGruopId );
                    ExecuteAddressDlgL( TPbk2ContactEditorParams::EEditorAddressView );
                    }
                break;
            case EPbk2FieldGroupIdHomeAddress:
                iUiFieldArray->AddNewFieldL( ret.iGruopId );
                ExecuteAddressDlgL( TPbk2ContactEditorParams::EEditorAddressHomeView );
                break;
            case EPbk2FieldGroupIdCompanyAddress:
                iUiFieldArray->AddNewFieldL( ret.iGruopId );
                ExecuteAddressDlgL( TPbk2ContactEditorParams::EEditorAddressOfficeView );
                break;
            }
        }
    CleanupStack::PopAndDestroy(addItemManager);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CloseWithoutSaving
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CloseWithoutSaving( TBool aInformObserver )
    {
    CloseDialog( 
        MVPbkContactObserver::EContactOperationUnknown, 
        aInformObserver );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::TryChangeFocusWithTypeIdL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::TryChangeFocusWithTypeIdL
        ( TInt aFieldTypeResId, const TDesC& aFieldTypeXSpName )
    {
    const TInt count( iUiFieldArray->Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        if (!iUiFieldArray->At(i).ContactEditorField())
            {
            continue;
            }
        MPbk2ContactEditorField& uiEditorField =
                *(iUiFieldArray->At(i).ContactEditorField());
        const MVPbkFieldType* fieldType =
            uiEditorField.ContactField().BestMatchingFieldType();
        TInt candidate( KErrNotFound );
        if ( fieldType )
            {
            candidate = fieldType->FieldTypeResId();
            }

        MPbk2FieldProperty2* fieldPropertyExtension =
            reinterpret_cast<MPbk2FieldProperty2*>(
                const_cast<MPbk2FieldProperty&>( uiEditorField.FieldProperty() ).
                    FieldPropertyExtension(
                        KMPbk2FieldPropertyExtension2Uid ) );
        
        if ( fieldPropertyExtension != NULL ) 
            {
            if ( (aFieldTypeResId == candidate ) &&
                    !fieldPropertyExtension->XSpName().CompareF(
                        aFieldTypeXSpName) )
                {
                TryChangeFocusL( uiEditorField.ControlId() );
                MVPbkStoreContactField& storeContactField = uiEditorField.ContactField();
                iParams.iFocusedContactField = &storeContactField;
                // Must be DrawNow and not DrawDeferred otherwise field label
                // and content of the following field will be incorrect
                DrawNow();
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2ContactEditorDlgImpl::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
     TKeyResponse ret(EKeyWasNotConsumed);    

     if(iAddressView)
         {
         return ret;
         }
     
     /* Uncomment this code if it is necessary to enable navigation hardware keys
      * on devices which have these keys (its Map Extension feature) 
     if( aKeyEvent.iScanCode == EStdKeyApplicationD && aType == EEventKeyUp )
         {
         HandleCustomFieldCommandL(EPbk2ExtensionAssignFromMap);
         return EKeyWasConsumed;
         }
             
     if( aKeyEvent.iScanCode == EStdKeyApplicationE && aType == EEventKeyUp )
         {
         HandleCustomFieldCommandL(EPbk2ExtensionShowOnMap);
         return EKeyWasConsumed;
         }*/

    // Display menu bar if the focused control doesnt consume selection keys
    CPbk2ContactEditorArrayItem* currentField = iUiFieldArray->Find(
            IdOfFocusControl());

    if (currentField)
        {
        CheckCurrentFieldTextL( currentField, aKeyEvent, aType );
    
        if (currentField->ContactEditorField()
                && currentField->ContactEditorField()->ConsumesKeyEvent(
                        aKeyEvent, aType))
            {
            ret = CAknForm::OfferKeyEventL(aKeyEvent, aType);
            }
        else if (currentField->ContactEditorUIField()
                && currentField->ContactEditorUIField()->ConsumesKeyEvent(
                        aKeyEvent, aType))
            {
            ret = CAknForm::OfferKeyEventL(aKeyEvent, aType);
            }
        }

    if (ret != EKeyWasConsumed && aType == EEventKey && aKeyEvent.iCode
            == EKeyOK)
        {
        iContextMenuBar->TryDisplayMenuBarL();
        ret = EKeyWasConsumed;
        }

    if ( ret != EKeyWasConsumed && !iEndKeyWasPressed )
        {
        if ( aKeyEvent.iCode == EKeyEscape )
            {
            iEditorStrategy.StopQuery();
            iEndKeyWasPressed = ETrue;
            }
        ret = CAknForm::OfferKeyEventL( aKeyEvent, aType );
        if ( ret == EKeyWasConsumed && 
            aKeyEvent.iCode != EKeyPhoneEnd && aKeyEvent.iCode != EKeyEscape )
            {
            UpdateCbasL();
            }        
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::GetHelpContext
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::GetHelpContext
        ( TCoeHelpContext& aContext ) const
    {
    if (iParams.iHelpContext)
        {
        // If external help context has been set, return it..
        aContext = *iParams.iHelpContext;
        }
    else
        {
        // ..otherwise go with the default context
        iEditorStrategy.GetHelpContext(aContext);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::PreLayoutDynInitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::PreLayoutDynInitL()
    {
    SetEditableL(ETrue);

    if (!ContactHasAddress(EPbk2FieldGroupIdPostalAddress)
            && !ContactHasAddress(EPbk2FieldGroupIdHomeAddress)
            && !ContactHasAddress(EPbk2FieldGroupIdCompanyAddress))
        {
        AddGroupFieldsL(EPbk2FieldGroupIdPostalAddress);
        }
    
    if(iAddressViewStandalone)
        {
        AddGroupFieldsL(Pbk2AddressTools::MapViewTypeToAddress(iParams.iActiveView));
        }

    iUiFieldArray->CreateFieldsFromContactL(iParams);
    SetAddressValidationIconsL();

    // Offer button group container to extensions.
    // This is needed here because previous settings might override extension's
    // buttons.
    iEditorExtension->ModifyButtonGroupContainerL(ButtonGroupContainer());

    ConstructNaviPaneL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::PostLayoutDynInitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::PostLayoutDynInitL()
    {
    UpdateTitleL();
    UpdateTitlePictureL();
    if(iParams.iFocusedContactField)
        {
        const TInt count = iUiFieldArray->Count();
        for (TInt i = 0; i < count; ++i)
            {
                if(iUiFieldArray->At(i).ContactEditorField())
                {
                MVPbkBaseContactField & field =
                        iUiFieldArray->At(i).ContactEditorField()->ContactField();
                if (field.IsSame(*iParams.iFocusedContactField))
                    {
                    UpdateCbasL(&(iUiFieldArray->At(i)));
                    break;
                    }
                }
            }
        }
    else if(iUiFieldArray->Count())
        {
        UpdateCbasL(&(iUiFieldArray->At(0)));
        }
    
    // Notify relocator, that editor is ready
    iRelocator.EditorReadyL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetInitialCurrentLine
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetInitialCurrentLine()
    {
    TRAPD( err, ActivateFirstPageL() );
    if(err)
        {
        return;
        }

    TInt focusedIndex = 0;
    if ( iParams.iFocusedContactField )
        {
        const TInt count = iUiFieldArray->Count();
        for (TInt i = 0; i < count; ++i)
            {
                if (iUiFieldArray->At(i).ContactEditorField())
                {
                MVPbkBaseContactField & field =
                        iUiFieldArray->At(i).ContactEditorField()->ContactField();
                if (field.IsSame(*iParams.iFocusedContactField))
                    {
                    focusedIndex = i;
                    break;
                    }
                }
            }
        CAknForm::SetInitialCurrentLine();
        iUiFieldArray->SetFocus(focusedIndex);
        }
    else
        {
        CAknForm::SetInitialCurrentLine();
        iUiFieldArray->SetFocus(focusedIndex);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ProcessCommandL(TInt aCommandId)
    {
    HideMenu();

    iContextMenuBar->StopDisplayingMenuBar();

    // Extensions get the command first
    // If decide to exit already, do not handle new commands until this exit cmd clear
    if (!iEditorExtension->ProcessCommandL(aCommandId) &&
        !CheckBaseCommandL(aCommandId) &&
        !CheckFieldSpecificCommandL(aCommandId) &&
        iExitRecord.IsClear( EExitOrdered ) )
        {
        switch (aCommandId)
            {
            case EPbk2CmdAddItem:
                {
                CmdAddItemL();
                break;
                }

            case EPbk2CmdDeleteItem:
                {
                CmdDeleteItemL();
                break;
                }

            case EPbk2CmdEditItemLabel:
                {
                CmdEditItemLabelL();
                break;
                }
#ifdef ECE_DISABLE_CONTEXT_MENU
            case EPbk2CmdEditorContextMenu:
                {
                iContextMenuBar->TryDisplayMenuBarL();
                break;
                }
#endif
            case EEikCmdExit:
            case EAknCmdExit:
                {
                // Exit Phonebook and if existing save 
                // contact (happens later on in OkToExit..)
                
                // Escape or end key pressed.
                // We must save changes and exit the application
                
                if( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView || iAddressViewStandalone )
                    {
                    if (!iExitRecord.IsSet(EOkToExitWithoutHandlingIt))
                        {
                        if( OkToExitApplicationL( EAknCmdExit ) )
                            {
                            iExitRecord.Set(EExitOrdered);
                            // Remove empty status pane
                            if (iNaviContainer)
                                {
                                iNaviContainer->Pop();
                                RestorePrevNaviDecoratorL();
                                iNaviContainer = NULL;
                                }
    
                            MPbk2ContactEditorEventObserver::TParams params;
                            params.iFlags = EPbk2EditorCommandId;
                            params.iKeyCode = EKeyEscape;
                            params.iCommandId = EAknCmdExit;
                            
                            CmdDoneL(params);
                            if ( iUseState == EUseOperatingContact ) 
                                 {
                                 WaitFinishL();
                                 }
                            }
                        }
                    break;    
                    }
                else
                    {
                    if (!iExitRecord.IsSet(EOkToExitWithoutHandlingIt))
                        {
                        iExitRecord.Set(EOkToExitWithoutHandlingIt);
                        OkToExitApplicationL(EAknCmdExit);
                            {
                            // Remove empty status pane
                            if (iNaviContainer)
                                {
                                iNaviContainer->Pop();
                                RestorePrevNaviDecoratorL();
                                iNaviContainer = NULL;
                                }
    
                            MPbk2ContactEditorEventObserver::TParams params;
                            params.iFlags = EPbk2EditorKeyCode;
                            params.iKeyCode = EKeyEscape;
                            params.iCommandId = EAknCmdExit;
                            
                            CmdDoneL(params);
                            if ( iUseState == EUseOperatingContact ) 
                                 {
                                 WaitFinishL();
                                 }
                            }
                        }
                    }                                
                
                break;
                }
            case EAknCmdHelp:
                {
                HlpLauncher::LaunchHelpApplicationL(
                        CCoeEnv::Static()->WsSession(),
                        iEikonEnv->EikAppUi()->AppHelpContextL());
                break;
                }
            default:
                {
                // Command not handled here, forward it to the app ui.
                iEikonEnv->EikAppUi()->HandleCommandL(aCommandId);
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CheckBaseCommandL
// Check whether there is a base command to execute
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::CheckBaseCommandL(TInt aCommandId)
    {
    TBool ret(ETrue);
    if (!iCommandPending)
        {
        //Prevent recursive execution of ExecuteBaseCommandL as touch events are  
        //handled in it too. So it causes multiple callback to here if 
        //user repeats the same pointer event very quickly.
        iCommandPending = ETrue;

        if (iPbk2BaseCommand)
            {
            //Cancel and delete previous possible pending command. 
            delete iPbk2BaseCommand;
            iPbk2BaseCommand = NULL;
            }
        TRAPD(err, ret = ExecuteBaseCommandL(aCommandId));
        iCommandPending = EFalse;
        User::LeaveIfError(err);
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ExecuteBaseCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::ExecuteBaseCommandL(TInt aCommandId)
    {
    TBool executingCommand(ETrue);

    switch (aCommandId)
        {
        case EPbk2CmdEditorChangeRingtone:
            {
            iPbk2BaseCommand = Pbk2RingtoneCommands::SetRingtoneL(
                    &iContact.StoreContact());
            break;
            }

        case EPbk2CmdEditorRemoveRingtone:
            {
            iPbk2BaseCommand = Pbk2RingtoneCommands::RemoveRingtoneL(
                    &iContact.StoreContact());
            break;
            }

        case EPbk2CmdEditorSelectAddress:
            {
            ExecuteAddressDlgL(TPbk2ContactEditorParams::EEditorAddressView);
            return executingCommand;
            }

        case EPbk2CmdEditorSelectAddressHome:
            {
            ExecuteAddressDlgL(TPbk2ContactEditorParams::EEditorAddressHomeView);
            return executingCommand;
            }

        case EPbk2CmdEditorSelectAddressOffice:
            {
            ExecuteAddressDlgL(
                    TPbk2ContactEditorParams::EEditorAddressOfficeView);
            return executingCommand;
            }

        case EPbk2CmdEditorOpen:
            {
            CPbk2ContactEditorArrayItem* currentField = iUiFieldArray->Find(
                    IdOfFocusControl());

            if (currentField->ContactEditorUIField())
                {
                TPbk2ContactEditorParams::TActiveView
                        viewType =
                                Pbk2AddressTools::MapCtrlTypeToViewType(
                                        currentField->ContactEditorUIField()->UIField()->CtrlType());
                if (viewType != TPbk2ContactEditorParams::EEditorUndefinied)
                    {
                    ExecuteAddressDlgL(viewType);
                    return executingCommand;
                    }
                }
            break;
            }
        case EPbk2CmdEditorSelectAssignFromMaps:
            {
            HandleCustomFieldCommandL(EPbk2ExtensionAssignFromMap);
            break;
            }

        case EPbk2CmdEditorAddImage:
        case EPbk2CmdEditorChangeImage:
            {
            iPbk2BaseCommand = Pbk2ImageCommands::SetImageL(
                &iContact,this);
            break;
            }

        case EPbk2CmdEditorViewImage:
            {
            iPbk2BaseCommand = Pbk2ImageCommands::ViewImageL(
                    &iContact.StoreContact());
            break;
            }

        case EPbk2CmdEditorRemoveImage:
            {
            iPbk2BaseCommand = Pbk2ImageCommands::RemoveImageL(&iContact);
            break;
            }

        default:
            {
            executingCommand = EFalse;
            break;
            }
        }

    if (executingCommand)
        {
        CPbk2ContactEditorArrayItem* currentField = iUiFieldArray->Find(
                IdOfFocusControl());

        if (currentField && currentField->ContactEditorField())
            {
            TInt id = currentField->ControlId();
            TAny
                    * ext =
                            currentField->ContactEditorField()->ContactEditorFieldExtension(
                                    TUid::Uid(NULL));
            if (id == EPbk2EditorLineRingtone)
                {
                CPbk2ContactEditorRingtoneField* fld =
                        static_cast<CPbk2ContactEditorRingtoneField*> (ext);
                fld->SetTextL();
                }
            else if( id == EPbk2EditorLineImage && 
                     aCommandId == EPbk2CmdEditorRemoveImage)
                {
                //If image removed, no callback to ImageLoadingComplete() so do it here
                DoImageLoadingCompleteL();
                }
            }
        }

    return executingCommand;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SwitchView
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ExecuteAddressDlgL(
        TPbk2ContactEditorParams::TActiveView aActiveView)
    {
    if (iParams.iActiveView == TPbk2ContactEditorParams::EEditorView)
        {
        AddGroupFieldsL(Pbk2AddressTools::MapViewTypeToAddress(aActiveView));
        }

    iParams.iActiveView = aActiveView;
    
    CPbk2ContactEditorDlgImpl* adressDlg = CPbk2ContactEditorDlgImpl::NewL(
            iParams, iContact, iFieldProperties, iContactObserver,
            iEditorStrategy, iRelocator, iAppServices );
    
    iAddressView = adressDlg;
    adressDlg->ExecuteLD();
    iAddressView = NULL;
    iParams.iActiveView = TPbk2ContactEditorParams::EEditorView;
    UpdateControlsL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::AddAddressFields
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::AddGroupFieldsL(TPbk2FieldGroupId aGroupId)
    {
    if (aGroupId == EPbk2FieldGroupIdNone)
        return;

    TInt count = iFieldProperties.Count();
    for (TInt idx = 0; idx < count; idx++)
        {
        const MPbk2FieldProperty& fieldProp = iFieldProperties.At(idx);
        if (fieldProp.GroupId() == aGroupId)
            {
            AddItemToContactL(fieldProp.FieldType().FieldTypeResId(),
                    KNullDesC(), EFalse);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::AreAllFieldsEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::AreAllFieldsEmpty()
    {
    TBool result(ETrue);
    TInt countAll = iContact.PresentationFields().FieldCount();
    for (TInt idx = 0; idx < countAll; idx++)
        {
        CPbk2PresentationContactField& field =
                iContact.PresentationFields().At(idx);
        
        MVPbkContactFieldData& data = field.FieldData();
        TInt fld = field.FieldProperty().FieldType().FieldTypeResId();        
        if(fld != R_VPBK_FIELD_TYPE_RINGTONE && 
           fld != R_VPBK_FIELD_TYPE_THUMBNAILPIC &&
           fld != R_VPBK_FIELD_TYPE_CALLEROBJIMG &&
           fld != R_VPBK_FIELD_TYPE_SYNCCLASS && 
           fld != R_VPBK_FIELD_TYPE_TOPCONTACT) 
            {                   
            if(!data.IsEmpty())
                {
                if(data.DataType() == EVPbkFieldStorageTypeText)
                    {
                    HBufC* fieldText = MVPbkContactFieldTextData::Cast(data).Text().AllocL();
                    fieldText->Des().TrimAll();                    
                    if(fieldText->Length() > 0)
                        {
                        result = EFalse;
                        }
                    delete fieldText;                      
                    }
                else                
                    {
                    result = EFalse;
                    }
                }
            }           
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ContactHasAddress
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::ContactHasAddress(
        TPbk2FieldGroupId aAddressGroup)
    {
    TInt countAll = iContact.PresentationFields().FieldCount();
    for (TInt idx = 0; idx < countAll; idx++)
        {
        CPbk2PresentationContactField& field =
                iContact.PresentationFields().At(idx);
        if (field.FieldProperty().GroupId() == aAddressGroup)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ClearControls
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ClearControls()
    {
    TInt count = iUiFieldArray->Count();
    for (TInt idx = count - 1; idx >= 0; idx--)
        {
        iUiFieldArray->RemoveFieldFromUI(idx);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ContactEditorControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactEditorDlgImpl::ContactEditorControlExtension(TUid /*aExtensionUid*/)
    {
    return static_cast<MPbk2ContactEditorControlExtension*> (this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::UpdateControlsL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::UpdateControlsL()
    {
    iUiFieldArray->UpdateControlsL();
    SetAddressValidationIconsL();
    UpdatePageL(ETrue);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CheckFieldSpecificCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::CheckFieldSpecificCommandL(TInt aCommandId)
    {
    TBool ret(EFalse);
    CPbk2ContactEditorArrayItem* currentField = iUiFieldArray->Find(
            IdOfFocusControl());

    if (currentField)
        {
        if (currentField->ContactEditorUIField())
            {
            ret = currentField->ContactEditorUIField()->HandleCustomFieldCommandL(
                        aCommandId);
            }
        else
            {
            MPbk2ContactEditorField2* tempCurrentField =
                reinterpret_cast<MPbk2ContactEditorField2*> 
                    (currentField->ContactEditorField()->ContactEditorFieldExtension(
                        KMPbk2ContactEditorFieldExtension2Uid));
            if (tempCurrentField)
                {
                ret = tempCurrentField->HandleCustomFieldCommandL(aCommandId);
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    iEditorExtension->DynInitMenuPaneL(aResourceId, aMenuPane);

    if (aResourceId == R_PBK2_CONTACTEDITOR_MENUPANE ||
        aResourceId == R_PBK2_CONTACTEDITOR_CONTEXT_MENUPANE)
        {
        //Label editing not anymore supported. Remove later all code
        //related to editing labels
        DimItem( aMenuPane,  EPbk2CmdEditItemLabel);
        // Filter delete item away from menu for first and last name fields
        CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
                IdOfFocusControl());
        MPbk2ContactEditorField* currentField = current->ContactEditorField();
        if (currentField)
            {
            if (currentField->FieldProperty().Flags()
                    & KPbk2FieldFlagCanNotBeRemoved)
                {
                DimItem(aMenuPane, EPbk2CmdDeleteItem);
                }
            //Label editing not anymore supported. Remove later all code
            //related to editing labels
            //MVPbkStoreContactField& field = currentField->ContactField();
            //if (!field.SupportsLabel())
            //    {
            //    DimItem( aMenuPane,  EPbk2CmdEditItemLabel);                
            //    }
            //else
            //    {
            //    TVPbkFieldVersitProperty prop;
            //    prop.SetName(EVPbkVersitNameIMPP);
            //    if (field.BestMatchingFieldType()->Matches(prop, 0))
            //        {
            //        DimItem( aMenuPane,  EPbk2CmdEditItemLabel);                
            //        }
            //    }
            DynInitRingtoneL(aResourceId, aMenuPane, *currentField);
            DynInitImageL(aResourceId, aMenuPane, *currentField);
            DynInitSyncL(aResourceId, aMenuPane, *currentField);
            }
        else
            {
            DimItem(aMenuPane, EPbk2CmdDeleteItem);
            DimItem(aMenuPane, EPbk2CmdEditorChangeRingtone);
            DimItem(aMenuPane, EPbk2CmdEditorRemoveRingtone);
            DimItem(aMenuPane, EPbk2CmdEditorAddImage);
            DimItem(aMenuPane, EPbk2CmdEditorViewImage);
            DimItem(aMenuPane, EPbk2CmdEditorChangeImage);
            DimItem(aMenuPane, EPbk2CmdEditorRemoveImage);
            DimItem(aMenuPane, EPbk2CmdEditorChangeSync);
            }

        DynInitAddressL(aResourceId, aMenuPane);
        // If there are no available fields to add, do not show
        // the add item dialog. If phone memory is in configuration
        // this check is not needed.
        if (!iRelocator.IsPhoneMemoryInConfigurationL())
            {
            CArrayPtr<const MPbk2FieldProperty>* availableFields =
                    iContact.AvailableFieldsToAddL();
            if (!availableFields || availableFields->Count() == 0)
                {
                DimItem(aMenuPane, EPbk2CmdAddItem);
                }
            delete availableFields;
            }

        if (aResourceId == R_PBK2_CONTACTEDITOR_MENUPANE)
            {
            // Hide exit, if wanted. Exit command is available only in
            // main menu.
            if (iParams.iFlags & TPbk2ContactEditorParams::EHideExit)
                {
                DimItem(aMenuPane, EAknCmdExit);
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DynInitRingtoneL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DynInitRingtoneL(
    TInt /*aResourceId*/, 
    CEikMenuPane* aMenuPane, 
    MPbk2ContactEditorField& aCurrentField )
    {
    TInt id = aCurrentField.FieldProperty().FieldType().FieldTypeResId();  
    
    if(id != R_VPBK_FIELD_TYPE_RINGTONE)
        {
        DimItem( aMenuPane,  EPbk2CmdEditorChangeRingtone);
        DimItem( aMenuPane,  EPbk2CmdEditorRemoveRingtone);
        }
    else 
        {
        if( aCurrentField.ContactField().FieldData().DataType() != 
            EVPbkFieldStorageTypeText || 
            aCurrentField.ContactField().FieldData().IsEmpty()  )
            {
            DimItem( aMenuPane,  EPbk2CmdEditorRemoveRingtone);
            }
        //Label editing not anymore supported (is dimmed in DynInitMenuPaneL)
        //DimItem( aMenuPane,  EPbk2CmdEditItemLabel);
        DimItem( aMenuPane,  EPbk2CmdDeleteItem);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DynInitImageL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DynInitImageL(
    TInt /*aResourceId*/, 
    CEikMenuPane* aMenuPane, 
    MPbk2ContactEditorField& aCurrentField )
    {
    TInt id = aCurrentField.FieldProperty().FieldType().FieldTypeResId(); 
    
    if( id != R_VPBK_FIELD_TYPE_CALLEROBJIMG )
        {
        DimItem( aMenuPane,  EPbk2CmdEditorAddImage );
        DimItem( aMenuPane,  EPbk2CmdEditorViewImage);
        DimItem( aMenuPane,  EPbk2CmdEditorChangeImage);
        DimItem( aMenuPane,  EPbk2CmdEditorRemoveImage);
        }
    else 
        {
        if( aCurrentField.ContactField().FieldData().DataType() == 
            EVPbkFieldStorageTypeText && 
            !aCurrentField.ContactField().FieldData().IsEmpty() )
            {
            DimItem( aMenuPane, EPbk2CmdEditorAddImage );
            
            //Dim 'View image' if currently cold file name, e.g mem card not there.
            TBool dimView(ETrue);
            TPbk2FieldCtrlType type = aCurrentField.FieldProperty().CtrlType();
            TAny* ext = aCurrentField.ContactEditorFieldExtension(TUid::Uid(NULL));
            if (type == EPbk2FieldCtrlTypeImageEditor && ext)
                {
                CPbk2ContactEditorImageField* fld =
                        static_cast<CPbk2ContactEditorImageField*> (ext);
                TPtrC dataPtr(MVPbkContactFieldTextData::Cast(  
                        fld->ContactField().FieldData()).Text());
                TEntry entry;
                RFs& fs( iCoeEnv->FsSession() );        
                dimView = fs.Entry( dataPtr, entry ) == 0 ? EFalse : ETrue;
                }        
            if( dimView )
                {
                DimItem( aMenuPane,  EPbk2CmdEditorViewImage);
                }
            }
        else
            {
            DimItem( aMenuPane,  EPbk2CmdEditorViewImage);                    
            DimItem( aMenuPane,  EPbk2CmdEditorChangeImage);
            DimItem( aMenuPane,  EPbk2CmdEditorRemoveImage);
            }

        //Label editing not anymore supported (is dimmed in DynInitMenuPaneL)        
        //DimItem( aMenuPane,  EPbk2CmdEditItemLabel);
        DimItem( aMenuPane,  EPbk2CmdDeleteItem);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DynInitSyncL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DynInitSyncL(
    TInt /*aResourceId*/, 
    CEikMenuPane* aMenuPane, 
    MPbk2ContactEditorField& aCurrentField )
    {
    TInt id = aCurrentField.FieldProperty().FieldType().FieldTypeResId();  
    
    // If the selection popup is open, hide menu option "Change" 
    if( id == R_VPBK_FIELD_TYPE_SYNCCLASS ) 
        {
        CEikCaptionedControl* line=CurrentLine();  
        if ( line && (line->iControlType == EAknCtPopupField || line->iControlType == EAknCtPopupFieldText ) )
            {
            CAknPopupField* ctrl = static_cast<CAknPopupField*>( line->iControl );
            if ( ctrl && ctrl->NumLines() > 1 ) 
                {
                DimItem( aMenuPane, EPbk2CmdEditorChangeSync);
                }
            }
        }
    else  // id != R_VPBK_FIELD_TYPE_SYNCCLASS
        { 
        DimItem( aMenuPane,  EPbk2CmdEditorChangeSync);
        }
    
    }
// CPbk2ContactEditorDlgImpl::DynInitAddressL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DynInitAddressL(TInt /*aResourceId*/,
        CEikMenuPane* aMenuPane)
    {
    CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
            IdOfFocusControl());
    if (iParams.iActiveView == TPbk2ContactEditorParams::EEditorView)
        {
        DimItem(aMenuPane, EPbk2ExtensionAssignFromMap);
        if (current && current->ContactEditorUIField()
                && Pbk2AddressTools::MapCtrlTypeToAddress(
                        current->ContactEditorUIField()->UIField()->CtrlType())
                        != EPbk2FieldGroupIdNone)
            {
            UnDimItem(aMenuPane, EPbk2CmdEditorOpen);
            UnDimItem(aMenuPane, EPbk2CmdDeleteItem); 
            }
        else
            {
            DimItem(aMenuPane, EPbk2CmdEditorOpen);
            }
        }
    else
        {
        DimItem(aMenuPane, EPbk2CmdAddItem);
        DimItem(aMenuPane, EPbk2CmdEditorOpen);
        DimItem(aMenuPane, EPbk2CmdDeleteItem); 
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DimItem
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DimItem(CEikMenuPane* aMenuPane, TInt aCmd)
    {
    TInt pos;
    if (aMenuPane->MenuItemExists(aCmd, pos))
        {
        aMenuPane->SetItemDimmed(aCmd, ETrue);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::UnDimItem
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::UnDimItem(CEikMenuPane* aMenuPane, TInt aCmd)
    {
    TInt pos;
    if (aMenuPane->MenuItemExists(aCmd, pos))
        {
        aMenuPane->SetItemDimmed(aCmd, EFalse);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::OkToExitL(TInt aKeycode)
    {
    TBool ret = EFalse;
    if( iCommandPending )
        {
        //When command in process prevent further processing
        ret = EFalse;
        }
    else if ( iExitRecord.IsSet( EOkToExitWithoutHandlingIt ) )
        {
        ret = ETrue;
        }
    else if ( aKeycode == EAknSoftkeyDone )
        {
        // Done key pressed
        MPbk2ContactEditorEventObserver::TParams params;
        params.iFlags = EPbk2EditorKeyCode;
        params.iKeyCode = aKeycode;
        CmdDoneL( params );
        }
    else if (aKeycode == EKeyEscape || aKeycode == EEikBidCancel)
        {
        // this happens when service is canceled, so in this case, 
        // do nothing but give ret ETrue, so that this object
        // is able to be deleted later.
        if( iServiceCanceled )
            {
            ret = ETrue;
            }
        // Escape or end key pressed.
        // We must save changes and exit the applicatio
        else if( !iExitRecord.IsSet( EExitOrdered ) )
            {
            // Remove empty status pane
            if (iNaviContainer)
                {
                 iNaviContainer->Pop();
                 RestorePrevNaviDecoratorL();
                 iNaviContainer = NULL;
                 }
            MakeVisible( EFalse );
            MPbk2ContactEditorEventObserver::TParams params;
            params.iFlags = EPbk2EditorKeyCode;
            params.iKeyCode = aKeycode;

            // If the application is not allowed to hide itself in the
            // background, end of the dialog is also end of the 
            // application. 
            if ( !iAvkonAppUi->ExitHidesInBackground() )
                {
                iExitRecord.Set( EExitOrdered );
                
                if( OkToExitApplicationL( EAknCmdExit ) )
                    {
                    params.iCommandId = EAknCmdExit;
                    }
                }
            
            CmdDoneL( params );
            }
        }
    else if (aKeycode == EAknSoftkeyContextOptions)
        {
        // Launch context menu if the control is not popup control
        CEikCaptionedControl* line = CurrentLine();
        TInt type = line->iControlType;
        TBool isPopUp = ( type == EAknCtPopupFieldText ) ||
            ( type == EAknCtPopupField );
        if (isPopUp)
            {
            CAknPopupField* ctrl =
                static_cast<CAknPopupField*>( line->iControl );
            ctrl->ActivateSelectionListL();
            }
        else
            {
            CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
                    IdOfFocusControl());
            if (current && current->ContactEditorUIField())
                {
                TPbk2ContactEditorParams::TActiveView
                        viewType =
                                Pbk2AddressTools::MapCtrlTypeToViewType(
                                        current->ContactEditorUIField()->UIField()->CtrlType());
                if (viewType != TPbk2ContactEditorParams::EEditorUndefinied)
                    {
                    ExecuteAddressDlgL(viewType);
                    return ret;
                    }
                else if (current->ContactEditorUIField()->UIField()->CtrlType()
                        == EPbk2FieldCtrlTypeExtAssignFromMapsEditor)
                    {
                    HandleCustomFieldCommandL(EPbk2ExtensionAssignFromMap);
                    return ret;
                    }
                }

            iContextMenuBar->SetMenuTitleResourceId(
                    R_PBK2_CONTACTEDITOR_CONTEXT_MENUBAR);
            iContextMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
            iContextMenuBar->StopDisplayingMenuBar();
            iContextMenuBar->TryDisplayMenuBarL();
            iContextMenuBar->SetMenuTitleResourceId
                ( R_PBK2_CONTACTEDITOR_MENUBAR );
            iContextMenuBar->SetMenuType( CEikMenuBar::EMenuOptions );
            }
        }
    else if( aKeycode == EAknSoftkeyChange ) 
        {
        //For custom controls (ringtone, image) msk middle key press need to be 
        //caught here
        CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
                IdOfFocusControl());
        MPbk2ContactEditorField* currentField = NULL;
        if (current)
            {
            currentField = current->ContactEditorField();
            }

        if (currentField)
            {
            TInt id = currentField->ControlId();
            if (id == EPbk2EditorLineRingtone || id == EPbk2EditorLineImage)
                {
                MPbk2ContactEditorField2
                        * tempCurrentField =
                                reinterpret_cast<MPbk2ContactEditorField2*> (currentField->ContactEditorFieldExtension(
                                        KMPbk2ContactEditorFieldExtension2Uid));
                if (tempCurrentField)
                    {
                    tempCurrentField->HandleCustomFieldCommandL(
                            EPbk2CmdEditorHandleCustomSelect);
                    }
                }
            else
                {
                //For non-custom fields (sync...) forward to framework 
                ForwardOkToExitL( aKeycode );
                }
            }
        }
    else
        {
        ForwardOkToExitL( aKeycode );
        }

    // Clear the exit state after usage.
    iExitRecord.Clear( EOkToExitWithoutHandlingIt );
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ForwardOkToExitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ForwardOkToExitL( TInt aKeycode )
    {
    //Prevent further execution of menu commands while a command is running.  
    //In touch tapping RSK very quickly for ringtone gets otherwise through 
    //when ringtone fetch command is running.
    TBool okToSkip = iCommandPending && aKeycode != EAknSoftkeyDone && 
                     aKeycode != EAknCmdExit && aKeycode != EAknCmdExit;  
    if(!okToSkip)
        {
        CAknForm::OkToExitL( aKeycode );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleControlStateChangeL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandleControlStateChangeL(TInt aControlId)
    {
    CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
            aControlId);
    MPbk2ContactEditorField* changedField = current->ContactEditorField();
    __ASSERT_DEBUG(changedField, Panic(EHandleControlStateChangeL_NullField));
    if (changedField && iNameFormatter->IsTitleField(
            changedField->ContactField()))
        {
        UpdateTitleL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CreateLineL
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2ContactEditorDlgImpl::CreateLineL
        ( const TDesC& aCaption, TInt aControlId, TInt aControlType )
    {
    return CreateLineByTypeL( aCaption, aControlId, aControlType, NULL );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::LineControl
// --------------------------------------------------------------------------
//
CEikCaptionedControl* CPbk2ContactEditorDlgImpl::LineControl
        ( TInt aControlId ) const
    {
    return Line( aControlId );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DeleteControl
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DeleteControl(TInt aControlId)
    {
    DeleteLine( aControlId );
    TRAP_IGNORE( UpdateTitleL() );    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::TryChangeFocusL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::TryChangeFocusL(TInt aControlId)
    {
    //Ensure current line is set before trying to change focus
    if(!CurrentLine())
        {
        SetInitialCurrentLine(); 
        }

    //Attempt setting the focus
    if ( ControlOrNull( aControlId ) )
        {
        TryChangeFocusToL( aControlId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetCurrentLineCaptionL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetCurrentLineCaptionL(const TDesC& aText)
    {
    SetControlCaptionL(IdOfFocusControl(), aText);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::Control
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2ContactEditorDlgImpl::Control(TInt aControlId) const
    {
    return ControlOrNull(aControlId);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetEditableL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetEditableL( TBool aState )
    {
    CAknForm::SetEditableL( aState );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::LoadBitmapToFieldL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::LoadBitmapToFieldL
        ( const MPbk2FieldProperty& aFieldProperty,
          const CPbk2IconInfoContainer& aIconInfoContainer,
          TInt aControlId )
    {
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( aIconInfoContainer );
    CEikImage* image = factory->CreateImageLC( aFieldProperty.IconId() );
    
    if ( image )
        {
        image->SetPictureOwnedExternally( EFalse );
        image->CopyControlContextFrom( LineControl( aControlId ) );
        image->SetContainerWindowL( *LineControl( aControlId ) );
        image->SetNonFocusing();
        image->SetBrushStyle( CGraphicsContext::ENullBrush );

        delete LineControl( aControlId )->iBitmap;
        LineControl( aControlId )->iBitmap = image;
        CleanupStack::Pop( image );

        TRect rect = LineControl( aControlId )->Rect();
        rect.Resize( -1, -1 );
        LineControl( aControlId )->SetRect( rect );
        rect.Resize( 1, 1 );
        LineControl( aControlId )->SetRect( rect );
        }

    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ContactEditorOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ContactEditorOperationCompleted
        ( MVPbkContactObserver::TContactOpResult aResult,
          MPbk2ContactEditorEventObserver::TParams aParams )
    {
    iUseState = EUseFinished;
    
    // Notify extensions
    iEditorExtension->ContactEditorOperationCompleted
        ( aResult, aParams );

    switch ( aResult.iOpCode )
        {
        case MVPbkContactObserver::EContactOperationUnknown: // FALLTHROUGH
            {
            CloseDialog( aResult.iOpCode, ETrue );
            break;
            }
        case MVPbkContactObserver::EContactCommit: // FALLTHROUGH
        case MVPbkContactObserver::EContactDelete:
            {
            if ( aParams.iFlags & EPbk2EditorKeyCode &&
                 aParams.iKeyCode == EAknSoftkeyDone )
                {
                CloseDialog( aResult.iOpCode, ETrue );
                }
            else if ( ( aParams.iFlags & EPbk2EditorKeyCode &&
                        aParams.iKeyCode == EEikBidCancel ) ||
                     ( aParams.iFlags & EPbk2EditorCommandId &&
                       aParams.iCommandId == EEikBidCancel ) ) 
                {
                if( iExitRecord.IsSet( EExitOrdered ) )
                    {
                    ExitApplication( aResult.iOpCode, EAknCmdExit );
                    }
                else
                    {
                    CloseDialog( aResult.iOpCode, ETrue );
                    }
                }
            else if ( ( aParams.iFlags & EPbk2EditorCommandId ) &&
                        ( ( aParams.iCommandId == EEikCmdExit ) ||
                        ( aParams.iCommandId == EAknCmdExit ) ) )
                {
                ExitApplication( aResult.iOpCode, aParams.iCommandId );
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CloseDialog
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CloseDialog()
	{
    // Ignor error and close editor without further user interaction
    MVPbkContactObserver::TContactOpResult exitResult;
    exitResult.iExtension = NULL;
    exitResult.iStoreContact = NULL;
    exitResult.iOpCode = MVPbkContactObserver::EContactDelete;
    MPbk2ContactEditorEventObserver::TParams exitParams;
    if ( iAvkonAppUi->ExitHidesInBackground() )
        {
        exitParams.iCommandId = 0;
        exitParams.iFlags = EPbk2EditorKeyCode;
        exitParams.iKeyCode = EAknSoftkeyDone;
        }
    else
        {
        exitParams.iCommandId = EAknCmdExit;
        exitParams.iFlags = EPbk2EditorCommandId;
        exitParams.iKeyCode = 0;
          
        iExitRecord.Set( EExitApproved );
        iExitRecord.Set( EExitOrdered );
        }
      
    ContactEditorOperationCompleted(exitResult, exitParams);
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ContactEditorOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ContactEditorOperationFailed
        ( MVPbkContactObserver::TContactOp aOpCode,
          TInt aErrorCode, MPbk2ContactEditorEventObserver::TParams aParams,
          MPbk2ContactEditorEventObserver::TFailParams& aFailParams )
    {
    iUseState = EUseReady;
    
    if( iEndKeyWasPressed )
    {
    CloseDialog();
    }
    else
    {
    // Offer the failure to the extensions first
    iEditorExtension->ContactEditorOperationFailed(aOpCode,
        aErrorCode, aParams, aFailParams );

    // If extensions did not handle the error, handle it here
    if ( !aFailParams.iErrorNotified )
        {
        CCoeEnv::Static()->HandleError( aErrorCode );
        }

    if ( aFailParams.iErrorCode != KErrNone  )
        {
        TPbk2RelocationQyeryPolicy policy(
            Pbk2ContactRelocator::EPbk2DisplayNoQueries );

        if ( aFailParams.iErrorCode == KErrNotSupported )
            {
            policy = Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery;
            }
        else if ( aFailParams.iErrorCode == KErrGeneral )
            {
            policy = Pbk2ContactRelocator::EPbk2DisplayBasicQuery;
            }
        
        TBool result( EFalse );
        TRAPD( err, result = iRelocator.RelocateContactL( policy ) );
        
        if ( result )
            {
            // Do not close editor if contact need to relocate the first.
            aFailParams.iCloseEditor = EFalse;
            }
                    
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    // Clear EExitOrdered to let next exit cmd available
    iExitRecord.Clear(EExitOrdered);
    if ( aFailParams.iCloseEditor )
        {
        CloseDialog( MVPbkContactObserver::EContactOperationUnknown, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::RequestExitL(TInt aCommandId)
    {
    if (iAddressView)
        {
        iAddressView->RequestExitL(aCommandId);
        }
    
    if ( iUseState == EUseFinished )
        {
        ForceExit();
        }
    else if ( iUseState == EUseOperatingContact  )
        {
        iEditorStrategy.StopQuery();
        iExitRecord.Set( EExitApproved );
        iExitRecord.Set( EExitOrdered );
        }
    else
        { 
        if ( aCommandId == EKeyEscape )
            {
            iEndKeyWasPressed = ETrue;
            OkToExitL( EAknSoftkeyDone );
            // If iUseState is set EUseOperatingContact by above function 
            if ( iUseState == EUseOperatingContact )
                {
                WaitFinishL();
                }
            }
        else
            {
            iExitRecord.Set( EExitApproved );
            iExitRecord.Set( EExitOrdered );
            MPbk2ContactEditorEventObserver::TParams params;
            params.iFlags = EPbk2EditorCommandId;
            params.iCommandId = aCommandId;
            CmdDoneL( params );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ForceExit()
    {
    iServiceCanceled = ETrue;
    
    if (iAddressView)
        {
        iAddressView->ForceExit();
        }
    
    if ( iUseState == EUseOperatingContact )
        {
        iEditorStrategy.StopQuery();
        iExitRecord.Set( EExitApproved );
        iExitRecord.Set( EExitOrdered );
        // It is ignored for dlg would exit
        // Leave is not valuable
        TRAP_IGNORE( WaitFinishL() );
        }
    else
        {
        iExitRecord.Set( EOkToExitWithoutHandlingIt );

        TRAPD( err, TryExitL( EAknSoftkeyBack ) );
        if ( err != KErrNone )
            {
            // If not nicely then use the force
            delete this;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ResetWhenDestroyed
        ( MPbk2DialogEliminator** aSelfPtr )
    {
    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetFocus
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetFocus(TInt aIndex)
    {
    TBool found = EFalse;
    TInt count = 0;
    TInt idx;
    for (idx = 0; idx < iUiFieldArray->Count(); idx++)
        {
        if (iUiFieldArray->At(idx).ContactEditorField())
            {
            if (count == aIndex)
                {
                found = ETrue;
                break;
                }
            count++;
            }
        }
    if (found)
        {
        iUiFieldArray->SetFocus(idx);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::NumberOfControls
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorDlgImpl::NumberOfControls() const
    {
    TInt count = 0;
    for (TInt idx = 0; idx < iUiFieldArray->Count(); idx++)
        {
        if (iUiFieldArray->At(idx).ContactEditorField())
            {
            count++;
            }
        }
    return count;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IndexOfCtrlType
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorDlgImpl::IndexOfCtrlType(TPbk2FieldCtrlType aType,
        TInt& aIndex) const
    {
    TInt ret = KErrNotFound;
    const TInt fieldCount = iUiFieldArray->Count();
    for (TInt i = Max(aIndex, 0); i < fieldCount; ++i)
        {
        if (iUiFieldArray->At(i).ContactEditorField())
            {
            const MPbk2FieldProperty & property =
                    iUiFieldArray->At(i).ContactEditorField()->FieldProperty();
            if (property.CtrlType() == aType)
                {
                ret = i;
                break;
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::AreAllControlsEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::AreAllControlsEmpty() const
    {
    return iUiFieldArray->AreAllUiFieldsEmpty();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsControlEmptyL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::IsControlEmptyL(TInt aIndex) const
    {
    __ASSERT_DEBUG( iUiFieldArray->Count()> aIndex,
            Panic( EPanic_IsControlEmptyL_OOB ) );
    TBool ret = EFalse;
    
    // There are two types field controls in the edit dialog, one is the editor
    // field control and the other is editor ui field control. The editor field 
    // control are concreted field mapping to one field, it also can be divided
    // into two types, custom control and normal control. The editor ui field 
    // control are more like a container, currently only be used as the address
    // container which can contain several editor field controls.
    if ( iUiFieldArray->At(aIndex).ContactEditorField() )
        {
        // The custom control are special controls, like image, ring tone or date,
        // they need use special way to judge if empty.
        TBool empty = ETrue;
        if( IsCustomControlEmptyL(aIndex, empty ) )
            {
            ret = empty;
            }
        // The control is normal editor field control.
        else
            {
            ret = iUiFieldArray->IsEditorFieldControlEmpty( aIndex );
            }
        }
    // If it's not an editor field control, judge if it's an editor ui field control.
    else
        {
        // Check if contact editor ui field like address field is empty.
        if ( iUiFieldArray->At( aIndex ).ContactEditorUIField() )
            {
            ret = iUiFieldArray->IsEditorUiFieldControlEmpty( aIndex );
            }
        }


    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsCustomControlEmptyL
//
// Check emptiness of those fields that technically may contain data although
// in view of user and ui are considered as empty (e.g. "Add image" text)
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::IsCustomControlEmptyL(TInt aIndex, TBool& aRet) const
    {
    __ASSERT_DEBUG( iUiFieldArray->Count()> aIndex,
            Panic( EPanic_IsControlEmptyL_OOB ) );
    TBool isCustom(EFalse);
    if (!iUiFieldArray->At(aIndex).ContactEditorField())
        {
        return isCustom;
        }
    MPbk2ContactEditorField& currentField =
            *(iUiFieldArray->At(aIndex).ContactEditorField());
    TPbk2FieldCtrlType type = currentField.FieldProperty().CtrlType();
    TAny* ext = currentField.ContactEditorFieldExtension(TUid::Uid(NULL));

    if (type == EPbk2FieldCtrlTypeImageEditor && ext)
        {
        CPbk2ContactEditorImageField* fld =
                static_cast<CPbk2ContactEditorImageField*> (ext);
        aRet = fld->TextState() == CPbk2ContactEditorImageField::ENoData;
        isCustom = ETrue;
        }
    else if (type == EPbk2FieldCtrlTypeRingtoneEditor && ext)
        {
        CPbk2ContactEditorRingtoneField* fld =
                static_cast<CPbk2ContactEditorRingtoneField*> (ext);
        aRet = fld->TextState() == CPbk2ContactEditorRingtoneField::ENoData;
        isCustom = ETrue;
        }
    else if (type == EPbk2FieldCtrlTypeDateEditor)
        {
        // If contact has a date field, for index counting
        // date editor field is considered as a custom control
        // and it is not empty.
        isCustom = ETrue;
        aRet = EFalse;
        }

    return isCustom;
    }
// CPbk2ContactEditorDlgImpl::EditorField
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField& CPbk2ContactEditorDlgImpl::EditorField(TInt aIndex) const
    {
    __ASSERT_DEBUG( iUiFieldArray->Count()> aIndex,
            Panic( EPanic_EditorField_OOB ) );

    TBool found = EFalse;
    TInt count = 0;
    TInt idx;
    for (idx = 0; idx < iUiFieldArray->Count(); idx++)
        {
        if (iUiFieldArray->At(idx).ContactEditorField())
            {
            if (count == aIndex)
                {
                found = ETrue;
                break;
                }
            count++;
            }
        }

    __ASSERT_DEBUG( found,
            Panic( EPanic_EditorField_OOB ) );
    return *(iUiFieldArray->At(idx).ContactEditorField());
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::StoreUnavailable
    ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    if ( &aContactStore == &iContact.ParentStore() )
        {
        /*
		 * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
		 */
        const CPbk2StoreProperty* property = NULL;
        if( iAppServices )
        	{
        	property =
					iAppServices->StoreProperties().FindProperty
							( aContactStore.StoreProperties().Uri() );
        	}
        else
        	{
        	property =
					Phonebook2::Pbk2AppUi()->ApplicationServices().
						StoreProperties().FindProperty
							( aContactStore.StoreProperties().Uri() );
        	}
        

        TPtrC storeName;
        if ( property )
            {
            storeName.Set( property->StoreName() );
            }

        // If this leaves the note is not shown.
        // Cannot do anything about that
        TRAP_IGNORE( ShowStoreNotAvailableNoteL( storeName ) );

        // Close editor if store which came unavailable is iContact's store
        iContactObserver.ContactEditingAborted();
        // TryExitL is not called because it would call OkToExitL
        // which is not wanted
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::LineChangedL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::LineChangedL( TInt /*aControlId*/ )
    {
    SetCbaButtonsL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::StoreTitlePaneTextL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorDlgImpl::StoreTitlePaneTextL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities(
        TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
        {
        iTitlePane = static_cast<CAknTitlePane*>
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
        if (iTitlePane->Text())
            {
            iStoredTitlePaneText = iTitlePane->Text()->AllocL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::FocusedControlType
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorDlgImpl::FocusedControlType()
    {
    CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
            IdOfFocusControl());
    if (current && current->ContactEditorField())
        {
        return current->ContactEditorField()->FieldProperty().CtrlType();
        }
    else if (current && current->ContactEditorUIField())
        {
        return current->ContactEditorUIField()->UIField()->CtrlType();
        }

    return EPbk2FieldCtrlTypeNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ConstructContextMenuL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorDlgImpl::ConstructContextMenuL()
    {
    CEikMenuBar* newMenuBar = new ( ELeave ) CEikMenuBar();
    CleanupStack::PushL( newMenuBar );
    newMenuBar->ConstructL
        ( this, NULL, R_PBK2_CONTACTEDITOR_CONTEXT_MENUBAR );
    iEikonEnv->EikAppUi()->AddToStackL( newMenuBar, ECoeStackPriorityMenu,
        ECoeStackFlagRefusesFocus );
    iContextMenuBar = newMenuBar;
    CleanupStack::Pop( newMenuBar );
    iContextMenuBar->SetMenuType( CEikMenuBar::EMenuContext );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ConstructNaviPaneL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorDlgImpl::ConstructNaviPaneL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities(TUid::Uid(
            EEikStatusPaneUidNavi)).IsPresent())
        {
        // Create a default empty status pane, otherwise sync field will show
        iNaviContainer
                = static_cast<CAknNavigationControlContainer *> (statusPane->ControlL(
                        TUid::Uid(EEikStatusPaneUidNavi)));
        
        iPrevNaviDecorator =  iNaviContainer->Top();        
        
        iNaviContainer->PushDefaultL(ETrue);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::UpdateTitleL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::UpdateTitleL()
    {
    if (iTitlePane)
        {
        HBufC* title = NULL;
        switch (iParams.iActiveView)
            {
            case TPbk2ContactEditorParams::EEditorAddressView:
            	if( !iTitleText )
            		{
            		title = StringLoader::LoadL(R_QTN_PHOB_HEADER_ADDRESS);
            		}
                break;
            case TPbk2ContactEditorParams::EEditorAddressHomeView:
            	if( !iTitleText )
            		{
            		title = StringLoader::LoadL(
            				R_QTN_PHOB_HEADER_ADDRESS_HOME);
            		}
                break;
            case TPbk2ContactEditorParams::EEditorAddressOfficeView:
            	if( !iTitleText )
            		{
            		title = StringLoader::LoadL(
            				R_QTN_PHOB_HEADER_ADDRESS_WORK);
            		}
                break;
            case TPbk2ContactEditorParams::EEditorView:
                {
                if ( !iStoreContact )
                    {
                    // Create the contact once
                    iStoreContact = iContact.StoreContact().ParentStore().CreateNewContactLC();
                    CleanupStack::Pop();
                    }
                    
                // Remove all fields before re-add
                iStoreContact->RemoveAllFields();
                
                const TInt fieldCount = iUiFieldArray->Count();
                for (TInt i = 0; i < fieldCount; ++i)
                    {
                    CPbk2ContactEditorArrayItem& item = iUiFieldArray->At(i);
                    MPbk2ContactEditorField* uiField = item.ContactEditorField();
                    if (uiField && iNameFormatter->IsTitleField(uiField->ContactField()))
                        {
                        HBufC* uiData = uiField->ControlTextL();
                        if (uiData)
                            {
                            CleanupStack::PushL(uiData);
                            const MVPbkFieldType& type =
                                uiField->FieldProperty().FieldType();
                            MVPbkStoreContactField* field = iStoreContact->CreateFieldLC(type);
                            MVPbkContactFieldTextData::Cast(
                                field->FieldData()).SetTextL(*uiData);
                            iStoreContact->AddFieldL(field);
                            CleanupStack::Pop(); // field
                            CleanupStack::PopAndDestroy(uiData);
                            }
                        }
                    }
                if( !iTitleText )
                	{
                	title = iNameFormatter->GetContactTitleOrNullL(iStoreContact->Fields(),
                			KEditorNameFormatFlags);
                	}
                break;
                }
            default:
                break;
            }
        
        // if custom title text is provided
        if( iTitleText )
        	{
        	iTitlePane->SetTextL( iTitleText->Des() );
        	}
        else 
        	{
        	 if (title)
				{
				iTitlePane->SetText(title);
				}
			else
				{
				iTitlePane->SetTextL(iEditorStrategy.DefaultTitle());
				}
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::UpdateTitlePictureL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::UpdateTitlePictureL()
    {
    if (iTitlePane)
        {
        // Update picture in title pane
        
        /*
		 * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
		 */
        CEikImage* image = NULL;
        if( iAppServices )
        	{
        	image =
				Pbk2TitlePanePictureFactory::CreateTitlePanePictureLC
					( &iContact.StoreContact(),
							iAppServices->StoreProperties() );
        	}
        else
        	{
        	image =
				Pbk2TitlePanePictureFactory::CreateTitlePanePictureLC
					( &iContact.StoreContact(),
					  Phonebook2::Pbk2AppUi()->ApplicationServices().
						StoreProperties() );
        	}
        
        
        // ownership of the picture is transfered to title pane
        image->SetPictureOwnedExternally( ETrue );
        iTitlePane->SetSmallPicture(
            image->Bitmap(), image->Mask(), image->Bitmap() != NULL );
        // bitmap and mask from image were transfered to title pane,
        // so image can be destroyed now
        CleanupStack::PopAndDestroy( image );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CloseDialog
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CloseDialog(
        MVPbkContactObserver::TContactOp aOpCode, TBool aInformObserver)
    {
    // For knowing if the destructor has been called
    TBool destroyed = EFalse;
    iDestroyedPtr = &destroyed;

    if (aInformObserver && iParams.iActiveView
            == TPbk2ContactEditorParams::EEditorView || iAddressViewStandalone)
        {
        switch (aOpCode)
            {
            case MVPbkContactObserver::EContactOperationUnknown: // FALLTHROUGH
            case MVPbkContactObserver::EContactCommit:
                {
                iContactObserver.ContactEditingComplete(
                        &iContact.StoreContact());
                break;
                }
            case MVPbkContactObserver::EContactDelete:
                {
                iContactObserver.ContactEditingDeletedContact(
                        &iContact.StoreContact());
                break;
                }
            default:
                {
                // Either end key was pressed or some uncommon scenario took place,
                // abort
                iContactObserver.ContactEditingAborted();
                break;
                }
            };
        }

    // In VOIP/new contacts/exit case, this object will be destructed by
    // iContactObserver.ContactEditingComplete() above.
    // If this object is destructed, don't excute the following anymore
    if ( destroyed )
        {
        return;
        }

    // Don't save any contact data (already saved)
    iExitRecord.Set(EOkToExitWithoutHandlingIt);
    iExitRecord.Clear(EExitOrdered);

    // Close dialog using TryExitL and returning ETrue from OkToExit
    TInt err = KErrNone;
        TRAP( err, TryExitL( EAknSoftkeyBack ) );
    if (err != KErrNone)
        {
        // If not nicely then use the force.
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ExitApplication
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::ExitApplication(
        MVPbkContactObserver::TContactOp aOpCode,
        TInt aCommandId )
    {
    if ( iExitRecord.IsSet( EExitOrdered ) &&
         iExitRecord.IsSet( EExitApproved ) )
        {

        CloseDialog( aOpCode, ETrue );

        // Dialog is closed so there is nothing to do if
        // HandleCommandL leaves. 
		TRAP_IGNORE(CEikonEnv::Static()->EikAppUi()->HandleCommandL(aCommandId)); 
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CmdAddItemL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CmdAddItemL()
    {
    AddItemToContactL(KErrNotFound, KNullDesC);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CmdDeleteItemL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CmdDeleteItemL()
    {
    TPbk2DeleteItemManager deleteItem( iContact, *iUiFieldArray, iAppServices );
    if (deleteItem.DeleteFieldL(IdOfFocusControl()))
        {
        // Redraw using DrawNow, DrawDeferred does not get the job done
        DrawNow();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CmdEditItemLabelL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CmdEditItemLabelL()
    {
    CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
            IdOfFocusControl());
    MPbk2ContactEditorField* field = current->ContactEditorField();
    if (field && field->ContactField().SupportsLabel())
        {
        // Find out the presentation contact index and then label length
        TInt fieldIndx = iContact.PresentationFields().FindFieldIndex(
            field->ContactField() );
        TInt maxLabelLength = iContact.PresentationFields().
            FieldAt( fieldIndx ).MaxLabelLength();

        HBufC* textBuf = HBufC::NewLC(maxLabelLength);
        TPtr text = textBuf->Des();
        text.Copy(field->FieldLabel().Left(text.MaxLength()));
        HBufC* prompt = StringLoader::LoadLC(R_QTN_FORM_PRMPT_FIELD_LABEL);

        CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(text);
        dlg->SetMaxLength(maxLabelLength);
        if (dlg->ExecuteLD(R_PBK2_EDIT_FIELD_LABEL_QUERY, *prompt))
            {
            if (text != field->FieldLabel())
                {
                field->SetFieldLabelL(text);
                }
            }
        CleanupStack::PopAndDestroy(2); // prompt, textBuf
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsUIDataChanged
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::IsUIDataChanged()
    {
    TInt count = iUiFieldArray->Count();
    for(TInt idx1 = 0; idx1 < count; idx1++)
        {
        CPbk2ContactEditorArrayItem& item = iUiFieldArray->At(idx1);
        if(item.ContactEditorField() && item.ContactEditorField()->FieldDataChanged())
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsAddressValidated
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::IsAddressValidated(TPbk2FieldGroupId aGroupId)
    {
    CPbk2PresentationContactFieldCollection& fields = iContact.PresentationFields();
    for(TInt idx = 0; idx < fields.FieldCount(); idx++)
        {
        MVPbkStoreContactField& field = fields.FieldAt(idx);
        TInt countProps =
            field.BestMatchingFieldType()->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            field.BestMatchingFieldType()->VersitProperties();
        for ( TInt idx2 = 0; idx2 < countProps; idx2++ )
            {
            if ( props[ idx2 ].Name() == EVPbkVersitNameGEO &&
                 ( ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
                 aGroupId == EPbk2FieldGroupIdHomeAddress ) ||
                   ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
                   aGroupId == EPbk2FieldGroupIdCompanyAddress ) ||
                     ( !props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
                       !props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
                       aGroupId == EPbk2FieldGroupIdPostalAddress ) ) )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetAddressValidationIconsL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetAddressValidationIconsL()
    {
    TInt count = iUiFieldArray->Count();
    for(TInt idx = 0; idx < count; idx++)
        {
        CPbk2ContactEditorArrayItem& uiItem = iUiFieldArray->At(idx);
        MPbk2ContactEditorUIField* uiField = uiItem.ContactEditorUIField();
        if(uiField)
            {
            switch(uiField->UIField()->CtrlType())
                {
                case EPbk2FieldCtrlTypeExtAddressEditor:
                case EPbk2FieldCtrlTypeExtAddressHomeEditor:
                case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
                    if(IsAddressValidated(Pbk2AddressTools::MapCtrlTypeToAddress(
                            uiField->UIField()->CtrlType())))
                        {
                        TPbk2IconId iconID(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_locev_map);
                        uiField->LoadBitmapToFieldL(iconID);
                        }
                    else
                        {
                        TPbk2IconId iconID(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_pb_no_valid_lm);
                        uiField->LoadBitmapToFieldL(iconID);
                        }
                    break;
                case EPbk2FieldCtrlTypeExtAssignFromMapsEditor:
                    if(IsAddressValidated( Pbk2AddressTools::MapViewTypeToAddress(iParams.iActiveView)))
                        {
                          TPbk2IconId iconID(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_locev_map);
                          uiField->LoadBitmapToFieldL(iconID);
                          }
                      else
                          {
                          TPbk2IconId iconID(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_pb_no_valid_lm);
                          uiField->LoadBitmapToFieldL(iconID);
                          }
                    break;  
                default:
                    return;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DeleteActiveAddressGeoCoords
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::DeleteActiveAddressGeoCoords()
    {
    CPbk2PresentationContactFieldCollection& fields = iContact.PresentationFields();
    for(TInt idx = 0; idx < fields.FieldCount(); idx++)
        {
        MVPbkStoreContactField& field = fields.FieldAt(idx);
        TInt countProps =
            field.BestMatchingFieldType()->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            field.BestMatchingFieldType()->VersitProperties();
        for ( TInt idx2 = 0; idx2 < countProps; idx2++ )
            {
            if ( props[ idx2 ].Name() == EVPbkVersitNameGEO &&
                 ( ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
                 iParams.iActiveView ==
                   TPbk2ContactEditorParams::EEditorAddressHomeView ) ||
                   ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
                   iParams.iActiveView ==
                     TPbk2ContactEditorParams::EEditorAddressOfficeView ) ||
                     ( !props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
                       !props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
                       iParams.iActiveView ==
                         TPbk2ContactEditorParams::EEditorAddressView ) ) )
                {
                iContact.RemoveField(idx);
                return;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CmdDoneL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::CmdDoneL(
        MPbk2ContactEditorEventObserver::TParams& aParams)
    {
    // The event that "end" key is pressed is informed into 
    // CPsu2ContactEditorExtension to do some process
    if( iEndKeyWasPressed )
        {
        iEditorExtension->ProcessCommandL( EAknCmdHideInBackground );
        }
    
    // Saving/deleting is asynchronic so it is never to ok exit rightaway
    // No need to save fields when all field are empty.
    // To estimate dialog is a contact editor or a address dialog.  
    // If it is address dialog, then save all field.
    if (iUseState != EUseOperatingContact)
        {
        if ( !iUiFieldArray->AreAllUiFieldsEmpty() 
                || iParams.iActiveView == TPbk2ContactEditorParams::EEditorAddressView 
                || iParams.iActiveView == TPbk2ContactEditorParams::EEditorAddressHomeView
                || iParams.iActiveView == TPbk2ContactEditorParams::EEditorAddressOfficeView )
            {
            iUiFieldArray->SaveFieldsL();
            }
    
        if(iParams.iActiveView != TPbk2ContactEditorParams::EEditorView && IsUIDataChanged() && 
           IsAddressValidated(Pbk2AddressTools::MapViewTypeToAddress(iParams.iActiveView)))
            {
            if(iUiFieldArray->AreAllUiFieldsEmpty())
                {
                DeleteActiveAddressGeoCoords();
                }
            else
                {
                HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_CONFIRM_KEEP_COORDINATES);
                CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                if(!dlg->ExecuteLD(R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt))
                    {
                    DeleteActiveAddressGeoCoords();
                    }
                CleanupStack::PopAndDestroy( prompt );
                }
            }
        
        TInt error(KErrNone);
        if (iParams.iActiveView == TPbk2ContactEditorParams::EEditorView || 
            iAddressViewStandalone)
            {
            // Estimate all field are empty by UI field.
        if (( AreAllControlsEmpty() && !iAddressViewStandalone )||
            (iAddressViewStandalone && AreAllFieldsEmpty()))
                {
                if (iEditorExtension->OkToDeleteContactL(aParams))
                    {
                    // iUseState must be set before DeleteContactL() because 
                    // during deletion ForceExit() might be entered and needs to 
                    // check the state to make sure operation finished.
                    iUseState = EUseOperatingContact;

                    // If Endkey is pressed, no need of query dialog in DeleteContactL()
                    if ( iEndKeyWasPressed )
                        {
                        aParams.iCommandId = EEikBidCancel;
                        }

                    TRAP( error, iEditorStrategy.DeleteContactL( *this, aParams ) );

                    DecideToLeaveL( error );
                    }
                }
            else
                {
                iParams.iFocusedIndex = FocusedFieldIndexL();
                if (iEditorExtension->OkToSaveContactL(aParams))
                    {
                    iUseState = EUseOperatingContact;
                    
                    TRAP( error, iEditorStrategy.SaveContactL( *this, aParams ) );
                
                    // Close the dialog if save contact fail caused by the
                    // memory full
                    if ( error == KErrDiskFull )
                        {
                        CloseDialog( MVPbkContactObserver::EContactCommit, ETrue );
                        }
                    
                    DecideToLeaveL(error);
                    }
                else
                    {
                    if ( iEndKeyWasPressed )
                        {
                        CloseDialog();
                        }
                    else
                    	{
                    	iExitRecord.Clear( EExitOrdered );
                    	}
                    }
                }
            }
       else
            {
            CloseWithoutSaving(EFalse);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::FocusedFieldIndexL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorDlgImpl::FocusedFieldIndexL()
    {
    if( iAddressViewStandalone )
        {
        return KErrNotFound;
        }
    
    const TInt idOfFocusedControl(IdOfFocusControl());
    TInt fieldIndex(0);
    const TInt count(iUiFieldArray->Count());
    
    for (TInt i(0); i < count; ++i)
        {
        CPbk2ContactEditorArrayItem& uiField = iUiFieldArray->At(i);

        if(uiField.ControlId() == idOfFocusedControl)
            {
            if( IsControlEmptyL(i) )
                {
                break;
                }
            return fieldIndex;
            }
        else
            {
            if( !IsControlEmptyL(i) )
                {
                fieldIndex++;
                }
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetCbaButtonsL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::SetCbaButtonsL()
    {
    if ( AknLayoutUtils::MSKEnabled() )
        {
        TBool hideMsk(EFalse);

        if ( !iRelocator.IsPhoneMemoryInConfigurationL() )
            {
            if ( EmptyContextMenuL() )
                {
                hideMsk = ETrue;
                }
            }

        ButtonGroupContainer().DimCommandByPosition(
                CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                hideMsk );

        ButtonGroupContainer().MakeCommandVisibleByPosition(
                CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                !hideMsk );

        ButtonGroupContainer().DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::EmptyContextMenuL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::EmptyContextMenuL()
    {
    TBool empty(EFalse);

    CPbk2ContactEditorArrayItem* current = iUiFieldArray->Find(
            IdOfFocusControl());
            
    if( current )
        {
        MPbk2ContactEditorField* currentField = current->ContactEditorField();

        CArrayPtr<const MPbk2FieldProperty>* availableFields =
            iContact.AvailableFieldsToAddL();

        if ( ( !currentField ||
             ( currentField->FieldProperty().Flags()
               & KPbk2FieldFlagCanNotBeRemoved ) ) &&
             ( !currentField ||
               !currentField->ContactField().SupportsLabel() ) &&
             ( !availableFields || availableFields->Count() == 0 ) ) 
            {
            empty = ETrue;
            }
        delete availableFields;
        }
    else
        {
        empty = EFalse;
        }

    return empty;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::OkToExitApplicationL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorDlgImpl::OkToExitApplicationL
        ( TInt aCommandId )
    {
    TBool okToExit = ETrue;

    // If exit callback returned EFalse, the exit is cancelled
    if ( iParams.iExitCallback &&
         !iParams.iExitCallback->OkToExitL( aCommandId ) )
        {
        okToExit = EFalse;
        }

    if ( !okToExit )
        {
        iExitRecord.Clear( EExitApproved );
        }

    return okToExit;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::SetCbaCommandSetL
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorDlgImpl::SetCbaCommandSetL( TInt aResourceId )
    {
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandSetL( aResourceId );
    iCbaCommandSet = aResourceId;
    cba.DrawDeferred();
    }    

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::UpdateCbasL
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorDlgImpl::UpdateCbasL(CPbk2ContactEditorArrayItem* aItemToUpdate)
    {
    CPbk2ContactEditorArrayItem* current = aItemToUpdate;
    if(!current)
        {
        current = iUiFieldArray->Find(IdOfFocusControl());
        }

    MPbk2ContactEditorField* currentField = current->ContactEditorField();
    MPbk2ContactEditorUIField* currentUIField = current->ContactEditorUIField();
    if ( currentField )
        {
        TPbk2FieldCtrlType type =
            currentField->FieldProperty().CtrlType();
        if (type == EPbk2FieldCtrlTypeImageEditor )
            {
            TInt id  = currentField->ControlId();  
            TAny* ext = currentField->ContactEditorFieldExtension(TUid::Uid(NULL));
            CPbk2ContactEditorImageField* fld = 
                static_cast<CPbk2ContactEditorImageField*>(ext);
            if(fld->TextState() == CPbk2ContactEditorImageField::ENoData)
                {
                if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_ADD )
                    {
                    SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_ADD  );   
                    } 
                }
            else
                {
                if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE )
                    {
                    SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE  );   
                    } 
                }
            }
        else if ( type == EPbk2FieldCtrlTypeChoiceItems ||
                  type == EPbk2FieldCtrlTypeRingtoneEditor )
            {
            if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE )
                {
                SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE  );   
                } 
            }
        else
            {
#ifndef ECE_DISABLE_CONTEXT_MENU            
            if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_CONTEXT )
                {
                SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_CONTEXT );
                }
#else
            if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_EMPTY )
                {
                SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_EMPTY );
                }
#endif            
            }
        }
   
   if(currentUIField)
       {
       TPbk2FieldCtrlTypeExt type =
           currentUIField->UIField()->CtrlType();
       switch(type)
           {
           case EPbk2FieldCtrlTypeExtAddressEditor:
           case EPbk2FieldCtrlTypeExtAddressHomeEditor:
           case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
               if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE )
                   {
                   SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_CHANGE  );   
                   } 
               break;
           case EPbk2FieldCtrlTypeExtAssignFromMapsEditor:
               if ( iCbaCommandSet != R_PBK2_SOFTKEYS_OPTIONS_DONE_SELECT )
                   {
                   SetCbaCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_DONE_SELECT  );   
                   } 
               break;
           }
       }

    // Offer button group container to extensions.
    // This is needed here because previous settings might override extension's
    // buttons.
    iEditorExtension->ModifyButtonGroupContainerL(ButtonGroupContainer());
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ImageLoadingComplete
// --------------------------------------------------------------------------
//    
TInt CPbk2ContactEditorDlgImpl::ImageLoadingComplete()
    {
    TRAPD(err,DoImageLoadingCompleteL())
    return err;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::DoImageLoadingCompleteL
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorDlgImpl::DoImageLoadingCompleteL()
    {
    CPbk2ContactEditorArrayItem* currentField =
        iUiFieldArray->Find(IdOfFocusControl());
    if (currentField && currentField->ContactEditorField())
        {
        TInt id  = currentField->ContactEditorField()->ControlId();  
        TAny* ext = currentField->ContactEditorField()->ContactEditorFieldExtension(TUid::Uid(NULL));
        if( id == EPbk2EditorLineImage )
            {
            CPbk2ContactEditorImageField* fld = 
                static_cast<CPbk2ContactEditorImageField*>(ext);
            fld->SetTextL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ImageLoadingFailed
// --------------------------------------------------------------------------
//    
TInt CPbk2ContactEditorDlgImpl::ImageLoadingFailed() 
    {
    return KErrNone;
    }
// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::ImageLoadingCancelled
// --------------------------------------------------------------------------
//    
TInt CPbk2ContactEditorDlgImpl::ImageLoadingCancelled() 
    {
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::WaitFinish
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorDlgImpl::WaitFinishL()
    {
    if ( !iWaitFinish && *iSelfPtr )
        {
        iInputBlock = CAknInputBlock::NewLC();
        CleanupStack::Pop( iInputBlock );
        
        iWaitFinish = new (ELeave) CActiveSchedulerWait();
        iWaitFinish->Start();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsUnicodeL
// --------------------------------------------------------------------------
//    
TBool CPbk2ContactEditorDlgImpl::IsUnicodeL( const TDesC& aText )
    {
    if ( !iConverter )
        {
        iConverter = CCnvCharacterSetConverter::NewLC();
        CleanupStack::Pop();
        RFs& fs = Phonebook2::Pbk2AppUi()->
                ApplicationServices().ContactManager().FsSession();
        iConverter->PrepareToConvertToOrFromL( KCharacterSetIdentifierSms7Bit, fs );
        }
    
    TBool isUnicode( EFalse );
    TInt unconvertedCount(0);
    HBufC8* convertedText = HBufC8::NewLC( aText.Length()*KTwoBytes );
    TPtr8 convertedTextPtr = convertedText->Des(); 

    iConverter->ConvertFromUnicode( convertedTextPtr, aText, unconvertedCount );

    // If any characters were not converted or if the converted text is longer
    // than the original the text must be unicode
    if( unconvertedCount > 0 ||
        aText.Length() < convertedText->Length() )
        {
        isUnicode = ETrue;
        }

    CleanupStack::PopAndDestroy( convertedText );
    return isUnicode;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::IsCheckPointEvent
// --------------------------------------------------------------------------
//    
TBool CPbk2ContactEditorDlgImpl::IsCheckPointEvent(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool check = EFalse;
    if ( EEventKeyDown == aType )
        {
        // Don't handle 'EStdKeyBackspace', 'EStdKeyHash' and those between 
        // 'EStdKeyPrintScreen' and 'EStdKeyScrollLock'. 
        // If the definition of standard scancode is changed, the scope here needs to 
        // be examined again.
        if ( ( EStdKeyPrintScreen > aKeyEvent.iScanCode || EStdKeyScrollLock < aKeyEvent.iScanCode ) &&
                EStdKeyBackspace != aKeyEvent.iScanCode &&
                EStdKeyHash != aKeyEvent.iScanCode )
            {
            check = ETrue;
            }
        }
    else if ( EEventKey == aType && EStdKeyNull == aKeyEvent.iScanCode )
        {
        check = ETrue;
        }
    
    return check;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::CheckCurrentFieldTextL
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorDlgImpl::CheckCurrentFieldTextL( 
        CPbk2ContactEditorArrayItem* aCurrentField,
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
    {
    MPbk2ContactEditorField* editorField = aCurrentField->ContactEditorField();
    MVPbkStoreContactField& contactField = editorField->ContactField();
    TVPbkFieldStorageType dataType = contactField.FieldData().DataType();
    
    if ( EVPbkFieldStorageTypeText == dataType )
        {
        const MVPbkContactFieldTextData& textData = 
            MVPbkContactFieldTextData::Cast(contactField.FieldData());
        TInt maxSize = textData.MaxLength();
        
        if ( KVPbkUnlimitedFieldLength != maxSize &&
                IsCheckPointEvent( aKeyEvent, aType ) )
            {
            CEikEdwin* ctrl = editorField->Control();
            HBufC* textBuf = ctrl->GetTextInHBufL();
            
            if ( textBuf )
                {
                TInt maxLen = maxSize;
                if ( IsUnicodeL( *textBuf ) )
                    {
                    maxLen = maxSize / KTwoBytes - KExtraByte;
                    }
                if ( ctrl->MaxLength() != maxLen && textBuf->Length() <= maxLen )
                    {
                    ctrl->SetMaxLength( maxLen );
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleLosingForeground
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandleLosingForeground() 
    {
    //Fix for ou1cimx1#308012
    iEditorStrategy.StopQuery();            
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::HandleGainingForeground
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::HandleGainingForeground() 
    {
    /*
     * iAppServices is provided to enable editor use outside from pbk2 context
     * no need to disable startup monitor in that case
     */
    if( !iAppServices )
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
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlgImpl::RestorePrevNaviDecoratorL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDlgImpl::RestorePrevNaviDecoratorL() 
    {
    if ( iNaviContainer && iPrevNaviDecorator ) 
        {                                  
        iNaviContainer->PushL(*iPrevNaviDecorator);        
        }
    }


// End of File
