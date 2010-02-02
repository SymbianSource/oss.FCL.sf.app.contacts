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
*       Performs menu pane filtering. Common filtering methods are in
*		app view classes, this class contains filtering methods for
*		commands that differ in different Series 60 releases.
*
*/


#ifndef __CPbkMenuFiltering_H__
#define __CPbkMenuFiltering_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikMenuPane;

// CLASS DECLARATION

/**
 * Provides some additional common application view functionality. 
 */
class CPbkMenuFiltering : public CBase
    {
	public:  // Interface
		/**
         * Performs application launcher submenu filtering.
		 * @param aMenuPane menu pane to filter
		 * @param aFlags view flags
         */
		static void SettingsPaneMenuFilteringL
			(CEikMenuPane& aMenuPane, TUint aFlags = 0);

		/**
         * Performs groups list menu filtering.
		 * @param aMenuPane menu pane to filter
		 * @param aFlags view flags
         */
		static void GroupsListPaneMenuFilteringL
			(CEikMenuPane& aMenuPane, TUint aFlags = 0);
    };

#endif  // __CPbkMenuFiltering_H__

// End of File
