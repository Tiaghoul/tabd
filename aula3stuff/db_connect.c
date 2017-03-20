#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>


int c_db_row(void){
	YAP_Term rs = YAP_ARG1;
	YAP_Term args = YAP_ARG2;

	MYSQL_ROW row;
	MYSQL_RES *resul_set = (MYSQL_RES *) YAP_IntOfTerm(rs);

	if(row = mysql_fetch_row(resul_set) != NULL){
		
	}

}


int c_db_query(void){
	YAP_Term handler = YAP_ARG1;
	YAP_Term sql = YAP_ARG2;
	YAP_Term rs = YAP_ARG3;

	MYSQL *conn = (MYSQL *) YAP_IntOfTerm(handler);
	char *query = YAP_AtomName(YAP_AtomOfTerm(sql));
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
	strcat(query, " limit 1");
	mysql_query(conn, query);
	MYSQL_RES *res_set = mysql_store_result(conn);
	int num_fields = mysql_num_fields(res_set);
	if (!YAP_Unify(arity, YAP_MkIntTerm((int) num_fields))){
		return FALSE;
	}

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
	char *host = YAP_AtomName(YAP_AtomOfTerm(arg_host));
	char *user = YAP_AtomName(YAP_AtomOfTerm(arg_user));
	char *passwd = YAP_AtomName(YAP_AtomOfTerm(arg_passwd));

	char *database = YAP_AtomName(YAP_AtomOfTerm(arg_database));
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
	 YAP_UserBackCPredicate("db_row", c_db_row, c_db_row, 2, 0);
}
