#include "gc.h"

#include <stdlib.h>
#include <stdio.h>

struct GCFreeFunction *gc_free_function_create(size_t res_id, gc_free_function free_function)
{
    struct GCFreeFunction *ff_root = malloc(sizeof(struct GCFreeFunction));

    if(ff_root == NULL)
        return ff_root;
    
    ff_root->free_function = free_function;
    ff_root->res_id = res_id;

    ff_root->next = NULL;

    return ff_root;
}

void gc_free_function_free_all(struct GCFreeFunction *ff_root)
{
    if(ff_root == NULL)
        return;
    
    struct GCFreeFunction *next = ff_root;

    while(next != NULL)
    {
        struct GCFreeFunction *current = next;

        next = next->next;

        free(current);
    }
}

struct GCInfo *gc_init(void)
{
    struct GCInfo *gc_new = malloc(sizeof(struct GCInfo));

    if(gc_new == NULL)
        return NULL;
    
    gc_new->resources = NULL;
    gc_new->length = 0;

    return gc_new;
}

void *gc_malloc(struct GCInfo *info, size_t size)
{
    if(info == NULL)
        return NULL;
    
    if(info->resources == NULL)
        return NULL;
    
    if(size < 1)
        return NULL;
    
    void *res_new = malloc(size);

    if(res_new == NULL)
        return NULL;
    
    if(gc_track(info, res_new, NULL) != 0)
    {
        free(res_new);
        return NULL;
    }

    return res_new;
}

void *gc_calloc(struct GCInfo *info, size_t nmemb, size_t size)
{
    if(info == NULL)
        return NULL;
    
    if(info->resources == NULL)
        return NULL;
    
    if(size < 1)
        return NULL;
    
    void *res_new = calloc(nmemb, size);

    if(res_new == NULL)
        return NULL;
    
    if(gc_track(info, res_new, NULL) != 0)
    {
        free(res_new);
        return NULL;
    }

    return res_new;
}

void *gc_realloc(struct GCInfo *info, void *res, size_t size)
{
    if(info == NULL)
        return NULL;
    
    if(info->resources == NULL)
        return NULL;
    
    if(res == NULL)
        return NULL;
    
    size_t match_count = 0;

    size_t *match_ids = gc_match_resource(info, res, &match_count);

    if(match_count == 0)
    {
        free(match_ids);
        return NULL;
    }

    void *res_new = realloc(res, size);

    if(res_new == NULL)
    {
        free(match_ids);
        return NULL;
    }

    for(size_t i = 0; i < match_count; i++)
    {
        info->resources[i] = res_new;
    }

    free(match_ids);

    return res_new;
}

int gc_track(struct GCInfo *info, void *res, gc_free_function free_function)
{
    if(info == NULL || res == NULL)
        return 1;
    
    if(info->resources == NULL)
        return 1;
    
    info->resources = realloc(info->resources, (info->length + 1)*sizeof(void*));

    if(info->resources == NULL)
    {
        return 1;
    }
    
    info->resources[info->length] = res;

    info->length++;

    if(free_function != NULL)
    {
        struct GCFreeFunction *ff_new = gc_free_function_create(info->length - 1, free_function);

        if(ff_new == NULL)
        {
            info->resources[info->length - 1] = NULL;

            return 2;
        }

        if(info->ff_root == NULL)
        {
            info->ff_root = ff_new;
        }
        else
        {
            struct GCFreeFunction *ff_next = info->ff_root;

            while(ff_next != NULL)
            {
                if(ff_next->next == NULL)
                {
                    ff_next->next = ff_new;
                    break;
                }

                ff_next = ff_next->next;
            }
        }
    }

    return 0;
}

void *gc_untrack(struct GCInfo *info, void *res)
{
    if(info == NULL || res == NULL)
        return NULL;
    
    if(info->resources == NULL)
        return NULL;
    
    size_t match_count = 0;

    size_t *match_ids = gc_match_resource(info, res, &match_count);

    if(match_count == 0)
        return NULL;
    
    for(size_t i = 0; i < match_count; i++)
    {
        info->resources[i] = NULL;
    }

    return res;
}

void gc_destroy(struct GCInfo *info)
{
    if(info == NULL)
        return;
    
    if(info->resources == NULL)
        return;
    
    /* First free items with custom free functions */

    struct GCFreeFunction *ff_next = info->ff_root;

    while(ff_next != NULL)
    {
        if(ff_next->free_function != NULL && info->resources[ff_next->res_id] != NULL)
        {
            ff_next->free_function(info->resources[ff_next->res_id]);

            info->resources[ff_next->res_id] = NULL;
        }

        ff_next = ff_next->next;
    }

    gc_free_function_free_all(info->ff_root);

    /* Then free all other items */

    for(size_t i = 0; i < info->length; i++)
    {
        size_t match_count = 0;

        size_t *match_ids = gc_match_resource(info, info->resources[i], &match_count);

        free(info->resources[i]);

        for(size_t j = 0; j < match_count; j++)
        {
            info->resources[j] = NULL;
        }

        free(match_ids);
    }

    free(info->resources);

    free(info);

    info = NULL;
}

size_t *gc_match_resource(struct GCInfo *info, void *res, size_t *count)
{
    if(info == NULL || count == NULL)
        return NULL;
    
    if(info->resources == NULL)
        return NULL;
    
    size_t *match_ids = calloc(1, sizeof(size_t));

    size_t num_allocated = 1;
    size_t num_written = 0;

    for(size_t i = 0; i < info->length; i++)
    {
        if(num_allocated == num_written)
        {
            match_ids = realloc(match_ids, (num_allocated+1)*sizeof(size_t));

            if(match_ids == NULL)
                return NULL;
        }

        if(res == info->resources[i])
        {
            match_ids[num_written] = i;
            num_written++;
        }
    }

    *count = num_written;

    return match_ids;
}

void gc_free_fp(void *fp)
{
    FILE *f = (FILE*)fp;

    fclose(f);
}