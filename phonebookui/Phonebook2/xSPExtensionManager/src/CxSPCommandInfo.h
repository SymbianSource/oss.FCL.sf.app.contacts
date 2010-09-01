/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       xSP command info
*
*/


#ifndef __CXSPCOMMANDINFO_H__
#define __CXSPCOMMANDINFO_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;

/**
 * xSP command info
 */
class CxSPCommandInfo : public CBase
    {

    public:
        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @param aReader Reference to resource reader to be used
         * @param aId Id of the component owning the command IDs
         * @param anewCommandIds New command IDs
         * @return New instance of this class
         */
        static CxSPCommandInfo* NewLC( TResourceReader& aReader,
        							TUint32 aId,
        							TInt& aNewCommandIds );

        /**
         * Destructor.
         */
        ~CxSPCommandInfo();

    public:

    	/**
    	 * Gets the ID related to this command ID
    	 *
    	 * @return The ID related to this command ID
    	 */
    	TUint32 Id() const;

    	/**
    	 * Gets the new command ID
    	 *
    	 * @return The new command ID
    	 */
    	TInt32 NewCommandId() const;

    	/**
    	 * Gets the old/original command ID
    	 *
    	 * @return The old/original command ID
    	 */
    	TInt32 OldCommandId() const;

    private:

    	/**
         * Constructor.
         */
        CxSPCommandInfo( TUint32 aId );

        /**
         * 2nd phase constructor.
         */
        void ConstructL( TResourceReader& aReader,
        					TInt& aNewCommandIds );

    private: // Data

        TUint32 iId;
		TInt32 iNewCommandId;
		TInt32 iOldCommandId;
    };


#endif // __CXSPCOMMANDINFO_H__

// End of File
