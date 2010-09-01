/*
* Copyright (c) 2002 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  GlobalNoteObserver interface for listening global note events.
*
*/


#ifndef M_GLOBAL_NOTE_OBSERVER_H
#define M_GLOBAL_NOTE_OBSERVER_H

#include <e32def.h>

/**
 * An observer API for listening state changes for global notes.
 */
class MGlobalNoteObserver
    {
    public:
        /**
         * A global note has been closed.
         *
         * @param aResult The result.
         */
        virtual void GlobalNoteClosed( const TInt aResult ) = 0;

        /**
         * There was an error in showing the global note.
         *
         * @param aErr Symbian error code.
         */
        virtual void GlobalNoteError( const TInt aErr ) = 0;
    };

#endif // M_GLOBAL_NOTE_OBSERVER_H

// End of File
