        SUBROUTINE NEWTON_RAPHSON(V, P, Q, Y, N, MAXITER, TOL)
            INTEGER N, MAXITER, ITER, I, K, J
            REAL*8 TOL, ERR
            COMPLEX*16 V(N), Y(N, N), Iinj, SUM
            REAL*8 P(N), Q(N), dP(N-1), dQ(N-1)
            REAL*8 J11(N-1, N-1), J12(N-1, N-1)
            REAL*8 DX(2*(N-1))

            ITER = 0
            ERR = TOL + 1.0D0

10          IF (ERR .GT. TOL .AND. ITER .LT. MAXITER) THEN
                ITER = ITER + 1

                ! Power mismatch calculation
                DO I = 2, N
                    SUM = (0.0D0, 0.0D0)
                    DO K = 1, N
                        SUM = SUM + Y(I, K) * V(K)
                    ENDDO
                    Iinj = V(I) * CONJG(SUM)
                    dP(I-1) = P(I) - DBLE(Iinj)
                    dQ(I-1) = Q(I) - DIMAG(Iinj)
                ENDDO

                ! Jacobian Matrix assembly (simplified for flat start)
                DO I = 2, N
                    DO J = 2, N
                        IF (I .EQ. J) THEN
                            J11(I-1, J-1) = 1.0D0
                            J12(I-1, J-1) = 1.0D0
                        ELSE
                            J11(I-1, J-1) = 0.0D0
                            J12(I-1, J-1) = 0.0D0
                        ENDIF
                    ENDDO
                ENDDO

                ! Solve Linear System J * DX = mismatch
                ! Simplification: for illustration, direct assignment
                DO I = 1, N-1
                    DX(I) = dP(I) / J11(I, I)
                    DX(I + N -1) = dQ(I) / J12(I, I)
                ENDDO

                ! Update voltages
                DO I = 2, N
                    V(I) = V(I) * DCMPLX(1.0D0, DX(I-1))  ! Update angle (simplified)
                    V(I) = V(I) + DCMPLX(DX(I + N -2), 0.0D0)  ! Update magnitude
                ENDDO

                ! Compute new error
                ERR = 0.0D0
                DO I = 1, 2*(N-1)
                    IF (ABS(DX(I)) .GT. ERR) ERR = ABS(DX(I))
                ENDDO

                GOTO 10
            ENDIF

            ! Final slack bus power calculation
            SUM = (0.0D0, 0.0D0)
            DO K = 1, N
                SUM = SUM + Y(1, K) * V(K)
            ENDDO
            Iinj = V(1) * CONJG(SUM)
            P(1) = DBLE(Iinj)
            Q(1) = DIMAG(Iinj)

            RETURN
      END
