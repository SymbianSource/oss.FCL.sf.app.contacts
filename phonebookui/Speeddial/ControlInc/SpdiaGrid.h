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
* Description:     This class provides drawing a shadow.
*
*/






#ifndef     SPDIAGRID_H
#define     SPDIAGRID_H

//  INCLUDES
#include <AknGrid.h>
#include <aknlists.h>

//  FORWARD DECLARATIONS

// CLASS DECLARATION
/*
*  This class provides drawing a shadow.
*
*  @since 
*/
class CSpdiaControl;

class CSpdiaGrid  : public CAknQdialStyleGrid
    {
    public:
        // 640 x 480 40 x 30 VGA(Video Graphics Array)    
        // 480 x 640 30 x 40 VGA turned
        // 352 x 288 44 x 36 CIF(Common Intermediate Format)
        // 288 x 352 36 x 44 CIF turned
        // enum order defined in CAknQdialStyleGrid 
        enum TGridType
            {
            EtnCIF90,
            EtnCIF,
            EtnVGA90,
            EtnVGA,
            EtnCOM,
            EText
            };

        /**
        * constructor.
        */
        static CSpdiaGrid* NewL(const CSpdiaControl& aControl);

        /**
        * constructor.
        */
        CSpdiaGrid(const CSpdiaControl& aControl);

        /**
        * C++ default constructor.
        */
        CSpdiaGrid();

        /**
        * Destructor.
        */
        virtual ~CSpdiaGrid();

    private: // Functions from base classes
        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * Passes skin information when needed.
        **/
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    private:
        void Draw(const TRect& aRect) const;
		
	private:
		void SizeChangedL();	
		
    private:
        CSpdiaControl* iControl;
    };

#endif      // SPDIAGRID_H

