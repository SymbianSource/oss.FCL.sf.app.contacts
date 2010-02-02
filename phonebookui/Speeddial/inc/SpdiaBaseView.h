/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares base view for application.
*
*/






#ifndef SPDIABASEVIEW_H
#define SPDIABASEVIEW_H

// INCLUDES
#include <aknview.h>
#include <TPbkContactItemField.h>
#include <PhCltTypes.h>

// CONSTANTS
// UID of view
const TUid KViewId = { 0x01 };
const TUid KCallingVmbxViewId = { 0x02 };



// CLASS DECLARATION

/**
*  Speed dial Application view class. Provides some common application 
*  view functionality. 
* 
*  @since 
*/
class CSpdiaBaseView : public CAknView                       
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        */
        CSpdiaBaseView();

        /**
        * Default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CSpdiaBaseView();

    public: // New functions

        /**
        * Performs the call operation. Empty function
        * @param aNumber  call number
        */
        virtual void NumberCallL( const TDesC& /*aNumber*/,
                                  TInt& /*aNumberType*/) {}

    public: // Data

        CEikStatusPane* iStatusPane;

    };

#endif        // SPDIALBASEVIEW_H

// end of file
