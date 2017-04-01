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

	if(argc ==2){
		printf("arg: %s\n", argv[1]);
	}
	else{
		printf("Only one result set expected\n");
		return 0;
	}

	conn = mysql_init(&mysql);
	if (!mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0))
	{
	    fprintf(stderr, "Failed to connect to database: Error: %s\n",
	          mysql_error(&mysql));
	}

	else{
		const char * information = mysql_info(&conn);
        printf("Success: %s\n",information);
	}
	char query[80] = "SELECT * FROM ";
	strcat(query, argv[1]);

	mysql_query(conn, query); // issue the query for execution

	res_set = mysql_store_result(conn); // generate the result set
	num_fields = mysql_num_fields(res_set);
	int array[100];
	printf("num_fields =  %d\n", num_fields);

	printf("|" );

	while((field = mysql_fetch_field(res_set)) != NULL){
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

	while ((row = mysql_fetch_row(res_set)) != NULL) {
		printf("|" );
		for(int j=0; j < num_fields; j++){
			field = mysql_fetch_field_direct(res_set, j);
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
	for(int k=0; k < num_fields; k++){
		printf("%.*s", array[k]+2, "------------------------------------------");
		printf("+");
	}
	printf("\n");

	mysql_free_result(res_set);
	mysql_close(conn);

	return 0;
}
