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
 * Description:  Phonebook 2 merge contacts resolver interface.
 *
 */

#ifndef MPBK2MERGERESOLVER_H_
#define MPBK2MERGERESOLVER_H_


// FORWARD DECLARATIONS
class MPbk2MergeConflict;


// CLASS DECLARATION
/**
 * Phonebook 2 merge contacts resolver interface.
 */
class MPbk2MergeResolver
    {

    
public:
   
    /**
     * Counts conflicts.
     *
     * @return  Number of conflicts.
     */
    virtual TInt CountConflicts() = 0;
    
    /**
     * Gets conflict.
     *
     * @param aConflict    Index of conflict in array.
     * @return  Base class of conflict.
     */
    virtual MPbk2MergeConflict& GetConflictAt( TInt aConflict ) = 0;
        
    };

#endif /* MPBK2MERGERESOLVER_H_ */
