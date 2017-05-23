#!/usr/bin/python3

import psycopg2

if __name__ == "__main__":
    try:
        conn = psycopg2.connect("dbname='tabdfun' user='tiaghoul' host='localhost' password=''")
        conn.autocommit = True
        cur = conn.cursor()

        cur.execute("""DELETE FROM taxi_services
                       WHERE EXTRACT(EPOCH FROM(to_timestamp(final_ts)-to_timestamp(initial_ts))) < 60""")

        cur.execute("""DELETE FROM taxi_services WHERE  st_distancesphere(initial_point, final_point) < 200""")

        cur.close()
        conn.close()

    except ConnectionError:
        print("I am unable to connect to the database")