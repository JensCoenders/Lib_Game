#ifndef GAME_UTILS_HPP
#define GAME_UTILS_HPP

#include <string>

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

template <typename T, typename SearchType>
struct LinkedList
{
	public:
		LinkedListNode<T>* nodes;

		void add(T* object);
		void remove(T* object);
		void removeAll();

		bool hasSearchFunc();
		void setSearchFunc(bool (*searchFunction)(T* currentObject, SearchType searchTerm));
		T* search(SearchType searchTerm);

		LinkedList();
		~LinkedList();

	private:
		bool (*m_searchFunction)(T* currentObject, SearchType searchTerm);

};

/* Linked list functions */

template <typename T, typename SearchType>
void LinkedList<T, SearchType>::add(T* object)
{
	LinkedListNode<T>* newNode = new LinkedListNode<T>();
	newNode->nextNode = nodes;
	newNode->value = object;

	if (nodes)
		nodes->prevNode = newNode;

	nodes = newNode;
}

template <typename T, typename SearchType>
void LinkedList<T, SearchType>::remove(T* object)
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
		}

		currentNode = currentNode->nextNode;
	}
}

template <typename T, typename SearchType>
void LinkedList<T, SearchType>::removeAll()
{
	delete nodes;
	nodes = NULL;
}

template <typename T, typename SearchType>
bool LinkedList<T, SearchType>::hasSearchFunc()
{
	return m_searchFunction != NULL;
}

template <typename T, typename SearchType>
void LinkedList<T, SearchType>::setSearchFunc(bool (*searchFunction)(T* currentObject, SearchType searchTerm))
{
	if (searchFunction)
		m_searchFunction = searchFunction;
}

template <typename T, typename SearchType>
T* LinkedList<T, SearchType>::search(SearchType searchTerm)
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

template <typename T, typename SearchType>
LinkedList<T, SearchType>::LinkedList()
{
	nodes = NULL;
	m_searchFunction = NULL;
}

template <typename T, typename SearchType>
LinkedList<T, SearchType>::~LinkedList()
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

/* Combiner functions */

template <typename First>
std::string combineStringPath(First& firstString = "")
{
	return firstString;
}

template <typename First, typename ... Rest>
std::string combineStringPath(First firstString, Rest&... rest)
{
	return std::string(firstString) + "\\" + combineStringPath(rest...);
}

template <typename First>
int combineModuleTypes(First firstModule = 0)
{
	return firstModule;
}

template <typename First, typename ... Rest>
int combineModuleTypes(First firstModule, Rest ... restModules)
{
	return (int) firstModule | combineModuleTypes(restModules...);
}

#endif
