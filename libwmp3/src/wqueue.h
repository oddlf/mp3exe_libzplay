/*
 * WQueue class - free C++ class for playing wave files
 * Copyright (C) 2003-2008 Zoran Cindori ( zcindori@inet.hr )
 *
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 */



#ifndef __W_QUEUE_H__
#define __W_QUEUE_H__




typedef struct w_queue_elem {
	void *alloc_buf;
	void *data;
	void *data_id;
	unsigned int size;
	struct w_queue_elem *prev; 
	struct w_queue_elem *next; 
} QUEUE_ELEMENT;


class WNumberQueue {
public:
	WNumberQueue();
	~WNumberQueue();

	// add elenemt as first element
	//
	//	INPUT:
	//		unsigned int data	- some data to add, can be anything, number, pointer ...
	//		unsigned int size	- size of data, can be enything
	//
	//	RETURN:
	//		1	- all OK
	//		0	- error, can't add element, memory allocation error
	int PushFirst(void *buf, unsigned int size);

	// add elenemt as last element
	//
	//	INPUT:
	//		unsigned int data	- some data to add, can be anything, number, pointer ...
	//		unsigned int size	- size of data, can be enything
	//
	//	RETURN:
	//		1	- all OK
	//		0	- error, can't add element, memory allocation error
	int PushLast(void *buf, unsigned int size);

	// pull first element from queue
	//
	//	INPUT:
	//		unsigned int *data	- pointer to variable receiving data ( can be 0 )
	//		unsigned int size	- pointer to variable receiving size of data ( can be 0 )
	//
	//	REUTRN:
	//		1	- element pulled
	//		0	- queue is empty

	int PullFirst(void **data, unsigned int *size);




	// Query first element from queue, don't remove element from queue
	//
	//	INPUT:
	//		unsigned int *data - pointer to variable receiving data ( can be 0 )
	//		unsigned int *size	- pointer to variable receiving size ( can be 0 )
	//
	//	REUTRN:
	//		1	- all OK
	//		0	- queue is empty
	int QueryFirst(void **data, unsigned int *size);


	// Query last element from queue, don't remove element from queue
	//
	//	INPUT:
	//		unsigned int *data - pointer to variable receiving data ( can be 0 )
	//		unsigned int *size	- pointer to variable receiving size ( can be 0 )
	//
	//	REUTRN:
	//		1	- all OK
	//		0	- queue is empty
	int QueryLast(void **data, unsigned int *size);



	// pull last element from queue
	//
	//	INPUT:
	//		unsigned int *data - pointer to variable receiving data ( can be 0 )
	//		unsigned int *size	- pointer to variable receiving size ( can be 0 )
	//
	//	REUTRN:
	//		1	- element pulled
	//		0	- queue is empty
	int PullLast(void **data, unsigned int *size);

	// search queue for data from first element
	//
	//	INPUT:
	//		unsigned int data	- data to search
	//
	//	RETURN:
	//		1	- data found
	//		0	- data isn't in queue
	int FindFromFirst(void *data);

	// search queue for data from last element
	//
	//	INPUT:
	//		unsigned int data	- data to search
	//
	//	RETURN:
	//		1	- data found
	//		0	- data isn't in queue
	int FindFromLast(void *data);


	// clear queue
	//
	void Clear();

	// get number of elements in queue
	//
	//	RETURN:
	//		number of elements in queue
	unsigned int GetNum();


	// push data into fifo queue
	//
	//	INPUT:
	//		unsigned int data	- some data to add, can be anything, number, pointer ...
	//		unsigned int size	- size of data, can be enything
	//
	//	RETURN:
	//		1	- all OK
	//		0	- error, can't push element, memory allocation error
	int Push(void *data, unsigned int size);


	// pull data from fifo queue
	//
	//	INPUT:
	//		unsigned int *data - pointer to variable receiving data ( can be 0 )
	//		unsigned int *size	- pointer to variable receiving size ( can be 0 )
	//
	//	REUTRN:
	//		1	- element pulled
	//		0	- queue is empty
	int Pull(void **data, unsigned int *size);


	

	// pull data from fifo queue into buffer
	//
	//	INPUT:
	//		pOutputBuffer	- pointer to receiving buffer
	//		nBytesToRead	- number of bytes to read into buffer
	//		nBytesRead		- number of bytes retrieved from queue into buffer
	//		pfBufferIsOut	- number of buffers thrown from stream
	//
	//	RETURN:
	//		1	- some data are retrieved into buffer
	//		0	- queue is empty, no data in buffer
	//
	//	REMARKS:
	//		This function can retrieve data from multiple elements to fill output buffer
	//		If output buffer is larger than all data in queue, you can empty whole queue with
	//		one call to this function.
	//	
	int PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, unsigned int *nBytesRead, int *pnBufferIsOut);

	// get size of all data in queue ( used only with PullDataFifo mode )
	// this function returns number of bytes in queue after you pull some data with  PullDataFifo 
	unsigned int GetDataSize();


private:
	QUEUE_ELEMENT *c_first;
	QUEUE_ELEMENT *c_last;
	unsigned int c_num;
	unsigned int c_sum_size;
	unsigned int c_data_size;	// size of all data into queue
	char *c_rest_data;
	unsigned int c_rest_size;



};




class WQueue {
public:
	WQueue();
	~WQueue();

// ==================================================================================================

	// push data into queue at queue start

