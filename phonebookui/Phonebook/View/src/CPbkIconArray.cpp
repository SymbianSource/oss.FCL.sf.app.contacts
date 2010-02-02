/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*           Provides phonebook icon array class methods.
*
*/


// INCLUDE FILES
#include <CPbkIconArray.h>  // This class
#include <barsread.h>       // TResourceReader
#include <gulicon.h>        // CGulIcon
#include <coemain.h>        // CCoeEnv
#include <PbkView.rsg>
#include "PbkIconInfo.h"

#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_AppendFromResourceL,
    EPanicPreCond_FindIcon,
    EPanicLogic_ReadArrayL,
    EPanicPreCond_HandleResourceChangeL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkIconArray");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} // namespace

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkIconArray::CPbkIconArray
        (TInt aGranurality) : 
    CArrayPtrFlat<CGulIcon>(aGranurality), 
    iIdMap(aGranurality)
    {
    }

EXPORT_C CPbkIconArray::~CPbkIconArray()
    {
    // Free contents of this array for leave-safety
    ResetAndDestroy();
    }

EXPORT_C void CPbkIconArray::ConstructFromResourceL
        (TInt aResId)
    {
    AppendIconsFromResourceL(R_PBK_ICON_INFO_ARRAY, aResId);
    }

EXPORT_C void CPbkIconArray::ConstructFromResourceL
        (TResourceReader& aReader)
    {
    CPbkIconInfoContainer* iconInfoContainer = CPbkIconInfoContainer::NewL(
        R_PBK_ICON_INFO_ARRAY);
    CleanupStack::PushL(iconInfoContainer);

    ReadFromResourceL(aReader, *iconInfoContainer);
    CleanupStack::PopAndDestroy(); // iconInfoContainer
    }

void CPbkIconArray::ReadFromResourceL
        (TResourceReader& aReader,
        CPbkIconInfoContainer& aIconInfoContainer)
    {
    TInt count = aReader.ReadInt16();
#ifdef _DEBUG
    const TInt dbgCount = Count() + count;
#endif
    SetReserveL(Count() + count);
    iIdMap.SetReserveL(iIdMap.Count() + count);
    while (count-- > 0)
        {
        const TPbkIconId iconId = static_cast<TPbkIconId>(aReader.ReadInt8());      
        CGulIcon* gulIcon = aIconInfoContainer.LoadBitmapL(iconId);
        __ASSERT_DEBUG(gulIcon, Panic(EPanicLogic_ReadArrayL));

        // Following AppendLs call cannot leave due to lack of memory because 
        // of SetReserveL calls above.
        AppendL(gulIcon);
        iIdMap.AppendL(iconId);
        }
    __ASSERT_DEBUG(Count() == dbgCount && iIdMap.Count() == dbgCount,
        Panic(EPanicPostCond_AppendFromResourceL));
    }

EXPORT_C TInt CPbkIconArray::FindIcon
        (TPbkIconId aIconId) const
    {
    __ASSERT_DEBUG(Count() == iIdMap.Count(),
        Panic(EPanicPreCond_FindIcon));

    const TInt count = iIdMap.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iIdMap[i] == aIconId)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

EXPORT_C void CPbkIconArray::AppendIconsFromResourceL
        (TInt aArrayIconInfoId, TInt aArrayIconId)
    {
    CPbkIconInfoContainer* iconInfoContainer = CPbkIconInfoContainer::NewL(
        aArrayIconInfoId);
    CleanupStack::PushL(iconInfoContainer);

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, aArrayIconId);
    ReadFromResourceL(reader, *iconInfoContainer);
    CleanupStack::PopAndDestroy(2);  // reader, iconInfoContainer
    }

EXPORT_C void CPbkIconArray::RefreshL(TInt aArrayIconInfoId)
    {
    __ASSERT_DEBUG(iIdMap.Count() == Count(), 
                   Panic(EPanicPreCond_HandleResourceChangeL));
    CPbkIconInfoContainer* iconInfoContainer = CPbkIconInfoContainer::NewL(
        aArrayIconInfoId);
    CleanupStack::PushL(iconInfoContainer);

    // Add icons from extensions
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();    
    factory.AddPbkFieldIconsL(iconInfoContainer);
    CleanupStack::PopAndDestroy(extGlobal);    

    const TInt count = iIdMap.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const TPbkIconId iconId = iIdMap.At(i);
        CGulIcon* gulIcon = iconInfoContainer->LoadBitmapL(iconId);
        if (gulIcon)
            {
            // replace the old icon only if new one was loaded
            delete At(i);
            At(i) = gulIcon;
            }
        }

    CleanupStack::PopAndDestroy(); // iconInfoContainer
    }

//  End of File  
