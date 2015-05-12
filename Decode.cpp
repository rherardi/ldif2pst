#include "stdafx.h"

#include "decode.hpp"

typedef unsigned char byte;

byte dtable[256];

void decode(char *inbuf, int inbuflen, char *outbuf, long *decoded)
{
	int i;
	int processed = 0;
	*decoded = 0;

	for(i= 0;i<255;i++){
		dtable[i]= 0x80;
	}
	for(i= 'A';i<='I';i++){
		dtable[i]= 0+(i-'A');
	}
	for(i= 'J';i<='R';i++){
		dtable[i]= 9+(i-'J');
	}
	for(i= 'S';i<='Z';i++){
		dtable[i]= 18+(i-'S');
	}
	for(i= 'a';i<='i';i++){
		dtable[i]= 26+(i-'a');
	}
	for(i= 'j';i<='r';i++){
		dtable[i]= 35+(i-'j');
	}
	for(i= 's';i<='z';i++){
		dtable[i]= 44+(i-'s');
	}
	for(i= '0';i<='9';i++){
		dtable[i]= 52+(i-'0');
	}
	dtable['+']= 62;
	dtable['/']= 63;
	dtable['=']= 0;


	while(TRUE){
		if (processed > inbuflen) {
//			printf("decoded: %lu, processed: %lu, inbuflen: %lu\n", *decoded, processed, inbuflen);
			return;
		}
		byte a[4],b[4],o[3];

		for(i= 0;i<4;i++){
//			int c= insig();
			int c = inbuf[processed++];

//			if(c==EOF){
//				if(errcheck&&(i> 0)){
//					fprintf(stderr,"Input file incomplete.\n");
//					exit(1);
//				}
//				return;
//			}
			if(dtable[c]&0x80){
//				if(errcheck){
//					fprintf(stderr,"Illegal character '%c' in input file.\n",c);
//					exit(1);
//				}

				i--;
				if (processed > inbuflen) {
					return;
				}
				continue;
			}
			a[i]= (byte)c;
			b[i]= (byte)dtable[c];
		}
		o[0]= (b[0]<<2)|(b[1]>>4);
		o[1]= (b[1]<<4)|(b[2]>>2);
		o[2]= (b[2]<<6)|b[3];
		i= a[2]=='='?1:(a[3]=='='?2:3);
//		if(fwrite(o,i,1,fo)==EOF){
//			exit(1);
//		}
		memcpy(outbuf + *decoded, o, i);
		*decoded = *decoded + i;
		if(i<3){
			return;
		}
	}
}
