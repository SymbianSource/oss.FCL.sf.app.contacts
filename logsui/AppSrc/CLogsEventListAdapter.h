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
*     Adaptor to EventList copies data from model listbox
*
*/


#ifndef CLogsEventListAdapter_H
#define CLogsEventListAdapter_H

//  INCLUDES
#include <bamdesca.h>  // MDesCArray
#include <e32base.h>
#include "CLogsEngine.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEikLabel;
class MLogsModel;
class CPhoneNumberFormat;
class CEikColumnListBox;
class CLogsEventListControlContainer;

// CLASS DECLARATION

/**
*  Adaptor to EventList, copies data from model listbox
*/
class CLogsEventListAdapter : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aContainer
         * @return new object
         */
        static CLogsEventListAdapter* NewL( CLogsEventListControlContainer* aContainer );

        /**
         *  Destructor.
         */
        virtual ~CLogsEventListAdapter();

    private:        
        /**
         * C++ default constructor.
         * @param aContainer
         */
        CLogsEventListAdapter( CLogsEventListControlContainer* aContainer );

        /**
         *  Symbian OS constructor.
         */
        void ConstructL();

    private: // from MDesCArray

        /**
         *  Returns number of entries
         *
         *  @return TInt 
         */
        TInt MdcaCount() const;
    
        /**
         *  Returns a TPtrC related with the position given by aIndex
         *
         *  @param  TInt    aIndex
         *  @return TPtrC16
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;        
    
    private: //data

        /// Ref: Reference to engine
        CLogsEngine* iEngine;

        /// Ref: Reference to model
        MLogsModel* iModel;
        /// Own: Conversion buffer 
        HBufC* iBuffer;

        /// Own: localized texts for various purposes
        HBufC* iCallText;
        HBufC* iDataText;
        HBufC* iSMSText;
        HBufC* iMMSText;
        HBufC* iPacketText;
        HBufC* iWLANText;
        HBufC* iFaxText;
        HBufC* iVideoText;
        HBufC* iPoCText;
        HBufC* iPoCTextInfo;        
        HBufC* iVoIPText;
        HBufC* iPrivateNumber;
        HBufC* iUnknownNumber;
        HBufC* iPayphoneNumber;        
        HBufC* iSATNumber;
        HBufC* iGroupCall;  
        HBufC* iEmergencyCall;
        HBufC* iCsdNumber;
        
        /// Own: label for clipping
        CEikLabel* iLabel;

        /// Own: show csd flag
        //TBool iShowCsd;

        /// Own: object with groupping, clipping and A&H functionality
        CPhoneNumberFormat* iPhoneNumber;

        /// Ref: reference to the list box owned by the control container
        CEikColumnListBox*  iListBox;
    };

#endif  // CLogsEventListAdapter_H
           
// End of File
