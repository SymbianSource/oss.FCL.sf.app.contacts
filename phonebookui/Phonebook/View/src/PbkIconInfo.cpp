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
*       Provides methods for Phonebook Icon info class.
*
*/


// INCLUDE FILES
#include "PbkIconInfo.h"
#include <barsread.h>
#include <coemain.h>
#include <gulicon.h>
#include <pbkview.rsg>
#include <AknsUtils.h>
#include <aknenv.h>

// unnamed namespace
namespace {

/**
 * Creates a resource reader.
 * 
 * @aReader will contain filled reader
 * @aResourceId resource to be loaded
 * @aCoeEnv coe env to be used for resource reading
 */
void CreateReaderLC
        (TResourceReader& aReader, 
        TInt aResourceId, 
        CCoeEnv* aCoeEnv = NULL)
    {
    CCoeEnv* coeEnv = aCoeEnv;
    if (!coeEnv)
        {
        coeEnv = CCoeEnv::Static();
        }
    coeEnv->CreateResourceReaderLC(aReader, aResourceId);
    }
}

// ==================== MEMBER FUNCTIONS ====================

EXPORT_C TPbkIconId TPbkIconInfo::PbkIconInfoId() const 
    { 
    return iPbkIconInfoId; 
    }

EXPORT_C const TDesC& TPbkIconInfo::MbmFileName() const 
    { 
    return *iMbmFileName; 
    }

EXPORT_C TInt TPbkIconInfo::IconId() const 
    { 
    return iIconId;
    }

EXPORT_C TInt TPbkIconInfo::MaskId() const 
    { 
    return iMaskId;
    }

EXPORT_C TAknsItemID TPbkIconInfo::SkinId() const
    {
    return iSkinId;
    }

TAknsItemID TPbkIconInfo::ColorId() const
    {
    return iColorId;
    }

TInt TPbkIconInfo::ColorIndex() const
    {
    return iColorIndex;
    }

TInt TPbkIconInfo::DefaultColorIndex() const
    {
    return iDefaultColorIndex;
    }

inline TPbkIconInfo::TPbkIconInfo()
    {
    // Nothing needs to be intialised here.
    // All fields are intialised by CPbkIconInfoContainer::ConstructL() before
    // this object can be used.
    }

inline TPbkIconInfo::~TPbkIconInfo()
    {
    // Destroy() handles releasing resources of this class
    }

inline void TPbkIconInfo::ReadFromResourceLC(TResourceReader& aReader)
    {
    iPbkIconInfoId = static_cast<TPbkIconId>(aReader.ReadInt8());
	HBufC* buf = aReader.ReadHBufCL();
    CleanupStack::PushL(buf);
    iMbmFileName = buf;
    iIconId = aReader.ReadInt32();
    iMaskId = aReader.ReadInt32();
    TInt skinIdMajor = aReader.ReadInt32();
    TInt skinIdMinor = aReader.ReadInt32();
    iSkinId.Set(skinIdMajor, skinIdMinor);
    TInt colorIdMajor = aReader.ReadInt32();
    TInt colorIdMinor = aReader.ReadInt32();
    iColorId.Set(colorIdMajor, colorIdMinor);
    iColorIndex = aReader.ReadInt32();
    iDefaultColorIndex = aReader.ReadInt32();
    }

inline void TPbkIconInfo::Destroy()
    {
    // delete data members
    delete iMbmFileName;
    iMbmFileName = NULL;
    }

inline CPbkIconInfoContainer::CPbkIconInfoContainer()
    {
    // CBase::operator new(TLeave) resets member data
    }

inline void CPbkIconInfoContainer::ConstructL(TResourceReader& aReader)
    {
    DoAppendIconsFromResourceL(aReader);
    }

EXPORT_C CPbkIconInfoContainer* CPbkIconInfoContainer::NewL
        (CCoeEnv* aCoeEnv /*=NULL*/)
    {
    return NewL(R_PBK_ICON_INFO_ARRAY, aCoeEnv);
    }

