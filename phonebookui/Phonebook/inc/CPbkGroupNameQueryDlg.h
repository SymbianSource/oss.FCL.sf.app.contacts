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
*     Group name query dialog.
*
*/


#ifndef __CPbkGroupNameQueryDlg_H__
#define __CPbkGroupNameQueryDlg_H__

//  INCLUDES
#include <AknQueryDialog.h>     // CAknTextQueryDialog

//  FORWARD DECLARATIONS
class CPbkContactEngine;

//  CLASS DECLARATION
/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook Group name query dialog. 
 */
class CPbkGroupNameQueryDlg :
        public CAknTextQueryDialog
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aDataText data text
		 * @param aEngine reference to phonebook engine
		 * @param aNameGeneration if ETrue, also the group name is updated
         */
        IMPORT_C static CPbkGroupNameQueryDlg* NewL(TDes& aDataText,
			CPbkContactEngine& aEngine, TBool aNameGeneration = ETrue);

        /**
         * Destructor.
         */
        ~CPbkGroupNameQueryDlg();

    public: // from CAknTextQueryDialog
    	IMPORT_C TBool OkToExitL(TInt aButtonId);

    private:  // implementation
        CPbkGroupNameQueryDlg(TDes& aDataText, CPbkContactEngine& aEngine);
        void ConstructL(TBool aNameGeneration);
        void UpdateGroupTitleL();

    private:  // data
        /// Ref: Contact engine
        CPbkContactEngine& iEngine; 
        /// Own: group name array
        CDesCArrayFlat* iGroupLabelsArray;
    };

#endif // __CPbkGroupNameQueryDlg_H__

// End of File
