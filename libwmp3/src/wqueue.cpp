#include <windows.h>
#include <stdio.h>

#include "wqueue.h"





WNumberQueue::WNumberQueue()
{
	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;
	c_data_size = 0;
	c_rest_data = 0;
	c_rest_size = 0;

}


WNumberQueue::~WNumberQueue()
{
	Clear();
}



int WNumberQueue::PushFirst(void *data, unsigned int size)
{
	// allocate new memory

	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)
		return 0;


	elem->data = data;
	elem->data_id = data;
	elem->size = size;
	c_sum_size += size;
	c_num++;

	if(c_first == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return 1;
	}

	elem->prev = 0;
	elem->next = c_first;
	c_first->prev = elem;
	c_first = elem;

	return 1;
}


int WNumberQueue::PushLast(void *data, unsigned int size)
{
	// allocate new memory

	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)
		return 0;



	elem->data = data;
	elem->data_id = data;
	elem->size = size;
	c_sum_size += size;
	c_num++;

	if(c_last == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return 1;
	}

	elem->prev = c_last;
	elem->next = 0;
	c_last->next = elem;
	c_last = elem;

	return 1;

}

unsigned int WNumberQueue::GetNum()
{
	return c_num;
}


int WNumberQueue::PullFirst(void **data, unsigned int *size)
{
	if(c_num == 0)
		return 0;

	if(data)
		*data = c_first->data;

	if(size)
		*size = c_first->size;

	c_num--;
	c_sum_size -= c_first->size;


	QUEUE_ELEMENT *next = c_first->next;
	
	if(next != 0)
	{
		next->prev = 0;
		free(c_first);
		c_first = next;
	}
	else
	{
		free(c_first);
		c_first = 0;
		c_last = 0;
	}



	return 1;
}



int WNumberQueue::QueryFirst(void **data, unsigned int *size)
{
	if(c_num == 0)
		return 0;

	if(data)
		*data = c_first->data;

	if(size)
		*size = c_first->size;

	return 1;
}


int WNumberQueue::QueryLast(void **data, unsigned int *size)
{
	if(c_num == 0)
		return 0;

	if(data)
		*data = c_last->data;

	if(size)
		*size = c_last->size;


	return 1;

}



int WNumberQueue::PullLast(void **data, unsigned int *size)
{
	if(c_num == 0)
		return 0;

	if(data)
		*data = c_last->data;

	if(size)
		*size = c_last->size;

	c_num--;
	c_sum_size -= c_last->size;

	QUEUE_ELEMENT *prev = c_last->prev;
	if(prev)
	{
		prev->next = 0;
		free(c_last);
		c_last = prev;
	}
	else
	{
		free(c_last);
		c_first = 0;
		c_last = 0;
	}

	return 1;

}


void WNumberQueue::Clear()
{
	if(c_num) // free memory
	{
		QUEUE_ELEMENT *elem = c_first;
		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			next = next->next;
			free(elem);
			elem = next;	
		}
	}

	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;
	c_data_size = 0;
	c_rest_data = 0;
	c_rest_size = 0;
}

int WNumberQueue::FindFromFirst(void *data)
{
	if(c_num) // free memory
	{

		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			if(next->data_id == data)
				return 1;
			next = next->next;	
		}
	}

	return 0;
}

int WNumberQueue::FindFromLast(void *data)
{
	if(c_num) // free memory
	{

		QUEUE_ELEMENT *prev = c_last;
		while(prev)
		{
			if(prev->data_id == data)
				return 1;
			prev = prev->prev;	
		}
	}

	return 0;
}






unsigned int WNumberQueue::GetDataSize()
{
	return c_data_size;
}


int WNumberQueue::Push(void *data, unsigned int size)
{
	int ret = PushLast(data, size);
	if(ret)
	{
		c_data_size += size;
		return 1;
	}

	return 0;
}

int WNumberQueue::Pull(void **data, unsigned int *size)
{
	unsigned int t_size;
	int ret = PullFirst(data, &t_size);
	if(ret)
	{
		c_rest_size = 0;
		c_rest_data = 0;

		c_data_size -= t_size;
		if(size)
			*size = t_size;
		return 1;
	}

	return 0;
}