	unsigned int PushFirst(void *pSource, unsigned int nSize);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you need to push into queue.
	//			This parameter is also queue element identifier and you can use this
	//			value to check if this block of data is still in queue using functions
	//			FindFromFirst and FindFromLast.
	//			This value can't be 0. If this value is 0 function fails and returns 0.
	//
	//		nSize
	//			Specifies the size, in bytes, of the block of memory to push into queue.
	//			This value can't be 0. If this value is 0 function fails and returns 0.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied into queue.
	//		Function returns 0 if memory allocation error occurs.
	//		
	//
	//	REMARKS:
	//		Queue allocates memory to store data and copies data into this memory.
	//		After you push data into queue you can destroy data pointed by pSource.	


// ==================================================================================================

	// push data into queue at the queue end

	unsigned int PushLast(void *pSource, unsigned int nSize);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you need to push into queue.
	//			This parameter is also queue element identifier and you can use this
	//			value to check if this block of data is still in queue using functions
	//			FindFromFirst and FindFromLast.
	//			This value can't be 0. If this value is 0 function fails and returns 0.
	//
	//		nSize
	//			Specifies the size, in bytes, of the block of memory to push into queue.
	//			This value can't be 0. If this value is 0 function fails and returns 0.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied into queue.
	//		Function returns 0 if memory allocation error occurs.
	//		
	//
	//	REMARKS:
	//		Queue allocates memory to store data and copies data into this memory.
	//		After you push data into queue you can destroy data pointed by pSource.	


// ==================================================================================================

	//	copy data from first element into memory block and remove first element
	//	from queue. 

	unsigned int PullFirst(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function copies no data and element is removed
	//			from queue.
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in first
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data and rest of data is lost.
	//
	//		If you need to remove element from queue without data retrieving, call
	//		this function with pDest set to 1 and nSize set to 0.


// ==================================================================================================


	//	copy data from last element into memory block and remove last element
	//	from queue. 

	unsigned int PullLast(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function copies no data and element is removed
	//			from queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in last
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data and rest of data is lost.
	//
	//		If you need to remove element from queue without data retrieving, call
	//		this function with pDest set to 1 and nSize set to 0.


// ==================================================================================================


// ==================================================================================================

	//	copy data from first element into memory block, but don't remove element from queue

	unsigned int QueryFirst(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in first
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data.


// ==================================================================================================


	//	copy data from last element into memory block, but don't remove element from queue

	unsigned int QueryLast(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in first
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data.


// ==================================================================================================


	// check if data buffer is still in queue, search from first element

	int FindFromFirst(void *pSource);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you pushed into queue
	//			with PushFirst od PushLast function.
	//			This parameter can't be 0.	
	//	
	//	RETURN VALUES:
	//		If buffer identified with pSource is still in queue, function returns 1.
	//		If buffer isn't in queue anymore, function returns 0
	//		
	//	REMARKS:
	//		This function search from first element of queue.

// ==================================================================================================


	// check if data buffer is still in queue, search from last element

	int FindFromLast(void *pSource);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you pushed into queue
	//			with PushFirst od PushLast function.
	//			This parameter can't be 0.	
	//	
	//	RETURN VALUES:
	//		If buffer identified with pSource is still in queue, function returns 1.
	//		If buffer isn't in queue anymore, function returns 0
	//		
	//	REMARKS:
	//		This function search from last element of queue.


// ==================================================================================================

	// clear queue, free all memory and remove all elements from queue.

	unsigned int Clear();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns number of elements removed from queue.
	//		If queue was empty, function returns 0.	

// ==================================================================================================


	// get number of elements in queue

	unsigned int GetCount();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns number of elements in queue.


// ==================================================================================================

	// get size of all data into queue

	unsigned int GetSizeSum();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns size of all data in queue


// ==================================================================================================


	// copy data from queue into buffer, start from first element in queue
	// this function treat queue al one large continuous buffer

	unsigned int PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, int *pnBufferIsOut);

	//	PARAMETERS:
	//		pOutputBuffer
	//			Points to the starting address of the destination block.
	//			This value can't be 0.
	//
	//		nBytesToRead
	//			Number of bytes to copy into pOutputBuffer. If first element doesn't contain
	//			enough data, function copies data from second buffer, third ...
	//			This function deals with queue as with one large memory buffer which contains
	//			all data you pushed into queue. If you read all data from some element, this
	//			element is removed from queue.
	//
	//		pnBytesRead
	//			Pointer to variable receivind number of bytes read from queue.
	//
	//		pnBufferIsOut
	//			Number of buffers thrown from stream.	
	//
	//	RETURN VALUES:
	//		Function returns number of data read from queue.
	//		If function returns 0, queue is empty or error occurs.
	//		
	//
	//	REMARKS:
	//		If you need more data than you have in queue, function copies all data from queue, and
	//		then queue is empty.
	
	
	// ==================================================================================================


	// get pointer to first data buffer and get data buffer size
	unsigned int QueryFirstPointer(void **ppDest, unsigned int *pnSize);	


	//	PARAMETERS:
	//		ppDest
	//			Points to variable receiving addres of first data block
	//
	//		pnSize
	//			Points to variable receiving size of data block		
	//	
	//	RETURN VALUES:
	//
	//		If return value is 0, queue is empty.
	//		
	//


private:
	QUEUE_ELEMENT *c_first;
	QUEUE_ELEMENT *c_last;
	unsigned int c_num;
	unsigned int c_sum_size;




};


#endif

