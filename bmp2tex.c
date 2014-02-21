/******************************************************************************
  
                           BMP2TEX

               a converter from BitMap to LaTeX


 *  Copyright (c) 1994 Maurizio Giunti - All rights reserved  *
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement:
    "This product includes software developed by Maurizio Giunti"

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
If you change this source to compile on a different environment
please contact me:

                    Maurizio Giunti
                    Via Foggini, 24
                    I-50142 Firenze

                    Fidonet: 2:332/102.3
                    Internet: giunti@stat.ds.unifi.it
*******************************************************************************
History:
04-09-1994 First release

*******************************************************************************
Compiled on:
    OS               BY                                 Note
 OS/2 2.11      Maurizio Giunti                     First release
 DOS            Maurizio Giunti

******************************************************************************/



#include <stdio.h>

#define VER "1.00"


/* Bitmap Header */
typedef struct tagBITMAPFILEHEADER {    
    char ID[2];
    short fill1[8];
    long  DimX;
    long  DimY;
    short fill2;
    short Planes;
    short fill3;
} BITMAPH;


/* Functions declaration */
short getbit(FILE *f);


/* Global data */
static short count;
static unsigned char byte;
static long pos;

int main(int argc,char **argv)
{
 FILE *f;       /* Input file (BitMap)   */
 FILE *out;     /* Output file           */
 BITMAPH head;  /* Bitmap header         */
 long x,y;      /* Line/column counters  */
 long lx;       /* Line points max       */
 long Pbuf;     /* Number of near points */
 long Psx;      /* Start point           */


 printf("\nBMP2TEX V%s - (C) 1994/95 Maurizio Giunti\n\n",VER);

 if(argc!=3)
    {
     puts("Usage:");
     puts(" bmp2tex <bitmap> <texfile>");
     puts("\nEx:\n bmp2tex dummy.bmp dummy.pic");
     return 0;
    }

 if((f=fopen(argv[1],"rb"))==NULL)
    {
     puts("BMP file not found!");
     return 1;
    }

 if((out=fopen(argv[2],"wt"))==NULL)
    {
     puts("Can't create destination file!");
     fclose(f);
     return 2;
    }

 /* Bitmap header reading */
 fread(&head,sizeof(BITMAPH),1,f);

 if(head.ID[0]!='B' || head.ID[1]!='M'  )
    {
     puts("This is not a bitmap!");
     fclose(f);
     return 3;
    }

 printf("BMP: %ldx%ld - %ld planes\n",head.DimX,head.DimY,head.Planes);

 if(head.Planes!=1)
    {
     puts("BMP must have 1 bit plane!");
     fclose(f);
     return 4;
    }

 if((out=fopen(argv[2],"wt"))==NULL)
    {
     puts("Can't create destination file!");
     fclose(f);
     return 5;
    }

 /* Begin picture */
 fprintf(out,"%%\n%% Created by BMP2TEX V%s - (C) 1994/95 Maurizio Giunti\n%%\n",VER);
 fprintf(out,"\\begin{picture}(%ld,%ld)(0,-%ld)\n",head.DimX,head.DimY,head.DimY);
 fprintf(out,"\\linethickness{\\unitlength}\n");

 /* Compute lx */
 lx=head.DimX;
 while((lx%32)!=0) lx++;
 count=9;
 pos=0;
 Pbuf=0;
 for(y=0;y<head.DimY;y++)
    {
     pos-=(lx/8);
     printf("\rLine: %ld/%ld  \r",y+1,head.DimY);


     /* Convertion cycle */
     for(x=0;x<lx;x++)
        {
         short bit=getbit(f);
         if(x>=head.DimX) continue;
         if(bit==1)
            {
             if(Pbuf==0)
                {
                 Psx=x;
                }
             Pbuf ++;
            }
        else if(Pbuf>0)
            {
             fprintf(out,"\\put(%ld,-%ld){\\line(1,0){%ld}}\n",Psx,y,Pbuf);
             Pbuf=0;
            }

        }
    if(Pbuf>0)
            {
             fprintf(out,"\\put(%ld,-%ld){\\line(1,0){%ld}}\n",Psx,y,Pbuf);
             Pbuf=0;
            }
    pos-=(lx/8);
    }


 /* End picture */
 fprintf(out,"\\end{picture}\n");
 fprintf(out,"%% End.\n");

 fclose(out);
 fclose(f);
 printf("\n<Done!>\n");
 return 0;
}


/* Get a single bit from the file */
short getbit(FILE *f)
{
 const unsigned char pow[8]={128,64,32,16,8,4,2,1};
 short bit;
 if(count>=8)
    {
     fseek(f,pos,SEEK_END);
     byte=getc(f);
     count=0;
     pos++;
    }
 bit= !((byte&pow[count])==pow[count]);
 count ++;
 return bit;
}




