#include <stdio.h>
#include "Config.h"
#include "AES128.h"
#include "global.h"

unsigned char cosemExtDataBuf[200];
unsigned char cc;
void Aes128::KeyExpansion(void)// This function produces Nb(Nr+1) round keys. The round keys are used in each round to encrypt the states. 
{
  unsigned char temp[4],k,RVG,RVG1,RVG2,i,j;
  // The first round key is the key itself.
 for(i=0;i<16;i++)
 {
   cosemExtDataBuf[i]  = fg_HLSSecret[i];
 }
 // EpromRead(US_HLS_PASSWORD_OFF,&cosemExtDataBuf[0],16);//read HLS password from eeprom
  i = 4;
  // All other round keys are found from the previous round keys.
  RVG = 12;
  while (i < 44)//(NO_OF_COLUMNS * (NO_OF_ROUNF+1)))
  {
    for(j=0;j<4;j++)
    {
      temp[j]=cosemExtDataBuf[RVG++];
    }
    if (i % NO_OF_WORD == 0)
    {
      // This function rotates the 4 bytes in a word to the left once. [a0,a1,a2,a3] becomes [a1,a2,a3,a0]
      // Function RotWord()
      k = temp[0];
      temp[0] = temp[1];
      temp[1] = temp[2];
      temp[2] = temp[3];
      temp[3] = k;
    // SubWord() is a function that takes a four-byte input word and  applies the S-box to each of the four bytes to produce an output word.
    // Function Subword()
      temp[0]=sbox[temp[0]];
      temp[1]=sbox[temp[1]];
      temp[2]=sbox[temp[2]];
      temp[3]=sbox[temp[3]];

      temp[0] =  temp[0] ^ Rcon[i/NO_OF_WORD];
    }
    RVG1 = i*4;
    RVG2 = (i-NO_OF_WORD)*4;
    cosemExtDataBuf[RVG1++] = cosemExtDataBuf[RVG2++] ^ temp[0];
    cosemExtDataBuf[RVG1++] = cosemExtDataBuf[RVG2++] ^ temp[1];
    cosemExtDataBuf[RVG1++] = cosemExtDataBuf[RVG2++] ^ temp[2];
    cosemExtDataBuf[RVG1++] = cosemExtDataBuf[RVG2++] ^ temp[3];
    i++;
  }
}
void Aes128::AddRoundKey(unsigned int round) 
{
  unsigned char i,j,RVG;
  for(i=0;i<4;i++)
  {
    for(j=0;j<4;j++)
    {
      RVG = ((round * 16) + (i * NO_OF_COLUMNS) + j);
      state[j][i] ^= cosemExtDataBuf[RVG];
    }
  }
}
void Aes128::SubBytes(void)// The SubBytes Function Substitutes the values in the state matrix with values in an S-box.
{
  unsigned char i,j;
  for(i=0;i<4;i++)
  {
    for(j=0;j<4;j++)
    {
      state[i][j] = sbox[state[i][j]];
    }
  }
}
void Aes128::ShiftRows(void)// The ShiftRows() function shifts the rows in the state to the left. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted.
{
  unsigned char temp;
  // Rotate first row 1 columns to left 
  temp=state[1][0];
  state[1][0]=state[1][1];
  state[1][1]=state[1][2];
  state[1][2]=state[1][3];
  state[1][3]=temp;

  // Rotate second row 2 columns to left  
  temp=state[2][0];
  state[2][0]=state[2][2];
  state[2][2]=temp;

  temp=state[2][1];
  state[2][1]=state[2][3];
  state[2][3]=temp;

  // Rotate third row 3 columns to left
  temp=state[3][0];
  state[3][0]=state[3][3];
  state[3][3]=state[3][2];
  state[3][2]=state[3][1];
  state[3][1]=temp;
}
void Aes128::MixColumns(void)
{
  unsigned char i;
  unsigned char Tmp,Tm,t;
  for(i=0;i<4;i++)
  { 
    t   = state[0][i];
    Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
    Tm  = state[0][i] ^ state[1][i] ; 
    Tm  = xtime(Tm); 
    state[0][i] ^= Tm ^ Tmp ;
    Tm  = state[1][i] ^ state[2][i] ; 
    Tm  = xtime(Tm); state[1][i] ^= Tm ^ Tmp ;
    Tm  = state[2][i] ^ state[3][i] ; 
    Tm  = xtime(Tm); state[2][i] ^= Tm ^ Tmp ;
    Tm  = state[3][i] ^ t ; 
    Tm  = xtime(Tm); 
    state[3][i] ^= Tm ^ Tmp ;
  }
}
void Aes128::Cipher(void)// Cipher is the main function that encrypts the PlainText.
{
  unsigned int i,j,cc ;
  unsigned char round=0;
 
cc=0;
          for(i=0;i<4;i++)
          {
            for(j=0;j<4;j++)
            {
               state[j][i] = MeterChallenge[cc];
               cc++;
            }
          }
  AddRoundKey(0); // Add the First round key to the state before starting the rounds.
  
  // There will be NO_OF_ROUNF rounds./ The first NO_OF_ROUNF-1 rounds are identical. These NO_OF_ROUNF-1 rounds are executed in the loop below.
  for(round=1;round<NO_OF_ROUNF;round++)
  {
    SubBytes();
    ShiftRows();
    MixColumns();
    AddRoundKey(round);
  }
  
  // The last round is given below. The MixColumns function is not here in the last round.
  SubBytes();
  ShiftRows();
  AddRoundKey(NO_OF_ROUNF);
}
void Aes128::Encryption128()
{
  KeyExpansion();   // The KeyExpansion routine must be called before encryption.
  Cipher();         // The next function call encrypts the PlainText with the Key using AES algorithm.
}

/*
void main(void)
{
unsigned char i,j;
//==============================================
    clrscr();
    printf("\n\n");
      for(i=0;i<16;i++)
        printf(" %02X",MeterChallenge[i]);
 //==============================================
  Encryption128();
  printf("\n\n");
  cc=0;
   for(i=0;i<4;i++)
	  {
            for(j=0;j<4;j++)
            {
              // state[j][i] = MeterChallenge[cc];
                printf(" %02X",state[j][i]);
               // getch();
               cc++;
            }
          }
  getch();
}
*/
