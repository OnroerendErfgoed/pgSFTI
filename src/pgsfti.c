#include "postgres.h"
#include "fmgr.h"

#include "lib/stringinfo.h"
#include "libpq/pqformat.h"

#include "pgsfti.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


/**
 * Functions for the sfti type.
 */

PG_FUNCTION_INFO_V1(sfti_in);
Datum sfti_in(PG_FUNCTION_ARGS);

Datum
sfti_in(PG_FUNCTION_ARGS)
{
	char		*str = PG_GETARG_CSTRING(0);
    double		sa, ka, kb, sb, lambda;
    sfti		*result;

    if (sscanf(str, "(%lf,%lf,%lf,%lf,%lf)", &sa, &ka, &kb, &sb, &lambda) != 5)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for TFTI: \"%s\"", str)));

    result = (sfti *) palloc(sizeof(sfti));
    result->sa = sa;
    result->ka = ka;
    result->kb = kb;
    result->sb = sb;
    result->lambda = lambda;
    PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(sfti_out);
Datum sfti_out(PG_FUNCTION_ARGS);

Datum
sfti_out(PG_FUNCTION_ARGS)
{
    sfti	*frame = (sfti *) PG_GETARG_POINTER(0);
    char	*result;
    result = (char *) palloc(250);
    snprintf(result, 250, "(%f,%f,%f,%f,%f)", frame->sa, frame->ka, frame->kb, frame->sb, frame->lambda);
    PG_RETURN_CSTRING(result);
}


PG_FUNCTION_INFO_V1(sfti_recv);
Datum sfti_recv(PG_FUNCTION_ARGS);

Datum
sfti_recv(PG_FUNCTION_ARGS)
{
    StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
    sfti    *result;

    result = (sfti *) palloc(sizeof(sfti));
    result->sa = pq_getmsgfloat8(buf);
	result->ka = pq_getmsgfloat8(buf);
    result->kb = pq_getmsgfloat8(buf);
    result->sb = pq_getmsgfloat8(buf);
    result->lambda = pq_getmsgfloat8(buf);
    PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(sfti_send);
Datum sfti_send(PG_FUNCTION_ARGS);

Datum
sfti_send(PG_FUNCTION_ARGS)
{
    sfti    *source = (sfti *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendfloat8(&buf, source->sa);
    pq_sendfloat8(&buf, source->ka);
    pq_sendfloat8(&buf, source->kb);
    pq_sendfloat8(&buf, source->sb);
    pq_sendfloat8(&buf, source->lambda);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/**
 * Casts
 */

PG_FUNCTION_INFO_V1(sfti_to_float);
Datum sfti_to_float(PG_FUNCTION_ARGS);

Datum
sfti_to_float(PG_FUNCTION_ARGS)
{
    sfti	*frame = (sfti *) PG_GETARG_POINTER(0);
    double  result;

    result = frame->ka + ( (frame->kb - frame->ka) / 2 );
    PG_RETURN_FLOAT8(result);
}

PG_FUNCTION_INFO_V1(sfti_to_int);
Datum sfti_to_int(PG_FUNCTION_ARGS);

Datum
sfti_to_int(PG_FUNCTION_ARGS)
{
    sfti	*frame = (sfti *) PG_GETARG_POINTER(0);
    double  result;

    result = round(frame->ka + ( (frame->kb - frame->ka) / 2 ));
    PG_RETURN_INT32(result);
}

/**
 * Supporting functions
 */

double tw(double x, double y)
{
    return max(0.0, x + y - 1.0);
}

double iw(double x, double y)
{
    return min(1.0, 1.0 - x + y);
}

double sw(double x, double y)
{
    return min(1.0, x + y);
}


double long_before(double a, double b, double alpha, double beta)
{
    if ((b - a) > ( alpha + beta) ) {
        return 1.0;
    }
    if ((b - a) <= alpha ) {
        return 0.0;
    }
    return (b - a - alpha) / beta;
}

double before_or_equals(double a, double b, double alpha, double beta)
{
    return 1.0 - long_before(b, a, alpha, beta);
}

double sl(sfti *sfti_ptr)
{
    return sfti_ptr -> ka - sfti_ptr -> sa;
}

double sr(sfti *sfti_ptr)
{

    return sfti_ptr -> sb - sfti_ptr -> kb;
}


double before_bb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha1, j2;
    double slb, sla;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    sla = sl(a_ptr);
    slb = sl(b_ptr);
    i = tw(la, 1.0 - lb);
    alpha1 =    alpha +
                min(0.0, slb - beta)*(1.0-lb) +
                max(beta, sla )*(1.0-la) -
                sla +
                la * min(max(beta,slb),sla);
    j2 = long_before(a_ptr -> ka, b_ptr -> ka, alpha1, max(beta, max( sla, slb ) ) );
    j = min(la,j2);
    return max(i,j);
}

double equals_bb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha2, j2;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = iw(lb,la);
    alpha2 =    alpha
                + min(0.0, sl(a_ptr) - beta) * (1.0-la)
                + max(beta, sl(a_ptr))*(1.0-lb)
                - sl(b_ptr)
                + lb * min(max(beta,sl(a_ptr)),sl(b_ptr));
    j2 = before_or_equals(a_ptr -> ka, b_ptr -> ka, alpha2, max(beta, max( sl(a_ptr), sl(b_ptr) ) ) );
    j = max(1 - lb, j2);
    return min(i,j);
}

double before_ee(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha3, j2;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = tw(lb, 1.0 - la);
    alpha3 =    alpha
                + min(0.0,sr(a_ptr)-beta)*(1.0-la)
                + max(beta,sr(a_ptr))*(1.0-lb)
                - sr(b_ptr)
                + lb*min(max(beta,sr(a_ptr)),sr(b_ptr));
    j2 = long_before(a_ptr -> kb, b_ptr -> kb, alpha3, max(beta, max( sr(a_ptr), sr(b_ptr) ) ) );
    j = min(lb,j2);
    return max(i,j);
}

double equals_ee(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha4, j2;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = iw(la,lb);
    alpha4 =    alpha
                + min(0.0, sr(b_ptr) - beta)*(1.0-lb)
                + max(beta,sr(b_ptr))*(1.0-la)
                - sr(a_ptr)
                + la*min(max(beta,sr(b_ptr)),sr(a_ptr));
    j2 = before_or_equals(a_ptr -> kb, b_ptr -> kb, alpha4, max(beta, max( sr(a_ptr), sr(b_ptr) ) ) );
    j = max(1.0-la,j2);
    return min(i,j);
}

double before_eb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha5;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = sw(1.0 - la, 1.0 - lb);
    alpha5 =    alpha +
                min(0.0, sl(b_ptr) - beta)*(1.0-lb) +
                min(max(beta,sl(b_ptr)),sr(a_ptr)) -
                lb * max(beta,sl(b_ptr)) -
                sr(a_ptr) * la +
                max(beta,max(sl(b_ptr),sr(a_ptr)))*tw(la,lb);
    j = long_before(a_ptr -> kb, b_ptr -> ka, alpha5, max(beta,max(sr(a_ptr),sl(b_ptr))));
    return max(i,j);
}

