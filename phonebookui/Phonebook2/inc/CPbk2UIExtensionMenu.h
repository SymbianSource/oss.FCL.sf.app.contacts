/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 UI Extension menu information.
*
*/


#ifndef CPBK2UIEXTENSIONMENU_H
#define CPBK2UIEXTENSIONMENU_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CPbk2UIExtensionMenu;

/**
 * Phonebook 2 UI Extension menu information.
 */
NONSHARABLE_CLASS(CPbk2UIExtensionMenu) : public CBase
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class by reading the
         * data from the resource reader (PHONEBOOK2_EXTENSION_MENU_RESOURCE).
         *
         * @param aReader   Resource reader where the menu data
         *                  is read from.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionMenu* NewLC(
                TResourceReader& aReader );

        /**
         * Creates a new instance of this class.
         *
         * @param aViewId   Id of the view whose menu these items are brought to.
         * @param aMenuFilteringFlags   Flags that must have been turned on to 
         *                              enable this menu. 
         *                              See TPbk2UIExtensionMenuFilteringFlags.
         * @param aParentMenuResourceId Resource id of the menu that these 
         *                              items will be added to.
         * @param aPreviousCommandId    Command id of the item that these menu 
         *                              items will follow.
         * @param aMenuResourceId       Menu resource id that will be appended.
         * @param aCascadingMenuCmd     In case the menuPane is a cascading menu 
         *                              pane and the same cascading menu pane is 
         *                              used from multiple extensions, this has 
         *                              to be the command id of the cascading 
         *                              menu pane.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2UIExtensionMenu* NewLC(TUid aViewId, 
                TUint32 aMenuFilteringFlags,
                TInt aParentMenuResourceId, TInt aPreviousCommandId, 
                TInt aMenuResourceId, TInt aCascadingMenuCmd = 0 );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionMenu();

    public: // Interface
        /**
         * Returns the id of the view that this menu is attached to.
         *
         * @return  Id of the view that this menu is attached to.
         */
        TUid ViewId() const;

        /**
         * Returns the id of the menu that this menu is part of.
         *
         * @return  Id of the menu that this menu is part of.
         */
        TInt ParentMenuId() const;

        /**
         * Returns the id of the command that this menu pane should follow.
         *
         * @return  Id of the command that this menu pane should follow.
         */
        TInt PreviousCommand() const;

        /**
         * Returns the id of this menu in the resources.
         *
         * @return  Id of this menu in the resources.
         */
        TInt MenuResId() const;

        /**
         * Returns the command id of this menu in case
         * this is a cascading menu.
         *
         * @return  Command id of this menu in case this
         *          is a cascading menu.
         */
        TInt CascadingMenuCmd() const;

        /**
         * Returns ETrue if this menu pane is visible in the given view
         * and in the given menu when the given flags are on.
         *
         * @param aViewId           Id of the view where the menu
         *                          should be on.
         * @param aParentMenuId     Id of the menu that will be
         *                          the parent of the menu.
         * @param aFilteringFlags   Menu filtering flags that are on
         *                          in the current state of the UI.
         * @return  ETrue if this menu pane is visible when
         *          the given flags are on.
         */
        TBool IsVisible(
                TUid aViewId,
                TInt aParentMenuId,
                TUint32 aFilteringFlags ) const;

    private: // Implementation
        CPbk2UIExtensionMenu();
        CPbk2UIExtensionMenu(TUid aViewId, TUint32 aMenuFilteringFlags, 
                TInt aParentMenuResourceId, TInt aPreviousCommandId, 
                TInt aMenuResourceId, TInt aCascadingMenuCmd );
        void ConstructL(
                TResourceReader& aReader );
        void ConstructL();

    private: // Data
        /// Own: Id of the view that this menu is attached to
        TUid iViewId;
        /// Own: Menu filtering flags that has to be on for
        ///      this menu pane to be visible
        TUint32 iMenuFilteringFlags;
        /// Own: Id of the menu that this menu is part of
        TInt iParentMenuResId;
        /// Own: Id of the command that this menu pane should follow
        TInt iPreviousCommand;
        /// Own: Id of this menu in the resources
        TInt iMenuResId;
        /// Own: Command id of this menu in case this is a cascading menu
        TInt iCascadingMenuCmd;
    };

#endif // CPBK2UIEXTENSIONMENU_H

// End of File
