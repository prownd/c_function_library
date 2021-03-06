/**
 * @file ssort.h
 * @time 2015-6-21
 * @author hanjinpeng127@163.com
 * @version 1.0
 */
 
#ifndef _SSORT_H_
#define _SSORT_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//bubble sort
void bubble_sort(void* data,int len,int size,int(*compare)(const void*,const void*));

//insert sort
void insert_sort(void* data,int len,int size, int(*compare)(const void*,const void*));

//select sort
void select_sort(void * data,int len,int size,int(*compare)(const void*,const void*));

//quick sort partition
void quick_sort_partition(void * base,size_t left,size_t right,size_t size,int(* compare)(const void *,const void *));

//quick sort
void quick_sort(void* base,size_t nmemb,size_t size ,int(*compare)(const void *,const void*));

#endif //_SSORT_H_
