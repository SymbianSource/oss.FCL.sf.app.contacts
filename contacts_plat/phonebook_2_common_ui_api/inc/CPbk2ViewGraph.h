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
* Description:  Phonebook 2 view graph related classes.
*
*/


#ifndef CPBK2VIEWGRAPH_H
#define CPBK2VIEWGRAPH_H

// INCLUDES
#include <e32base.h>
#include <pbk2viewinfo.hrh>

// FORWARD DECLARATIONS
class CPbk2ViewNode;
class TResourceReader;

// CLASS DECLARATION

/**
 * Phonebook 2 view transition class.
 */
class TPbk2ViewTransition
    {
    public: // Data
        /// Own: ID of an event triggering this transition
        TPbk2ViewTransitionEvent iEvent;
        /// Own: Target node view id
        TUid iTargetViewId;
        /// Ref: Node to activate if this transition is accepted
        CPbk2ViewNode* iNode;
    };

/**
 * Phonebook 2 view navigation node class.
 */
class CPbk2ViewNode : public CBase
    {
    public: // Constructor and destructor

        /**
         * Constructor. 
         *
         * @param aViewId               View id of this node.
         * @param aDefaultPrevViewId    This views default
         *                               previous view id.
         * @param aExitNode             ETrue if an exit node.
         * @param aTabGroupId           Tab group id.
         * @param aTabGroupOrdering     Tab group ordering item.
         * @param aTabResourceId        Tab resource id. Can be defined as 0,
         *                              which means that the tab should be 
         *                              created by hand.
         * @param aTransitionReader     A resource reader pointed to an 
         *                              array of PBK2_VIEW_TRANSITION's.
         * @return  New instance of this class.
         */
        IMPORT_C static CPbk2ViewNode* NewL(
                TUid aViewId, 
                TUid aDefaultPrevViewId, 
                TBool aExitNode,
                TPbk2TabGroupId aTabGroupId, 
                TPbk2TabGroupOrdering aTabGroupOrdering,
                TInt aTabResourceId,
                TResourceReader& aTransitionReader );

        /**
         * Destructor.
         */
        ~CPbk2ViewNode();

    public: // Interface

        /**
         * Returns the view id of this node.
         *
         * @return  View id of this node.
         */
        inline TUid ViewId() const;

        /**
         * Returns the default previous node.
         *
         * @return  Default previous node.
         */
        inline CPbk2ViewNode* DefaultPreviousNode() const;

        /**
         * Returns the currently set previous node.
         *
         * @return  Currently set previous node.
         */
        inline CPbk2ViewNode* PreviousNode() const;

        /**
         * Returns ETrue if this is an exit node.
.        *
         * @return  ETrue if an exit node, EFalse otherwise.
         */
        inline TBool ExitNode() const;

        /**
         * Returns tab group id.
         *
         * @return  Tab group id.
         */
        inline TPbk2TabGroupId TabGroupId() const;

        /**
         * Returns the ordering item of this node in the tab group.
         *
         * @return  Tab group ordering item.
         */
        inline TPbk2TabGroupOrdering TabGroupOrdering() const;

        /**
         * Returns a tab resource id pointing to a PBK2_VIEW_NODE_TAB.
         *
         * @return  Tab resource id.
         */
        inline TInt TabResourceId() const;

        /**
         * Sets previous node to aNode.
         *
         * @param aNode     The node to set.
         */
        inline void SetPreviousNode(
                CPbk2ViewNode* aNode );

        /**
         * Returns the number of transitions from this node.
         *
         * @return  Number of transitions.
         */
        inline TInt TransitionCount() const;

        /**
         * Returns aIndex:th transition from this node.
         *
         * @param aIndex    The transition index to query.
         * @return  View transition.
         */
        inline const TPbk2ViewTransition& TransitionAt(
                TInt aIndex ) const;

        /**
         * Returns ETrue if this node contains a transition equal to
         * aTransition.
         *
         * @param aTransition   The transition to look for.
         * @return  ETrue if transition found, EFalse otherwise.
         */
        IMPORT_C TBool FindTransition(
                const TPbk2ViewTransition& aTransition ) const;

        /**
         * Sets the ordering item of this node in the tab group.
         *
         * @param aTabGroupOrdering  Tab group ordering item.
         */
        inline void SetTabGroupOrdering(
                TPbk2TabGroupOrdering aTabGroupOrdering );

        /**
         * Sets this nodes tab resource id.
         *
         * @param aTabResourceId  Tab resource id.
         *                         @see PBK2_VIEW_NODE_TAB
         */
        inline void SetTabResourceId(
                TInt aTabResourceId );

    private: // Implementation
        CPbk2ViewNode(); // private contructor for internal use
        CPbk2ViewNode(
                TUid aViewId,
                TUid aDefaultPrevViewId, 
                TBool aExitNode,
                TPbk2TabGroupId aTabGroupId,
                TPbk2TabGroupOrdering aTabGroupOrdering,
                TInt aTabResourceId );
        TPbk2ViewTransition* FindTransition(
                TPbk2ViewTransitionEvent aEvent );
        void ReadTransitionsL(
                TResourceReader& aResReader);

    private: // Data
        /// Own: View id
        TUid iViewId;
        /// Own: Default previous node id
        TUid iDefaultPrevViewId;
        /// Ref: Default previous node
        CPbk2ViewNode* iDefaultPreviousNode;
        /// Ref: Previous node
        CPbk2ViewNode* iPreviousNode;
        /// Own: Indicates whether this is an exit node
        TBool iExitNode;
        /// Own: Transitions array
        CArrayFixFlat<TPbk2ViewTransition> iTransitions;
        /// Own: Tab group id
        TPbk2TabGroupId iTabGroupId;
        /// Own: Ordering item in tab group
        TPbk2TabGroupOrdering iTabGroupOrdering;
        /// Own: Tab resource id
        TInt iTabResourceId;

    private: // Friends
        friend class CPbk2ViewGraph;
    };


