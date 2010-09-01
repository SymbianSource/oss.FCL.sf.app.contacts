/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates extension's instances.
*
*/



#ifndef C_CLOGSEXTENSIONFACTORY_H
#define C_CLOGSEXTENSIONFACTORY_H


//  INCLUDES
#include <e32base.h>

#include "MLogsExtensionFactory.h"
#include "MLogsViewExtension.h"
#include "MLogsUiControlExtension.h"
//#include "clogsviewextension.h"
//#include "clogsuicontrolextension.h"

class CLogsViewExtension;
class CLogsUiControlExtension;

/**
 * Extension factory. 
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsExtensionFactory) : public CBase,
                                           public MLogsExtensionFactory                                    
    {

public: 
        
    /**
     * Creates a new CLogsExtensionFactory.
     */
    static CLogsExtensionFactory* NewL();

    /**
     * Destructor.
     */
    ~CLogsExtensionFactory();


// From MLogsExtensionFactory
    
    /**
     * From MLogsExtensionFactory
     * Cleans up the object that is owned through a pointer to
     * this interface (for a C class this could simply be 
     * accomplished by calling e.g. 'delete this' ).
     *
     * @since S60 v3.2 
     */
    void Release(); 
    
    /**
     * From MLogsExtensionFactory
     * Creates a logs view extension.
     *
     * @since S60 v3.2
     * @param aId id of the logs view to create the extension
     *            for.
     * @param aEngine pbk engine to be provided to extensions.
     * @param aObserver observer
     * @return Logs view extension object.
     */
    MLogsViewExtension* CreateLogsViewExtensionL
        ( CPbkContactEngine& aEngine, MLogsExtObserver* aObserver );
        

    /**
     * From MLogsExtensionFactory 
     * Creates a Logs UI control extension.
     *
     * @since S60 v3.2
     * @return Logs control extension object.
     */        
    MLogsUiControlExtension* CreateLogsUiControlExtensionL();


private:

    /**
     * Symbian second-phase constructor
     */
    void ConstructL();

    /**
     * Constructor.
     */
    CLogsExtensionFactory();


private: // data

    /**
     * view extension
     * Own.
     */
    CLogsViewExtension* iViewExtension;
    
    /** 
     * app extension
     * Own.
     */
    CLogsUiControlExtension* iControlExtension;    

    };

#endif // C_CLOGSEXTENSIONFACTORY_H
