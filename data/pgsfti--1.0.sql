CREATE TYPE sfti;
	
CREATE OR REPLACE FUNCTION  sfti_in(cstring)
    RETURNS sfti
    AS 'MODULE_PATHNAME', 'sfti_in'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION sfti_out(sfti)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'sfti_out'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION sfti_recv(internal)
   RETURNS sfti
   AS 'MODULE_PATHNAME', 'sfti_recv'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION sfti_send(sfti)
   RETURNS bytea
   AS 'MODULE_PATHNAME', 'sfti_send'
   LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE sfti (
   internallength = 40, 
   input = sfti_in,
   output = sfti_out,
   receive = sfti_recv,
   send = sfti_send,
   alignment = double
);



CREATE OR REPLACE FUNCTION allen_before(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_before'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_meets(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_meets'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_overlaps(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_overlaps'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_during(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_starts(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_starts'
	LANGUAGE C IMMUTABLE STRICT;	

CREATE OR REPLACE FUNCTION allen_finishes(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_finishes'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_equals(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_equals'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_after(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_after'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_overlapped_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_overlapped_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_contains(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_met_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_met_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_started_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_started_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_finished_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_allen_finished_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_before(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_kvd_before'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_after(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_kvd_after'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_during(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_kvd_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_contains(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_kvd_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_intersects(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'pg_kvd_intersects'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pg_strict_less(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_strict_less'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR << (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_strict_less
);

CREATE OR REPLACE FUNCTION pg_less(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_less'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_less
);

CREATE OR REPLACE FUNCTION pg_strict_greater(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_strict_greater'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR >> (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_strict_greater
);

CREATE OR REPLACE FUNCTION pg_greater(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_greater'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_greater
);

CREATE OR REPLACE FUNCTION pg_equal(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_equal'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_equal
);

CREATE OR REPLACE FUNCTION pg_intersects(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_intersects'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR && (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_intersects
);

CREATE OR REPLACE FUNCTION pg_during(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR @ (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_during
);

CREATE OR REPLACE FUNCTION pg_contains(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'pg_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR ~ (
    leftarg = sfti,
    rightarg = sfti,
    procedure = pg_contains
);

CREATE OR REPLACE FUNCTION sfti_makeX(datum date) RETURNS float AS $$
DECLARE
	year int;
	last_day_of_year date;
	nr_of_days_in_year int;
BEGIN
	year := EXTRACT (year FROM $1);
	IF year < 0 THEN
		last_day_of_year = (lpad(abs(year)::text,4,'0') || '-12-31 BC')::date;
		year := year + 1;
	ELSE
		last_day_of_year = (lpad(abs(year)::text,4,'0') || '-12-31 AD')::date;
	END IF;
	nr_of_days_in_year := EXTRACT(doy FROM (last_day_of_year));
	RETURN year + ((EXTRACT(doy FROM $1) -1) / nr_of_days_in_year);
END
$$ LANGUAGE plpgsql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(sa date, ka date, kb date, sb date, l float) RETURNS sfti AS $$
	SELECT format('(%s,%s,%s,%s,%s)',sfti_makeX($1),sfti_makeX($2),sfti_makeX($3),sfti_makeX($4),$5)::sfti;
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(sa date, ka date, kb date, sb date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$2,$3,$4,1);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(ka date, kb date, l float) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$2,$2,$3);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(ka date, kb date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$2,$2,1);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(d date, l float) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$1,$1,$2);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(d date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$1,$1,1);
$$ LANGUAGE sql IMMUTABLE;