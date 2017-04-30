CREATE TABLE tempo (
  tempo_id INTEGER NOT NULL CHECK(tempo_id>0),
  hora INTEGER NOT NULL CHECK(hora>=0 and hora<24),
  dia INTEGER NOT NULL CHECK(dia>=1 and dia<=31),
  mes INTEGER NOT NULL CHECK(mes>=1 and mes<=12),
  PRIMARY KEY (tempo_id)
);

CREATE TABLE taxi (
  taxi_id INTEGER NOT NULL CHECK(taxi_id>0),
  n_licenca INTEGER NOT NULL CHECK(n_licenca>0),
  PRIMARY KEY (taxi_id)
);

CREATE TABLE stand (
  stand_id INTEGER NOT NULL CHECK(stand_id>0),
  nome varchar(100) NOT NULL,
  lotacao INTEGER NOT NULL CHECK(lotacao>=0),
  PRIMARY KEY (stand_id)
);

CREATE TABLE local (
  local_id INTEGER NOT NULL CHECK(local_id>0),
  stand_id INTEGER NOT NULL CHECK(stand_id>0),
  freguesia varchar(30) NOT NULL,
  concelho varchar(30) NOT NULL,
  PRIMARY KEY(local_id),
  FOREIGN KEY(stand_id) REFERENCES stand(stand_id)

);

CREATE TABLE services (
  taxi_id INTEGER NOT NULL CHECK(taxi_id>0),
  tempo_id INTEGER NOT NULL CHECK(tempo_id>0),
  n_viagens INTEGER NOT NULL CHECK(n_viagens>=0),
  tempo_total INTEGER NOT NULL CHECK(tempo_total>0),
  local_id_fim INTEGER NOT NULL CHECK(local_id_fim>0),
  local_id_inicio INTEGER NOT NULL CHECK(local_id_inicio>0),
  FOREIGN KEY(taxi_id) REFERENCES taxi(taxi_id),
  FOREIGN KEY(tempo_id) REFERENCES tempo(tempo_id),
  FOREIGN KEY(local_id_fim) REFERENCES local(local_id),
  FOREIGN KEY(local_id_inicio) REFERENCES local(local_id)
);
