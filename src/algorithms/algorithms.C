#include "algorithms.H"

void GAUSS_SEIDEL(DOUBLE_COMPLEX *V, DOUBLE *P, DOUBLE *Q, DOUBLE_COMPLEX *Y, INTEGER *N, INTEGER *MAXITER, DOUBLE *TOL) {
    API_GAUSS_SEIDEL(V, P, Q, Y, N, MAXITER, TOL);
}

void NEWTON_RAPHSON(DOUBLE_COMPLEX *V, DOUBLE *P, DOUBLE *Q, DOUBLE_COMPLEX *Y, INTEGER *N, INTEGER *MAXITER, DOUBLE *TOL) {
    API_NEWTON_RAPHSON(V, P, Q, Y, N, MAXITER, TOL);
}

void solve(Config& config, PowerSystemData& pData) {
    int N = pData.N;
    int maxIter = config.getMaxNumberOfIterations();
    double tol = config.getTolerance();
    switch (config.getSolverMethod()) {
        case Method::GAUSS:
            GAUSS_SEIDEL(
                pData.V,
                pData.P,
                pData.Q,
                pData.Y,
                &N,
                &maxIter,
                &tol
            );
            break;
        case Method::NEWTON:
            NEWTON_RAPHSON(
                pData.V,
                pData.P,
                pData.Q,
                pData.Y,
                &N,
                &maxIter,
                &tol
            );
            break;
        default:
            break;
    }
}