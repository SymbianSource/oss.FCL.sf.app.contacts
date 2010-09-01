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
* Description:  Execute Maps application.
*
*/

#ifndef CPMAPCMD_H
#define CPMAPCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <lbsfieldids.h>
#include <mngeocoder.h>
#include <VPbkFieldType.hrh>
#include <Pbk2UIFieldProperty.hrh>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MPbk2CommandObserver;
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class CPosLandmark;
class MPbk2ContactEditorControl;
class MPbk2ContactEditorControlExtension;
class MVPbkStoreContactField;
class TLocality;
class CMnMapView;

enum TCurrentMapLaunchedByState
    {
    EMapNone = 1,
    EMapAssignFromMaps,
    EMapShowOnMaps
    };
// CLASS DECLARATION

/**
 * Phonebook 2 show on map command object.
 */
class CPmapCmd : public CActive,
				 public MPbk2Command,
				 public MVPbkContactObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         * @param iEditorControl    Editor control.
         * @param aContact          Contact store.
         * @param aCommandId        Command.
         * @return  A new instance of this class.
         */
        static CPmapCmd* NewL(
        		MPbk2ContactEditorControl& iEditorControl,
        		MVPbkStoreContact& aContact, 
        		TInt aCommandId );
        
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl        UI control.
         * @param aCommandId        Command.
         * @return  A new instance of this class.
         */
        static CPmapCmd* NewL(
        		MPbk2ContactUiControl& aUiControl, 
        		TInt aCommandId );
        
        /**
         * Destructor.
         */
        ~CPmapCmd();

        // CActive-related methods
       /**
       * RunL method to handle the user selection
       */
       void RunL();
       
       /**
       * Cancel method to handle the user selection
       */
       void DoCancel();
       
       /**
       * Function to handle any errors in async request
       * @param aError   Error Code
       */
       TInt RunError( TInt aError );
      
       /**
       * Disconnects from provider, when operation is completed
       */
       void Reset();
    public: //From MVPbkContactObserver
    	void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed(
        		TContactOp aOpCode, 
    	        TInt aErrorCode, 
    	        TBool aErrorNotified);
        
    public: // From MPbk2Command
        void ExecuteLD();
        void ExecuteL();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    public: // Implementation
    	/**
         * Check if exist Maps Api provider. 
         *
         * @return  True if provider exist.
         */
    	static TBool CheckViewProviderL();
		
    private: // Members
        CPmapCmd(
        		MPbk2ContactUiControl& aUiControl, 
        		TInt aCommandId );
        CPmapCmd(
        		MPbk2ContactEditorControl& iEditorControl,
        		MVPbkStoreContact& aContact,
                TInt aCommandId );
        
        void ConstructL();
        void FinishProcess();   
        
        /**
         * Try to get address from contact editor 
         *
         * @return  Address group ID or EPbk2FieldGroupIdNone.
         */
        TPbk2FieldGroupId GetAddressFromEditorView();
        
        /**
         * Try to get address from UI control 
         *
         * @return  Address group ID or EPbk2FieldGroupIdNone.
         */
        TPbk2FieldGroupId GetAddressFromUiControl();
        
        /**
         * Try to get address if there is only one address in contact. 
         *
         * @return  Address group ID or EPbk2FieldGroupIdNone.
         */
        TPbk2FieldGroupId GetAddressIfIsAlone();
        
        /**
         * Run show on Maps API.
         *
         * @param  aAddressType Address to show.
         */
        void EditorShowOnMapsL( TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Run assign on Maps API.
         *
         * @param  aAddressType Address to assign.
         */
        void EditorAssignFromMapsL( TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Updates Contact address fields.
         *
         * @param  aLandmark 		fields returned from Maps API.
         * @param  aPositionField 	Type of CPosLandmark field.
         * @param  aVersitSubField 	Type of Contact field.
         * @param  aAddressType 	Address type.
         */
        void UpdateFieldL(
        		const CPosLandmark& aLandmark,
        		_TPositionFieldId aPositionField,
        		TVPbkSubFieldType aVersitSubField,
        		TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Updates Contact geocoordinates.
         *
         * @param  aLandmark        fields returned from Maps API.
         * @param  aAddressType 	Address type.
         */
        void UpdateCoordsL(
                const CPosLandmark& aLandmark,
				TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Fills CPosLandmark object with Contact address fields.
         *
         * @param  aLandmark 		object have geocoordinates.
         * @param  aAddressType 	Address type.
         */
        void FillLandmarkL(
        		CPosLandmark& aLandmark, 
        		TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Sets CPosLandmark object with land mark name.
         *
         * @param  aLandmark        object have geocoordinates.
         */
        void SetLandmarkNameL(CPosLandmark& aLandmark);
        
        /**
         * Fills CPosLandmark object with Contact geocoordinates.
         *
         * @param  aLandmark 		object have geocoordinates.
         * @param  aAddressType 	Address type.
         */
        TBool FillGeoLandmarkL(
                CPosLandmark& aLandmark, 
                TVPbkFieldTypeParameter aAddressType );
        
        /**
         * Fills CPosLandmark object with geocoordinates.
         *
         * @param  aLandmark 		object have geocoordinates.
         * @param  aDataText 		text with geocoordinates.
         */
        TBool DoFillGeoLandmarkL(
                CPosLandmark& aLandmark, 
                const TDesC& aDataText );
        
        /**
         * Get Control extension. 
         *
         * @return  Contact Editor Extension or NULL.
         */
        MPbk2ContactEditorControlExtension* ControlExtension();
        
        /**
         * Shows querry for selecting address. 
         *
         * @return  Address group ID or EPbk2FieldGroupIdNone.
         */
        TPbk2FieldGroupId SelectAddressL();
        
        /**
         * Check if exist address in Contact. 
         *
         * @return  True if any address exist in Contact.
         */
        TBool IsAddressInContact();
        
        /**
         * Gets group id from Store Field.
         *
         * @param  aField 			Store field.
         * @return   		 		Address group ID or EPbk2FieldGroupIdNone.
         */
        TPbk2FieldGroupId GetFieldGroupL( MVPbkStoreContactField& aField );
        
        /**
         * Handle the selection of map which was launched from Show On Map option. 
         */
        void HandleSelectiOnShowOnMapsL();
        
        /**
         * Handle the selection of map which was launched from Assign From Maps option. 
         */
        void HandleSelectiOnAssignFromMapsL();
        
    private: // Data
    	/// Ref: Contact editor control
        MPbk2ContactEditorControl* iEditorControl;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iUiControl;
    	/// Ref: The edited contact
    	MVPbkStoreContact* iContact;
        /// Ref: Command observer
        MPbk2CommandObserver* iObserver;
        /// Own: Map View Provider
        CMnProvider* iMapViewProvider;
        /// Own: Map View
        CMnMapView* iMapView;
        /// Own: Is in editor address view?
        TBool iAddressView;
        /// Own: command id
        TInt iCommandId;
        /// Own: Is in editor address view?
		TBool iAddressUpdatePrompt;
		/// Own: Current map launched is by which option
		TCurrentMapLaunchedByState iCurrentMapLaunchedByState;
		/// Own: Current AddressType
		TVPbkFieldTypeParameter iAddressType;
		/// Own: Is already address is there?
		TBool iNoAddress;
    };

#endif // CPMAPCMD_H

// End of File
