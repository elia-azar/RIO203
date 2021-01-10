docker run --name postgres-docker -e POSTGRES_PASSWORD=postgres -p 5432:5432 \
-v postgres-volume:/var/lib/postgresql/data -d postgres

#docker exec -it postgres-docker bash
#psql -U postgres
CREATE TABLE sensors (
	time DATE UNIQUE NOT NULL,
    home_id INT NOT NULL,
    sensor_id INT NOT NULL,
    ipv6 VARCHAR(50),
    value real,
    state VARCHAR(50),
    Consumption real,
    PRIMARY KEY (home_id, sensor_id)
);

CREATE TABLE consumption (
    home_id INT PRIMARY KEY,
    Consumption real[168],
    position INT
);