int WNumberQueue::PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, unsigned int *nBytesRead, int *pnBufferIsOut)
{
	if(pnBufferIsOut == 0)
		return 0;

	if(pOutputBuffer == 0 || nBytesRead == 0)
		return 0;

	*pnBufferIsOut = 0;


	char *data;
	unsigned int size;
	int ret;
	char *tmp_buffer = (char*) pOutputBuffer;
	unsigned int tmp_bytes_to_read = nBytesToRead;
	*nBytesRead = 0;

	while(1)
	{
		if(c_rest_size)
		{
			if(c_rest_size >= tmp_bytes_to_read) // we have more than we need
			{
				memcpy(tmp_buffer, c_rest_data, tmp_bytes_to_read);
				*nBytesRead += tmp_bytes_to_read;
				c_rest_data += tmp_bytes_to_read;
				c_rest_size -= tmp_bytes_to_read;
				c_data_size -= tmp_bytes_to_read;
				if(c_rest_size == 0)
				{
					c_rest_data = 0;
					PullFirst(0,0);	// remove element from queue
					(*pnBufferIsOut)++;
				}

				return 1;
			}

			// we don't have all data we need
			

			memcpy(tmp_buffer, c_rest_data, c_rest_size);

			c_data_size -= c_rest_size;
			*nBytesRead += c_rest_size;
			tmp_buffer += c_rest_size;
			tmp_bytes_to_read -= c_rest_size;
			c_rest_size = 0;
			c_rest_data = 0;
			PullFirst(0,0);	// remove element from queue
			(*pnBufferIsOut)++;

			// and now try to get more data
				
		}

		ret = QueryFirst((void**) &data, &size);
		if(ret)
		{
			if(size >= tmp_bytes_to_read)	// we have more data than we need
			{
				memcpy(tmp_buffer, data, tmp_bytes_to_read); 
				c_data_size -= tmp_bytes_to_read;
				*nBytesRead += tmp_bytes_to_read;
				size -= tmp_bytes_to_read;
			
				if(size > 0) // we have rest of data
				{
					c_rest_data = data + tmp_bytes_to_read;
					c_rest_size = size;
				}
				else // we don't have rest data, remove element from queue
				{
					PullFirst(0,0);	// remove element from queue
					(*pnBufferIsOut)++;
				}

				return 1;
			}
			else // we don't have all data we need
			{

				memcpy(tmp_buffer, data, size);
				c_data_size -= size;
				*nBytesRead += size;
				tmp_buffer += size;
				tmp_bytes_to_read -= size;
				PullFirst(0,0);	// remove element from queue
				(*pnBufferIsOut)++;
				c_rest_data = 0;
				c_rest_size = 0;
			}


		}
		else // no more data in queue
		{
			if(*nBytesRead)
				return 1;

			return 0;
		}
	}


	return 0;
}


// =======================================================================================================================




WQueue::WQueue()
{
	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;

}


WQueue::~WQueue()
{
	Clear();
}



unsigned int WQueue::PushFirst(void *pSource, unsigned int nSize)
{
	// check input parameters

	if(pSource == 0 || nSize == 0)
		return 0;

	// allocate memory for element
	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)	// memory allocation error 
		return 0;

	// allocate memory for data
	void *data = malloc(nSize);
	if(data == 0)	// memory allocation error
	{
		free(elem);
		return 0;
	}

	memcpy(data, pSource, nSize);

	elem->alloc_buf = data;
	elem->data = data;
	elem->data_id = pSource;
	elem->size = nSize;
	c_sum_size += nSize;
	c_num++;

	if(c_first == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return nSize;
	}

	elem->prev = 0;
	elem->next = c_first;
	c_first->prev = elem;
	c_first = elem;

	return nSize;
}


unsigned int WQueue::PushLast(void *pSource, unsigned int nSize)
{
	// check input parameters
	if(pSource == 0 || nSize == 0)
		return 0;

	// allocate memory for element
	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)	// memory allocation error 
		return 0;

	// allocate memory for data
	void *data = malloc(nSize);
	if(data == 0)	// memory allocation error
	{
		free(elem);
		return 0;
	}

	memcpy(data, pSource, nSize);

	elem->alloc_buf = data;
	elem->data = data;
	elem->data_id = pSource;
	elem->size = nSize;
	c_sum_size += nSize;
	c_num++;


	if(c_last == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return nSize;
	}

	elem->prev = c_last;
	elem->next = 0;
	c_last->next = elem;
	c_last = elem;

	return nSize;

}


unsigned int WQueue::GetCount()
{
	return c_num;
}


