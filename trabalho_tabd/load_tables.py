#!/usr/bin/python3

import psycopg2


def dados_tempo():
    print("DADOS TEMPO")
    cur.execute("""select date_part('hour', timestamp) as h,
                          date_part('day', timestamp) as d,
                          date_part('month', timestamp) as m
                   from (select to_timestamp(initial_ts) as timestamp from taxi_services) as S
                   group by h,d,m;""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO tempo (hora, dia, mes)
                       VALUES(%s, %s, %s);""", (row[0], row[1], row[2]))


def dados_taxi():
    print("DADOS TAXI")
    cur.execute("""select DISTINCT(taxi_id) as id from taxi_services order by id""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO taxi (n_licenca) VALUES(%s);""", [(row[0])])


def dados_stand():
    print("DADOS STAND")
    cur.execute("""select name from taxi_stands;""")
    rows = cur.fetchall()
    for row in rows:
        cur.execute("""INSERT INTO stand (nome, lotacao) VALUES(%s, %s);""", (row[0], 1))


def dados_services():
    print("DADOS SERVICES")
    cur.execute("""select * from taxi;""")
    taxis = cur.fetchall()
    for taxi in taxis:
        taxi_id, n_licenca = taxi[0], taxi[1]
        print(taxi_id)
        cur.execute("""select date_part('hour', to_timestamp(initial_ts)) as hour,
                              date_part('day', to_timestamp(initial_ts)) as day,
                              date_part('month', to_timestamp(initial_ts)) as month,
                              initial_local_id,
                              final_local_id,
                              count(*),
                              sum(EXTRACT(EPOCH FROM(to_timestamp(serv.final_ts)
                                  - to_timestamp(serv.initial_ts)))/60)
                       from taxi_services as serv
                       where taxi_id = %s
                       group by  1,2,3,4,5;""", [n_licenca])

        all_rows = cur.fetchall()
        for row in all_rows:
            hora, dia, mes, local_inicio_id, local_fim_id = row[0], row[1], row[2], row[3], row[4]
            n_viagens, tempo_total = row[5], row[6]
            cur.execute("""select tempo_id from tempo where hora=%s and dia=%s and mes=%s;""", (hora, dia, mes))
            fetch_tempo = cur.fetchall()
            tempo_id = fetch_tempo[0][0]
            cur.execute("""insert into services(taxi_id, tempo_inicio_id, local_inicio_id, local_fim_id, tempo_total, n_viagens)
                                   VALUES(%s, %s, %s, %s, %s, %s);""",
                        (taxi_id, tempo_id, local_inicio_id, local_fim_id, tempo_total, n_viagens))



