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



select id, freg, conc from (select stand.stand_id as id, caop.freguesia as freg, caop.concelho as conc from stand, caop, taxi_stands where id = taxi_stands.id and st_contains(caop.geom, taxi_stands.location)) as S;


select initial_ts, initial_point, final_point, count(*), sum(final_ts-initial_ts) from taxi_services where taxi_id = 37 group by 1,2,3 order by 4;


-- teste
select initial_ts, final_ts from taxi_services where taxi_id = 37 and date_part('hour', to_timestamp(initial_ts)) = 3 and date_part('day', to_timestamp(initial_ts)) = 2 and date_part('month', to_timestamp(initial_ts)) = 6;

-- query to select values to put in Services table
select date_part('hour', to_timestamp(initial_ts)) as hour,
			 date_part('day', to_timestamp(initial_ts)) as day,
			 date_part('month', to_timestamp(initial_ts)) as month,
 		 	 initial_point, final_point, count(*), sum(final_ts-initial_ts)
from taxi_services where taxi_id=37 group by 1,2,3,4,5 order by 6 DESC;

-- para ter a diferença de tempo em minutos
select EXTRACT(EPOCH FROM(to_timestamp(final_ts) - to_timestamp(initial_ts)))/60 as tempo
from taxi_services where taxi_id = 37 order by tempo;



select date_part('hour', to_timestamp(serv.initial_ts)) as hour,
 date_part('day', to_timestamp(serv.initial_ts)) as day,
 date_part('month', to_timestamp(serv.initial_ts)) as month, ca1.freguesia, ca2.freguesia, count(*),  sum(final_ts-initial_ts)
from taxi_services as serv, caop as ca1, caop as ca2 where taxi_id=37 and st_contains(ca1.geom, serv.initial_point) and st_contains(ca2.geom, serv.final_point) group by 1,2,3,4,5 order by 6 DESC;



-- with extrac(epoch())
select date_part('hour', to_timestamp(serv.initial_ts)) as hour,
 date_part('day', to_timestamp(serv.initial_ts)) as day,
 date_part('month', to_timestamp(serv.initial_ts)) as month,
 ca1.freguesia, ca2.freguesia, count(*),  sum(EXTRACT(EPOCH FROM(to_timestamp(serv.final_ts)-to_timestamp(serv.initial_ts)))/60)
 from taxi_services as serv, caop as ca1, caop as ca2
 where taxi_id=37 and st_contains(ca1.geom, serv.initial_point)
 				and st_contains(ca2.geom, serv.final_point) group by 1,2,3,4,5 order by 6 DESC;


select date_part('hour', to_timestamp(serv.initial_ts)) as hour,
 date_part('day', to_timestamp(serv.initial_ts)) as day,
 date_part('month', to_timestamp(serv.initial_ts)) as month,
 gay1.id, gay2.id, count(*),  sum(EXTRACT(EPOCH FROM(to_timestamp(serv.final_ts)-to_timestamp(serv.initial_ts)))/60)
 from taxi_services as serv, taxi_stands as gay1, taxi_stands as gay2
 where serv.taxi_id=37
 				group by 1,2,3,4,5 order by 6 DESC;


				and (select st_distance(gay1.location, serv1.initial_point) from taxi_services as serv1  LIMIT 1) in
														 (SELECT min(st_distance(serv11.initial_point, gay1.location)) from taxi_services as serv11 where serv11.taxi_id = 37)
													 and (select st_distance(gay2.location, serv2.final_point) from taxi_services as serv2  LIMIT 1) in
													 (SELECT min(st_distance(serv22.final_point, gay2.location)) from taxi_services as serv22 where serv22.taxi_id = 37)
