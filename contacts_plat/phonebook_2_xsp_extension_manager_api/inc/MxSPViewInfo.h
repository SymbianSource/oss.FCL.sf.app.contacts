/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract interface for getting Phonebook xSP extension view data.
*
*/


#ifndef __MXSPVIEWINFO_H__
#define __MXSPVIEWINFO_H__


//  INCLUDES
#include <e32std.h>

//  FORWARD DECLARATIONS

// 	CLASS DECLARATION

/**
 * Abstract interface for getting Phonebook xSP extension view data.
 */
class MxSPViewInfo
    {        
    public:  // Destructor
    
        /**
         * Destructor.
         */
        virtual ~MxSPViewInfo() {}

    public: // Interface
        
        /**
         * Gets the old/original view ID
         * 
         * @return Old/original view ID
         */
        virtual TInt32 OldViewId() const = 0;
        
        /**
         * Gets info whether this view is a tabbed view
         * 
         * @return ETrue if this is a tab-view, otherwise EFalse
         */
        virtual TBool TabView() const = 0;
        
        /**
         * Gets the tab icon ID of tab view
         * 
         * @return tab icon ID of tab view, KErrNotFound if
         *	not a tab view or graphics not used
         */
        virtual TInt32 TabIconId() const = 0;
        
        /**
         * Gets the tab icon mask ID of tab view
         * 
         * @return tab icon mask ID of tab view,
         *	KErrNotFound if not a tab view or graphics not used	
         */
        virtual TInt32 TabMaskId() const = 0;
        
        /**
         * Gets the tab icon file of tab view
         * 
         * @return tab icon file of tab view.
         *	file path must be given without drive part
         *	i.e. "\\path\\file.mbm". If not a tab view or
         *	graphics not used "\\" should be returned
         */
        virtual const TDesC& TabIconFile() const = 0;
                
        /**
         * Gets the view name
         * 
         * @return view name or KNullDesC
         */
        virtual const TDesC& Name() const = 0; 
        
        /**
         * Gets the sort icon ID of tab view
         * 
         * @return sort icon ID of tab view, KErrNotFound if
         *	not a tab view or graphics not used
         */
        virtual TInt32 SortIconId() const = 0;
        
        /**
         * Gets the sort icon mask ID of tab view
         * 
         * @return sort icon mask ID of tab view,
         *	KErrNotFound if not a tab view or graphics not used	
         */
        virtual TInt32 SortMaskId() const = 0;
        
        /**
         * Gets the sort icon file of tab view
         * 
         * @return sort icon file of tab view.
         *	file path must be given without drive part
         *	i.e. "\\path\\file.mbm". If not a tab view or
         *	graphics not used "\\" should be returned
         */
        virtual const TDesC& SortIconFile() const = 0;
    };
        
    
#endif // __MXSPVIEWINFO_H__

// End of File
