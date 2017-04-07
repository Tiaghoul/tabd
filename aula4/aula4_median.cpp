// MEDIAN function
#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <algorithm>
#include <vector>
#include <iostream>

typedef struct aggr_prot {
	char *name;
	void (*init)(void **handle);
	void (*iter)(void *handle, char *value);
	void (*final)(void *handle, void *result);
	void *result;
} aggr_prot_type;

struct median {
	std::vector<double> all_values;
};

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a);
void median_init(void **handle);
void median_iter(void *handle, char *value);
void median_final(void *handle, void *result);

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a){
	MYSQL_ROW row;
	void *handle;
	(*a->init)(&handle);
	while ((row = mysql_fetch_row(res_set))!=NULL){
		(*a->iter)(handle,row[0]);
	}
	(*a->final)(handle,a->result);
}

void median_init(void **handle){
	struct median *local_handle =  new struct median;
 	*handle = (void *) local_handle;
}


void median_iter(void *handle, char *value){
	struct median *local_handle = (struct median *) handle;
	double new_value = atof(value);
	local_handle->all_values.push_back(new_value);
}

void median_final(void *handle,void *result){
	float *local_result;
	local_result = (float *) result;
	struct median *local_handle = (struct median *) handle;
	int size = local_handle->all_values.size();

	std::sort(local_handle->all_values.begin(), local_handle->all_values.end());

	if(size % 2 != 0){
		*local_result = local_handle->all_values[size/2];
	}
	else{
		*local_result = (local_handle->all_values[size/2] + local_handle->all_values[(size/2)-1]) / 2;
	}
	free(handle);
}

int main(){
	aggr_prot_type a;
	a.name = (char *)"median";
	a.init = &median_init;
	a.iter = &median_iter;
	a.final = &median_final;
	a.result= malloc(sizeof(float));
	MYSQL mysql;
	MYSQL_RES *res_set;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0);
	mysql_query(&mysql, "SELECT length FROM film limit 10");
	res_set = mysql_store_result(&mysql);
	aggr_function(res_set,&a);
	printf("%s: %f\n",a.name,*((float *) a.result));
	free(a.result);
	mysql_free_result(res_set);
}
