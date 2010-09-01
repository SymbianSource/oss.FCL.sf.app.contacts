/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote Contact Lookup
*
*/


#ifndef RCLTEST_SUITE_INPUT_DATA_H
#define RCLTEST_SUITE_INPUT_DATA_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES


//Forward Declaration


/**
 *  This class performs parsing of the data input from the configuration file
 *
 *  @since S60 v5.2
 */
class CRclTestSuiteInputData :public CBase
{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRclTestSuiteInputData* NewL(CStifItemParser& aItem);

        /**
        * Destructor.
        */
        virtual ~CRclTestSuiteInputData();	
		

private: // functions
	
		/**
		* Default Constructor
		*/
		CRclTestSuiteInputData();

		/**
		* Second phase constructor
		*/
		void ConstructL(CStifItemParser& aItem);		
		


private:  // data
		
			

};

#endif // RCLTEST_SUITE_INPUT_DATA_H

// End of file
