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
* Description:  Phonebook 2 server application title pane manager.
*
*/


#include "TPbk2TitlePaneOperator.h"

// Phonebook 2
#include <Pbk2UID.h>
#include <Pbk2CommonUi.rsg>

// Virtual Phonebook

// System includes
#include <akntitle.h>
#include <akncontext.h>
#include <StringLoader.h>
#include <eikspane.h>
#include <avkon.hrh> // EEikStatusPaneUidTitle
#include <AknsUtils.h>

// --------------------------------------------------------------------------
// TPbk2TitlePaneOperator::TPbk2TitlePaneOperator
// --------------------------------------------------------------------------
//
TPbk2TitlePaneOperator::TPbk2TitlePaneOperator()
    {
    }

// --------------------------------------------------------------------------
// TPbk2TitlePaneOperator::SetTitlePaneL
// --------------------------------------------------------------------------
//
void TPbk2TitlePaneOperator::SetTitlePaneL( HBufC* aTitlePaneText )
    {
    if ( !aTitlePaneText )
        {
        HBufC* title = StringLoader::LoadL( R_QTN_PHOB_TITLE );
        ChangeTitlePaneL( title );
        ChangeContextIconL();
        }
    else
        {
        ChangeTitlePaneL( aTitlePaneText );
        }
    }

// --------------------------------------------------------------------------
// TPbk2TitlePaneOperator::ChangeTitlePaneL
// --------------------------------------------------------------------------
//
void TPbk2TitlePaneOperator::ChangeTitlePaneL( HBufC* aTitle  )
    {
    CleanupStack::PushL( aTitle );

    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
            ( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        CleanupStack::Pop( aTitle );

        titlePane->SetText( aTitle ); // takes ownership of title
        }
    else
        {
        CleanupStack::PopAndDestroy( aTitle );
        }
    }

// --------------------------------------------------------------------------
// TPbk2TitlePaneOperator::ChangeContextIconL
// --------------------------------------------------------------------------
//
inline void TPbk2TitlePaneOperator::ChangeContextIconL()
    {
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
            ( TUid::Uid( EEikStatusPaneUidContext ) ).IsInCurrentLayout() )
        {
        CAknContextPane* contextPane = static_cast<CAknContextPane*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        AknsUtils::CreateAppIconLC
            ( AknsUtils::SkinInstance(), TUid::Uid( KPbk2UID3 ),
              EAknsAppIconTypeContext, bitmap, mask );

        CleanupStack::Pop( 2 ); // bitmap, mask
        contextPane->SetPicture(bitmap, mask);
        }
    }

// End of File
