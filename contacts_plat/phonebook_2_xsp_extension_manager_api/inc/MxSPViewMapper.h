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
*    Abstract interface for getting mapped view ids
*
*/


#ifndef __MXSPVIEWMAPPER_H__
#define __MXSPVIEWMAPPER_H__


// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 *  Abstract interface for getting mapped view IDs. View ID mapping means
 *  that xSP's own view IDs ( constants that are usually defined in a *.hrh
 *  file ) are substituted with IDs provided by Extension Manager. This is 
 *  needed because there may be extensions using same view IDs. After
 *  the substitution, the original IDs are not valid anymore. Still, in order
 *  to allow original view IDs usage inside the xSP's own code, MxSPViewMapper
 *  provides the mechanism for retrieving new IDs which correspond to the
 *  old original ones and vice versa.
 */
class MxSPViewMapper
    {
    public:
    
        /**
         *  Gets old original view ID which corresponds to the given new one
         *
         *  @param aId xSP implementation ID for identification
         *  @param aNewView New mapped view ID
         *  @param aOldView On return old/original view ID
         *  @return KErrNone if mapping exist, otherwise KErrNotFound
         */
        virtual TInt GetOldView( TUint32 aId, 
                                 TInt aNewView,
                                 TInt& aOldView ) const = 0;
                                    
        /**
         *  Gets new mapped view ID which corresponds to the given old one
         *
         *  @param aId xSP implementation ID for identification
         *  @param aOldView Old/original view ID
         *  @param aNewView On return new mapped view ID
         *  @return KErrNone if mapping exist, otherwise KErrNotFound
         */
        virtual TInt GetNewView( TUint32 aId, 
                                 TInt aOldView,
                                 TInt& aNewView ) const = 0;  

    protected: // Protected interface

        /**
         * Destructor.
         */
        virtual ~MxSPViewMapper()
                {};
                                       
    };

#endif // __MXSPVIEWMAPPER_H__

// End of File
