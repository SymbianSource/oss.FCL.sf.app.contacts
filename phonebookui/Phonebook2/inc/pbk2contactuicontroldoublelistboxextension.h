/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 UI control UI extension interface.
*
*/


#ifndef MPBK2CONTACTUICONTROLDOUBLELISTBOXEXTENSION_H
#define MPBK2CONTACTUICONTROLDOUBLELISTBOXEXTENSION_H

//  FORWARD DECLARATIONS
class MVPbkContactLink;
class MPbk2CommandItemUpdater;
class TPbk2IconId;

class MPbk2DoubleListboxDataElement
    {
public:
    enum TIconDataElementItems
        {
        EThumbnail      = 0x0,
        EMainIcon,
        EServiceIcon
        };
    enum TTextDataElementItems
        {
        EName           = 0x0,
        EStatusText
        };
    
    /**
     * Type of the text element.
     */
    enum TTextDataType
        {
        // Text's type not set or unknown
        ETypeNone           = 0x0,
        // Text's type is generic text
        ETypeGenericText,
        // Text's type is phone number
        ETypePhoneNumber
        };    
    
    virtual const TPbk2IconId& IconId( 
            MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex ) const =0;
    virtual const TDesC& Text( 
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const =0;
    virtual TTextDataType TextType( 
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex ) const =0;

    virtual void SetIconId( 
            MPbk2DoubleListboxDataElement::TIconDataElementItems aIndex, 
            const TPbk2IconId& aIconId ) =0;
    virtual void SetText( 
            MPbk2DoubleListboxDataElement::TTextDataElementItems aIndex, 
            HBufC* aBuffer,
            MPbk2DoubleListboxDataElement::TTextDataType aType ) =0;
    
protected:
    ~MPbk2DoubleListboxDataElement() {};
    };


// CLASS DECLARATION
/**
 * This class is an extension to MPbk2ContactUiControlExtension.
 * See documentation of MPbk2ContactUiControlExtension from header
 * MPbk2ContactUiControlExtension.h 
 * 
 * You can access this extension by calling
 * MPbk2ContactUiControlExtension->ContactUiControlExtensionExtension()
 */
class MPbk2ContactUiControlDoubleListboxExtension
    {
    public: // Interface

        /**
         * 
         */
        virtual void FormatDataL(
                const MVPbkContactLink& aContactLink, 
                MPbk2DoubleListboxDataElement& aDataElement ) =0;
        
        /**
         * Sets Command item updater for this extension.
         *
         * @param aCommandUpdator   UI control command updater.
         *                          If NULL, association  is removed.
         */
        virtual void SetCommandItemUpdater(
            MPbk2CommandItemUpdater* aCommandUpdater ) = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MPbk2ContactUiControlDoubleListboxExtension(){}
    };

#endif // MPBK2CONTACTUICONTROLDOUBLELISTBOXEXTENSION_H

// End of File
