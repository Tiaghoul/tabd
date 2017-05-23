CREATE TABLE tempo (
  tempo_id serial,
  hora INTEGER NOT NULL CHECK(hora>=0 and hora<24),
  dia INTEGER NOT NULL CHECK(dia>=1 and dia<=31),
  mes INTEGER NOT NULL CHECK(mes>=1 and mes<=12),
  PRIMARY KEY (tempo_id)
);

CREATE TABLE taxi (
  taxi_id serial,
  n_licenca INTEGER NOT NULL CHECK(n_licenca>0),
  PRIMARY KEY (taxi_id)
);

CREATE TABLE stand (
  stand_id serial,
  nome varchar(100) NOT NULL,
  lotacao INTEGER NOT NULL CHECK(lotacao>=0),
  PRIMARY KEY (stand_id)
);

CREATE TABLE local (
  local_id serial,
  stand_id INTEGER CHECK(stand_id>0),
  freguesia varchar(100) NOT NULL,
  concelho varchar(100) NOT NULL,
  PRIMARY KEY(local_id),
  FOREIGN KEY(stand_id) REFERENCES stand(stand_id)
);

CREATE TABLE services (
  taxi_id INTEGER NOT NULL CHECK(taxi_id>0),
  tempo_inicio_id INTEGER NOT NULL CHECK(tempo_inicio_id>0),
  local_inicio_id INTEGER NOT NULL CHECK(local_inicio_id>0),
  local_fim_id INTEGER NOT NULL CHECK(local_fim_id>0),
  tempo_total INTEGER NOT NULL CHECK(tempo_total>=0),
  n_viagens INTEGER NOT NULL CHECK(n_viagens>=0),
  FOREIGN KEY(taxi_id) REFERENCES taxi(taxi_id),
  FOREIGN KEY(tempo_inicio_id) REFERENCES tempo(tempo_id),
  FOREIGN KEY(local_fim_id) REFERENCES local(local_id),
  FOREIGN KEY(local_inicio_id) REFERENCES local(local_id)
);