EXPORT_C CPbkIconInfoContainer* CPbkIconInfoContainer::NewL
        (TResourceReader& aReader)
    {
    CPbkIconInfoContainer* self = new(ELeave) CPbkIconInfoContainer;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkIconInfoContainer* CPbkIconInfoContainer::NewL
        (TInt aResourceId, CCoeEnv* aCoeEnv /*=NULL*/)
    {
    CPbkIconInfoContainer* self = new(ELeave) CPbkIconInfoContainer;
    CleanupStack::PushL(self);
    TResourceReader reader;
    CreateReaderLC(reader, aResourceId, aCoeEnv);
    self->ConstructL(reader);
    CleanupStack::PopAndDestroy(); // reader (buffer)
    CleanupStack::Pop(self);
    return self;
    }

CPbkIconInfoContainer::~CPbkIconInfoContainer()
    {
    // close and delete data members
    for (TInt i = iIconArray.Count()-1; i >= 0; --i)
        {
        iIconArray[i].Destroy();
        }
    iIconArray.Close();
    }

EXPORT_C const TPbkIconInfo* CPbkIconInfoContainer::Find
        (TPbkIconId aIconId) const
    {
    const TInt count = iIconArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iIconArray[i].iPbkIconInfoId == aIconId)
            {
            return &iIconArray[i];
            }
        }
    return NULL;
    }

EXPORT_C CGulIcon* CPbkIconInfoContainer::LoadBitmapL
        (TPbkIconId aIconId) const
    {
    const TPbkIconInfo* iconInfo = Find(aIconId);
    if (iconInfo)
        {
        CGulIcon* icon = CGulIcon::NewLC();
        icon->SetBitmapsOwnedExternally(EFalse);

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        PbkIconUtils::CreateIconL(AknsUtils::SkinInstance(), bitmap, mask, *iconInfo);

        icon->SetBitmap(bitmap);
        icon->SetMask(mask);

        CleanupStack::Pop(); // icon

        return icon;
        }
    return NULL;
    }

EXPORT_C void CPbkIconInfoContainer::AppendIconsFromResourceL
        (TInt aResourceId)
    {
    TResourceReader reader;
    CreateReaderLC(reader, aResourceId);
    DoAppendIconsFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

/**
 * Appends icon infor to this container.
 *
 * @param aReader resource to be added to this info container
 */
void CPbkIconInfoContainer::DoAppendIconsFromResourceL
        (TResourceReader& aReader)
    {
    // read from resource
    TInt count = aReader.ReadInt16();
    while (count-- > 0)
        {
        TPbkIconInfo iconInfo;
        iconInfo.ReadFromResourceLC(aReader);
        
        TPbkIconInfo* info = const_cast<TPbkIconInfo*>
            (Find(iconInfo.PbkIconInfoId()));
        if ( !info )
            {
            // add if icon is not found
            User::LeaveIfError(iIconArray.Append(iconInfo));
            }
        else
            {
            // if icon is found, update the data
            info->iIconId = iconInfo.iIconId;
            info->iMaskId = iconInfo.iMaskId;
            info->iSkinId = iconInfo.iSkinId;

            delete info->iMbmFileName;
            info->iMbmFileName = iconInfo.iMbmFileName;
            }

        CleanupStack::Pop(); // iconInfo
        }
    }

EXPORT_C void PbkIconUtils::CreateIconL(
        MAknsSkinInstance* aSkin, 
        CFbsBitmap*& aBitmap, 
        CFbsBitmap*& aMask, 
        const TPbkIconInfo& aIconInfo)
    {
    CreateIconLC(aSkin, aBitmap, aMask, aIconInfo);
    CleanupStack::Pop(2); // bitmap + mask
    }

EXPORT_C void PbkIconUtils::CreateIconLC(
        MAknsSkinInstance* aSkin, 
        CFbsBitmap*& aBitmap, 
        CFbsBitmap*& aMask, 
        const TPbkIconInfo& aIconInfo)
    {
    if (aIconInfo.ColorId() == KAknsIIDNone)
        {
        AknsUtils::CreateIconLC
            (aSkin, aIconInfo.SkinId(), aBitmap, aMask, 
             aIconInfo.MbmFileName(), 
             aIconInfo.IconId(), aIconInfo.MaskId());
        }
    else
        {
        AknsUtils::CreateColorIconLC
            (aSkin, aIconInfo.SkinId(), 
             aIconInfo.ColorId(), aIconInfo.ColorIndex(),
             aBitmap, aMask, 
             aIconInfo.MbmFileName(), 
             aIconInfo.IconId(), aIconInfo.MaskId(),
             AKN_LAF_COLOR_STATIC(aIconInfo.DefaultColorIndex()));
        }
    }


// End of File
