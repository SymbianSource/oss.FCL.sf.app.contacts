/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Interface to be used by field listbox model for analyzing
*       contact field instances. 
*
*/


#ifndef MPBK2FIELDANALYZER_H
#define MPBK2FIELDANALYZER_H

//  FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CPbk2PresentationContactField;
class MVPbkBaseContactFieldCollection;

// CLASS DECLARATION

/**
 * @internal Only Phonebook 2 internal use supported!
 */
class MPbk2FieldAnalyzer
	{
	
    public: // Enumeration used for the voip support flag
        enum TVoIPSupport
            {
            EVoIPSupportSip      = 1,
            EVoIPSupportBasic    = 2,
            EVoIPSupportXspId    = 4,
            EVoIPSupportCallout  = 8    
            };
	
    public: // Interface
        /**
         * Analyses if the field has a speed dial definition.
         *
         * @param aField    The field to analyze.
         * @return ETrue if field has speed dial definition, otherwise EFalse.
         */	
        virtual TBool HasSpeedDialL(
                const MVPbkStoreContactField& aField ) const = 0;

        /**
         * Analyses if the field has a voice tag.
         *
         * @param aField    The field to analyze.
         * @return ETrue if field has voice tag definition, otherwise EFalse.
         */	
        virtual TBool HasVoiceTagL(
                const MVPbkStoreContactField& aField ) const = 0;

        /**
         * Analyses is the field hidden, ie. should it not be shown in UI.
         *
         * @param aField    The field to analyze.
         * @return ETrue if field is hidden, EFalse otherwise.
         */	
        virtual TBool IsHiddenField(
                const CPbk2PresentationContactField& aField ) const = 0;
        /**
         * Analyses the installed voip capabilities in comparison to
         * the details stored in a contact.
         *
         * @param aFields    The fields from a contact to be analysed
         * @return TInt containing MPbk2FieldAnalyzer::TVoIPSupport flags.
         */ 
        virtual const TInt VoipSupportL( 
                const MVPbkBaseContactFieldCollection& aFields ) const = 0;
        /**
         * Analyses whether a specific field can be supported by the voip
         * capabilities installed. This function is used hand in hand with 
         * VoipSupportL()
         *
         * @param aVpbkFieldResId    The field's FieldTypeResId from contact
         *                           details of a contact to be analysed.
         * @param aVoipFlag          Voip capabilities available.     
         * @return ETrue if the field has voip support installed in the phone.
         */
        virtual TBool IsFieldVoipCapable( 
                TInt aVpbkFieldResId, const TInt aVoipFlag ) const = 0;
        

    };
	
#endif // MPBK2FIELDANALYZER_H

// End of File
