MODULES = ./module/pgsfti
EXTENSION = pgsfti
DATA = ./data/pgsfti--1.0.sql
DOCS = README.md

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
