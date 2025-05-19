      SUBROUTINE GAUSS_SEIDEL(V, P, Q, Y, N, MAXITER, TOL)
        INTEGER N, MAXITER, ITER, I, K
        REAL*8 TOL, ERR, DV
        COMPLEX*16 V(N), Y(N*N), SUM, VNEW, Iinj
        REAL*8 P(N), Q(N)

        ITER = 0
        ERR = TOL + 1.0D0

10      IF (ERR .GT. TOL .AND. ITER .LT. MAXITER) THEN
            ERR = 0.0D0
            ITER = ITER + 1

            DO I = 2, N
                SUM = (0.0D0, 0.0D0)
                DO K = 1, N
                    IF (K .NE. I) THEN
                        SUM = SUM + Y((I-1)*N + K) * V(K)
                    ENDIF
                ENDDO

                VNEW = (DCMPLX(P(I), -Q(I)) / CONJG(V(I)) - SUM) /
     &                   Y((I-1)*N + I)
                DV = ABS(VNEW - V(I))
                IF (DV .GT. ERR) THEN
                    ERR = DV
                ENDIF
                V(I) = VNEW
            ENDDO

            GOTO 10
        ENDIF

        SUM = (0.0D0, 0.0D0)
        DO K = 1, N
            SUM = SUM + Y((1-1)*N + K) * V(K)
        ENDDO
        Iinj = V(1) * CONJG(SUM)
        P(1) = DBLE(Iinj)
        Q(1) = DIMAG(Iinj)

        RETURN
      END
