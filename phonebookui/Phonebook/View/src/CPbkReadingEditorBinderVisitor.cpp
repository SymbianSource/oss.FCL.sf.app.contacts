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
#include "CPbkReadingEditorBinderVisitor.h"  // this class
#include "MPbkFieldEditorControl.h"
#include <PbkDebug.h>

#include <AknReadingConverter.h>
#include <featmgr.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkReadingEditorBinderVisitor::CPbkReadingEditorBinderVisitor()
    {
    }


CPbkReadingEditorBinderVisitor::~CPbkReadingEditorBinderVisitor()
    {
    if (iFirstNameConverter)
        {
        delete iFirstNameConverter;
        }
    if (iLastNameConverter)
        {
        delete iLastNameConverter;
        }
    }
    

CPbkReadingEditorBinderVisitor* CPbkReadingEditorBinderVisitor::NewL()
    {
    return new(ELeave) CPbkReadingEditorBinderVisitor();
    }


void CPbkReadingEditorBinderVisitor::NotifyEditorDeletion(TPbkFieldId aFieldId)
    {
    // If a reading field is being destroyed, the NameConverter must be deleted
    // first
    if (aFieldId == EPbkFieldIdLastNameReading)
        {
        delete iLastNameConverter;
        iLastNameConverter = NULL;
        }
    else if (aFieldId == EPbkFieldIdFirstNameReading)
        {
        delete iFirstNameConverter;
        iFirstNameConverter = NULL;
        }
    }
    
    
void CPbkReadingEditorBinderVisitor::VisitL(MPbkFieldEditorControl& aThis)
    {
    if (!FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        return;
        }

    if (!iFirstNameConverter)
        {
        iFirstNameConverter = CReadingConverter::NewL();
        }
    if (!iLastNameConverter)
        {
        iLastNameConverter = CReadingConverter::NewL();
        }

    // Set reading converters according to field id
    switch(aThis.FieldId())
        {
        case EPbkFieldIdLastName:
            {
            iLastNameConverter->SetMainEditor(*aThis.Control());
            break;
            }
        case EPbkFieldIdLastNameReading:
            {
            iLastNameConverter->SetReadingEditor(*aThis.Control());
            break;
            }
        case EPbkFieldIdFirstName:
            {
            iFirstNameConverter->SetMainEditor(*aThis.Control());
            break;
            }
        case EPbkFieldIdFirstNameReading:
            {
            iFirstNameConverter->SetReadingEditor(*aThis.Control());
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }


// End of File