double equals_eb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha6;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = sw(1.0 - la, 1.0 - lb);
    alpha6 =    alpha
                + min (beta-sl(b_ptr),(beta-sl(b_ptr))*(1.0-lb))
                + max (beta,sl(b_ptr))*lb
                + sr(a_ptr)*la
                - sr(a_ptr)
                - max(beta,max(sr(a_ptr),sl(b_ptr)))*tw(la,lb);
    j = before_or_equals(a_ptr -> kb, b_ptr -> ka, alpha6, max(beta,max(sr(a_ptr),sl(b_ptr))));
    return max(i,j);
}

double before_be(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha7;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = tw(la,lb);
    alpha7 =    alpha
                + min (beta-sr(b_ptr),(beta-sr(b_ptr))*(1.0-lb))
                + sl(a_ptr)*la
                + max(beta,sr(b_ptr))*lb
                - sl(a_ptr)
                - max(beta,max(sl(a_ptr),sr(b_ptr)))*tw(la,lb);
    j = long_before(a_ptr -> ka, b_ptr -> kb, alpha7, max(beta,max(sl(a_ptr),sr(b_ptr))));
    return min(i,j);
}

double equals_be(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
    double i, j;
    double la, lb;
    double alpha8;
    la = a_ptr -> lambda;
    lb = b_ptr -> lambda;
    i = tw(la,lb);
    alpha8 =    alpha
                + min(0.0, sr(b_ptr) - beta)*(1.0-lb)
                + min(max(beta,sr(b_ptr)),sl(a_ptr))
                - la * sl(a_ptr)
                - lb * max(beta,sr(b_ptr))
                + max(beta,max(sl(a_ptr),sr(b_ptr)))*tw(la,lb);
    j = before_or_equals(a_ptr -> ka, b_ptr -> kb, alpha8, max(beta,max(sl(a_ptr),sr(b_ptr))));
    return min(i,j);
}


/**
 * Allen Relations
 */

double allen_before(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 1;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	return before_eb(a_ptr, b_ptr, alpha, beta);
}


double allen_meets(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double equal_eb, equal_be;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	equal_eb = equals_eb(a_ptr, b_ptr, alpha, beta);
	if(equal_eb == 0) {
		return 0;
	}
	equal_be = equals_be(b_ptr, a_ptr, alpha, beta);
	if(equal_be == 0) {
		return 0;
	}
	return min(equal_eb, equal_be);
}


