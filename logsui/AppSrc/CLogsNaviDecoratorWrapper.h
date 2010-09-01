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
* Description: 
*      A singleton wrapper for Logs views CAknNavigationDecorator objects (Tab groups). 
*      Each unique Tab group is only created if it doesn't already exist. This fixes 
*      error MVHA-6TJDMJ.
*
*
*/


#ifndef CLogsNaviDecoratorWrapper_H
#define CLogsNaviDecoratorWrapper_H


//  INCLUDES
#include <e32base.h>
#include <coemain.h>
#include "MLogsNaviDecoratorWrapper.h"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class MAknTabObserver;


// CLASS DECLARATION

/**
 * A singleton class to wrap Navi Pane tab group handling for 
 * Logs views.
 */
class CLogsNaviDecoratorWrapper : public CCoeStatic, 
                                  public MLogsNaviDecoratorWrapper
    {
    public: 
    /**
     * Returns the singleton CLogsNaviDecoratorWrapper instance. If no instance
	 * exists yet it is created.
     */
    static CLogsNaviDecoratorWrapper* InstanceL();
    
    /**
     * Returns the wrapped iNaviDecorator instance. If no instance
	 * exists yet a new one is created. If there is an instance but it is
	 * of wrong resource type, a new instance is created and the old deleted. 
	 * 
	 * Called from CLogsBaseControlContainer::NavigationTabGroupL
	 *
	 * @param aNaviPane, the calling views status pane's navigation pane control
	 * @param aTabObserver, an interface to the LogsAppUi::TabChangedL
	 * @param aResource, the resource id of the 
	 *
	 * @return CAknNavigationDecorator object for the view's tab group
     */
    CAknNavigationDecorator* GetNaviDecoratorL(CAknNavigationControlContainer* aNaviPane,
                                              MAknTabObserver* aTabObserver,
                                              TInt aResource);
                                              
    private:  
    /**
     * NewLC is called from InstanceL.
     */
    static CLogsNaviDecoratorWrapper* NewLC();
    
    /**
     * Constructor
     */
    CLogsNaviDecoratorWrapper(); 
    
    /**
     * Destructor is called from CCoeEnv::DestroyEnvironmentStatic
     */
    ~CLogsNaviDecoratorWrapper();
    

    private:  // Data
    
    // Wrapped CAknNavigationDecorator object.
    CAknNavigationDecorator* iNaviDecorator;  
    
    // State holder for the current resource id of the iNaviDecorator
    TInt iCurrentNaviResource;
    
    
    };

#endif //CLogsNaviDecoratorWrapper_H
            

// End of File