unsigned int WQueue::PullFirst(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0)
		return c_first->size;

	// get smaller value of size
	unsigned int size = nSize < c_first->size ? nSize : c_first->size;

	if(size)
		memcpy(pDest, c_first->data, size); 

	c_num--;
	c_sum_size -= c_first->size;


	QUEUE_ELEMENT *next = c_first->next;
	
	if(next != 0)
	{
		next->prev = 0;
		free(c_first->alloc_buf);
		free(c_first);
		c_first = next;
	}
	else
	{
		free(c_first->alloc_buf);
		free(c_first);
		c_first = 0;
		c_last = 0;
	}


	return size;
}



unsigned int WQueue::PullLast(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0)
		return c_last->size;

	// get smaller value of size
	unsigned int size = nSize < c_last->size ? nSize : c_last->size;

	if(size)
		memcpy(pDest, c_last->data, size); 

	c_num--;
	c_sum_size -= c_last->size;


	QUEUE_ELEMENT *prev = c_last->prev;
	if(prev)
	{
		prev->next = 0;
		free(c_last->alloc_buf);
		free(c_last);
		c_last = prev;
	}
	else
	{
		free(c_last->alloc_buf);
		free(c_last);
		c_first = 0;
		c_last = 0;
	}

	return size;

}


unsigned int WQueue::QueryFirst(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0 || nSize == 0)
		return c_first->size;

	// get smaller value of size
	unsigned int size = nSize < c_first->size ? nSize : c_first->size;
	memcpy(pDest, c_first->data, size); 

	return size;
}



unsigned int WQueue::QueryLast(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0 || nSize == 0)
		return c_last->size;

	// get smaller value of size
	unsigned int size = nSize < c_last->size ? nSize : c_last->size;
	memcpy(pDest, c_last->data, size); 

	return size;

}



unsigned int WQueue::Clear()
{
	if(c_num) // free memory
	{
		QUEUE_ELEMENT *elem = c_first;
		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			free(next->alloc_buf);
			next = next->next;
			free(elem);
			elem = next;	
		}
	}

	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;
	return c_num;
}



int WQueue::FindFromFirst(void *data)
{
	if(c_num)
	{
		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			if(next->data_id == data)
				return 1;
			next = next->next;	
		}
	}

	return 0;
}


int WQueue::FindFromLast(void *data)
{
	if(c_num)
	{
		QUEUE_ELEMENT *prev = c_last;
		while(prev)
		{
			if(prev->data_id == data)
				return 1;
			prev = prev->prev;	
		}
	}

	return 0;
}






unsigned int WQueue::GetSizeSum()
{
	return c_sum_size;
}



unsigned int WQueue::PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, int *pnBufferIsOut)
{
	if(pnBufferIsOut == 0)
		return 0;

	*pnBufferIsOut = 0;


	if(pOutputBuffer == 0 || nBytesToRead == 0)
		return 0;



	if(c_num == 0 || c_sum_size == 0)
	{
		return 0;
	}

	unsigned int nBytesRead = 0;
	unsigned int nByteNeed = nBytesToRead;
	char *pBuffer = (char*) pOutputBuffer;

	while(1)
	{
		if(c_first->size >= nByteNeed) // we have enough data
		{
			memcpy(pBuffer, c_first->data, nByteNeed);
			nBytesRead += nByteNeed;
			
			c_first->size -= nByteNeed;
			c_sum_size -= nByteNeed;

			// check if buffer is empty
			if(c_first->size == 0)	// remove this element from queue
			{
				(*pnBufferIsOut)++;
				PullFirst((void*) 1, 0);
			}
			else	// element isn't empty, but we have removed some data from element
			{
				c_first->data = (char*)c_first->data +  nByteNeed;	
		
			}

			nByteNeed = 0;
		
		}
		else
		{
			memcpy(pBuffer, c_first->data, c_first->size);
			nBytesRead += c_first->size;
			pBuffer += c_first->size;
			nByteNeed -= c_first->size;
			c_sum_size -= c_first->size;
			c_first->size = 0;
			(*pnBufferIsOut)++;

			PullFirst((void*) 1, 0);	

		}


		if(nByteNeed == 0 || c_num == 0)
			return nBytesRead;

	}

}



unsigned int WQueue::QueryFirstPointer(void **ppDest, unsigned int *pnSize)
{
	if(c_num == 0)
		return 0;

	if(ppDest == 0 || pnSize == 0)
		return 0;

	*ppDest = c_first->alloc_buf;
	*pnSize = c_first->size;

	return 1;
}