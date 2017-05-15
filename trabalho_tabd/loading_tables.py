#!/usr/bin/python3

import psycopg2


def load_into_tempo():
    cur.execute("""select date_part('hour', timestamp) as h, date_part('day', timestamp) as d, date_part('month', timestamp) as m
                   from (select to_timestamp(initial_ts) as timestamp from taxi_services) as S
                   group by h,d,m;""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO tempo (hora, dia, mes) VALUES(%s, %s, %s);""", (row[0], row[1], row[2]))
        print(row)
    conn.commit()


def load_into_taxi():
    cur.execute("""select DISTINCT(taxi_id) as id from taxi_services order by id""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO taxi (n_licenca) VALUES(%s);""", [(row[0])])
        print(row[0])
    conn.commit()


def load_into_stand():
    cur.execute("""select name from taxi_stands;""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO stand (nome, lotacao) VALUES(%s, %s);""", (row[0], 1))
    conn.commit()


def load_into_local():
    cur.execute("""select stand_id, nome from stand;""")
    rows = cur.fetchall()
    for row in rows:
        stand_id = row[0]
        cur.execute("""select freguesia, concelho from caop, taxi_stands where taxi_stands.id = %s and st_contains(caop.geom, taxi_stands.location);""", [(stand_id)])
        rows2 = cur.fetchall()
        freg = rows2[0][0]
        conc = rows2[0][1]
        cur.execute("""INSERT INTO local(stand_id, freguesia, concelho) VALUES(%s,%s,%s);""", (stand_id, freg, conc))
        # print(str(stand_id) + " " + freg + " " + conc)
    conn.commit()


def load_into_services():
    # cur.execute("""select n_licenca from taxi;""")
    # taxis_ids = cur.fetchall()
    taxis_ids = [(37,)]
    for taxi in taxis_ids:
        taxi_id = taxi[0]
        print(taxi_id)
        cur.execute("""select date_part('hour', to_timestamp(initial_ts)) as hour,
                              date_part('day', to_timestamp(initial_ts)) as day,
                              date_part('month', to_timestamp(initial_ts)) as month,
                              initial_point, final_point, count(*), sum(fi)
                       from taxi_services where taxi_id = %s""", [(taxi_id)])
        all_rows = cur.fetchall()
        print(all_rows)


if __name__ == "__main__":
    try:
        conn = psycopg2.connect("dbname='trabalho' user='tiaghoul' host='localhost' password=''")
        cur = conn.cursor()

        # load_into_tempo()
        # load_into_taxi()
        # load_into_stand()
        # load_into_local()
        load_into_services()

        cur.close()
        conn.close()
    except ConnectionError:
        print("I am unable to connect to the database")
