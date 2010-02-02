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
*
*/


#ifndef PBK2CONTACTVIEWDOUBLELISTBOXDATAELEMENT_H
#define PBK2CONTACTVIEWDOUBLELISTBOXDATAELEMENT_H

//  INCLUDES
#include <e32base.h>
#include <pbk2contactuicontroldoublelistboxextension.h>
#include <TPbk2IconId.h>

//FORWARD

// CLASS DECLARATION

/**
 * Double listbox data element
 *  this helper class implements MPbk2DoubleListboxDataElement interface
 */
NONSHARABLE_CLASS( CPbk2ContactViewDoubleListboxDataElement ) :
        public CBase,
        public MPbk2DoubleListboxDataElement
    {
public: // constructor and destructor
    static CPbk2ContactViewDoubleListboxDataElement* NewLC();
    ~CPbk2ContactViewDoubleListboxDataElement();

    TPtr TextPtr(
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex) const;

public: // from MPbk2DoubleListboxDataElement
    const TPbk2IconId& IconId( 
            MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex ) const;
    const TDesC& Text( 
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const;
    TTextDataType TextType(
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const;
    
    void SetIconId( 
            MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex, 
            const TPbk2IconId& aIconId );
    void SetText( 
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex, 
            HBufC* aBuffer,
            MPbk2DoubleListboxDataElement::TTextDataType aType );
private:
    inline CPbk2ContactViewDoubleListboxDataElement();
    inline void ConstructL();

private:
    TFixedArray<TPbk2IconId, 3> iIcons;
    TFixedArray<HBufC*, 2> iTexts;
    TFixedArray<TTextDataType, 2 > iTextType;
    HBufC* iEmptyBuffer;
    };
        
#endif // PBK2CONTACTVIEWDOUBLELISTBOXDATAELEMENT_H

// End of File
