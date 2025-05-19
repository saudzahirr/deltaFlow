      SUBROUTINE GAUSS_SEIDEL(V, P, Q, Y, N, MAXITER, TOL, R_COEFF, 
     &                        BUS_TYPE, QMIN, QMAX, ITER_OUT, ERR_OUT)
        INTEGER N, MAXITER, ITER, I, K, BUS_TYPE(N)
        REAL*8 TOL, R_COEFF, ERR, DV, QMIN(N), QMAX(N)
        COMPLEX*16 V(N), Y(N*N), SUM, VNEW, IINJ
        COMPLEX*16 VUPD, DVEC
        REAL*8 P(N), Q(N)
        INTEGER ITER_OUT
        REAL*8 ERR_OUT

        ITER = 0
        ERR = TOL + 1.0D0

10      IF (ERR .GT. TOL .AND. ITER .LT. MAXITER) THEN
            ERR = 0.0D0
            ITER = ITER + 1

            DO I = 2, N
                IF (BUS_TYPE(I) .EQ. 2) THEN
                    SUM = (0.0D0, 0.0D0)
                    DO K = 1, N
                        SUM = SUM + Y((I-1)*N + K) * V(K)
                    ENDDO
                    Q(I) = -DIMAG(CONJG(V(I)) * SUM)

                    IF (Q(I) .GE. QMIN(I) .AND. Q(I) .LE. QMAX(I)) THEN
                        SUM = (0.0D0, 0.0D0)
                        DO K = 1, N
                            IF (K .NE. I) THEN
                                SUM = SUM + Y((I-1)*N + K) * V(K)
                            ENDIF
                        ENDDO
                        VUPD = ((DCMPLX(P(I), -Q(I)) / CONJG(V(I))) - SUM)
     &                       / Y((I-1)*N + I)
                        VNEW = ABS(V(I)) * VUPD / ABS(VUPD)
                        DV = ABS(VNEW - V(I))
                        IF (DV .GT. ERR) ERR = DV
                            V(I) = VNEW
                        ELSE
                            IF (Q(I) .LT. QMIN(I)) THEN
                                Q(I) = QMIN(I)
                            ELSE
                                Q(I) = QMAX(I)
                            ENDIF
                        SUM = (0.0D0, 0.0D0)
                        DO K = 1, N
                            IF (K .NE. I) THEN
                                SUM = SUM + Y((I-1)*N + K) * V(K)
                            ENDIF
                        ENDDO
                        VUPD = ((DCMPLX(P(I), -Q(I)) / CONJG(V(I))) - SUM)
     &                       / Y((I-1)*N + I)
                        DVEC = R_COEFF * (VUPD - V(I))
                        DV = ABS(DVEC)
                        IF (DV .GT. ERR) ERR = DV
                        V(I) = V(I) + DVEC
                    ENDIF

                ELSE  ! PQ BUS
                    SUM = (0.0D0, 0.0D0)
                    DO K = 1, N
                        IF (K .NE. I) THEN
                            SUM = SUM + Y((I-1)*N + K) * V(K)
                        ENDIF
                    ENDDO
                VUPD = ((DCMPLX(P(I), -Q(I)) / CONJG(V(I))) - SUM)
     &               / Y((I-1)*N + I)
                DVEC = R_COEFF * (VUPD - V(I))
                DV = ABS(DVEC)
                IF (DV .GT. ERR) ERR = DV
                V(I) = V(I) + DVEC
                ENDIF
            ENDDO

            GOTO 10
        ENDIF

        ! Compute Slack Bus Power
        SUM = (0.0D0, 0.0D0)
        DO K = 1, N
            SUM = SUM + Y((1-1)*N + K) * V(K)
        ENDDO
        IINJ = V(1) * CONJG(SUM)
        P(1) = DBLE(IINJ)
        Q(1) = DIMAG(IINJ)

        ITER_OUT = ITER
        ERR_OUT = ERR

        RETURN
        END
