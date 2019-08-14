#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>
#include <stack>
#include <queue>

using namespace std;

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
	// This is a way to refer to the base class object.
	using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
	// Each node within the pairing heap
	class Node {
	public:
		explicit Node(const TYPE &val)
			: elt{ val }, child{ nullptr }, sibling{ nullptr }, previous{ nullptr } //may not work
		{}

		// Description: Allows access to the element at that Node's position.
		// There are two versions, getElt() and a dereference operator, use
		// whichever one seems more natural to you.
		// Runtime: O(1) - this has been provided for you.
		const TYPE &getElt() const { return elt; }
		const TYPE &operator*() const { return elt; }

		// The following line allows you to access any private data members of this
		// Node class from within the PairingPQ class. (ie: myNode.elt is a legal
		// statement in PairingPQ's add_node() function).
		friend PairingPQ;

	private:
		TYPE elt;
		Node *child;
		Node *sibling;
		Node *previous;
		// DONE: Add one extra pointer (parent or previous) as desired.
	}; // Node


	// Description: Construct an empty priority_queue with an optional comparison functor.
	// Runtime: O(1)
	explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
		BaseClass{ comp }, mainNode{ nullptr } {
	} // PairingPQ()


	// Description: Construct a priority_queue out of an iterator range with an optional
	//              comparison functor.
	// Runtime: O(n) where n is number of elements in range.
	// TODO: when you implement this function, uncomment the parameter names.
	template<typename InputIterator>
	PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
		BaseClass{ comp }, mainNode{ nullptr } {
		while (start != end) {
			push(*start);
			++start;
		}
		// TODO: Implement this function.
	} // PairingPQ()


	// Description: Copy constructor.
	// Runtime: O(n)
	PairingPQ(const PairingPQ& other) :
		BaseClass{ other.compare }, mainNode{ nullptr } {
		// TODO: Implement this function.
		stack<Node*> q;
		q.push(other.mainNode);
		Node* nodeptr;
		while (!q.empty()) {//keep going until empty
			nodeptr = q.top();
			q.pop();
			if (nodeptr->sibling != nullptr) {//if sibling exists
				q.push(nodeptr->sibling);
			}
			if (nodeptr->child != nullptr) {//if child exists
				q.push(nodeptr->child);
			}
			push(nodeptr->getElt());
		}
	} // PairingPQ()

	// Description: Copy assignment operator.
	// Runtime: O(n)
	// TODO: when you implement this function, uncomment the parameter names.
	PairingPQ& operator=(const PairingPQ& rhs) {
		PairingPQ<TYPE, COMP_FUNCTOR> temp(rhs);//temp copy of rhs
		swap(temp);
		return *this;
	} // operator=()


	// Description: Destructor
	// Runtime: O(n)
	~PairingPQ() {
		// TODO: Implement this function.
		if (mainNode == nullptr) {//if already a nullptr, do nothing
			return;
		}
		stack<Node*> q;
		q.push(mainNode);
		Node *nodeptr;
		while (!q.empty()) {//keep going until empty
			nodeptr = q.top(); //should be declaring inside here??????@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			q.pop();
			if (nodeptr->sibling != nullptr) {//if sibling exists
				q.push(nodeptr->sibling);
			}
			if (nodeptr->child != nullptr) {//if child exists
				q.push(nodeptr->child);
			}
			delete nodeptr;
			nodeptr = nullptr;
		}
	} // ~PairingPQ()


	// Description: Assumes that all elements inside the priority_queue are out of order and
	//              'rebuilds' the priority_queue by fixing the priority_queue invariant.
	// Runtime: O(n)
	virtual void updatePriorities() {
		// TODO: Implement this function.
		stack<Node*> q;
		q.push(mainNode);
		mainNode = nullptr;
		Node *nodeptr;
		while (!q.empty()) {//keep going until empty
			nodeptr = q.top();
			q.pop();
			if (nodeptr->sibling != nullptr) {//if sibling exists
				q.push(nodeptr->sibling);
			}
			if (nodeptr->child != nullptr) {//if child exists
				q.push(nodeptr->child);
			}
			//break connections
			if (nodeptr->sibling != nullptr) {
				nodeptr->sibling = nullptr;
			}
			if (nodeptr->child != nullptr) {
				nodeptr->child = nullptr;
			}
			if (nodeptr->previous != nullptr) {
				nodeptr->previous = nullptr;
			}
			mainNode = meld(mainNode, nodeptr);
		}
	} // updatePriorities()


	// Description: Add a new element to the priority_queue. This is almost done,
	//              in that you should implement push functionality in the addNode()
	//              function, and this function should call addNode().
	// Runtime: O(1)
	// TODO: when you implement this function, uncomment the parameter names.
	virtual void push(const TYPE & val) {
		addNode(val);
	} // push()


	// Description: Remove the most extreme (defined by 'compare') element from
	//              the priority_queue.
	// Note: We will not run tests on your code that would require it to pop an
	// element when the priority_queue is empty. Though you are welcome to if you are
	// familiar with them, you do not need to use exceptions in this project.
	// Runtime: Amortized O(log(n))
	virtual void pop() {
		// TODO: Implement this function.
		//remove the root node and get new trees
		Node* nodeptr = nullptr;
		if (mainNode->child != nullptr) {//if theres a child
			nodeptr = mainNode->child; //new pointer to child
			mainNode->child = nullptr;
			delete mainNode;
			mainNode = nullptr; //mainNode is nullptr now!!
			nodeptr->previous = nullptr;
		}
		else {//mainnode's child is a nullptr... only one elt left!!
			delete mainNode;
			mainNode = nullptr;
			--Size;
			return;
		}
		queue<Node*> q;
		while (nodeptr->sibling != nullptr) {//can always find another sibling
			q.push(nodeptr);//push it in
			nodeptr = nodeptr->sibling;//move on to next one
			nodeptr->previous->sibling = nullptr;
			nodeptr->previous = nullptr;
		}
		q.push(nodeptr);//nodeptr pointing to last elt
		nodeptr = nullptr;//now this nodeptr is useless

		//start melding
		while (q.size() != 1) {//stop when there's one more
			Node* tree1 = q.front();
			q.pop();
			Node* tree2 = q.front();
			q.pop();

			tree1 = meld(tree1, tree2); //legal
			q.push(tree1);
		}
		mainNode = q.front();
		q.pop();
		--Size;
	} // pop()


	// Description: Return the most extreme (defined by 'compare') element of
	//              the heap.  This should be a reference for speed.  It MUST be
	//              const because we cannot allow it to be modified, as that
	//              might make it no longer be the most extreme element.
	// Runtime: O(1)
	virtual const TYPE & top() const {
		// DONE: Implement this function
		return mainNode->getElt();
	} // top()


	// Description: Get the number of elements in the priority_queue.
	// Runtime: O(1)
	virtual std::size_t size() const {
		return Size; // DONE: Delete or change this line
	} // size()

	// Description: Return true if the priority_queue is empty.
	// Runtime: O(1)
	virtual bool empty() const {
		if (Size == 0) {
			return true;
		}
		return false; // DONE: Delete or change this line
	} // empty()



	// Description: Updates the priority of an element already in the priority_queue by
	//              replacing the element refered to by the Node with new_value.
	//              Must maintain priority_queue invariants.
	//
	// PRECONDITION: The new priority, given by 'new_value' must be more extreme
	//               (as defined by comp) than the old priority.
	//
	// Runtime: As discussed in reading material.
	// TODO: when you implement this function, uncomment the parameter names.
	void updateElt(Node* node, const TYPE & new_value) {//node is a pointer to the right address already
		// TODO: Implement this function
		node->elt = new_value; //replaces the value of that node

		if (node == mainNode) {//if its the main root... exit. 
			return;
		}
		//fix it nonetheless
		else if (node != node->previous->child) {//not leftmost, there will be a previous sibling
			//this is NOT the leftmost, so either middle or last
				//middle
			if (node->sibling != nullptr) {//if it has a sibling, then its a middle
				//fix sibling positions
				node->sibling->previous = node->previous;
				node->previous->sibling = node->sibling;

				//fix middle node connections to isolate as new root
				node->sibling = nullptr;
				node->previous = nullptr;

				//fix version 1
				mainNode = meld(mainNode, node);//new main will now be fixed
			}
				//last node
			else if (node->sibling == nullptr) { 
				node->previous->sibling = nullptr;
				node->previous = nullptr;

				//fix version 2
				mainNode = meld(mainNode, node);
			}
		}
		else {//this is the leftmost and needs to be fixed
			//disconnection
			node->previous->child = node->sibling;
			node->sibling->previous = node->previous;
			node->previous = nullptr;
			node->sibling = nullptr;

			//fix verison 3
			mainNode = meld(mainNode, node);
		}
		//fix heap now with invariant
		/*
		if (node == mainNode) {//if its the main root... exit. 
			return;
		}
		else if (node == node->previous->child && !(this->compare((node->previous)->getElt(), node->getElt()))) { //this is the leftmost node. PREVIOUS WIL EXIST since mainnode already checked for
			return;
		}
		else if (node != node->previous->child) {//not leftmost
			Node* nodePtr = node;
			while (nodePtr != nodePtr->previous->child) {//get to the left most
				nodePtr = nodePtr->previous;
			}
			if (!(this->compare(nodePtr->getElt(), node->getElt()))) { //the changed value was not the leftmost, but still less than leftmost
				return;
			}
			else if (this->compare(nodePtr->getElt(), node->getElt())) {//the changed value was not the leftmost, but is greater than the leftmost
				if (this->compare((nodePtr->previous)->getElt(), node->getElt())) {//if the parent is less than the node
					//fix

					//this is NOT the leftmost, so either middle or last
						//middle
					if (node->sibling != nullptr) {//if it has a sibling, then its a middle
						//fix sibling positions
						node->sibling->previous = node->previous;
						node->previous->sibling = node->sibling;

						//fix middle node connections to isolate as new root
						node->sibling = nullptr;
						node->previous = nullptr;

						//fix version 1
						mainNode = meld(mainNode, node);//new main will now be fixed
					}

					//last node
					else if (node->sibling == nullptr) {//right most 
						node->previous->sibling = nullptr;
						node->previous = nullptr;

						//fix version 2
						mainNode = meld(mainNode, node);
					}
				}
				else {//its still greater than or equal to root. It's OK 
					return;
				}
			}
		}
		else {
			//this is the leftmost and needs to be fixed

			//disconnection
			node->previous->child = node->sibling;
			node->sibling->previous = node->previous;
			node->previous = nullptr;
			node->sibling = nullptr;

			//fix verison 3
			mainNode = meld(mainNode, node);
		}
		*/
	} // updateElt()
	
	void testUpdateElt() {//5 1 1 0
		updateElt(mainNode, 6);
		assert(mainNode->getElt() == 6);
		updateElt(mainNode, 5);//return to normal, 5 1 1 0
		updateElt(mainNode->child, 8); //should be 8 5 1 0
		assert(mainNode->getElt() == 8);
		updateElt(mainNode->child->child->sibling, 6);//8 6 5 1

		//is the tree really in the right structure tho?@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@this implies melding is correct
		assert(mainNode->getElt() == 8);
		assert(mainNode->child->getElt() == 6);
		assert(mainNode->child->sibling->getElt() == 5);
		assert(mainNode->child->sibling->child->getElt() == 1);
	}

	void testUpdateElt2() {
		//made purely to test a middle node
		//first test to see the change was fine
		updateElt(mainNode->child->sibling, 7);
		assert(mainNode->getElt() == 8);
		assert(mainNode->child->getElt() == 7);
		assert(mainNode->child->sibling->getElt() == 1);
		assert(mainNode->child->sibling->sibling->getElt() == 5);
		assert(mainNode->child->sibling->sibling->child->getElt() == 1);

		updateElt(mainNode->child->sibling, 9);
		assert(mainNode->getElt() == 9);
		assert(mainNode->child->getElt() == 8);
		assert(mainNode->child->child->getElt() == 7);
		assert(mainNode->child->child->sibling->getElt() == 5);
		assert(mainNode->child->child->sibling->child->getElt() == 1);
	}

	void testUpdateElt3() {

		updateElt(mainNode->child->sibling->sibling->child->sibling->sibling->sibling, 100);
		assert(mainNode->child->child->sibling->sibling->child->sibling->sibling->sibling->getElt() == 2);
		assert(mainNode->child->child->sibling->sibling->child->sibling->sibling->getElt() == 34);
		assert(mainNode->child->child->sibling->sibling->child->sibling->getElt() == 3);
		assert(mainNode->child->child->sibling->sibling->child->getElt() == 13);
		assert(mainNode->child->child->sibling->sibling->getElt() == 71);
		assert(mainNode->child->child->sibling->getElt() == 8);
		assert(mainNode->child->child->getElt() == 10);
		assert(mainNode->child->getElt() == 78);
		assert(mainNode->getElt() == 100);

		updateElt(mainNode->child->child->sibling->sibling, 80);
		assert(mainNode->child->child->sibling->sibling->sibling->getElt() == 2);
		assert(mainNode->child->child->sibling->sibling->getElt() == 34);
		assert(mainNode->child->child->sibling->getElt() == 3);
		assert(mainNode->child->child->getElt() == 13);
		assert(mainNode->child->getElt() == 80);
		assert(mainNode->getElt() == 100);
		assert(mainNode->child->sibling->child->sibling->getElt() == 8);
		assert(mainNode->child->sibling->child->getElt() == 10);
		assert(mainNode->child->sibling->getElt() == 78);

		updateElt(mainNode->child->sibling, 100);
		assert(mainNode->child->child->child->sibling->sibling->sibling->getElt() == 2);
		assert(mainNode->child->child->child->sibling->sibling->getElt() == 34);
		assert(mainNode->child->child->child->sibling->getElt() == 3);
		assert(mainNode->child->child->child->getElt() == 13);
		assert(mainNode->child->child->getElt() == 80);
		assert(mainNode->child->getElt() == 100);
		assert(mainNode->child->sibling->sibling->getElt() == 8);
		assert(mainNode->child->sibling->getElt() == 10);
		assert(mainNode->getElt() == 100);

		updateElt(mainNode->child, 101);
		assert(mainNode->child->child->sibling->getElt() == 8);
		assert(mainNode->child->child->getElt() == 10);
		assert(mainNode->child->getElt() == 100);
		assert(mainNode->child->sibling->child->sibling->sibling->sibling->getElt() == 2);
		assert(mainNode->child->sibling->child->sibling->sibling->getElt() == 34);
		assert(mainNode->child->sibling->child->sibling->getElt() == 3);
		assert(mainNode->child->sibling->child->getElt() == 13);
		assert(mainNode->child->sibling->getElt() == 80);
		assert(mainNode->getElt() == 101);
	}

	
	// Description: Add a new element to the priority_queue. Returns a Node* corresponding
	//              to the newly added element.
	// Runtime: O(1)
	// TODO: when you implement this function, uncomment the parameter names.
	// NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
	//       never move or copy/delete that node in the future, until it is eliminated
	//       by the user calling pop().  Remember this when you implement updateElt() and
	//       updatePriorities().
	Node* addNode(const TYPE & val) {
		// TODO: Implement this function
		Node *newNode = new Node(val);
		mainNode = meld(mainNode, newNode);//could possibly lead to memory loss!!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		++Size;
		return newNode; // Done: Delete or change this line
	} // addNode()


