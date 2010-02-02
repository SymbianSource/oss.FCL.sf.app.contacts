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
*     Fetch dialog page interfaces for Phonebook.
*
*/


#ifndef __MPbkFetchDlgPage_H__
#define __MPbkFetchDlgPage_H__

//  INCLUDES
#include <e32std.h>
#include <cntdef.h>     // TContactItemId

//  FORWARD DECLARATIONS
class MPbkFetchDlg;
class MPbkFetchDlgSelection;
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 * Phonebook fetch dialog page interface.
 */
class MPbkFetchDlgPage
    {
    public:  // Interface
        /**
         * Destructor
         */
        virtual ~MPbkFetchDlgPage()
			{
			}

        /**
         * Returns ETrue if the page initialisation is complete and the page
         * can be used.
         */
        virtual TBool DlgPageReady() const = 0;

        /**
         * Returns this page's id.
         */
        virtual TInt FetchDlgPageId() const = 0;

        /**
         * Activates this dialog page.
         */
        virtual void ActivateFetchDlgPageL() = 0;

        /**
         * Deactivates this dialog page.
         */
        virtual void DeactivateFetchDlgPage() = 0;

        /**
         * Returns the currently focused contact on this page.
         */
        virtual TContactItemId FocusedContactIdL() const = 0;

        /**
         * Returns ETrue if this fetch dialog page is empty
         */
        virtual TBool IsFetchDlgPageEmpty() const = 0;
        
        /**
         * Sets contact item -selection accepter
         * @param aAccepter The accepter
         */
       	virtual void SetMPbkFetchDlgSelection
       					(MPbkFetchDlgSelection* aAccepter) = 0;
       					
        /**
         * Returns ETrue if there are no selected items in this page
         */      					
       	virtual TBool ItemsMarked() const = 0;
       	
        /**
         * Sets proper layout for the controls of this dialog page.
         */
        virtual void LayoutContents() = 0;
                
         /**
         * Returns an array of currently marked items.
		 * If no items marked the array contains the focused item only.
         */
        virtual const CContactIdArray& MarkedItemsL() const =0;     
       
         /**
         * Processes Mark and Unmark commands
         * @param aCommandId Softkey mark command
         */
         virtual TBool ProcessSoftkeyMarkCommandL(TInt aCommandId) = 0;
         
         /**
         * Returns the corresponding list control of this dlg page
         * @return list control
         */
         virtual CPbkContactViewListControl& Control() = 0;
    };


/**
 * A collection of MPbkFetchDlgPage objects.
 */
class MPbkFetchDlgPages
    {
    public:  // Interface
        /**
         * Destructor
         */
        virtual ~MPbkFetchDlgPages() { }

        /**
         * Returns the number of dialog page objects in the collection.
         */
        virtual TInt DlgPageCount() const = 0;

        /**
         * Returns the aIndexth dialog page in this collection.
         */
        virtual MPbkFetchDlgPage& DlgPageAt(TInt aIndex) const = 0;

        /**
         * Returns a page with page id aPageId, NULL if not found.
         */
        virtual MPbkFetchDlgPage* DlgPageWithId(TInt aPageId) const = 0;
    };

#endif // __MPbkFetchDlgPage_H__
      
// End of File
