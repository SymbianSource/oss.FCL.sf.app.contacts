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
*     STM's Received Calls view control container
*
*/


#ifndef     __Logs_App_CLogsRecentListControlContainer_H__
#define     __Logs_App_CLogsRecentListControlContainer_H__


//  INCLUDES
#include "CLogsBaseControlContainer.h"

//  FORWARD DECLARATIONS
class CCoeControl;
class CAknDoubleGraphicStyleListBox;
class CLogsRecentListView;
class MLogsUiControlExtension;
class CLogsRecentListAdapter;

//  CLASS DECLARATIONS

/**
 *  STM's Received Calls view control container
 */
class CLogsRecentListControlContainer : public CLogsBaseControlContainer
    {
    public:  // interface
        /**
         *  Create the STM app view control container
         *
         *  @param aRecentView, pointer to view
         *  @param aRect, client rect size
         *
         *  @return New STM app view control container object
         */
        static CLogsRecentListControlContainer* NewL
                                                (   CLogsRecentListView* aView
                                                ,   const TRect& aRect
                                                );

        /**
         *  Destructor
         */
        ~CLogsRecentListControlContainer();

    private:
        /**
         *  C++ Constructor 
         *
         *  @param aAppView Application view's pointer
         */
        CLogsRecentListControlContainer( CLogsRecentListView* aView );

        /**
         *  Constructor - Second Phase
         *
         *  @param aRect, client rect size.
         */
        void ConstructL( const TRect& aRect );

    public:
        /**
         *  Get method to iListBox
         *
         *  @return iListBox
         */
        CAknDoubleGraphicStyleListBox* ListBox();
        
        /**
         * Force an empty listbox text
         */
        void ForceEmptyTextListBox();

        /**
         * Return empty listbox resource id
         * @return recent list specific emptylistbox resource id
         */
        TInt EmptyListboxResourceL() const;

        /**
         * ControlExtension
         * @return ControlExtension
         */
        MLogsUiControlExtension* ControlExtension();

        /**
         * CLogsRecentListView
         * @return CLogsRecentListView
         */
        CLogsRecentListView* View();

		/**
		 * Disabled, possible future need: Initiate some other type of call  
         * with a long tap event (like video call).
         *
         * Overrides baseclass callback function for CAknLongTapDetector member object.
	     * 
		 * @param aPenEventLocation Long tap event location relative to parent control
         * @param aPenEventScreenLocation Long tap event location relative to screen
		 */
        //void HandleLongTapEventL( const TPoint& aPenEventLocation, 
        //                    	    const TPoint& aPenEventScreenLocation );
       
    private:
        /**
         *  Append Phone type icons to icon array
         *
         *  @param aIcons                 Icon array
         */
         /**********************************************************************
        //Not in use anymore, Phonebook icons replaced by own icons 
        void AddPhoneTypeIconsL( CAknIconArray* aIcons );
         **********************************************************************/

        /**
         *  Create extension control
         */
        void CreateControlExtensionL();
        
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);

    protected:  // from CCoeControl
        /**
         *  Get the sub control at aIndex
         *
         *  @param aIndex   Pointer aIndex
         *
         *  @return Sub control at aIndex
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         *  Called when the control's size has changed
         */
        void SizeChanged();

        void GetHelpContext( TCoeHelpContext& aContext ) const;

    protected:  // CLogsBaseControlContainer
        /**
         *  Called from HandleResourceChange. Calls AddIconL funtion
         *  for each icon in the list box.
         *
         */
        void AddControlContainerIconsL();

    public:
        /**
         * Update contents
         */
        void UpdateL();

    private:  // data
        /// Received control's pointer
        CAknDoubleGraphicStyleListBox*  iListBox;

        /// Ref: Recent view
        CLogsRecentListView*            iView;

        /// Own: control extension
        MLogsUiControlExtension*        iControlExtension;

        // Offset n of last own icon in icon array (icons 0 to n). Note, in the 
        // same icon array may be additional icons that are loaded and used by 
        // control extension (icons n+1 to m).
        TInt                            iLastOwnIconOffset;

        //Ref: listbox adapter
        CLogsRecentListAdapter*         iAdapter;

    };

#endif  // __Logs_App_CLogsRecentListControlContainer_H__

// End of File
