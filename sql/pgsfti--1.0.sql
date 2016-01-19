-- Create a Simple Fuzzy Time Interval Type

CREATE TYPE sfti;

--
-- Type IO
--

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


--
-- Allen relations
--

CREATE OR REPLACE FUNCTION allen_before(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_before'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_meets(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_meets'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_overlaps(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_overlaps'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_during(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_starts(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_starts'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_finishes(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_finishes'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_equals(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_equals'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_after(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_after'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_overlapped_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_overlapped_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_contains(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_met_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_met_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_started_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_started_by'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION allen_finished_by(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_allen_finished_by'
	LANGUAGE C IMMUTABLE STRICT;

--
-- Extended Allen relations
--

CREATE OR REPLACE FUNCTION kvd_before(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_kvd_before'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_after(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_kvd_after'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_during(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_kvd_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_contains(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_kvd_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION kvd_intersects(sfti, sfti, double precision, double precision)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'sfti_kvd_intersects'
	LANGUAGE C IMMUTABLE STRICT;

--
-- Operators
--

CREATE OR REPLACE FUNCTION sfti_lt(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_lt'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_lt,
    commutator = >
);

CREATE OR REPLACE FUNCTION sfti_gt(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_gt'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_gt,
    commutator = <
);

CREATE OR REPLACE FUNCTION sfti_eq(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_eq'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_eq,
    commutator = =
);

CREATE OR REPLACE FUNCTION sfti_intersects(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_intersects'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR && (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_intersects,
    commutator = &&
);

CREATE OR REPLACE FUNCTION sfti_during(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_during'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR @ (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_during
);

CREATE OR REPLACE FUNCTION sfti_contains(sfti, sfti)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'sfti_contains'
	LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR ~ (
    leftarg = sfti,
    rightarg = sfti,
    procedure = sfti_contains
);


--
-- Turning time into SFTI
--

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
COMMENT ON FUNCTION sfti_makeX(date) IS
'Turns a date into a point on our X axis.';


CREATE OR REPLACE FUNCTION sfti_makeSFTI(sa date, ka date, kb date, sb date, l float) RETURNS sfti AS $$
	SELECT format('(%s,%s,%s,%s,%s)',sfti_makeX($1),sfti_makeX($2),sfti_makeX($3),sfti_makeX($4),$5)::sfti;
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(sa date, ka date, kb date, sb date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$2,$3,$4,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_makeSFTI(date, date, date, date) IS
'Create a SFTI based on four dates that are the start of the support,
the start of the core, the end of the core and the end of the support.';

CREATE OR REPLACE FUNCTION sfti_makeSFTI(ka date, kb date, l float) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$2,$2,$3);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(ka date, kb date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$2,$2,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_makeSFTI(date, date) IS
'Create a SFTI based on two dates that are the start and the end of the core.
The support is considered to be equal to the core.
In effect this creates a sharp time interval.';

CREATE OR REPLACE FUNCTION sfti_makeSFTI(d date, l float) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$1,$1,$2);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_makeSFTI(d date) RETURNS sfti AS $$
	SELECT sfti_makeSFTI($1,$1,$1,$1,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_makeSFTI(date) IS
'Create a SFTI based on one date that is both the start and the end of the core.
The support is considered to be equal to the core.
In effect this creates a sharp time interval of a single date.';

CREATE OR REPLACE FUNCTION sfti_fuzzify(ka date, kb date, lv interval, rv interval, l float) RETURNS sfti AS $$
    SELECT sfti_makeSFTI(($1 - $3)::date, $1, $2, ($2 + $4)::date,$5);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_fuzzify(ka date, kb date, lv interval, rv interval) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$2,$3,$4,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_fuzzify(date, date, interval, interval) IS
'Creates a SFTI based on two dates that form the core of the SFTI and two intervals
that determine the Fuzzy Beginning and Fuzzy End of the SFTI.';

CREATE OR REPLACE FUNCTION sfti_fuzzify(ka date, kb date, v interval, l float) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$2,$3,$3,$4);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_fuzzify(ka date, kb date, v interval) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$2,$3,$3,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_fuzzify(date, date, interval) IS
'Creates a SFTI based on two dates that form the core of the SFTI and one interval
that determines both the Fuzzy Beginning and Fuzzy End of the SFTI.';

CREATE OR REPLACE FUNCTION sfti_fuzzify(d date, lv interval, rv interval, l float) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$1,$2,$3,$4);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_fuzzify(d date, lv interval, rv interval) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$1,$2,$3,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_fuzzify(date, interval, interval) IS
'Create a SFTI based on one date that forms the core of the SFTI and two intervals
that determines the Fuzzy Beginning and Fuzzy End of the SFTI.';

CREATE OR REPLACE FUNCTION sfti_fuzzify(d date, v interval, l float) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$1,$2,$2,$3);
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION sfti_fuzzify(d date, v interval) RETURNS sfti AS $$
    SELECT sfti_fuzzify($1,$1,$2,$2,1);
$$ LANGUAGE sql IMMUTABLE;
COMMENT ON FUNCTION sfti_fuzzify(date, interval) IS
'Create a SFTI based on one date that forms the core of the SFTI and one interval
that determines both the Fuzzy Beginning and Fuzzy End of the SFTI.';
