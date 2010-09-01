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
* Description:  Phonebook 2 UI control UI extension interface.
*
*/


#ifndef PBK2DOUBLELISTBOXCMDITEMEXTENSION_H
#define PBK2DOUBLELISTBOXCMDITEMEXTENSION_H

//  FORWARD DECLARATIONS
class MPbk2DoubleListboxDataElement;
class MVPbkContactLink;

// CLASS DECLARATION
/**
 * This class is an extension to MPbk2UiControlCmdItem.
 * See documentation of MPbk2UiControlCmdItem from header
 * MPbk2UiControlCmdItem.h
 *
 * You can access this extension by calling
 * MPbk2UiControlCmdItem->ControlCmdItemExtension()
 */
class MPbk2DoubleListboxCmdItemExtension
    {
    public: // Interface

        /**
         *
         */
        virtual void FormatDataL(
                MPbk2DoubleListboxDataElement& aDataElement ) =0;

        /*
         * Returns MPbk2UiControlCmdItem related link (for example My card) if exists, otherwise NULL
         */
        virtual const MVPbkContactLink* Link() = 0;
        
    protected:
        /**
         * Destructor.
         */
        virtual ~MPbk2DoubleListboxCmdItemExtension() {};
    };

#endif // PBK2DOUBLELISTBOXCMDITEMEXTENSION_H

// End of File
