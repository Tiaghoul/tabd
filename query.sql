-- get all the time variations (hour, day month)
insert into tempo(hora,dia,mes)
	select date_part('hour', timestamp) as h, date_part('day', timestamp) as d, date_part('month', timestamp) as m
	from
		(select to_timestamp(initial_ts) as timestamp from taxi_services) as S
	group by h,d,m;

-- get all the taxis (id = n_licenca)
insert into taxi(n_licenca)
	select distinct(taxi_id) as id from taxi_services order by id;

-- get all the stands (id and name)
insert into stand(stand_id, nome, lotacao)
	select taxi_stands.id, taxi_stands.name, 1 from taxi_stands;


maybe???
select id, freg, conc from (select stand.stand_id as id, caop.freguesia as freg, caop.concelho as conc from stand, caop, taxi_stands where id = taxi_stands.id and st_contains(caop.geom, taxi_stands.location)) as S;


-- select stand.stand_id as s, taxi_stands.id as ts from caop, stand, taxi_stands where stand.stand_id = taxi_stands.id and ;
