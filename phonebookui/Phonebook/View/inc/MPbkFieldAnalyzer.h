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
*       Interface to be used by listbox model for analyzing
*       TPbkContactItemField instances. 
*
*/


#ifndef __MPbkFieldAnalyzer_H__
#define __MPbkFieldAnalyzer_H__

//  FORWARD DECLARATIONS
class TPbkContactItemField;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 */
class MPbkFieldAnalyzer
	{
	public: // Interface
        /**
         * Analyses if the TPbkContactItemField has a speed dial definition.
         *
         * @param   aField         field to analyze.
         * @return                 ETrue if field has speed dial definitionm,
         *                         otherwise false.
         */	
        virtual TBool HasSpeedDialL(const TPbkContactItemField& aField) const = 0;
        /**
         * Analyses if the TPbkContactItemField has a voice tag.
         *
         * @param   aField         field to analyze.
         * @return                 ETrue if field has voice tag definition
         *                         otherwise false.
         */	
        virtual TBool HasVoiceTagL(const TPbkContactItemField& aField) const = 0;
    };
	
#endif // __MPbkFieldAnalyzer_H__

// End of File
