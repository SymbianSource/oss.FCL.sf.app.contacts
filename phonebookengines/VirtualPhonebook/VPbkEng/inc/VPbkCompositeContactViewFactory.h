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
* Description:  Composite contact view factory.
*
*/


#ifndef VPBKCOMPOSITECONTACTVIEWFACTORY_H
#define VPBKCOMPOSITECONTACTVIEWFACTORY_H

#include "VPbkContactView.hrh"
#include <e32def.h>

class MVPbkContactViewObserver;
class CVPbkCompositeContactView;
class MVPbkFieldTypeList;
class RFs;

/**
 * Virtual Phonebook composite contact view factory.
 */
NONSHARABLE_CLASS( VPbkCompositeContactViewFactory )
	{
	public: // Interface

        /**
         * Creates a new composite contact view based on the sort policy.
         *
         * @param aObserver     Observer for the view.
         * @param aSortPolicy   Sort policy for determining the view type.
         * @param aSortOrder    Sort order to pass to created view.
         * @param aFieldTypes   Field type list.
         * @param aFs           File server handle
         * @return  New composite contact view instance.
         */
		static CVPbkCompositeContactView* CreateViewLC(
                MVPbkContactViewObserver& aObserver,
                TVPbkContactViewSortPolicy aSortPolicy, 
                const MVPbkFieldTypeList& aSortOrder,
                const MVPbkFieldTypeList& aFieldTypes,
                RFs& aFs);
        
    private: // Hidden constructor and destructor
        VPbkCompositeContactViewFactory();
        ~VPbkCompositeContactViewFactory();
	};

#endif // VPBKCOMPOSITECONTACTVIEWFACTORY_H

//End of File
