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
*
*/


// INCLUDE FILES
#include "TLogsDummyViewExtension.h"


// ================= MEMBER FUNCTIONS =======================

TLogsDummyViewExtension::TLogsDummyViewExtension()
    {
    }

TLogsDummyViewExtension::~TLogsDummyViewExtension()
    {
    }

void TLogsDummyViewExtension::DoRelease()
    {
    }
        
void TLogsDummyViewExtension::DynInitMenuPaneL
        (TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/,
        const MLogsEventGetter* /*aEvent*/)
    {
    }

TBool TLogsDummyViewExtension::HandleCommandL(TInt /*aCommandId*/)
    {
    return EFalse;
    }

// Sawfish VoIP changes  >>>>
void TLogsDummyViewExtension::SetSendUi( CSendUi& /*aSendUi*/ )
    {    
    }
    
void TLogsDummyViewExtension::SetSendUiText( const TDesC& /*aSendUiText*/ )
    {    
    }
// <<<<  Sawfish VoIP changes  

// End of File
