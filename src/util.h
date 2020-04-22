#pragma once

WCHAR* getWindowText_(HWND hwnd);


template <class T>
void insert_before(T* node, T* nNode) {
	T* pNode = nNode->prev; node->next = nNode;
	node->prev = pNode; pNode->next = nNode->prev = node; }
	
template <class T> 
T* remove_before(T* node) {
	T* nNode = node->next; T* pNode = node->prev;
	nNode->prev = pNode; return pNode->next = nNode;
}

template <class T>
void insert_before_root(T*& root, T* node) {
	if(!root) { root = node->next = node->prev = node; }
	else { insert_before(node, root); }
}

template <class T>
void remove_before_root(T*& root, T* node) {
	root = remove_before(node);
	if(root == node) root = NULL;
}


#define INIT_WND_CONTEXT(type, ...) type* This; \
	if(uMsg == WM_NCCREATE) { This = *(type**)lParam; \
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)This); \
		__VA_ARGS__; return TRUE; } \
	This = (type*)GetWindowLongPtr(hwnd, GWL_USERDATA);