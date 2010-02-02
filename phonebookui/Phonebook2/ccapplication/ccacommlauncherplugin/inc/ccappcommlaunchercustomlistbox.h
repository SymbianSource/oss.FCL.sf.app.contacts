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
* Description: 
*
*/
/*
 * ccappcommlaunchercustomlistbox.h
 *
 *  Created on: 2009-10-30
 *      Author: dev
 */

#ifndef CCAPPCOMMLAUNCHERCUSTOMLISTBOX_H_
#define CCAPPCOMMLAUNCHERCUSTOMLISTBOX_H_

//  INCLUDES
#include <aknlists.h>

// FORWARD DECLARATIONS
class CCCAppCommLauncherCustomListBoxData;


NONSHARABLE_CLASS(CCCAppCommLauncherCustomListBox) :
            public CAknDoubleLargeStyleListBox
    {                   
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         */
        static CCCAppCommLauncherCustomListBox* NewL();

        /**
         * Destructor.
         */
        ~CCCAppCommLauncherCustomListBox();
        
    public:
    	
        /** CreateItemDrawerL()
        *
        * Chooses which itemdrawer to use.
        */
        void CreateItemDrawerL(); 
        
        void SizeChanged();
        
    private: // Implementation
    	
    	CCCAppCommLauncherCustomListBox();

    private: // Data
        /// Doesnt Own - Ownership Transferred to CPbk2ContactViewCustomListBoxItemDrawer
    	CCCAppCommLauncherCustomListBoxData* iColumnData;
    };


#endif /* CCAPPCOMMLAUNCHERCUSTOMLISTBOX_H_ */