/**
 * Phonebook 2 view navigation graph class.
 */
class CPbk2ViewGraph : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new view graph from a PBK2_VIEW_GRAPH resource.
         *
         * @param aResReader    Resource reader pointed
         *                      to a PBK2_VIEW_GRAPH resource.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ViewGraph* NewL(
                TResourceReader& aResReader );

        /**
         * Destructor.
         */
        ~CPbk2ViewGraph();

    public: // Interface

        /**
         * Returns node with given resource id, NULL if not found.
         *
         * @param aViewId   Id of the node to found.
         * @return  View node.
         */
        IMPORT_C CPbk2ViewNode* FindNodeWithViewId(
                TUid aViewId ) const;

        /**
         * Modifies existing view graph. Updates transitions and
         * appends new view node according to given resource reader.
         *
         * @param aResReader    Resource reader pointed
         *                      to a PBK2_VIEW_GRAPH resource.
         */
        IMPORT_C void ModifyViewGraphL(
                TResourceReader& aResReader );

        /**
         * Appends and links aNode to the view graph structure.
         * Takes ownership of the node.
         *
         * @param aNode Node to add to the view graph.
         */
        IMPORT_C void AddViewNodeL(
                CPbk2ViewNode* aNode );

        /**
         * Finds and returns the ids of the views that are in
         * the specified tab group. Returned array is sorted in
         * the proper tab group ordering of the views.
         *
         * @param aTabGroupId   Id of the tab group to investigate.
         * @return  Array of views in the tab group.
         */
        IMPORT_C RPointerArray<CPbk2ViewNode> FindViewsInTabGroupL(
                TPbk2TabGroupId aTabGroupId ) const;

        /**
         * Returns view graph node count.
         *
         * @return  Node count.
         */
        inline TInt Count() const;

        /**
         * Returns view graph node at aIndex.
         *
         * @param aIndex    Index to query.
         * @return  View node.
         */
        inline const CPbk2ViewNode& operator[](
                TInt aIndex ) const;

    private: // Implementation
        inline CPbk2ViewGraph();
        void ConstructL(
                TResourceReader& aResReader );
        void AppendNodesFromResourceL(
                TResourceReader& aResReader );
        void LinkNodesWithTransitions();

    private: // Types
        /// Definition of the view node array type
        typedef CArrayPtrFlat<CPbk2ViewNode> CPbk2ViewNodeArray;

    private: // Data
        /// Own: Array of view nodes
        CPbk2ViewNodeArray* iNodes;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPbk2ViewNode::ViewId
// --------------------------------------------------------------------------
//
inline TUid CPbk2ViewNode::ViewId() const
    {
    return iViewId;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::DefaultPreviousNode
// --------------------------------------------------------------------------
//
inline CPbk2ViewNode* CPbk2ViewNode::DefaultPreviousNode() const
    {
    return iDefaultPreviousNode;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::PreviousNode
// --------------------------------------------------------------------------
//
inline CPbk2ViewNode* CPbk2ViewNode::PreviousNode() const
    {
    return iPreviousNode;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::ExitNode
// --------------------------------------------------------------------------
//
inline TBool CPbk2ViewNode::ExitNode() const
    {
    return iExitNode;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::TabGroupId
// --------------------------------------------------------------------------
//
inline TPbk2TabGroupId CPbk2ViewNode::TabGroupId() const
    {
    return iTabGroupId;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::TabGroupOrdering
// --------------------------------------------------------------------------
//
inline TPbk2TabGroupOrdering CPbk2ViewNode::TabGroupOrdering() const
    {
    return iTabGroupOrdering;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::TabResourceId
// --------------------------------------------------------------------------
//
inline TInt CPbk2ViewNode::TabResourceId() const
    {
    return iTabResourceId;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::SetPreviousNode
// --------------------------------------------------------------------------
//
inline void CPbk2ViewNode::SetPreviousNode( CPbk2ViewNode* aNode )
    {
    iPreviousNode = aNode;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::TransitionCount
// --------------------------------------------------------------------------
//
inline TInt CPbk2ViewNode::TransitionCount() const
    {
    return iTransitions.Count();
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::TransitionAt
// --------------------------------------------------------------------------
//
inline const TPbk2ViewTransition& CPbk2ViewNode::TransitionAt
        ( TInt aIndex ) const
    {
    // iTransitions::operator[] will check aIndex
    return iTransitions[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::SetTabGroupOrdering
// --------------------------------------------------------------------------
//
inline void CPbk2ViewNode::SetTabGroupOrdering
        ( TPbk2TabGroupOrdering aTabGroupOrdering )
    {
    iTabGroupOrdering = aTabGroupOrdering;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::SetTabResourceId
// --------------------------------------------------------------------------
//
inline void CPbk2ViewNode::SetTabResourceId( TInt aTabResourceId )
    {
    iTabResourceId = aTabResourceId;
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::CPbk2ViewGraph
// --------------------------------------------------------------------------
//
inline CPbk2ViewGraph::CPbk2ViewGraph()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::Count
// --------------------------------------------------------------------------
//
inline TInt CPbk2ViewGraph::Count() const
    {
    return iNodes->Count();
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::operator[]
// --------------------------------------------------------------------------
//
inline const CPbk2ViewNode& CPbk2ViewGraph::operator[]( TInt aIndex ) const
    {
    // iNodes::operator[] will check aIndex
    return *iNodes->At( aIndex );
    }

#endif // CPBK2VIEWGRAPH_H

// End of File
