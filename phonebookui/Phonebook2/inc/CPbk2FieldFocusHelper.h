/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Focus helper class
*
*/


#ifndef CPBK2FIELDFOCUSHELPER_H
#define CPBK2FIELDFOCUSHELPER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MPbk2ContactUiControl;
class MPbk2FieldPropertyArray;
class MVPbkStoreContactField;

// CLASS DECLARATIONS
class CPbk2FieldFocusHelper : public CBase
    {
    public: 
        /** 
         * Static constructor
         *
         * @param aUiControl Ui control
         * @param aPropertyArray Field property array
         * @return New instance of this class
         */
        IMPORT_C static CPbk2FieldFocusHelper* NewL( 
            MPbk2ContactUiControl& aUiControl,
            MPbk2FieldPropertyArray& aPropertyArray );
        ~CPbk2FieldFocusHelper();
        
    public:
        /**
         * Save initially focused field at this contact from ui control's
         * focused field index.
         * @see usage from CPbk2ImageCmdBase.
         * @param aStoreContact A contact that has been locked.
         */
        IMPORT_C void SaveInitiallyFocusedFieldL( MVPbkStoreContact& 
                                                  aStoreContact );
        
        /**
         * Restores the initially focused field by calling first SetFocusedContact()
         * and then SetFocusedFieldIndex(). The reference to store contact given with
         * SaveInitiallyFocusedFieldL() should be still valid.
         * @return Index of field focused or KErrNotFound.
         */
        IMPORT_C TInt RestoreSavedFieldL();
        
        /**
         * Set the iUiControl is valid or not
         * @param aUiControlValid, if ETrue then the iUiControl is valid otherwise invalid.
         */
        IMPORT_C void SetUiControlValid( TBool aUiControlValid );
        
    private: // implementation
        CPbk2FieldFocusHelper( 
            MPbk2ContactUiControl& aUiControl,
            MPbk2FieldPropertyArray& aPropertyArray ); 
               
    private: // Data
        /// Ref: ui control         
        MPbk2ContactUiControl& iUiControl;
        /// Ref: reference to the focused contact 
        MVPbkStoreContact* iStoreContact;   
        /// Ref: field property array
        MPbk2FieldPropertyArray& iFieldProperties;
        /// Own: initially focused field
        MVPbkStoreContactField* iFocusedStoreCntField; 
        /// Own: field index
        TInt iUiFieldIndex;   
        /// Own: flag for iUiControl
        TBool iUiControlValid;
    };
    
#endif // CPBK2FIELDFOCUSHELPER_H

// End of file
