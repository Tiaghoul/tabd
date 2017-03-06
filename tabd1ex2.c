#include <mysql.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char const *argv[]) {
	int num_fields;
	MYSQL mysql;
	MYSQL *conn;
	MYSQL_RES *res_set;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	unsigned int maximo;
	int aux = 0;

	conn = mysql_init(&mysql);
	if (!mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0)){
	    fprintf(stderr, "Failed to connect to database: Error: %s\n",
	          mysql_error(&mysql));
	}

	else{
		const char * information = mysql_info(conn);
		printf("Success: %s\n",information);
	}

	char query[80] = "SHOW TABLES";
	mysql_query(conn, query);

	res_set = mysql_store_result(conn);
	num_fields = mysql_num_rows(res_set);

	printf("---> %d\n", num_fields);
	while ((row = mysql_fetch_row(res_set)) != NULL) {
		printf("row = %s\n", row[0]);
		char query2[80] =  "DELETE FROM";

	}



	mysql_free_result(res_set);
	mysql_close(conn);


	return 0;
}
