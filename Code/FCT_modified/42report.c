/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */

#include "42.h"

/* #ifdef __cplusplus
** namespace _42 {
** using namespace Kit;
** #endif
*/

/*********************************************************************/
double FindTotalProjectedArea(struct SCType *S, double VecN[3])
{
   struct BodyType *B;
   struct GeomType *G;
   struct PolyType *P;
   double ProjArea = 0.0;
   double VecB[3], VoN;
   long Ib, Ipoly;

   for (Ib = 0; Ib < S->Nb; Ib++) {
      B = &S->B[Ib];

      /* Transform Direction Vector from N to B */
      MxV(B->CN, VecN, VecB);

      G = &Geom[B->GeomTag];
      for (Ipoly = 0; Ipoly < G->Npoly; Ipoly++) {
         P = &G->Poly[Ipoly];
         VoN = VoV(VecB, P->Norm);
         if (VoN > 0.0) {
            ProjArea += VoN*P->Area;
         }
      }
   }
   return (ProjArea);
}
/*********************************************************************/
double FindTotalUnshadedProjectedArea(struct SCType *S, double VecN[3])
{
   struct BodyType *B;
   struct GeomType *G;
   struct PolyType *P;
   double ProjArea = 0.0;
   double VecB[3], VoN;
   long Ib, Ipoly;

   FindUnshadedAreas(S, VecN);

   for (Ib = 0; Ib < S->Nb; Ib++)
   {
      B = &S->B[Ib];

      /* Transform Direction Vector from N to B */
      MxV(B->CN, VecN, VecB);

      G = &Geom[B->GeomTag];
      for (Ipoly = 0; Ipoly < G->Npoly; Ipoly++)
      {
         P = &G->Poly[Ipoly];
         VoN = VoV(VecB, P->Norm);
         if (VoN > 0.0)
         {
            ProjArea += VoN * P->UnshadedArea;
         }
      }
   }
   return (ProjArea);
}
/*********************************************************************/
void MagReport(void)
{
   static FILE *magfile;
   static long First = 1;

   if (First)
   {
      First = 0;
      magfile = FileOpen(InOutPath, "MagBVB.42", "wt");
   }

   fprintf(magfile, "%le %le %le %le %le %le %le %le %le \n",
           SC[0].bvb[0], SC[0].bvb[1], SC[0].bvb[2],
           SC[0].MAG[0].Field, SC[0].MAG[1].Field, SC[0].MAG[2].Field,
           SC[0].AC.bvb[0], SC[0].AC.bvb[1], SC[0].AC.bvb[2]);
}
/*********************************************************************/
void GyroReport(void)
{
   static FILE *gyrofile;
   static long First = 1;

   if (First)
   {
      First = 0;
      gyrofile = FileOpen(InOutPath, "Gyro.42", "wt");
   }

   fprintf(gyrofile, "%le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le \n",
           SC[0].B[0].wn[0], SC[0].B[0].wn[1], SC[0].B[0].wn[2],
           SC[0].Gyro[0].TrueRate, SC[0].Gyro[1].TrueRate, SC[0].Gyro[2].TrueRate,
           SC[0].Gyro[0].Bias, SC[0].Gyro[1].Bias, SC[0].Gyro[2].Bias,
           SC[0].Gyro[0].Angle, SC[0].Gyro[1].Angle, SC[0].Gyro[2].Angle,
           SC[0].Gyro[0].MeasRate, SC[0].Gyro[1].MeasRate, SC[0].Gyro[2].MeasRate,
           SC[0].AC.wbn[0], SC[0].AC.wbn[1], SC[0].AC.wbn[2]);
}
/*********************************************************************/
void FreqRespDiag(void)
{
      static FILE *outfile;
      struct FreqRespType *F;
      static long First = 1;
      
      if (First) {
         First = 0;
         outfile = FileOpen(InOutPath,"FreqRespDiag.42","wt");
      }
      
      F = &SC[0].FreqResp;
      fprintf(outfile,"%le %le %le %le %le %le\n",
         F->RefAng[0],F->RefAng[1],F->RefAng[2],
         F->OutAng[0],F->OutAng[1],F->OutAng[2]);
}
/*********************************************************************/
void Report(void)
{
   static FILE *timefile, *DynTimeFile;
   static FILE **xfile, **ufile, **xffile, **uffile;
   static FILE **ConstraintFile;
   static FILE **PosNfile, **VelNfile, **qbnfile, **wbnfile;
   static FILE **PosWfile, **VelWfile;
   static FILE **PosRfile, **VelRfile;
   static FILE **Hvnfile, **KEfile;
   static FILE **svnfile, **svbfile;
   static FILE **RPYfile;
   static FILE **Hwhlfile;
   static FILE **MTBfile;
   static FILE **AlbedoFile;
   // static FILE **ProjAreaFile;
   static FILE **AccFile;
   static char First = TRUE;
   long Isc, i;
   struct DynType *D;
   double CBL[3][3], Roll, Pitch, Yaw;
   struct WorldType *W;
   double WorldAngVel[3],wxR[3],VelN[3];
   double PosW[3], VelW[3], PosR[3], VelR[3];
   char s[40];
   //double ZAxis[3] = {0.0,0.0,1.0};

   if (First)
   {
      First = FALSE;
      timefile = FileOpen(InOutPath, "time.42", "w");
      DynTimeFile = FileOpen(InOutPath, "DynTime.42", "w");

      AlbedoFile = FileOpen(InOutPath,"Albedo.42","w");

      ufile = (FILE **)calloc(Nsc, sizeof(FILE *));
      xfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      uffile = (FILE **)calloc(Nsc, sizeof(FILE *));
      xffile = (FILE **)calloc(Nsc, sizeof(FILE *));
      ConstraintFile = (FILE **)calloc(Nsc, sizeof(FILE *));

      // Memory allocation for new variables
      PosNfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      VelNfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      PosWfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      VelWfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      PosRfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      VelRfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      qbnfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      wbnfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      Hvnfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      svnfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      svbfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      KEfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      RPYfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      Hwhlfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      MTBfile = (FILE **)calloc(Nsc, sizeof(FILE *));
      // ProjAreaFile = (FILE **) calloc(Nsc,sizeof(FILE *));
      AccFile = (FILE **)calloc(Nsc, sizeof(FILE *));

      for (Isc = 0; Isc < Nsc; Isc++)
      {
         if (SC[Isc].Exists)
         {
            sprintf(s, "u%03ld.42", Isc);
            ufile[Isc] = FileOpen(InOutPath, s, "w");
            sprintf(s, "x%03ld.42", Isc);
            xfile[Isc] = FileOpen(InOutPath, s, "w");
            if (SC[Isc].FlexActive)
            {
               sprintf(s, "uf%03ld.42", Isc);
               uffile[Isc] = FileOpen(InOutPath, s, "w");
               sprintf(s, "xf%03ld.42", Isc);
               xffile[Isc] = FileOpen(InOutPath, s, "w");
            }
            if (SC[Isc].ConstraintsRequested)
            {
               sprintf(s, "Constraint%03ld.42", Isc);
               ConstraintFile[Isc] = FileOpen(InOutPath, s, "w");
            }
         }
      }
      // Create files for new data variables
      for (Isc = 0; Isc < Nsc; Isc++)
      {
         sprintf(s, "PosN%03ld.42", Isc);
         PosNfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "VelN%03ld.42", Isc);
         VelNfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "PosW%03ld.42", Isc);
         PosWfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "VelW%03ld.42", Isc);
         VelWfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "PosR%03ld.42", Isc);
         PosRfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "VelR%03ld.42", Isc);
         VelRfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "qbn%03ld.42", Isc);
         qbnfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "wbn%03ld.42", Isc);
         wbnfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "Hvn%03ld.42", Isc);
         Hvnfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "svn%03ld.42", Isc);
         svnfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "svb%03ld.42", Isc);
         svbfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "KE%03ld.42", Isc);
         KEfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "RPY%03ld.42", Isc);
         RPYfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "Hwhl%03ld.42", Isc);
         Hwhlfile[Isc] = FileOpen(InOutPath, s, "w");
         sprintf(s, "MTB%03ld.42", Isc);
         MTBfile[Isc] = FileOpen(InOutPath, s, "w");
         // sprintf(s,"ProjArea%03ld.42",Isc);
         // ProjAreaFile = FileOpen(InOutPath,s,"w");
         sprintf(s, "Acc%03ld.42", Isc);
         AccFile[Isc] = FileOpen(InOutPath, s, "w");
      }
   }

   if (OutFlag)
   {
      fprintf(timefile, "%lf\n", SimTime);
      fprintf(DynTimeFile, "%lf\n", DynTime);
      for (Isc = 0; Isc < Nsc; Isc++)
      {
         if (SC[Isc].Exists)
         {
            D = &SC[Isc].Dyn;
            for (i = 0; i < D->Nu; i++)
               fprintf(ufile[Isc], "% le ", D->u[i]);
            fprintf(ufile[Isc], "\n");
            for (i = 0; i < D->Nx; i++)
               fprintf(xfile[Isc], "% le ", D->x[i]);
            fprintf(xfile[Isc], "\n");
            if (SC[Isc].FlexActive)
            {
               for (i = 0; i < D->Nf; i++)
                  fprintf(uffile[Isc], "% le ", D->uf[i]);
               fprintf(uffile[Isc], "\n");
               for (i = 0; i < D->Nf; i++)
                  fprintf(xffile[Isc], "% le ", D->xf[i]);
               fprintf(xffile[Isc], "\n");
            }
            if (SC[Isc].ConstraintsRequested)
            {
               for (i = 0; i < D->Nc; i++)
                  fprintf(ConstraintFile[Isc], "% le ",
                          D->GenConstraintFrc[i]);
               fprintf(ConstraintFile[Isc], "\n");
            }

            //Save data inside new variables
            fprintf(PosNfile[Isc], "%le %le %le\n",
                    SC[Isc].PosN[0], SC[Isc].PosN[1], SC[Isc].PosN[2]);
            fprintf(VelNfile[Isc], "%le %le %le\n",
                    SC[Isc].VelN[0], SC[Isc].VelN[1], SC[Isc].VelN[2]);
            MxV(World[EARTH].CWN, SC[Isc].PosN, PosW);
            MxV(World[EARTH].CWN, SC[Isc].VelN, VelW);
            fprintf(PosWfile[Isc], "%18.12le %18.12le %18.12le\n",
                    PosW[0], PosW[1], PosW[2]);
            fprintf(VelWfile[Isc], "%18.12le %18.12le %18.12le\n",
                    VelW[0], VelW[1], VelW[2]);

            if (Orb[SC[Isc].RefOrb].Regime == ORB_FLIGHT)
            {
               MxV(Rgn[Orb[SC[Isc].RefOrb].Region].CN, SC[Isc].PosR, PosR);
               MxV(Rgn[Orb[SC[Isc].RefOrb].Region].CN, SC[Isc].VelR, VelR);
               fprintf(PosRfile[Isc], "%le %le %le\n",
                       PosR[0], PosR[1], PosR[2]);
               fprintf(VelRfile[Isc], "%le %le %le\n",
                       VelR[0], VelR[1], VelR[2]);
            }
            else
            {
               fprintf(PosRfile[Isc], "%le %le %le\n",
                       SC[Isc].PosR[0], SC[Isc].PosR[1], SC[Isc].PosR[2]);
               fprintf(VelRfile[Isc], "%le %le %le\n",
                       SC[Isc].VelR[0], SC[Isc].VelR[1], SC[Isc].VelR[2]);
            }

            fprintf(qbnfile[Isc], "%le %le %le %le\n",
                    SC[Isc].B[0].qn[0], SC[Isc].B[0].qn[1], SC[Isc].B[0].qn[2], SC[Isc].B[0].qn[3]);
            fprintf(wbnfile[Isc], "%le %le %le\n",
                    SC[Isc].B[0].wn[0], SC[Isc].B[0].wn[1], SC[Isc].B[0].wn[2]);
            fprintf(Hvnfile[Isc], "%18.12le %18.12le %18.12le\n",
                    SC[Isc].Hvn[0], SC[Isc].Hvn[1], SC[Isc].Hvn[2]);
            fprintf(svnfile[Isc], "%18.12le %18.12le %18.12le\n",
                    SC[Isc].svn[0], SC[Isc].svn[1], SC[Isc].svn[2]);
            fprintf(svbfile[Isc], "%18.12le %18.12le %18.12le\n",
                    SC[Isc].svb[0], SC[Isc].svb[1], SC[Isc].svb[2]);
            fprintf(KEfile[Isc], "%18.12le\n", FindTotalKineticEnergy(&SC[Isc]));
            MxMT(SC[Isc].B[0].CN, SC[Isc].CLN, CBL);
            C2A(123, CBL, &Roll, &Pitch, &Yaw);
            fprintf(RPYfile[Isc], "%lf %lf %lf\n", Roll * R2D, Pitch * R2D, Yaw * R2D);
            for (i = 0; i < SC[Isc].Nw; i++)
               fprintf(Hwhlfile[Isc], "%lf ", SC[Isc].Whl[i].H);
            fprintf(Hwhlfile[Isc], "\n");
            if (SC[Isc].Nmtb > 0)
            {
               for (i = 0; i < SC[Isc].Nmtb; i++)
                  fprintf(MTBfile[Isc], "%lf ", SC[Isc].MTB[i].M);
               fprintf(MTBfile[Isc], "\n");
            }
            //fprintf(ProjAreaFile[Isc],"%18.12le %18.12le\n",
            //   FindTotalProjectedArea(&SC[Isc],ZAxis),
            //   FindTotalUnshadedProjectedArea(&SC[Isc],ZAxis));
            if (SC[Isc].Nacc > 0)
            {
               for (i = 0; i < SC[Isc].Nacc; i++)
                  fprintf(AccFile[Isc], "%le %le ", SC[Isc].Accel[i].TrueAcc, SC[Isc].Accel[i].MeasAcc);
               fprintf(AccFile[Isc], "\n");
            }
         }
      }
      // if (SC[0].Exists) {
      //
      //    //MagReport();
      //    //GyroReport();
      //
      // }
   }

   /* An example how to call specialized reporting based on sim case */
   /* if (!strcmp(InOutPath,"./Potato/")) PotatoReport(); */

   if (CleanUpFlag)
   {
      fclose(timefile);
   }
}

/* #ifdef __cplusplus
** }
** #endif
*/
