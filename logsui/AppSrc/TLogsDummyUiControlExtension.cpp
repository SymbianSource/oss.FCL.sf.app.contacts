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
#include "TLogsDummyUiControlExtension.h"


// ================= MEMBER FUNCTIONS =======================

TLogsDummyUiControlExtension::TLogsDummyUiControlExtension() 
    {
    }

TLogsDummyUiControlExtension::~TLogsDummyUiControlExtension()
    {
    }

void TLogsDummyUiControlExtension::AppendIconsL( CAknIconArray* /*aArray*/ )
    {
    }

void TLogsDummyUiControlExtension::DoRelease()
    {
    }

TBool TLogsDummyUiControlExtension::GetIconIndexL( TContactItemId /*aContactId*/,
                                                  TInt& /*aIndex*/ )
    {
    return EFalse;
    }

// Sawfish VoIP changes  >>>>
void TLogsDummyUiControlExtension::HandleAdditionalData( 
                                MLogsModel& /*aModel*/, 
                                CAknDoubleGraphicStyleListBox& /*aListBox*/ )
    {    
    }
    
void TLogsDummyUiControlExtension::ModifyIconString ( TDes& /*aDes*/, 
                                                      const MLogsEventGetter& /*aLogsEventGetter*/ )
    {    
    }
    
void TLogsDummyUiControlExtension::SetObserver( MLogsExtObserver&
                                                /*aObserver*/ )
    {    
    }
// <<<<  Sawfish VoIP changes  
