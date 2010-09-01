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
* Description:  Phonebook 2 UI Extension information.
*
*/


#include "CPbk2UIExtensionInformation.h"

// Phonebook 2
#include "CPbk2UIExtensionMenu.h"
#include "CPbk2UIExtensionIconInformation.h"

// System includes
#include <barsread.h>
#include <coemain.h>

#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KPbk2UIExtensionInformationOffset = 2;

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_IconInformation
    };
    
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbk2UIExtensionInformation");
    User::Panic(KPanicText, aPanicCode);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// TPbk2UIExtensionViewId::TPbk2UIExtensionViewId
// --------------------------------------------------------------------------
//
TPbk2UIExtensionViewId::TPbk2UIExtensionViewId
        ( TResourceReader& aReader )
    {
    aReader.ReadInt8(); // read version number
    iViewId = TUid::Uid(aReader.ReadInt32());
    iViewResourceId = aReader.ReadInt32();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "TPbk2UIExtensionViewId::TPbk2UIExtensionViewId, id:%d, resId %d"), 
            iViewId.iUid, iViewResourceId );
    }

// --------------------------------------------------------------------------
// TPbk2UIExtensionViewId::TPbk2UIExtensionViewId
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2UIExtensionViewId::TPbk2UIExtensionViewId(
        TUid aViewId, 
        TInt aViewResourceId) :
    iViewId(aViewId),
    iViewResourceId(aViewResourceId)
    {
    }

// --------------------------------------------------------------------------
// TPbk2UIExtensionViewId::ViewId
// --------------------------------------------------------------------------
//
TUid TPbk2UIExtensionViewId::ViewId() const
    {
    return iViewId;
    }

