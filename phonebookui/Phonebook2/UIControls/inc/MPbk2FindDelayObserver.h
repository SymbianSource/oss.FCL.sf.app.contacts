/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 find delay observer interface.
*
*/


#ifndef MPBK2FINDDELAYOBSERVER_H
#define MPBK2FINDDELAYOBSERVER_H

/**
 * @internal Only Phonebook 2 internal use supported!
 */
class MPbk2FindDelayObserver
	{
	public: // Interface
        /**
         * Find delay complete
         */	
        virtual void FindDelayComplete() = 0;
    };
	
#endif // MPBK2FINDDELAYOBSERVER_H

// End of File
