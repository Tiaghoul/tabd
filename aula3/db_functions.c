#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int c_db_descri(void){
	YAP_Term relation = YAP_ARG1;
	YAP_Term handler = YAP_ARG2;

	unsigned int maximo;
	int aux = 0;
	int array[100];

	MYSQL_FIELD *field;
	MYSQL_ROW row;
	MYSQL *conn = (MYSQL *) YAP_IntOfTerm(handler);
	char *query = (char *) YAP_AtomName(YAP_AtomOfTerm(relation));
	// printf("query = %s\n", query);

	mysql_query(conn, query);
	MYSQL_RES *result_set = mysql_store_result(conn);
	int num_fields = mysql_num_fields(result_set);


	printf("|" );
	while((field = mysql_fetch_field(result_set)) != NULL){
		if(strlen(field->name) < field->max_length){
			maximo = field->max_length;
		}
		else{
			maximo = strlen(field->name);
		}
		array[aux] = maximo;
		aux++;
		printf(" %-*s |", maximo, field->name);
	}

	printf("\n+");
	for(int k=0; k< num_fields; k++){
		printf("%.*s", array[k]+2, "------------------------------------------");
		printf("+");
	}
	printf("\n");

	while ((row = mysql_fetch_row(result_set)) != NULL) {
		printf("|" );
		for(int j=0; j< num_fields; j++){
			field = mysql_fetch_field_direct(result_set, j);
			if (IS_NUM(field->type)){
				printf(" %*s |", array[j], row[j]);
			}
			else{
				printf(" %-*s |", array[j], row[j]);
			}
		}
		printf("\n");
	}

	printf("+");
	for(int k=0; k< num_fields; k++){
		printf("%.*s", array[k]+2, "------------------------------------------");
		printf("+");
	}
	printf("\n");

	mysql_free_result(result_set);
	return TRUE;

}

int c_db_row(void){
	YAP_Term rs = YAP_ARG1;
	YAP_Term args = YAP_ARG2;
	YAP_Term head;
	YAP_Term to_unify;

	MYSQL_ROW row;
	MYSQL_RES *result_set = (MYSQL_RES *) YAP_IntOfTerm(rs);
	int arity = mysql_num_fields(result_set);

	if((row = mysql_fetch_row(result_set)) != NULL){
		for(int i = 0; i< arity; i++){
			MYSQL_FIELD *field = mysql_fetch_field_direct(result_set, i);

			if(field->type == FIELD_TYPE_LONG){
				to_unify = YAP_MkIntTerm(atoi(row[i]));
			}
			else if(field->type == FIELD_TYPE_FLOAT){
				to_unify = YAP_MkFloatTerm(atof(row[i]));
			}
			else{
				to_unify = YAP_MkAtomTerm(YAP_LookupAtom(row[i]));
			}

			head = YAP_HeadOfTerm(args);
			args = YAP_TailOfTerm(args);

			if(!YAP_Unify(head, to_unify)){
				return FALSE;
			}
		}
		return TRUE;
	}

	mysql_free_result(result_set);
	YAP_cut_fail();
	return FALSE;
}


int c_db_query(void){
	YAP_Term handler = YAP_ARG1;
	YAP_Term sql = YAP_ARG2;
	YAP_Term rs = YAP_ARG3;

	MYSQL *conn = (MYSQL *) YAP_IntOfTerm(handler);
	char *query = (char *) YAP_AtomName(YAP_AtomOfTerm(sql));
	mysql_query(conn, query);
	MYSQL_RES *res_set = mysql_store_result(conn);
	if(!YAP_Unify(rs, YAP_MkIntTerm((int) res_set))){
		return FALSE;
	}
	return TRUE;
}


int c_db_arity(void){
	YAP_Term handler = YAP_ARG1;
	YAP_Term relation = YAP_ARG2;
	YAP_Term arity = YAP_ARG3;

	MYSQL *conn = (MYSQL *) YAP_IntOfTerm(handler);
	char query[80] = "SELECT * FROM ";
	strcat(query, YAP_AtomName(YAP_AtomOfTerm(relation)));
	strcat(query, " limit 0");
	mysql_query(conn, query);
	MYSQL_RES *res_set = mysql_store_result(conn);
	int num_fields = mysql_num_fields(res_set);
	if (!YAP_Unify(arity, YAP_MkIntTerm((int) num_fields))){
		return FALSE;
	}
	printf("arity = %d\n", num_fields);
	return TRUE;

}



int c_db_disconnect(void){
	YAP_Term arg_conn = YAP_ARG1;
	MYSQL *conn = (MYSQL *) YAP_IntOfTerm(arg_conn);

	mysql_close(conn);
	return TRUE;
}


int c_db_connect(void) {
	YAP_Term arg_host = YAP_ARG1;
	YAP_Term arg_user = YAP_ARG2;
	YAP_Term arg_passwd = YAP_ARG3;
	YAP_Term arg_database = YAP_ARG4;
	YAP_Term arg_conn = YAP_ARG5;
	MYSQL *conn;
	char *host = (char *) YAP_AtomName(YAP_AtomOfTerm(arg_host));
	char *user = (char *) YAP_AtomName(YAP_AtomOfTerm(arg_user));
	char *passwd = (char *) YAP_AtomName(YAP_AtomOfTerm(arg_passwd));


	char *database = (char *) YAP_AtomName(YAP_AtomOfTerm(arg_database));
	conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("erro no init\n");
		return FALSE;
	}

	if (mysql_real_connect(conn, host, user, passwd, database, 0, NULL, 0) == NULL) {
		printf("erro no connect\n");
		return FALSE;
	}

	if (!YAP_Unify(arg_conn, YAP_MkIntTerm((int) conn)))
		return FALSE;

	return TRUE;

}

void init_my_predicates(){
     YAP_UserCPredicate("db_connect",c_db_connect,5);
	 YAP_UserCPredicate("db_disconnect",c_db_disconnect,1);
	 YAP_UserCPredicate("db_arity",c_db_arity,3);
	 YAP_UserCPredicate("db_query",c_db_query,3);
	 YAP_UserCPredicate("db_descri",c_db_descri,2);
	 YAP_UserBackCPredicate("db_row", c_db_row, c_db_row, 2, 0);
}