double allen_overlaps(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double bef_bb, bef_be, bef_ee;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	bef_bb = before_bb(a_ptr, b_ptr, alpha, beta);
	if(bef_bb == 0) {
		return 0;
	}
	bef_be = before_be(b_ptr, a_ptr, alpha, beta);
	if(bef_be == 0) {
		return 0;
	}
	bef_ee = before_ee(a_ptr, b_ptr, alpha, beta);
	if(bef_ee == 0) {
		return 0;
	}
	return min(bef_bb, min(bef_be, bef_ee));
}



double allen_during(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double bef_bb, bef_ee;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	bef_bb = before_bb(b_ptr, a_ptr, alpha, beta);
	if(bef_bb == 0) {
		return 0;
	}
	bef_ee = before_ee(a_ptr, b_ptr, alpha, beta);
	if(bef_ee == 0) {
		return 0;
	}
	return min(bef_bb, bef_ee);
}



double allen_starts(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double bb_ab, bb_ba, ee;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	bb_ab = equals_bb(a_ptr, b_ptr, alpha, beta);
	if(bb_ab == 0) {
		return 0;
	}
	bb_ba = equals_bb(b_ptr, a_ptr, alpha, beta);
	if(bb_ba == 0) {
		return 0;
	}
	ee = before_ee(a_ptr, b_ptr, alpha, beta);
	if(ee == 0) {
		return 0;
	}
	return min(ee, min(bb_ab, bb_ba));
}


double allen_finishes(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double ee_ab, ee_ba, bb;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	ee_ab = equals_ee(a_ptr, b_ptr, alpha, beta);
	if(ee_ab == 0) {
		return 0;
	}
	ee_ba = equals_ee(b_ptr, a_ptr, alpha, beta);
	if(ee_ba == 0) {
		return 0;
	}
	bb = before_bb(b_ptr, a_ptr, alpha, beta);
	if(bb == 0) {
		return 0;
	}
	return min(bb, min(ee_ab, ee_ba));
}


double allen_equals(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double ee_ab, ee_ba, bb_ab, bb_ba;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	ee_ab = equals_ee(a_ptr, b_ptr, alpha, beta);
	if(ee_ab == 0) {
		return 0;
	}
	ee_ba = equals_ee(b_ptr, a_ptr, alpha, beta);
	if(ee_ba == 0) {
		return 0;
	}
	bb_ab = equals_bb(a_ptr, b_ptr, alpha, beta);
	if(bb_ab == 0) {
		return 0;
	}
	bb_ba = equals_bb(b_ptr, a_ptr, alpha, beta);
	if(bb_ba == 0) {
		return 0;
	}
	return min(min(bb_ab, bb_ba), min(ee_ab, ee_ba));
}


double allen_after(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_before(b_ptr, a_ptr, alpha, beta);
}


double allen_overlapped_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_overlaps(b_ptr, a_ptr, alpha, beta);
}


double allen_contains(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_during(b_ptr, a_ptr, alpha, beta);
}


double allen_met_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_meets(b_ptr, a_ptr, alpha, beta);
}

double allen_started_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_starts(b_ptr, a_ptr, alpha, beta);
}

double allen_finished_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return allen_finishes(b_ptr, a_ptr, alpha, beta);
}


/**
 * Extended Allen Relations
 */

double kvd_before(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 1;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	return equals_eb(a_ptr, b_ptr, alpha, beta);
}

double kvd_after(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return kvd_before(b_ptr, a_ptr, alpha, beta);
}

double kvd_during(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double ee, bb;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	ee = equals_ee(a_ptr, b_ptr, alpha, beta);
	if(ee == 0) {
		return 0;
	}
	bb = equals_bb(b_ptr, a_ptr, alpha, beta);
	if(bb == 0) {
		return 0;
	}
	return min(ee, bb);
}

double kvd_contains(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	return kvd_during(b_ptr, a_ptr, alpha, beta);
}

double kvd_intersects(sfti *a_ptr, sfti *b_ptr, double alpha, double beta)
{
	double before_ab, before_ba;
	if((alpha == beta) == 0){
		if(a_ptr->sb < b_ptr->sa) {
			return 0;
		}
		if(a_ptr->sa > b_ptr->sb) {
			return 0;
		}
	}
	before_ab = before_be(a_ptr, b_ptr, alpha, beta);
	if(before_ab == 0) {
		return 0;
	}
	before_ba = before_be(b_ptr, a_ptr, alpha, beta);
	if(before_ba == 0){
		return 0;
	}
	return min(before_ab, before_ba);
}


PG_FUNCTION_INFO_V1(sfti_allen_before);
Datum sfti_allen_before(PG_FUNCTION_ARGS);

