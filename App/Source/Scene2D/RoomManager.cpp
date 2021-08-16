//#include "RoomManager.h"
//#include <iostream>
//using namespace std;
//
//// Include Shader Manager
//#include "RenderControl\ShaderManager.h"
//
//#include "System\filesystem.h"
//
//
//
///**
// @brief Constructor This constructor has protected access modifier as this class will be a Singleton
// */
//CRoomManager2D::CRoomManager2D(void)
//{
//	cursor = 0;
//	current = first;
//}
//
///**
// @brief Destructor
// */
//CRoomManager2D::~CRoomManager2D(void)
//{
//	//delete nodes in deconstructor
//	CRoomNode* tempcurrent = first;
//	while (tempcurrent != nullptr)
//	{
//		CRoomNode* NodeDelete = tempcurrent;
//		tempcurrent = tempcurrent->next;
//
//		delete NodeDelete;
//	}
//}
//
//bool CRoomManager2D::getprev()
//{
//	if (current->prev == nullptr)
//	{
//		//if prev is null cant go prev so return false/error
//		return false;
//	}
//	else
//	{
//		//Code for cursor to go prev
//		current = current->prev;
//		cursor--;
//		return true;
//	}
//
//}
//
//bool CRoomManager2D::getnext()
//{
//	if (current->next == nullptr)
//	{
//		//if next is null cant go next so return false/error
//		return false;
//	}
//	else
//	{
//		//Code for cursor to go next
//		current = current->next;
//		cursor++;
//		return true;
//	}
//}
//
//void CRoomManager2D::getadd()
//{
//	newnode = new CNode;
//	if (current == nullptr)
//	{
//		first = last = current = newnode;
//	}
//	else if (current->val == 'E')
//	{
//		//check for end node 
//		current->next = newnode;
//		newnode->prev = current;
//		last = newnode;
//
//		current->next->val = 'E';
//		current->val = 'o';
//
//	}
//	else
//	{
//		//with temp
//
//		/*CNode* newnode = new CNode;
//		CNode* tempnode = current->next;
//		current->next = newnode;
//		newnode->val = 'w';
//		newnode->next = tempnode;
//		tempnode->prev = newnode;
//		newnode->prev = current;*/
//
//		//without temp
//
//		newnode->next = current->next;
//		newnode->prev = current;
//
//		current->next->prev = newnode;
//		current->next = newnode;
//
//		newnode->val = 'w';
//
//	}
//}
//
