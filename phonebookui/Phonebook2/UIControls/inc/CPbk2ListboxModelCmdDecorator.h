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
* Description:  Listbox model decorator that adds commands to the list.
*
*/


#ifndef CPBK2LISTBOXMODELCMDDECORATOR_H_
#define CPBK2LISTBOXMODELCMDDECORATOR_H_

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>

#include <TPbk2IconId.h>
#include <MPbk2UiControlCmdItem.h>

// FORWARD DECLARATIONS
class CPbk2IconArray;

// CLASS DECLARATION

/**
 * A listbox model decorator that adds command placeholder
 * items to the list.
 */
NONSHARABLE_CLASS(CPbk2ListboxModelCmdDecorator) :
    public CBase,
    public MDesCArray
    {
    public: // Construction and destruction

        /**
         * Constructs the object.
         */
        static CPbk2ListboxModelCmdDecorator* NewL(
            const CPbk2IconArray& aIconArray,
            TPbk2IconId aEmptyIconId,
            TPbk2IconId aDefaultIconId );
        
        ~CPbk2ListboxModelCmdDecorator();

    public: // from MPbk2ContactViewListBoxModel
        TInt MdcaCount() const;
        TPtrC MdcaPoint( TInt aIndex ) const;

    public: // New methods

    	/**
    	 * Sets the model that is to be decorated.
    	 * @param aModel The model that is to be decorated.
    	 */
        void SetDecoratedModel( const MDesCArray& aModel );

        /**
         * Sets the list of commands that are to be used when decorating.
         * The commands will appear at the top of the list. Note that only the
         * enabled commands will be included.
         * @param aCommands The commands.
         */
        void SetListCommands(
        	const RPointerArray<MPbk2UiControlCmdItem>* aCommands );

        /**
         * The number of enabled commands.
         * @return The number of enabled commands.
         */
    	TInt NumberOfEnabledCommands() const;

    	/**
    	 * Access to an enabled command.
    	 * @param aIndex An index amongst the enabled commands.
    	 * @see NumberOfEnabledCommands()
    	 */
    	MPbk2UiControlCmdItem& EnabledCommand( TInt aIndex ) const;

    protected: // construction
        CPbk2ListboxModelCmdDecorator(
	        const CPbk2IconArray& aIconArray,
	        TPbk2IconId aEmptyIconId,
	        TPbk2IconId aDefaultIconId );
        void ConstructL();

    private: // Definitions
        enum
            {
            /// Maximum length of a formatted list box item text
            EMaxListBoxText = 256
            };

    protected: // data
	    const CPbk2IconArray& iIconArray;
	    TPbk2IconId iEmptyIconId;
	    TPbk2IconId iDefaultIconId;
    	/// Ref: Decorated model
    	const MDesCArray* iDecoratedModel;
        // Ref: The commands
        const RPointerArray<MPbk2UiControlCmdItem>* iCommands;
        /// Ref: Buffer where to format list box texts
        TDes& iBuffer;
        /// Own: Buffer for formatted list box item text
        TBuf<EMaxListBoxText> iFormattingBuffer;

    };

#endif //CPBK2LISTBOXMODELCMDDECORATOR_H_

// end of file
