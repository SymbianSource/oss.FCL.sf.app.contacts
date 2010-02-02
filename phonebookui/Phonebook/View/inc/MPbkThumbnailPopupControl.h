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
*     Phonebook phone number formatting interface.
*
*/


#ifndef MPBKTHUMBNAILPOPUPCONTROL_H
#define MPBKTHUMBNAILPOPUPCONTROL_H

//  INCLUDES
#include <e32base.h>
#include    <eiklbx.h>

// CLASS DECLARATION

/**
 * Phonebook thumbnail popup control interface
 */
class MPbkThumbnailPopupControl
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MPbkThumbnailPopupControl()
			{
			}

        /**
         * Change the bitmap aBitmap in the control.
         * Takes ownership of the bitmap
         * @aBitmap 
         * @aListBox if given and landscape is on
         *           it is used to set thumbnail near focus
         */
        virtual void ChangeBitmap( CFbsBitmap* aBitmap, CEikListBox* aListBox ) = 0;

        /**
         * Change the thumnail background.
         * Takes ownership of the bitmap
         */
        virtual void SetThumbnailBackg( CFbsBitmap* aThumbBackg, CFbsBitmap* aThumbBackgMask ) = 0;
        
        /* Calls CoeControl MakeVisible(ETrue) and DrawDeferred*/
        virtual void MakeControlVisible( TBool aVisible ) = 0;
    };

#endif // MPBKTHUMBNAILPOPUPCONTROL_H

// End of File
