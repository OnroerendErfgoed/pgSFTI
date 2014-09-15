DROP TABLE IF EXISTS test_sfti;

DROP EXTENSION IF EXISTS pgsfti;

CREATE EXTENSION pgsfti;

CREATE TABLE test_sfti (
	fti	sfti
);

\i testdata_1.sql
