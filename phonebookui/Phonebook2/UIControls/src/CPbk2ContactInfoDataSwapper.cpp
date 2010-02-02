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
* Description:  Phonebook 2 contact info data swapper utility class.
*
*/


#include "CPbk2ContactInfoDataSwapper.h"

// Phonebook 2
#include "CPbk2ThumbnailLoader.h"

// System includes
#include <eikfrlb.h>

// --------------------------------------------------------------------------
// CPbk2ContactInfoDataSwapper::CPbk2ContactInfoDataSwapper
// --------------------------------------------------------------------------
//
CPbk2ContactInfoDataSwapper::CPbk2ContactInfoDataSwapper()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoDataSwapper::~CPbk2ContactInfoDataSwapper
// --------------------------------------------------------------------------
//
CPbk2ContactInfoDataSwapper::~CPbk2ContactInfoDataSwapper()
    {
    if (iListBox)
        {
        iListBox->Model()->SetItemTextArray(iItemTextArray);
        if (iTopItemIndex >= 0)
            {
            iListBox->SetTopItemIndex(iTopItemIndex);
            }
        if (iCurrentItemIndex >= 0)
            {
            iListBox->SetCurrentItemIndex(iCurrentItemIndex);
            }
        }
    delete iListBoxModel;
    delete iThumbnailLoader;
    delete iTitlePaneText;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoDataSwapper::StoreListBoxState
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoDataSwapper::StoreListBoxState
        ( CEikTextListBox& aListBox )
    {
    iListBox = &aListBox;
    iItemTextArray = iListBox->Model()->ItemTextArray();
    iTopItemIndex = iListBox->TopItemIndex();
    iCurrentItemIndex = iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoDataSwapper::ResetListBoxState
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoDataSwapper::ResetListBoxState()
    {
    iListBox = NULL;
    }

// End of File
