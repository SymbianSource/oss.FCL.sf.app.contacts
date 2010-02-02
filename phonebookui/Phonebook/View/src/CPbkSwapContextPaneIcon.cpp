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
*         Methods for Class which loads Phonebook's or empty icon into context pane.
*
*/


// INCLUDE FILES
#include "CPbkSwapContextPaneIcon.h"  // This class
#include "PbkUID.h"
#include <eikspane.h>   // CEikStatusPane
#include <eikimage.h>   // CEikImage
#include <akncontext.h> // CAknContextPane
#include <PbkView.rsg>  // Phonebook view dll resource IDs
#include <AknsUtils.h>
#include <AknsControlContext.h>
#include <aknconsts.h>	// Context pane bitmap size
#include "PbkIconInfo.h"
#include "PbkIconId.hrh"
#include <layoutmetadata.cdl.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkSwapContextPaneIcon::CPbkSwapContextPaneIcon() : iState( EPhonebookIcon )
    {
    }

CPbkSwapContextPaneIcon* CPbkSwapContextPaneIcon::NewL(CEikonEnv& aEikonEnv)
    {
    CPbkSwapContextPaneIcon* self = new(ELeave) CPbkSwapContextPaneIcon;
    CleanupStack::PushL(self);
    self->ConstructL(aEikonEnv);
    CleanupStack::Pop(self);
    return self;
    }
void CPbkSwapContextPaneIcon::ConstructL(CEikonEnv& aEikonEnv)
    {
    CEikStatusPane* statusPane = aEikonEnv.AppUiFactory()->StatusPane();
    if (statusPane &&
        statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsPresent() &&
        !Layout_Meta_Data::IsLandscapeOrientation())
        {
        iContextPane = static_cast<CAknContextPane*>
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidContext)));
        SetPhonebookContextPaneIconL();
        LoadEmptyContextPaneIconL();
        }
    }

CPbkSwapContextPaneIcon::~CPbkSwapContextPaneIcon()
    {
    delete iContextPaneIcon;
    }

void CPbkSwapContextPaneIcon::SetPhonebookContextPaneIconL()
    {
    if (iContextPane)
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateAppIconLC( AknsUtils::SkinInstance(),
            TUid::Uid(KPbkUID3), EAknsAppIconTypeContext,
            bitmap, mask );

        iContextPane->SetPicture(bitmap, mask);        
        CleanupStack::Pop(2); //bitmap, mask
        }
    }
void CPbkSwapContextPaneIcon::LoadEmptyContextPaneIconL()
    {
    if (iContextPane)
        {
        CPbkIconInfoContainer* iconInfoContainer =
			CPbkIconInfoContainer::NewL(R_PBK_EMPTY_ICON_INFO_ARRAY, CCoeEnv::Static());
		CleanupStack::PushL( iconInfoContainer );
        const TPbkIconInfo* iconInfo = iconInfoContainer->Find( EPbkqgn_menu_empty_cxt );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        PbkIconUtils::CreateIconLC(AknsUtils::SkinInstance(), bitmap, mask, *iconInfo);
        iContextPaneIcon = new(ELeave)CEikImage;
        iContextPaneIcon->SetNewBitmaps( bitmap, mask );
        CleanupStack::Pop(2); //bitmap, mask
        CleanupStack::PopAndDestroy(); //iconInfoContainer
        }
    }
void CPbkSwapContextPaneIcon::ShowContextPaneIcon( const TSwapState aState )
    {
    if ( iState != aState && iContextPane )
        {
        iContextPaneIcon = iContextPane->SwapPicture( iContextPaneIcon );        
        iState = aState;
        }
    }

//  End of File
