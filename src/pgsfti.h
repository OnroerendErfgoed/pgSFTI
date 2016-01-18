#include <math.h>


/** Main Fuzzy Data Type declaration **/
typedef struct sfti{
    double sa;
    double ka;
    double kb;
    double sb;
    double lambda;
} sfti;

#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

double tw(double x, double y);
double iw(double x, double y);
double sw(double x, double y);

double long_before(double a, double b, double alpha, double beta);
double before_or_equals(double a, double b, double alpha, double beta);
double sl(sfti *sfti_ptr);
double sr(sfti *sfti_ptr);

/* Support functions */
double before_bb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double equals_bb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double before_ee(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double equals_ee(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double before_eb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double equals_eb(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double before_be(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double equals_be(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);

/* Allen relations */
double allen_before(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_meets(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_overlaps(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_during(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_starts(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_finishes(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_equals(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_after(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_overlapped_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_contains(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_met_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_started_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double allen_finished_by(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);

/* Kvd relations */
double kvd_before(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double kvd_after(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double kvd_during(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double kvd_contains(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
double kvd_intersects(sfti *a_ptr, sfti *b_ptr, double alpha, double beta);
