/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "pbk2contactviewdoublelistboxdataelement.h"

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListboxDataElement* 
CPbk2ContactViewDoubleListboxDataElement::NewLC()
    {
    CPbk2ContactViewDoubleListboxDataElement* self =
        new ( ELeave ) CPbk2ContactViewDoubleListboxDataElement();

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::CPbk2ContactViewDoubleListboxDataElement
// --------------------------------------------------------------------------
//
inline CPbk2ContactViewDoubleListboxDataElement::CPbk2ContactViewDoubleListboxDataElement()
    {
    iTexts.Reset();
    iIcons.Reset();
    iTextType.Reset();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactViewDoubleListboxDataElement::ConstructL()
    {
    iEmptyBuffer = KNullDesC().AllocL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::~CPbk2ContactViewDoubleListboxDataElement
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListboxDataElement::~CPbk2ContactViewDoubleListboxDataElement()
    {
    iTexts.DeleteAll();
    delete iEmptyBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::TextPtr
// --------------------------------------------------------------------------
//
TPtr CPbk2ContactViewDoubleListboxDataElement::TextPtr(
        MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex) const
    {
    if( iTexts[aIndex] )
        {
        return iTexts[aIndex]->Des();
        }
    return iEmptyBuffer->Des();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::IconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2ContactViewDoubleListboxDataElement::IconId( 
        MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex ) const
    {
    return iIcons[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::Text
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactViewDoubleListboxDataElement::Text( 
        MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const
    {
    if( iTexts[aIndex] )
        {
        return *(iTexts[aIndex]);
        }
    return *iEmptyBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::TextType
// --------------------------------------------------------------------------
//
MPbk2DoubleListboxDataElement::TTextDataType 
CPbk2ContactViewDoubleListboxDataElement::TextType(
    MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const
    {
    return iTextType[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::SetIconId
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListboxDataElement::SetIconId(
        MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex, 
        const TPbk2IconId& aIconId )
    {
    iIcons[aIndex] = aIconId;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListboxDataElement::SetText
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListboxDataElement::SetText( 
        MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex, 
        HBufC* aBuffer,
        MPbk2DoubleListboxDataElement::TTextDataType aType )
    {
    delete iTexts[aIndex];
    iTexts[aIndex] = aBuffer;
    iTextType[aIndex] = aType;
    }

// End of File
