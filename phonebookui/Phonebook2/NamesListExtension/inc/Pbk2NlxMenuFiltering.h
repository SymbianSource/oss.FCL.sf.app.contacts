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
* Description:  Phonebook 2 menu filtering helper.
*
*/


#ifndef PBK2NLXMENUFILTERING_H
#define PBK2NLXMENUFILTERING_H

#include <e32base.h>

// FORWARD
class MPbk2ContactUiControl;

// CLASS DECLARATIONS
/**
 * Helper functions to define current context for Options
 * and stylus popup menus. Takes into account markable contacts as well.
 * The class uses Pbk2AppUi() to access current store configuration.
 * 
 */
class Pbk2NlxMenuFiltering
    {
public:
    /**
     * @param aControl current ui control
     * @return ETrue if open (in CCA) command can be executed in the current context
     */
    static TBool OpenReady( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control 
     * @return ETrue if add to favorites command item is selected
     */
    static TBool AddToFavoritesCmdSelected( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control 
     * @return ETrue if open MyCard or Create MyCard command item is selected
     */
    static TBool MyCardCmdSelected( MPbk2ContactUiControl& aControl );

    
    /** 
     * @param aControl current ui control 
     * @return ETrue if Ssearch from remote command item is selected
     */
    static TBool RclCmdSelectOptSelected( MPbk2ContactUiControl& aControl );    
    
    /**
     * @param aControl current ui control 
     * @return ETrue if read only contact is selected
     */
    static TBool ReadOnlyContactSelectedL( MPbk2ContactUiControl& aControl );

    /**
     * @param aControl current ui control 
     * @return ETrue if current contact and context allows adding to top contacts
     */
    static TBool TopReadyL( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control 
     * @return ETrue if top contact is selected
     */
    static TBool TopContactSelectedL( MPbk2ContactUiControl& aControl );

    /**
     * @param aControl current ui control 
     * @return ETrue if non top contact is selected
     */
    static TBool NonTopContactSelectedL( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control 
     * @return ETrue if command item is selected and no item makred
     */
    static TBool TopRearrangingReadyL( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control
     * @return  ETrue if at least one top contact marked
     */
    static TBool TopContactMarkedL( MPbk2ContactUiControl& aControl );

    /**
     * @param aControl current ui control 
     * @return  ETrue if at least one non top contact marked 
     */
    static TBool NonTopContactMarkedL( MPbk2ContactUiControl& aControl );
    
    /**
     * @param aControl current ui control 
     * @return ETrue if marked contacts includes at least one non-readonly contact 
     */
    static TBool CheckDeletableInMarkedContactsL( MPbk2ContactUiControl& aControl );
    };

#endif // PBK2NLXMENUFILTERING_H

// End of File