private:
	// TODO: Add any additional member functions or data you require here.
	size_t Size = 0;
	Node* mainNode;//is this even necessary
	// DONE: We recommend creating a 'meld' function (see the Pairing Heap papers).

	Node* meld(Node* tree1Root, Node* tree2Root) {
		//push
		//this assumes at least one was a nullptr
		if (tree1Root == nullptr) {//specifically for push, for first push, tree1Root is main
			mainNode = tree2Root;
			return mainNode;
		}
		else if (tree2Root == nullptr) {//first push, tree2Root is main
			mainNode = tree1Root;
			return mainNode;
		}

		if (this->compare(tree2Root->getElt(), tree1Root->getElt())) {//merge second into first
			if (tree1Root->child == nullptr) {
				tree1Root->child = tree2Root;
				tree2Root->previous = tree1Root;
				return tree1Root;
			}
			else{ //tree1Root has a child!!
				//this assumes both are full grown trees
				tree2Root->sibling = tree1Root->child;
				tree1Root->child = tree2Root;
				tree2Root->sibling->previous = tree2Root;
				tree2Root->previous = tree1Root;
			//	tree2Root = nullptr; //dont i need this????????????????????????????????????????????@@@@@@@@@@@@@@
				return tree1Root;
			}
			
		}
		//now the first tree is smaller or they're equal.. merge first into second
		if (tree2Root->child == nullptr) {
			tree2Root->child = tree1Root;
			tree1Root->previous = tree2Root;
			return tree2Root;
		}
		else {// (tree2Root->child != nullptr)
			tree1Root->sibling = tree2Root->child;
			tree2Root->child = tree1Root;
			tree1Root->sibling->previous = tree1Root;
			tree1Root->previous = tree2Root;
		//	tree1Root = nullptr;
			return tree2Root;
		}
	}//meld()

	void swap(PairingPQ &other){
		std::swap(mainNode, other.mainNode);
		std::swap(Size, other.Size);
	}
};





#endif // PAIRINGPQ_H