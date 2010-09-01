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
*     Logs "GPRS counter" view container control class implementation
*
*/


#ifndef     __Logs_App_CGprsCtControlContainer_H__
#define     __Logs_App_CGprsCtControlContainer_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsBaseView;
class CAknDoubleLargeStyleListBox;


// CLASS DECLARATION

/**
 * Logs "GPRS counter" view container control class implementation.
 */
class CGprsCtControlContainer : public CLogsBaseControlContainer
	{
	public:  // interface
		/**
		 *  Standard creation function.
		 *
		 *  @param  aAppView    this control's application view.
		 *  @param aRect rect size
		 */
		static CGprsCtControlContainer* NewL
										(	CLogsBaseView* aAppView
										,	const TRect& aRect
										);

		/**
		 *  Destructor. Deletes child controls.
		 */
		~CGprsCtControlContainer();

	public:
		/**
		 *  Returns the iListBox.
		 *
		 *  @return listbox control
		 */
		CAknDoubleLargeStyleListBox* ListBox();

        /**
		 * Update contents
		 */
		void UpdateL();

	private:
		/**
		 *  Constructor, second phase.
		 *
		 *  @param aRect rect size
		 */
		void ConstructL( const TRect& aRect );

		/**
		 * C++ default constructor
		 */
		CGprsCtControlContainer( CLogsBaseView* aAppView );
		
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);

	protected:  // from CCoeControl
		/**
		 *  Returns the child controls at aIndex.
		 *
		 *  @param  aIndex  Index of the control.
		 *
		 *  @return Sub control from the aIndex.
		 */
		CCoeControl* ComponentControl( TInt aIndex ) const;

		/**
		 *  Called when control's size changed.
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

	private:  // data
		/// Ref: The application view whose control container this class is.
		CLogsBaseView*					iAppView;

		/// Own: ListBox style
		CAknDoubleLargeStyleListBox*	iListBox;
	};

#endif  // __Logs_App_CGprsCtControlContainer_H__

// End of File

