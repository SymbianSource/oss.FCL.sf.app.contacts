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
* Description:  Phonebook 2 contact editor dialog extension.
*
*/

#include "cpmapcontacteditorextension.h"

// Phonebook 2
#include <CPbk2UIExtensionPlugin.h>
#include <pbk2usimuires.rsg>
#include <pbk2uicontrols.rsg>
#include <pbk2commonui.rsg>
#include <Pbk2UIControls.hrh>
#include <MPbk2ContactEditorControl.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include "CPbk2ContactEditorUIAFMField.h"
#include <MPbk2UIField.h>
#include <CPbk2IconInfoContainer.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>
#include "cpmapcmd.h"
#include "pbk2mapcommands.hrh"
#include <CPbk2AppUiBase.h>
#include <Pbk2Commands.hrh>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <pbk2mapuires.rsg>
#include <pbk2commands.rsg>
#include <Pbk2UIControls.hrh>
#include <cpmapcontacteditorextension.h>
#include <CPbk2ContactEditorDlgImpl.h>

// System includes
#include <etelmmerr.h>
#include <gsmerror.h>
#include <exterror.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <charconv.h>
#include <AiwCommon.hrh>
#include <AiwServiceHandler.h>
#include <mnproviderfinder.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

} /// namespace

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::CPmapContactEditorExtension
// --------------------------------------------------------------------------
//
CPmapContactEditorExtension::CPmapContactEditorExtension
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl ) :
            iContactManager( aContactManager ),
            iContact( aContact ),
            iEditorControl( aEditorControl )
    {
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::~CPmapContactEditorExtension
// --------------------------------------------------------------------------
//
CPmapContactEditorExtension::~CPmapContactEditorExtension()
    {
    if(iCmd)
        {
        delete iCmd;
        iCmd = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ConstructL
// --------------------------------------------------------------------------
//
inline void CPmapContactEditorExtension::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::NewL
// --------------------------------------------------------------------------
//
CPmapContactEditorExtension* CPmapContactEditorExtension::NewL
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    CPmapContactEditorExtension* self =
        new (ELeave) CPmapContactEditorExtension
            (aContactManager, aContact, aEditorControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::CreateEditorFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField* CPmapContactEditorExtension::CreateEditorFieldL
    ( MVPbkStoreContactField& /*aField*/,
      const MPbk2FieldProperty& /*aFieldProperty*/,
      MPbk2ContactEditorUiBuilder& /*aUiBuilder*/,
      CPbk2IconInfoContainer& /*aIconInfoContainer*/ )
    {
    MPbk2ContactEditorField* field = NULL;
    return field;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPmapContactEditorExtension::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_PBK2_CONTACTEDITOR_MENUPANE )
    	{
    	if ( CPmapCmd::CheckViewProviderL() )
    		{
    		aMenuPane->SetItemDimmed( EPbk2ExtensionAssignFromMap, EFalse );
    		}
    	}
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ProcessCommandL
// --------------------------------------------------------------------------
//
TBool CPmapContactEditorExtension::ProcessCommandL
        ( TInt aCommandId )
    {
    switch( aCommandId )
        {
        case EPbk2ExtensionShowOnMap:
        	{
        	if(iCmd)
        	    {
                delete iCmd;
                iCmd = NULL;
        	    }
        	iCmd = CPmapCmd::NewL( iEditorControl, iContact, aCommandId );
        	// Execute the command 
        	iCmd->ExecuteLD();
            return ETrue;
            }
        case EPbk2ExtensionAssignFromMap:
        	{
        	if(iCmd)
                {
                delete iCmd;
                iCmd = NULL;
                }
        	iCmd = CPmapCmd::NewL( iEditorControl, iContact, aCommandId );
        	// Execute the command
        	iCmd->ExecuteLD();
            return ETrue;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::OkToDeleteContactL
// --------------------------------------------------------------------------
//
TBool CPmapContactEditorExtension::OkToDeleteContactL
        ( MPbk2ContactEditorEventObserver::TParams& /*aParams*/ )
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::OkToSaveContactL
// --------------------------------------------------------------------------
//
TBool CPmapContactEditorExtension::OkToSaveContactL
        ( MPbk2ContactEditorEventObserver::TParams& /*aParams*/ )
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ModifyButtonGroupContainerL
// --------------------------------------------------------------------------
//
void CPmapContactEditorExtension::ModifyButtonGroupContainerL
        ( CEikButtonGroupContainer& /*aButtonGroupContainer*/ )
	{
	}

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::DoRelease
// --------------------------------------------------------------------------
//
void CPmapContactEditorExtension::DoRelease()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ContactEditorOperationCompleted
// --------------------------------------------------------------------------
//
void CPmapContactEditorExtension::ContactEditorOperationCompleted
        ( MVPbkContactObserver::TContactOpResult /*aResult*/,
          TParams /*aParams*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapContactEditorExtension::ContactEditorOperationCompleted"));

    // Do nothing
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ContactEditorOperationFailed
// --------------------------------------------------------------------------
//
void CPmapContactEditorExtension::ContactEditorOperationFailed
        ( MVPbkContactObserver::TContactOp /*aOpCode*/,
          TInt aErrorCode, TParams /*aParams*/, TFailParams& /*aFailParams*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapContactEditorExtension::ContactEditorOperationFailed code: %d"),
        aErrorCode);
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::ContactEditorExtensionExtension
// --------------------------------------------------------------------------
//
TAny* CPmapContactEditorExtension::ContactEditorExtensionExtension(
        TUid /*aExtensionUid*/ )
    {
    return static_cast<MPbk2ContactEditorExtensionExtension*>(this);
    }

// --------------------------------------------------------------------------
// CPmapContactEditorExtension::CreateFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorUIField* CPmapContactEditorExtension::CreateFieldLC
        ( MPbk2UIField& aField,
          TInt aCustomPosition, 
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          const TDesC& /*aCustomText*/, CPbk2IconInfoContainer& aIconInfoContainer )
    {
    MPbk2ContactEditorUIField* field = NULL;

	 switch(aField.CtrlType())
		{
		case EPbk2FieldCtrlTypeExtAssignFromMapsEditor:
			if( CPmapCmd::CheckViewProviderL() )
				{
				field = CPbk2ContactEditorUIAFMField::NewL(
					&aField, aUiBuilder, aIconInfoContainer, aCustomPosition);
				CleanupStack::PushL(field);
				}
			break;
		}

    return field;
    }
// End of File