// --------------------------------------------------------------------------
// TPbk2UIExtensionViewId::ViewResourceId
// --------------------------------------------------------------------------
//
TInt TPbk2UIExtensionViewId::ViewResourceId() const
    {
    return iViewResourceId;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::CPbk2UIExtensionInformation
// --------------------------------------------------------------------------
//
CPbk2UIExtensionInformation::CPbk2UIExtensionInformation() :
        iResourceFile( *CCoeEnv::Static() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::~CPbk2UIExtensionInformation
// --------------------------------------------------------------------------
//
CPbk2UIExtensionInformation::~CPbk2UIExtensionInformation()
    {
    iMenus.ResetAndDestroy();
    iResourceFile.Close();
    iViewIds.Close();
    delete iIconInformation;
    iOverWrittenCmds.Close();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::NewLC
// --------------------------------------------------------------------------
//
CPbk2UIExtensionInformation* CPbk2UIExtensionInformation::NewLC
        ( const TDesC& aFileName )
    {
    CPbk2UIExtensionInformation* self =
        new ( ELeave ) CPbk2UIExtensionInformation;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::ConstructL
// Reads the extension information resource structure
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionInformation::ConstructL
        ( const TDesC& aFileName )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL(0x%x) START"), this );
            
    iResourceFile.OpenL(aFileName);
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, resFile %S opened"), 
            &aFileName );
            
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader,
        iResourceFile.Offset() + KPbk2UIExtensionInformationOffset );
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, reader initialized") );

    reader.ReadInt8(); // read version number, ignored for now
    
    iImplementationUid = TUid::Uid( reader.ReadInt32() );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, implUid %x"), 
        iImplementationUid.iUid );
        
    iLoadingPolicy = reader.ReadInt32();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, loadingPolicy %b"), 
        iLoadingPolicy );
        
    iMenuCommandRange.ReadFromResource( reader );

    // Overwritten Phonebook 2 command ids
    const TInt commandIdCount = reader.ReadInt16();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, num of overwritten Cmds:%d"), 
            commandIdCount );
    for ( TInt i = 0; i < commandIdCount; ++i )
        {
        iOverWrittenCmds.AppendL( reader.ReadInt16() );
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, Overwrites Cmd: %d"), 
            iOverWrittenCmds[iOverWrittenCmds.Count() - 1] );
        }

    // Menu definitions
    const TInt menuDefsCount = reader.ReadInt16();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, num of menu defs:%d"), 
            menuDefsCount );            
    for ( TInt i = 0; i < menuDefsCount; ++i )
        {
        CPbk2UIExtensionMenu* menu = CPbk2UIExtensionMenu::NewLC( reader );
        iMenus.AppendL( menu );
        CleanupStack::Pop( menu );
        }

    // Extension view ids
    const TInt extensionViewIdsCount = reader.ReadInt16();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, num of view ids:%d"), 
            extensionViewIdsCount );
    for ( TInt i = 0; i < extensionViewIdsCount; ++i )
        {
        TPbk2UIExtensionViewId viewId( reader );
        iViewIds.AppendL( viewId );
        }

    // Extension's view graph resource id
    iViewGraphModificationResId = reader.ReadInt32();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, view graph res id:%d"), 
            iViewGraphModificationResId );

    // Extension icons
    iIconInformation = CPbk2UIExtensionIconInformation::NewL( 
            reader.ReadInt32() );

    // Store property array
    iStorePropertyArrayResId = reader.ReadInt32();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL, store property res id:%d"), 
            iStorePropertyArrayResId );

    CleanupStack::PopAndDestroy(); // reader
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionInformation::ConstructL(0x%x) END"), this );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::MenuCount
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionInformation::MenuCount() const
    {
    return iMenus.Count();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::MenuAt
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenu& CPbk2UIExtensionInformation::MenuAt( 
        TInt aIndex)
    {
    return *iMenus[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::AddMenuL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2UIExtensionInformation::AddMenuL(
        CPbk2UIExtensionMenu* aExtensionMenu )
    {
    if (aExtensionMenu)
        {
        iMenus.AppendL( aExtensionMenu );
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::ImplementationUid
// --------------------------------------------------------------------------
//
TUid CPbk2UIExtensionInformation::ImplementationUid() const
    {
    return iImplementationUid;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::LoadingPolicy
// --------------------------------------------------------------------------
//
TUint CPbk2UIExtensionInformation::LoadingPolicy() const
    {
    return iLoadingPolicy;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::ImplementsMenuCommand
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionInformation::ImplementsMenuCommand( TInt aCommandId )
    {
    return ( iMenuCommandRange.IsIdIncluded( aCommandId ) ||
        iOverWrittenCmds.Find( aCommandId ) != KErrNotFound );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::ImplementsView
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionInformation::ImplementsView( TUid aViewId )
    {
    TBool result = EFalse;
    const TInt count = iViewIds.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iViewIds[i].ViewId() == aViewId )
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::HasViewGraphModifications
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionInformation::HasViewGraphModifications() const
    {
    return ( iViewGraphModificationResId != 0 );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::CreateViewGraphModificationReaderLC
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionInformation::CreateViewGraphModificationReaderLC
        ( TResourceReader& aReader ) const
    {
    if ( HasViewGraphModifications() )
        {
        CCoeEnv::Static()->CreateResourceReaderLC
            ( aReader, iViewGraphModificationResId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::ViewIds
// --------------------------------------------------------------------------
//
TArray<TPbk2UIExtensionViewId> CPbk2UIExtensionInformation::ViewIds() const
    {
    return iViewIds.Array();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::AddViewIdL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2UIExtensionInformation::AddViewIdL(
        TPbk2UIExtensionViewId& aViewId)
    {
    iViewIds.AppendL( aViewId );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::HasIconExtension
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionInformation::HasIconExtension() const
    {
    return iIconInformation->IconInfoArrayResourceId() != 0;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::IconInformation
// --------------------------------------------------------------------------
//
CPbk2UIExtensionIconInformation&
    CPbk2UIExtensionInformation::IconInformation() const
    {
    __ASSERT_DEBUG( HasIconExtension(), Panic( EPreCond_IconInformation ) );
    return *iIconInformation;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::HasStorePropertyArray
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionInformation::HasStorePropertyArray() const
    {
    return ( iStorePropertyArrayResId != 0 );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionInformation::CreateStorePropertyArrayReaderLC
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionInformation::CreateStorePropertyArrayReaderLC
        ( TResourceReader& aReader )
    {
     CCoeEnv::Static()->CreateResourceReaderLC
        ( aReader, iStorePropertyArrayResId );
    }

// End of File
