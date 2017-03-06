#include <mysql.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	MYSQL mysql;
	MYSQL_ROW rowMovie;
	MYSQL_ROW rowActor;

	MYSQL *conn = mysql_init(&mysql);
	if (!mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,CLIENT_MULTI_RESULTS)){
		fprintf(stderr, "Failed to connect to database: Error: %s\n",
			  mysql_error(&mysql));
	}

	else{
		const char * information = mysql_info(conn);
		printf("Success: %s\n",information);
	}

	char firstQuery[100] = "SELECT title, film_id FROM film";
	char loopQuery[1000] = "SELECT actor.actor_id, first_name, last_name, COUNT(*) as C FROM film JOIN actor JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id WHERE actor.actor_id IN (SELECT  actor.actor_id FROM film JOIN actor  JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id WHERE film.film_id = ";
	char auxQuery[100] = " )GROUP BY actor.actor_id ORDER BY C desc LIMIT 1";

	mysql_query(conn, firstQuery);
	MYSQL_RES *res_set = mysql_store_result(conn);

	int num_fields = mysql_num_fields(res_set);
	int num_rows = mysql_num_rows(res_set);
	printf("num fields = %d\n", num_fields);
	printf("num rows = %d\n", num_rows);

	while((rowMovie = mysql_fetch_row(res_set)) != NULL) {
		char *id = rowMovie[1];

		char finalQuery[2000] =  "";
		strcat(finalQuery, loopQuery);
		strcat(finalQuery, id);
		strcat(finalQuery, auxQuery);
		mysql_query(conn, finalQuery);
		MYSQL_RES *res_set2 = mysql_store_result(conn);
		while((rowActor = mysql_fetch_row(res_set2)) != NULL){
			printf("Id: %-4s Film: %-30s Actor with more movies: %s ", id, rowMovie[0], rowActor[1]);
			printf("\n");
		}
		// printf("Film: %s, Actor with more movies: %s ", rowMovie[0], rowActor[1]);
		// printf("\n");
	}


	mysql_free_result(res_set);
	mysql_close(conn);


	return 0;

}
