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
*     Logs "Sub application list" view container control class implementation
*
*/


#ifndef     __Logs_App_CLogsSubAppListControlContainer_H__
#define     __Logs_App_CLogsSubAppListControlContainer_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCoeControl;
class CAknDoubleLargeStyleListBox;
class CLogsBaseView;

// CLASS DECLARATION

/**
 * Logs "Sub application list" view container control class implementation.
 * From this view user can navigate to any sub application.
 */
class CLogsSubAppListControlContainer : public CLogsBaseControlContainer
	{
	public:  // interface
		/**
		 * Standard creation function.
		 *
		 * @param aAppView this control's application view.
		 * @param aRect control container visible size.
		 *
		 * @return New instance of this class.
		 */
		static CLogsSubAppListControlContainer* NewL
												(	CLogsBaseView* aAppView
												,	const TRect& aRect
												);

		/**
		 * Destructor. Deletes child controls.
		 */
		~CLogsSubAppListControlContainer();

	private:            
		/**
		 * C++ constructor
		 * @param aAppView this control's application view.
		 */
		CLogsSubAppListControlContainer( CLogsBaseView* aAppView );

		/**
		 * Constructor, second phase.
		 *
		 * @param aRect control container visible size.
		 */
		void ConstructL( const TRect& aRect );
		
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);

	public: 
		/**
		 * Returns the iListBox.
		 * @return listbox instance
		 */
		CAknDoubleLargeStyleListBox* ListBox();

		/**
		 * Create listbox with contents 
		 */
		void CreateListBoxL();

		/**
		 * Create listbox contents, can be called to refresh 
		 * data in listbox
		 */
        void CreateListBoxContentsL();
            
	protected:  // from CCoeControl
		/**
		 *  Returns the child controls at aIndex.
		 *
		 *  @param  aIndex  Index of the control.
		 *
		 *  @return Sub     control from the aIndex.
		 */
		CCoeControl* ComponentControl( TInt aIndex ) const;

		/**
		 * Called when control's size changed.
		 */
		void SizeChanged();

		/**
		 *  Help context getter
		 *
		 *  @param aContext reference to context
		 */
		void GetHelpContext( TCoeHelpContext& aContext ) const;

	protected:  // CLogsBaseControlContainer
		/**
		 *  Called from HandleResourceChange. Calls AddIconL funtion
		 *  for each icon in the list box.
		 *
		 */
		void AddControlContainerIconsL();

	private:  // data
		/// Ref: The application view whose control container this class is.
		CLogsBaseView*					iAppView;

		/// Own: ListBox control
		CAknDoubleLargeStyleListBox*	iListBox;
	};

#endif  // __Logs_App_CLogsSubAppListControlContainer_H__


// End of File

