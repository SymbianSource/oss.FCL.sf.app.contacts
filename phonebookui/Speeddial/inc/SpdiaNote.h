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
* Description:     Displays the phonenumber of a speed dial entry when 
*                'View number' is chosen from the menu.
*
*/





#ifndef     SPDIANOTE_H
#define     SPDIANOTE_H

//  INCLUDES
#include <aknstaticnotedialog.h>

//  FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Displays the phonenumber of a speed dial entry when 'View number'
*  is chosen from the menu.
*  This class provide clipping text area and clear self pointer when delete 
*  object.
*
*  @since 
*/
class CSpdiaNoteDialog  : public CAknStaticNoteDialog
    {
    public:
        /*
        * c++ Default constructor.
        */
        CSpdiaNoteDialog();
        /*
        * destructor.
        */
        virtual ~CSpdiaNoteDialog();
        /*
        * constructor.
        */
        CSpdiaNoteDialog(CSpdiaNoteDialog** aDialog);

    public: // new function 
        /*
        * Sets phone number string.
        */
        void SetPhoneNumberL(const TDesC& aNumber);

        /*
        * A text area is clipped from font size.
        */
        void ClipPhoneNumberFromBeginningL(TInt aWidth, TInt16 fontId);

    private:
        CSpdiaNoteDialog** iDialog;
        HBufC* iPhoneNumberString; // own
    };

#endif      // SPDIAINFO_H

