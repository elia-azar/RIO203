import psycopg2

# Connect to postgres DB
conn = psycopg2.connect("dbname='postgres' user='postgres' host='127.0.0.1' password='postgres' port='5432'")

# Open a cursor to perform database operations
cur = conn.cursor()

cur.execute("SELECT * FROM sensors;")
print(cur.fetchall())