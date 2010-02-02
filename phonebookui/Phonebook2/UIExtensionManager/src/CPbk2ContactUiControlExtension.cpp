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
* Description:  Phonebook 2 contact UI control composite UI extension.
*
*/


#include "CPbk2ContactUiControlExtension.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "ForEachUtil.h"
#include "CPbk2UIExtensionInformation.h"
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2UIExtension.hrh>
#include <TPbk2IconId.h>

#include <CPbk2IconArray.h>

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::CPbk2ContactUiControlExtension
// -----------------------------------------------------------------------------
//
CPbk2ContactUiControlExtension::CPbk2ContactUiControlExtension
        ( CVPbkContactManager& aContactManager,
          CPbk2UIExtensionLoader& aExtensionLoader ):
            iContactManager( aContactManager ), 
            iExtensionLoader( aExtensionLoader )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::ConstructL
// -----------------------------------------------------------------------------
//
inline void CPbk2ContactUiControlExtension::ConstructL()
    {
    LoadExtensionsL();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::NewL
// -----------------------------------------------------------------------------
//
CPbk2ContactUiControlExtension* CPbk2ContactUiControlExtension::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2UIExtensionLoader& aExtensionLoader )
    {
    CPbk2ContactUiControlExtension* self = 
        new ( ELeave ) CPbk2ContactUiControlExtension
            ( aContactManager, aExtensionLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::~CPbk2ContactUiControlExtension
// -----------------------------------------------------------------------------
//
CPbk2ContactUiControlExtension::~CPbk2ContactUiControlExtension()
    {
    iUiControlExtensions.ResetAndDestroy();
    iDummyArray.Close();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::DoRelease
// -----------------------------------------------------------------------------
//
void CPbk2ContactUiControlExtension::DoRelease()
    {
    delete this;
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::GetDynamicIconsL
// -----------------------------------------------------------------------------
//
const TArray<TPbk2IconId> CPbk2ContactUiControlExtension::GetDynamicIconsL
    ( const MVPbkViewContact* aContactHandle )
    {
    TInt count( iUiControlExtensions.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        TArray<TPbk2IconId> ids = 
            iUiControlExtensions[ i ]->GetDynamicIconsL( aContactHandle );
        if ( ids.Count() > 0 )
            {
            return ids;
            }
        }
    return iDummyArray.Array();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::SetContactUiControlUpdate
// -----------------------------------------------------------------------------
//
void CPbk2ContactUiControlExtension::SetContactUiControlUpdate
        ( MPbk2ContactUiControlUpdate* aContactUpdator )
    {
    ForEach( iUiControlExtensions,
             VoidMemberFunction
              ( &MPbk2ContactUiControlExtension::SetContactUiControlUpdate ),
                aContactUpdator );    
    }

// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::ContactUiControlExtensionExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2ContactUiControlExtension::ContactUiControlExtensionExtension
    ( TUid aUid )
    {
    if ( aUid == KMPbk2ContactUiControlExtensionExtension2Uid )
        {
        return static_cast<MPbk2ContactUiControlExtension2*>( this );
        }
    
    TInt count( iUiControlExtensions.Count() );
    
    for ( TInt i = 0; i < count; ++i )
        {
        TAny* ext = iUiControlExtensions[ i ]->ContactUiControlExtensionExtension( aUid );
        if(ext)
            {
            return ext;
            }
        }
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::SetIconArray
// -----------------------------------------------------------------------------
//
void CPbk2ContactUiControlExtension::SetIconArray( CPbk2IconArray& aIconArray )
    {
    TInt count( iUiControlExtensions.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        MPbk2ContactUiControlExtension2* ext = reinterpret_cast<MPbk2ContactUiControlExtension2*>(
            iUiControlExtensions[ i ]->ContactUiControlExtensionExtension(
                KMPbk2ContactUiControlExtensionExtension2Uid ) );
        if ( ext != NULL )
            {
            ext->SetIconArray( aIconArray );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactUiControlExtension::LoadExtensionsL
// -----------------------------------------------------------------------------
//            
void CPbk2ContactUiControlExtension::LoadExtensionsL()
    {
    TArray<CPbk2UIExtensionPlugin*> plugins = iExtensionLoader.Plugins();
    const TInt count = plugins.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2UIExtensionPlugin* plugin = plugins[ i ];
        MPbk2ContactUiControlExtension* extension =
            plugin->CreatePbk2UiControlExtensionL( iContactManager );
        if (extension)
            {
            CleanupDeletePushL(extension);
            iUiControlExtensions.AppendL(extension);
            CleanupStack::Pop(); // extension
            }
        }
    }

// End of File
