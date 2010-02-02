/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Range that is allocated for a plug-in.
*
*/




#ifndef TCCAPluginsOrderInfo_H

#define TCCAPluginsOrderInfo_H



// INCLUDES

#include <e32std.h>



// FORWARD DECLARATIONS

class CImplementationInformation;



/**

 * stores plugins order

 */

struct TCCAPluginsOrderInfo

    { 

    

    public: 

        

    // CCA plugins can be seperated two 3 groups.

    //

    // group ECCAInHousePlugins: in-house CCAsXPExtension plugins impelemented by ourself

    // group ECCAPluginsInBothCCAAndNameList: CCA service plugins, whose corresponding

    //                                        phonebook main view sXP plugins exists.

    // group ECCAPluginsOnlyInCCA:  CCA service plugins, which exists only in CCA.

    enum TCCAPluginGroup

        {

        ECCAInHousePlugins = 0,

        ECCAPlugindInBothCCAAndNameList = 1,

        ECCAPluginsOnlyInCCA = 2

        };

    

    public: // Construction

       

        inline TCCAPluginsOrderInfo() : iPluginInfor(0),iOrder(0)

            {

            }

      

        static inline TInt SortByOrder( const TCCAPluginsOrderInfo& aFirst, 

                const TCCAPluginsOrderInfo& aSecond )

            {

            if( aFirst.iOrder < aSecond.iOrder )

                {

                return -1;

                }

            else if( aFirst.iOrder == aSecond.iOrder )

                {

                return 0;

                }

            else

                {

                return 1;

                }

            }



    public: // Data



        CImplementationInformation* iPluginInfor;



        // the order is opaque value after object just created,

        // array of TCCAPluginsOrderInfo will be sorted by this iOrder;

        TInt iOrder; 

        

        // store plugins group infor, CCA sXPExtension plugins are seperated into 3 groups.

        // ECCAInHousePlugins, ECCAPlugindInBothCCAAndNameList and ECCAPluginsOnlyInCCA

        TCCAPluginGroup iGroupInfo;

        //ETrue for legacy interface implementations        
        TBool iOldInterFaceType;

       

    };

    

#endif // TCCAPluginsOrderInfo_H

//End of file

