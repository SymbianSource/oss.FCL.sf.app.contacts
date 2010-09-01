/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface class for managing CCA plugin views.
*
*/
#ifndef MCCAPARENTCLEANER_H
#define MCCAPARENTCLEANER_H

/**
 * Interface class for managing CCA plugin views.
 */
class MCcaParentCleaner
    {
    public:

       /**
         * Decrements refrerence count for children. When refrence count
	     * reaches zero, the instance of MCcaParentCleaner can be deleted.
         *
         * @since S60 v5.0
         * @return Remaining number of childs
         */    
        virtual TInt DeletedChild() = 0;

       /**
         * Destructor
         *
         * @since S60 v5.0
         */            
        virtual ~MCcaParentCleaner() {}
    };

#endif //MCCAPARENTCLEANER_H
