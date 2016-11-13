#include <stddef.h>

template <typename T>
struct LinkedListNode
{
	public:
		T* value;
		LinkedListNode<T>* prevNode;
		LinkedListNode<T>* nextNode;

		LinkedListNode();
		~LinkedListNode();

};

template <typename T, typename ST>
struct LinkedList
{
	public:
		typedef bool (*LinkedList_SearchFunc)(T* currentObject, ST searchTerm);

		LinkedListNode<T>* nodes;

		void add(T* object);
		bool remove(T* object);
		void removeAll();

		bool hasSearchFunc();
		void setSearchFunc(LinkedList_SearchFunc searchFunction);
		T* search(ST searchTerm);

		LinkedList();
		~LinkedList();

	private:
		LinkedList_SearchFunc m_searchFunction;

};

/* Linked list functions */

template <typename T, typename ST>
void LinkedList<T, ST>::add(T* object)
{
	LinkedListNode<T>* newNode = new LinkedListNode<T>();
	newNode->nextNode = nodes;
	newNode->value = object;

	if (nodes)
		nodes->prevNode = newNode;

	nodes = newNode;
}

template <typename T, typename ST>
bool LinkedList<T, ST>::remove(T* object)
{
	LinkedListNode<T>* currentNode = nodes;
	while (currentNode != NULL)
	{
		if (currentNode->value == object)
		{
			if (currentNode->prevNode)
				currentNode->prevNode->nextNode = currentNode->nextNode;
			if (currentNode->nextNode)
				currentNode->nextNode->prevNode = currentNode->prevNode;

			if (currentNode == nodes)
				nodes = currentNode->nextNode;

			currentNode->nextNode = NULL;
			currentNode->prevNode = NULL;
			delete currentNode;

			return true;
		}

		currentNode = currentNode->nextNode;
	}

	return false;
}

template <typename T, typename ST>
void LinkedList<T, ST>::removeAll()
{
	delete nodes;
	nodes = NULL;
}

template <typename T, typename ST>
bool LinkedList<T, ST>::hasSearchFunc()
{
	return m_searchFunction != NULL;
}

template <typename T, typename ST>
void LinkedList<T, ST>::setSearchFunc(LinkedList_SearchFunc searchFunction)
{
	if (searchFunction)
		m_searchFunction = searchFunction;
}

template <typename T, typename ST>
T* LinkedList<T, ST>::search(ST searchTerm)
{
	if (m_searchFunction == NULL)
		return NULL;

	LinkedListNode<T>* currentNode = nodes;
	while (currentNode != NULL)
	{
		if (m_searchFunction(currentNode->value, searchTerm))
			return currentNode->value;

		currentNode = currentNode->nextNode;
	}

	return NULL;
}

template <typename T, typename ST>
LinkedList<T, ST>::LinkedList()
{
	nodes = NULL;
	m_searchFunction = NULL;
}

template <typename T, typename ST>
LinkedList<T, ST>::~LinkedList()
{
	removeAll();
}

template <typename T>
LinkedListNode<T>::LinkedListNode()
{
	value = NULL;
	prevNode = NULL;
	nextNode = NULL;
}

template <typename T>
LinkedListNode<T>::~LinkedListNode()
{
	if (nextNode)
		delete nextNode;
}