Datum
sfti_allen_before(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_before(a_ptr, b_ptr, alpha, beta));
}


PG_FUNCTION_INFO_V1(sfti_allen_meets);
Datum sfti_allen_meets(PG_FUNCTION_ARGS);

Datum
sfti_allen_meets(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_meets(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_overlaps);
Datum sfti_allen_overlaps(PG_FUNCTION_ARGS);

Datum
sfti_allen_overlaps(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_overlaps(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_during);
Datum sfti_allen_during(PG_FUNCTION_ARGS);

Datum
sfti_allen_during(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_during(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_starts);
Datum sfti_allen_starts(PG_FUNCTION_ARGS);

Datum
sfti_allen_starts(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_starts(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_finishes);
Datum sfti_allen_finishes(PG_FUNCTION_ARGS);

Datum
sfti_allen_finishes(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_finishes(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_equals);
Datum sfti_allen_equals(PG_FUNCTION_ARGS);

Datum
sfti_allen_equals(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_equals(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_after);
Datum sfti_allen_after(PG_FUNCTION_ARGS);

Datum
sfti_allen_after(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_after(a_ptr, b_ptr, alpha, beta));
}


PG_FUNCTION_INFO_V1(sfti_allen_overlapped_by);
Datum sfti_allen_overlapped_by(PG_FUNCTION_ARGS);

Datum
sfti_allen_overlapped_by(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_overlapped_by(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_contains);
Datum sfti_allen_contains(PG_FUNCTION_ARGS);

Datum
sfti_allen_contains(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_contains(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_met_by);
Datum sfti_allen_met_by(PG_FUNCTION_ARGS);

Datum
sfti_allen_met_by(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_met_by(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_started_by);
Datum sfti_allen_started_by(PG_FUNCTION_ARGS);

Datum
sfti_allen_started_by(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_started_by(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_allen_finished_by);
Datum sfti_allen_finished_by(PG_FUNCTION_ARGS);

Datum
sfti_allen_finished_by(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(allen_finished_by(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_kvd_before);
Datum sfti_kvd_before(PG_FUNCTION_ARGS);

Datum
sfti_kvd_before(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(kvd_before(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_kvd_after);
Datum sfti_kvd_after(PG_FUNCTION_ARGS);

Datum
sfti_kvd_after(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(kvd_after(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_kvd_during);
Datum sfti_kvd_during(PG_FUNCTION_ARGS);

Datum
sfti_kvd_during(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(kvd_during(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_kvd_contains);
Datum sfti_kvd_contains(PG_FUNCTION_ARGS);

Datum
sfti_kvd_contains(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(kvd_contains(a_ptr, b_ptr, alpha, beta));
}

PG_FUNCTION_INFO_V1(sfti_kvd_intersects);
Datum sfti_kvd_intersects(PG_FUNCTION_ARGS);

Datum
sfti_kvd_intersects(PG_FUNCTION_ARGS)
{

	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	double alpha = PG_GETARG_FLOAT8(2);
	double beta = PG_GETARG_FLOAT8(3);
	PG_RETURN_FLOAT8(kvd_intersects(a_ptr, b_ptr, alpha, beta));
}

/**
 * Functions for operators.
 */

PG_FUNCTION_INFO_V1(sfti_lt);
Datum sfti_lt(PG_FUNCTION_ARGS);

Datum
sfti_lt(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(allen_before(a_ptr, b_ptr, 0, 0) > 0.5);
}

PG_FUNCTION_INFO_V1(sfti_gt);
Datum sfti_gt(PG_FUNCTION_ARGS);

Datum
sfti_gt(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(allen_after(a_ptr, b_ptr, 0, 0) > 0.5);
}

PG_FUNCTION_INFO_V1(sfti_eq);
Datum sfti_eq(PG_FUNCTION_ARGS);

Datum
sfti_eq(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(allen_equals(a_ptr, b_ptr, 0, 0) == 0.5);
}

PG_FUNCTION_INFO_V1(sfti_intersects);
Datum sfti_intersects(PG_FUNCTION_ARGS);

Datum
sfti_intersects(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(kvd_intersects(a_ptr, b_ptr, 0, 0) > 0.5);
}

PG_FUNCTION_INFO_V1(sfti_during);
Datum sfti_during(PG_FUNCTION_ARGS);

Datum
sfti_during(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(allen_during(a_ptr, b_ptr, 0, 0) > 0.5);
}

PG_FUNCTION_INFO_V1(sfti_contains);
Datum sfti_contains(PG_FUNCTION_ARGS);

Datum
sfti_contains(PG_FUNCTION_ARGS)
{
	sfti *a_ptr = (sfti *) PG_GETARG_POINTER(0);
	sfti *b_ptr = (sfti *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(allen_contains(a_ptr, b_ptr, 0, 0) > 0.5);
}
