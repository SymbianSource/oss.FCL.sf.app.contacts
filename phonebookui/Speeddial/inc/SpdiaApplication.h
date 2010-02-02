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
* Description:     Declares main application class.
*
*/






#ifndef SPDIAAPLICATION_H
#define SPDIAAPLICATION_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidSpdia = { 0x1000590A };

// CLASS DECLARATION

/**
*  SpeedDial Application class.
*  Provides factory to create concrete document object.
*
*  @since 
*/
class CSpdiaApplication : public CAknApplication
    {
    
    private:

        /**
        * From CApaApplication, creates CSpdiaDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidSpeedDial).
        * @return The value of KUidSpdia.
        */
        TUid AppDllUid() const;
    };

#endif  // SPDIAAPLICATION_H
// End of File

