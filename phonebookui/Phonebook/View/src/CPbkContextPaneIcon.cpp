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
*         Methods for Class which loads Phonebook's icon into context pane.
*
*/


// INCLUDE FILES
#include "CPbkContextPaneIcon.h"  // This class
#include "PbkUID.h"
#include <barsread.h>   // TResourceReader
#include <eikspane.h>   // CEikStatusPane
#include <avkon.hrh>    // Status pane control UIDs
#include <eikimage.h>   // CEikImage
#include <akncontext.h> // CAknContextPane
#include <pbkview.rsg>  // Phonebook view dll resource IDs
#include <AknsSkinInstance.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsControlContext.h>
#include <aknconsts.h>	// Context pane bitmap size
#include <layoutmetadata.cdl.h>

#include "PbkIconInfo.h"

/// Unnamed namespace for local definitions
namespace {

static CEikImage* CopyL(const CEikImage& aSrc)
    {       
    CEikImage* copy = new(ELeave) CEikImage;
    CleanupStack::PushL(copy);
    copy->SetPictureOwnedExternally(EFalse);
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    copy->SetBitmap(bitmap);

    const CFbsBitmap* sourceBitmap = aSrc.Bitmap();
    const CFbsBitmap* sourceMask = aSrc.Mask();
    if (sourceBitmap)
        {
        User::LeaveIfError(bitmap->Duplicate(sourceBitmap->Handle()));
        }
    if (sourceMask)
        {
        CFbsBitmap* mask = new(ELeave) CFbsBitmap;
        // Null mask is not be copied.
        copy->SetMask(mask);                   
        User::LeaveIfError(mask->Duplicate(sourceMask->Handle()));
        }
    CleanupStack::Pop();  // copy
    return copy;
    }


} // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkContextPaneIcon::CPbkContextPaneIcon()
    {
    }

CPbkContextPaneIcon* CPbkContextPaneIcon::NewL(CEikonEnv& aEikonEnv)
    {
    CPbkContextPaneIcon* self = new(ELeave) CPbkContextPaneIcon;
    CleanupStack::PushL(self);
    self->StoreContextPaneIconL(aEikonEnv);
    self->SetPhonebookContextPaneIconL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContextPaneIcon::~CPbkContextPaneIcon()
    {
    RestoreContextPaneIcon();
    }

void CPbkContextPaneIcon::StoreContextPaneIconL(CEikonEnv& aEikonEnv)
    {
    CEikStatusPane* statusPane = aEikonEnv.AppUiFactory()->StatusPane();
    if (statusPane &&
        statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsPresent() &&
        !Layout_Meta_Data::IsLandscapeOrientation())
        {
        iContextPane = static_cast<CAknContextPane*>
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidContext)));
        const CEikImage& picture = iContextPane->Picture();
        if (picture.Bitmap() &&
            picture.Bitmap()->Handle())
            {
            iPreviousContextPaneIcon = CopyL(picture);
            }
        }
    }

void CPbkContextPaneIcon::SetPhonebookContextPaneIconL()
    {
    if (iContextPane)
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        AknsUtils::CreateAppIconLC( AknsUtils::SkinInstance(), 
            TUid::Uid(KPbkUID3), EAknsAppIconTypeContext,
            bitmap, mask );

        CleanupStack::Pop(2); // bitmap, mask
        iContextPane->SetPicture(bitmap, mask);
        iContextPane->DrawNow();
        }
    }

void CPbkContextPaneIcon::RestoreContextPaneIcon()
    {
    if (iPreviousContextPaneIcon)
        {
        iContextPane->SetPicture(iPreviousContextPaneIcon);
        iPreviousContextPaneIcon = NULL;
        }
    }

//  End of File  
