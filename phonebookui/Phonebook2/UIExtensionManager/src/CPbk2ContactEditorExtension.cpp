/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor composite UI extension.
*
*/


#include "CPbk2ContactEditorExtension.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionInformation.h"
#include <CPbk2UIExtensionPlugin.h>
#include "ForEachUtil.h"

/// Unnamed namespace for local definitions
namespace {

const TInt KExtensionGranularity = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::CPbk2ContactEditorExtension
// --------------------------------------------------------------------------
//
CPbk2ContactEditorExtension::CPbk2ContactEditorExtension
        ( CPbk2UIExtensionLoader& aExtensionLoader ) :
            iExtensionLoader( aExtensionLoader ),
            iContactEditors( KExtensionGranularity )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::~CPbk2ContactEditorExtension
// --------------------------------------------------------------------------
//
CPbk2ContactEditorExtension::~CPbk2ContactEditorExtension()
    {
    iContactEditors.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorExtension::ConstructL
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    LoadEditorExtensionsL( aContactManager, aContact, aEditorControl );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorExtension* CPbk2ContactEditorExtension::NewL
        ( CPbk2UIExtensionLoader& aExtensionLoader,
          CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    CPbk2ContactEditorExtension* self =
        new ( ELeave ) CPbk2ContactEditorExtension( aExtensionLoader );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager, aContact, aEditorControl );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::DoRelease
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorExtension::DoRelease()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::CreateEditorFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField* CPbk2ContactEditorExtension::CreateEditorFieldL
    ( MVPbkStoreContactField& aField,
      const MPbk2FieldProperty& aFieldProperty,
      MPbk2ContactEditorUiBuilder& aUiBuilder,
      CPbk2IconInfoContainer& aIconInfoContainer )
    {
    MPbk2ContactEditorField* field = NULL;

    TInt count = iContactEditors.Count();
    TBool found = EFalse;
    for ( TInt i = 0; i < count && !found; ++i )
        {
        field = iContactEditors[ i ]->CreateEditorFieldL
            ( aField, aFieldProperty, aUiBuilder, aIconInfoContainer );
        if ( field )
            {
            found = ETrue;
            }
        }

    return field;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorExtension::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    ForEachL(iContactEditors,
         VoidMemberFunction
        ( &MPbk2ContactEditorExtension::DynInitMenuPaneL ),
            aResourceId, aMenuPane);    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ProcessCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorExtension::ProcessCommandL( TInt aCommandId )
    {
    return TryEachL( iContactEditors,
        MemberFunction( &MPbk2ContactEditorExtension::ProcessCommandL ),
            aCommandId);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::OkToDeleteContactL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorExtension::OkToDeleteContactL
        ( MPbk2ContactEditorEventObserver::TParams& aParams )
    {
    return TryEachL( iContactEditors,
        MemberFunction( &MPbk2ContactEditorExtension::OkToDeleteContactL ),
            aParams, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::OkToSaveContactL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorExtension::OkToSaveContactL
        ( MPbk2ContactEditorEventObserver::TParams& aParams )
    {
    return TryEachL( iContactEditors,
        MemberFunction( &MPbk2ContactEditorExtension::OkToSaveContactL ),
            aParams, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ModifyButtonGroupContainerL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorExtension::ModifyButtonGroupContainerL
        ( CEikButtonGroupContainer& aButtonGroupContainer )
	{
    ForEachL( iContactEditors,
         VoidMemberFunction
         ( &MPbk2ContactEditorExtension::ModifyButtonGroupContainerL ),
            aButtonGroupContainer );    	
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ContactEditorOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorExtension::ContactEditorOperationCompleted
        ( MVPbkContactObserver::TContactOpResult aResult,
          TParams aParams )
    {
    ForEach( iContactEditors,
             VoidMemberFunction
            ( &MPbk2ContactEditorExtension::ContactEditorOperationCompleted ),
                aResult, aParams);
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ContactEditorOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2ContactEditorExtension::ContactEditorOperationFailed
        ( MVPbkContactObserver::TContactOp aOpCode, 
          TInt aErrorCode, TParams aParams, TFailParams& aFailParams )
    {
    ForEach( iContactEditors,
             VoidMemberFunction
             ( &MPbk2ContactEditorExtension::ContactEditorOperationFailed ),
                aOpCode, aErrorCode, aParams, aFailParams );
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::LoadEditorExtensionsL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorExtension::LoadEditorExtensionsL
    ( CVPbkContactManager& aContactManager,
      MVPbkStoreContact& aContact,
      MPbk2ContactEditorControl& aEditorControl )
    {
    TArray<CPbk2UIExtensionInformation*> pluginInfo = 
        iExtensionLoader.PluginInformation();
    const TInt count = pluginInfo.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        TUint32 loadFlags =
            KPbk2LoadInStartup | KPbk2LoadOnContactEditorExecution;
        if ( pluginInfo[i]->LoadingPolicy() & loadFlags )
            {
            TUid pluginUid = pluginInfo[i]->ImplementationUid();
            iExtensionLoader.EnsureLoadedL(pluginUid);
            CPbk2UIExtensionPlugin* plugin =
                iExtensionLoader.LoadedPlugin(pluginUid);
            if(plugin)                                      
                {                                           
                MPbk2ContactEditorExtension* extension =
                    plugin->CreatePbk2ContactEditorExtensionL
                        ( aContactManager, aContact, aEditorControl );
                if ( extension )
                    {
                    CleanupDeletePushL( extension );
                    iContactEditors.AppendL( extension );
                    CleanupStack::Pop(); // extension
                    }
                }                                                              
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorExtension::ContactEditorExtensionExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactEditorExtension::ContactEditorExtensionExtension(
        TUid /*aExtensionUid*/ )
    {
    TInt count = iContactEditors.Count();
    for( TInt idx = 0; idx < count; idx++ )
    	{
    	MPbk2ContactEditorExtension* ext = iContactEditors.At(idx);
    	if( ext && ext->ContactEditorExtensionExtension(TUid::Null()) )
    		{
    		return ext->ContactEditorExtensionExtension(TUid::Null());
    		}
    	}
    return NULL;
    }
// End of File