def change_dados_taxi_services():
    print("CHANGE TAXI_SERVICES")

    cur.execute("SELECT * FROM taxi_services;")
    all_services = cur.fetchall()

    for entry in all_services:

        cur.execute("SELECT cp.freguesia, cp.concelho FROM caop AS cp WHERE st_contains(cp.geom, %s)", [entry[4]])
        initial_point_local = cur.fetchall()

        cur.execute("SELECT cp.freguesia, cp.concelho FROM caop AS cp WHERE st_contains(cp.geom, %s)", [entry[5]])
        final_point_local = cur.fetchall()

        if len(initial_point_local) == 0 or len(final_point_local) == 0:
            continue

        # ----- CREATE LOCAL FOR INITIAL POINT ----- #

        cur.execute("""SELECT st_distance_sphere(stand.location,%s), stand.id FROM taxi_stands stand ORDER BY 1 ASC LIMIT 1""", [entry[4]])


        initial_point_min_distance = cur.fetchall()

        # IF the distance to the closest stand is less than 150 meters, then the Local of the service will have a stand associated with it
        if initial_point_min_distance[0][0] < 150:
            cur.execute("""SELECT local_id FROM local WHERE freguesia=%s AND concelho=%s AND stand_id=%s""",
                        (initial_point_local[0][0], initial_point_local[0][1], initial_point_min_distance[0][1]))
            find_local_id = cur.fetchall()

            # Creates Local entry if it is not present
            if len(find_local_id) == 0:
                cur.execute("INSERT INTO local(freguesia,concelho,stand_id) VALUES(%s,%s,%s)",
                            (initial_point_local[0][0], initial_point_local[0][1], initial_point_min_distance[0][1]))

                conn.commit()
                cur.execute("SELECT MAX(local_id) FROM local")
                res = cur.fetchall()
                local_id_inicio = res[0]
            else:
                local_id_inicio = find_local_id[0][0]
        # ELSE create associate a Local without stand to the service
        else:
            cur.execute("""SELECT local_id FROM local WHERE freguesia=%s AND concelho=%s AND stand_id is null""",
                        (initial_point_local[0][0], initial_point_local[0][1]))

            find_local_id = cur.fetchall()

            # Creates Local entry if it is not present
            if len(find_local_id) == 0:
                cur.execute("INSERT INTO local(freguesia,concelho) VALUES(%s,%s)",
                            (initial_point_local[0][0], initial_point_local[0][1]))

                conn.commit()

                cur.execute("SELECT MAX(local_id) FROM local")
                res = cur.fetchall()
                local_id_inicio = res[0]
            else:
                local_id_inicio = find_local_id[0][0]

        # ----- CREATE LOCAL FOR FINAL POINT ----- #

        cur.execute("""SELECT st_distance_sphere(stand.location,%s), stand.id FROM taxi_stands stand ORDER BY 1 ASC LIMIT 1""", [entry[5]])

        final_point_min_distance = cur.fetchall()

        if final_point_min_distance[0][0] < 150:
            cur.execute("""SELECT local_id FROM local WHERE freguesia=%s AND concelho=%s AND stand_id=%s""",
                        (final_point_local[0][0], final_point_local[0][1], final_point_min_distance[0][1]))
            find_local_id = cur.fetchall()

            if len(find_local_id) == 0:
                cur.execute("INSERT INTO local(freguesia,concelho,stand_id) VALUES(%s,%s,%s)",
                            (final_point_local[0][0], final_point_local[0][1], final_point_min_distance[0][1]))
                conn.commit()
                cur.execute("SELECT MAX(local_id) FROM local")
                res = cur.fetchall()
                local_id_fim = res[0]
            else:
                local_id_fim = find_local_id[0][0]
        else:
            cur.execute("""SELECT local_id FROM local WHERE freguesia=%s AND concelho=%s AND stand_id is null""",
                        (final_point_local[0][0], final_point_local[0][1]))
            find_local_id = cur.fetchall()

            if len(find_local_id) == 0:
                cur.execute("INSERT INTO local(freguesia,concelho) VALUES(%s,%s)",
                            (final_point_local[0][0], final_point_local[0][1]))
                conn.commit()

                cur.execute("SELECT MAX(local_id) FROM local")
                res = cur.fetchall()
                local_id_fim = res[0]
            else:
                local_id_fim = find_local_id[0][0]

        # ----- UPDATE the taxi_services entry with the new info ----- #

        cur.execute("""UPDATE taxi_services
                        SET initial_local_id = %s , final_local_id = %s
                            WHERE id = %s;""", (local_id_inicio, local_id_fim, entry[0]))

        # --------------------- END ------------------------ #

    conn.commit()

    cur.execute("""DELETE FROM taxi_services WHERE initial_local_id is null or final_local_id is null""")

    conn.commit()


if __name__ == "__main__":
    try:
        conn = psycopg2.connect("dbname='tabdfun' user='tiaghoul' host='localhost' password=''")
        conn.autocommit = True
        cur = conn.cursor()

        change_dados_taxi_services()
        dados_tempo()
        dados_taxi()
        dados_stand()
        dados_services()

        cur.close()
        conn.close()

    except ConnectionError:
        print("I am unable to connect to the database")
