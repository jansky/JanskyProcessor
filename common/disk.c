#include <bios.h>
#include <stdio.h>
#include <stdlib.h>

IOUNIT *iounit_create(FILE *s)
{
	IOUNIT *nunit;

	nunit = malloc(sizeof(IOUNIT));

	if(nunit == NULL)
		return NULL;

	nunit->source = s;
	nunit->next = NULL;
	

	//get size of stream
	if(fseek(nunit->source, SEEK_END, 0) != 0)
		return 0;

	nunit->size = ftell(nunit->source);

	if(nunit->size == -1)
		return NULL;
}
	

int iounit_get_list_length(IOUNIT *root)
{
	IOUNIT *current;

	current = root;
	
	for(int i = 0;true;i++)
	{
		if(current == NULL)
			return 0;

		else if(current->next = NULL)
			return i+1;
		else
			current = current->next;
	}

	return 0;
}

IOUNIT *iounit_get_at_index(IOUNIT *root, int index)
{
	IOUNIT *current;

	current = root;
	
	if(index == -1)
	{
		//get last item
		for(;;)
		{
			if(current == NULL)
				return NULL;
			else if(current->next = NULL)
				return current;
			else
				current = current->next;
		}
	}
	else if(index == 0)
	{
		//get first item
		return root;
	}
	else if(index > 0)
	{
		for(int i = 0;true;i++)
		{
			if(i == index)
				return current;
			else if(current != NULL && current->next != NULL)
				current = current->next;
			else
				return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

bool iounit_insert(IOUNIT *root, IOUNIT *new)
{
	IOUNIT *last = iounit_get_at_index(root, -1);

	if(last == NULL)
		return false;

	last->next = new;

	return true;
}

bool iounit_remove(IOUNIT *root, int index)
{

	if(index == 0)
	{
		IOUNIT *newhead = root->next;

		if(newhead == NULL)
			return false;

		free(root);

		root = newhead;
	}
	else if(index == -1 || index == (iounit_get_list_length(root) - 1))
	{
		int ind = iounit_get_list_length(root);

		if(ind == 0)
			return false;

		free(iounit_get_at_index(root, -1));

		IOUNIT *second_to_last = iounit_get_at_index(root, (ind - 2));

		if(second_to_last == NULL)
			return false;

		second_to_last->next = NULL;
	}
	else
	{
		IOUNIT *newnext;
		IOUNIT *newold;

		newnext = iounit_get_at_index(root, (index + 1));
		newold = iounit_get_at_index(root, (index - 1));

		if(newnext == NULL || newold == NULL)
			return false;

		free(iounit_get_at_index(root, index));

		newold->next = newnext;
	}

	return true;
}

		

		
			

		



						      
						     
