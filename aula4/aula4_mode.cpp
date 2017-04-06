// AVG function
#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <iostream>
#include <map>

typedef struct aggr_prot {
	char *name;
	void (*init)(void **handle);
	void (*iter)(void *handle, char *value);
	void (*final)(void *handle, void *result);
	void *result;
} aggr_prot_type;

struct mode {
	std::map<double, int> ocurrences;
	int highest_ocurrences_number;
	double value_with_most_ocurrences;
};

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a);
void mode_init(void **handle);
void mode_iter(void *handle, char *value);
void mode_final(void *handle, void *result);

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a){
	MYSQL_ROW row;
	void *handle;
	(*a->init)(&handle);
	while ((row = mysql_fetch_row(res_set))!=NULL){
		(*a->iter)(handle,row[0]);
	}
	(*a->final)(handle,a->result);
}

void mode_init(void **handle){
	struct mode *local_handle =  new struct mode;
	local_handle->highest_ocurrences_number=0;
	local_handle->value_with_most_ocurrences=0.0;
 	*handle = (void *) local_handle;
}


void mode_iter(void *handle, char *value){
	struct mode *local_handle = (struct mode *) handle;

	double new_value = atof(value);

	std::map<double,int>::iterator it = local_handle->ocurrences.find(new_value);
	if(it != local_handle->ocurrences.end()){
		int counter = it->second + 1;
		it->second = counter;
		if(counter > local_handle->highest_ocurrences_number){
			local_handle->highest_ocurrences_number = counter;
			local_handle->value_with_most_ocurrences = new_value;
		}
	}
	else{
		local_handle->ocurrences.insert(std::pair<double, int>(new_value, 1));
	}

}

void mode_final(void *handle,void *result){
	double *local_result;
	struct mode *local_handle = (struct mode *) handle;
	local_result = (double *) result;

	*local_result = local_handle->value_with_most_ocurrences;
	int deeznuts = local_handle->highest_ocurrences_number;
	printf("Mode -> %f  | Valor -> %d\n", *local_result, deeznuts);
	free(handle);
}

int main(){
	aggr_prot_type a;
	a.name = (char *)"mode";
	a.init = &mode_init;
	a.iter = &mode_iter;
	a.final = &mode_final;
	a.result= malloc(sizeof(double));

	MYSQL mysql;
	MYSQL_RES *res_set;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0);
	// mysql_query(&mysql, "SELECT length FROM film limit 30");
	mysql_query(&mysql, "select store_id from customer order by customer_id ASC limit 10");
	res_set = mysql_store_result(&mysql);
	aggr_function(res_set,&a);
	printf("%s: %f\n",a.name,*((double *) a.result));
	free(a.result);
	mysql_free_result(res_set);
}
