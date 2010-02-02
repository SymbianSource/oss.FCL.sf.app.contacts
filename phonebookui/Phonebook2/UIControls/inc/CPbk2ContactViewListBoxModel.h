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
* Description:  Phonebook 2 contact view list box model.
*
*/


#ifndef CPBK2CONTACTVIEWLISTBOXMODEL_H
#define CPBK2CONTACTVIEWLISTBOXMODEL_H

//  INCLUDES
#include "MPbk2ContactViewListBoxModel.h"
#include <e32base.h>
#include <TPbk2IconId.h>
#include <AknUtils.h>
#include <CPcsDefs.h>

//  FORWARD DECLARATIONS
class MVPbkContactViewBase;
class MVPbkViewContact;
class CPbk2IconArray;
class MPbk2ContactNameFormatter;
class CVPbkContactManager;
class CPbk2StorePropertyArray;
class MVPbkBaseContactFieldCollection;
class MPbk2ClipListBoxText;
class MPbk2ContactUiControlExtension;
class CPbk2ContactIconsUtils;
class CPsQuery;
class CPSRequestHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 contact view list box model.
 * Responsible for:
 * - formatting lines to a list box used in a contact view
 */
NONSHARABLE_CLASS(CPbk2ContactViewListBoxModel) :
        public CBase,
        public MPbk2ContactViewListBoxModel
    {
    public: // Types

        /**
         * Model initialization parameters class.
         */
        class TParams
            {
            public:  // Constructor
                /**
                 * C++ Constructor.
                 */
                TParams(){}
            
            public:  // Input parameters

                /// MANDATORY: Contact manager
                const CVPbkContactManager* iContactManager;
                // MANDATORY: Contact view
                MVPbkContactViewBase* iView;
                /// MANDATORY: Store properties
                CPbk2StorePropertyArray* iStoreProperties;
                /// MANDATORY: Name formatter
                MPbk2ContactNameFormatter* iNameFormatter;
                /// MANDATORY: Icon array to use
                CPbk2IconArray* iIconArray;
                /// OPTIONAL: Id of the empty icon in iIconArray
                TPbk2IconId iEmptyId;
                /// OPTIONAL: Id of the default icon to use for all list
                TPbk2IconId iDefaultId;
                /// OPTIONAL: For clipping list box rows from beginning
                MPbk2ClipListBoxText* iClipListBoxText;
                /// OPTIONAL: Extension point to UI control
                MPbk2ContactUiControlExtension* iUiExtension;
                /// MANDATORY: Use Multiselection-specific icons layout
                ///            Place icon after the text
                TBool iMultiselection;
            };

    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   Parameters for the model.
         * @return  A new instance of this class.
         */
         static CPbk2ContactViewListBoxModel* NewL(
                CPbk2ContactViewListBoxModel::TParams& aParams );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewListBoxModel();


    public: // From MPbk2ContactViewListBoxModel
        TInt MdcaCount() const;
        TPtrC MdcaPoint(
                TInt aIndex ) const;
        void SetUnnamedText(
                const TDesC* aText );
        HBufC* GetContactTitleLC( TInt aIndex ) const;

    protected: // Implementation
        CPbk2ContactViewListBoxModel(
                CPbk2ContactViewListBoxModel::TParams& aParams );
        void ConstructL(CPbk2StorePropertyArray* aStoreProperties,
                MPbk2ContactUiControlExtension* aUiExtension);
        void FormatBufferL(
                const TInt aIndex ) const;
        void FormatEmptyBuffer() const;
        virtual void FormatBufferForContactL(
                const MVPbkViewContact& aViewContact,
                TInt aIndex ) const;
        void AppendName(
                const TDesC& aName ) const;
        void AppendIconIndex(
                const TPbk2IconId& aIconId) const;
        void AppendTrailingIcons(
                const TArray<TPbk2IconId>& aIds,
                TInt aStartIndex ) const;
        void AppendTrailingIcon(
                const TPbk2IconId& aIconId) const;
        
        void HandleError(
                TInt aError ) const;
        TBool CutFromBeginningFieldL(
                const MVPbkBaseContactFieldCollection& aContactFields ) const;

    public:
        /**
         * Return last Predictive Search query.
         * This object is used in underlying and bookmark restoring.
         * All Predictive Search result is valid only for one query.
         * @return pointer to the last PCS query object.
         */
        const CPsQuery* LastPCSQuery() const;
        
        /**
        * Get mathes part from data model object. 
        * Data model uses mathes patterns returned from Predictive Search server.
        * This method is added for bidirectional languages.

        * @param aSearchData  The input data to be searched.
        * @param aMatchLocation The list matched index and length of match
        */
        void GetMatchingPartsL( const TDesC& aSearchData,
                              RArray<TPsMatchLocation>& aMatchLocation);
        
        /**
        * Returns the instance of the request handler object for the Predicitve Search application
        * 
        * @return pointer to the CPSRequestHandler. 
        */
        CPSRequestHandler* PSHandler() const;
        
    private: // Definitions

        enum
            {
            /// Maximum length of a formatted list box item text
            EMaxListBoxText = 256
            };

    protected: // Data
        /// Ref: Virtual Phonebook contact manager
        const CVPbkContactManager& iContactManager;
        /// Ref: Contact view this list box model is attached to
        const MVPbkContactViewBase* iView;
        /// Ref: Contact name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Buffer where to format list box texts
        TDes& iBuffer;
        /// Own: Buffer for formatted list box item text
        TBuf<EMaxListBoxText> iFormattingBuffer;
        /// Ref: Text to use for unnamed items
        const TDesC* iUnnamedText;
        /// Ref: Icon index mapping array
        const CPbk2IconArray* iIconArray;
        /// Own: Empty icon id
        TPbk2IconId iEmptyIconId;
        /// Own: Default icon id to use for all rows if EPbk2NullIconId
        TPbk2IconId iDefaultIconId;
        /// Own: Previous view cont
        mutable TInt iPreviousCount;
        /// Ref: Clip listbox row text
        MPbk2ClipListBoxText* iClipListBoxText;
        // Own: get icons for a contact
        CPbk2ContactIconsUtils* iContactIcons;
        /// indicates whether to place the first icon at the A-column
        const TBool iIconsStartAtZero;
        /// indicates whether to show one icon or all
        const TBool iIconsOnlyOne;
        //Flag to indicate Feature manager initilization
        TBool iFeatureManagerInitilized ;
    };

#endif // CPBK2CONTACTVIEWLISTBOXMODEL_H

// End of File
