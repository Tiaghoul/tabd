:- load_foreign_files(['db_functions'], [], init_my_predicates).

db_open(Host, User, Passwd, Database, ConnName):-
	db_connect(Host,User,Passwd,Database,ConnHandler),
	set_value(ConnName,ConnHandler).


db_close(ConnName) :-
	get_value(ConnName,ConnHandler),
	db_disconnect(ConnHandler).


db_import(Pred, Relation, Connection):-
	get_value(Connection, Handler),
	db_arity(Handler, Relation, Arity),
	functor(P, Pred, Arity),
	P =.. [Pred|Args],
	atom_concat('select * from ', Relation, SQL),
	write(SQL),
	assertz(':-'(P, ','(db_query(Handler,SQL,RS), db_row(RS,Args)))).


db_describe(Relation, Connection):-
	get_value(Connection, Handler),
	atom_concat('describe ', Relation, SQL),
	db_descri(SQL, Handler).


db_sql_select(SQL, Connection, L):-
	get_value(Connection, Handler),
	db_query(Handler, SQL, RS),
	db_row(RS, L).
