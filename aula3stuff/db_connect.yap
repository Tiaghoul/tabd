:- load_foreign_files(['db_connect'], [], init_my_predicates).

db_open(Host,User,Passwd,Database,ConnName):-
	db_connect(Host,User,Passwd,Database,ConnHandler),
	set_value(ConnName,ConnHandler).


db_close(ConnName) :-
	get_value(ConnName,ConnHandler),
	db_disconnect(ConnHandler).

db_import(Pred, Relation, Connection):-
	get_value(Connection, Handler),
	db_arity(Handler, Relation, Arity),
	functor(P, Pred, Arity),
	P =..[Pred|Args],
	atom.concat('select * from ', Relation, SQL),
	assert(':-'(P, ','(db_query(Handler,SQL,RS), db_row(RS,Args)))).
