#include <REGtenxTM52F5288.H>

#define C1 P3_0
#define C2 P3_1
#define C3 P3_2
#define C4 P3_3

#define R1 P3_4
#define R2 P3_5
#define R3 P3_6
#define R4 P3_7

//lcm
#define LCM_DATA_BUS P2
sbit LCM_RS=P0^5;
sbit LCM_RW=P0^6;
sbit LCM_EN=P0^7;
sbit speaker = P4^0; 

//subrountine
unsigned char scankey();
void delay8us(char);
void music();

//lcm
void LCM_Write_CTRL(unsigned char);
void LCM_Write_DATA(unsigned char);
void ctrlup(unsigned char);
void ctrldown(unsigned char);
void LCM_initial(void);
void Delay_LCM(unsigned char);

//data
unsigned char key;
unsigned char IDindex = 0;
unsigned char vth = 3;

unsigned char BIR[16] = {'V','t','h','=','x',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};//6
unsigned char ShowV[16] = {'V','=',' ',' ',' ',' ',' ','V',' ',' ',' ',' ',' ',' ',' ',' '};//6
unsigned char SetV[16] = {'S','e','t',' ','N','e','w',' ','V','t','h',':',' ',' ',' ',' '};//6
unsigned int vin = 0;
unsigned int cmp = 0;
unsigned char action = 0;
unsigned char temp = 0;

#define LED P4
void main(void)
{
	unsigned char buf = 0;
	unsigned char temp = 0;
	unsigned char vthtemp = vth;
	
	//TIMER SPEED
	CLKCON=0x22;
	CLKCON=0x26;
	//IO ??
	P0OE = 0xff;
	P1MODH=0xC0; //P1.7 ADCPIN Mode3
	P2OE=0xFF; //P2  LED 
	P3 = 0xff;
	P4 = 0xff;
	//ADC ??
	OPTION=0x00; // ADC 
	CHSEL= 0x00;
	
	//lcm
	LCM_initial();
	ctrlup(IDindex);
	while(1)
	{
	
		ADSOC=1; //ADC 
		while(ADSOC==1); // ADC (ADSOC  0)
		vin = ADCDH*2;
		if(vin<=9)
		{
			ShowV[2] = '0';
			ShowV[3] = '.';
			ShowV[4] = '0';
			ShowV[5] = ADCDH+'0';
		}
		else 	if(vin<=99)
		{
			temp = 0;
			temp = vin/10;
			ShowV[2] = '0';
			ShowV[3] = '.';
			ShowV[4] = temp + '0';
			temp = vin%10;
			ShowV[5] = temp+'0';
		}
		else
		{
			temp = 0;
			temp = vin/100;
			ShowV[2] = temp +'0';
			ShowV[3] = '.';
			temp = vin%100;
			temp = temp/10;
			ShowV[4] = temp + '0';
			temp = vin%100;
			temp = temp%10;
			ShowV[5] = temp+'0';
		}
		key = scankey();
		if(key!=buf)
			buf = key;
		if(buf!=0xff)
		{
			P4 = ~buf;
			//vth = buf;
			if(buf=='*')
			{
				action = action + 1;
				if(action>1)
				{
					action = 0;
					//vth = temp;
				}
			}
			if(buf>=0&&buf<=9)
			{
				vth = buf;
				
			}
		}
		if(action==0)
		{
			BIR[4] = vthtemp+'0';
		}
		else
		{
			vthtemp = vth;
			BIR[4] = vthtemp+'0';
		}
		SetV[12] = vth+'0';
		cmp = vth*100;
		if(vin>=cmp)
			music();
		ctrlup(0);
		ctrldown(IDindex);
	} 
}

void music()
{
	unsigned char t[] = {108,102,91,86,77,68,61,57};
	unsigned tone = 0;
	unsigned music = 0;
			while(tone<=4)
			{
				for(music = 0;music<110;music++)
				{
					speaker = 1;
					delay8us(t[tone]);
					speaker = 0;
					delay8us(t[tone]);
				}
				tone = tone + 1;
			}
}

void delay8us(unsigned char x)
{
	unsigned char i,j;
	for(i=0;i<x;i++)
		for(j=0;j<1;j++);
}

void LCM_Write_CTRL(unsigned char CMD) //?? Command 
{
	LCM_RS=0; // 0: 1:
	LCM_RW=0; // 0: 1:
	LCM_DATA_BUS=CMD; // Command  LCM_DATA_BUS
	LCM_EN=1; //LCM 
	Delay_LCM(100); //
	LCM_EN=0; //LCM 
	Delay_LCM(1);
}

void LCM_Write_DATA(unsigned char DATA) // DATA 
{
	LCM_RS=1; // 0: 1:
	LCM_RW=0; // 0: 1:
	LCM_DATA_BUS=DATA; // DATA  LCM_DATA_BUS
	LCM_EN=1; // LCM 
	Delay_LCM(20); //
	LCM_EN=0; // LCM 
	Delay_LCM(1);
}

void LCM_initial(void) //LCM 
{
	LCM_Write_CTRL(0x30); // Command  0x30,datasheet  3 ?
	LCM_Write_CTRL(0x30); // LCM ?????
	LCM_Write_CTRL(0x30);
	
	LCM_Write_CTRL(0x38); // Command  0x38  
	Delay_LCM(100);
	LCM_Write_CTRL(0x08); // Command  0x08
	Delay_LCM(100);
	LCM_Write_CTRL(0x01); // Command  0x01 
	Delay_LCM(100);
	LCM_Write_CTRL(0x06); // Command  0x06 
	Delay_LCM(100);
	LCM_Write_CTRL(0x0E); // Command 0x0E 
	Delay_LCM(100);
}

void Delay_LCM(unsigned char time)
{
	unsigned int n;
	while(time>0)
	{
		n=4; 
		while(n>0) 
			n--;
		time--;
	}
}

void ctrlup(unsigned char index){
	unsigned char n;
	LCM_Write_CTRL(0x80);
	if(action==0)
	{
	for(n=0;n<16;n++){
		LCM_Write_DATA(BIR[index++]);
		index = index & 15;
	}
	}
	else
	{
		for(n=0;n<16;n++){
		LCM_Write_DATA(SetV[index++]);
		index = index & 15;
	}
	}
}

void ctrldown(unsigned char index){
	unsigned char n;
	LCM_Write_CTRL(0xC0);
	

	for(n=0;n<16;n++){
		LCM_Write_DATA(ShowV[index++]);
		index = index & 15;
	}

	
}	


unsigned char scankey()
{
		//colloum 1....1,4,7,*
		C2 = 1;
		C3 = 1;
		C4 = 1;
		C1 = 0;
		if(R1 == 0)
		{
			C1 = 1;
			return 1;
		}
		else if(R2 == 0)
		{
		C1 = 1;
		return 4;
		}
		else if(R3 == 0)
		{
			C1 = 1;
			return 7;
		}
		else if(R4 == 0)
		{
			C1 = 1;
			return '*';
		}
		else
		{
			C1 = 1;
		}
		
		//Coloum 2....2,5,8,0
		C2 = 0;
		C3 = 1;
		C4 = 1;
		C1 = 1;
		if(R1 == 0)
		{
			C2 = 1;
			return 2;
		}
		else if(R2 == 0)
		{
			C2 = 1;
			return 5;
		}
		else if(R3 == 0)
		{
			C2 = 1;
			return 8;
		}
		else if(R4 == 0)
		{
			C2 = 1;
			return 0;
		}
		
		//Coloum 3....3,6,9,#
		C2 = 1;
		C3 = 0;
		C4 = 1;
		C1 = 1;
		if(R1 == 0)
		{
			C3 = 1;
			return 3;
		}
		else if(R2 == 0)
		{
			C3 = 1;
			return 6;
		}
		else if(R3 == 0)
		{
			C3 = 1;
			return 9;
		}
		else if(R4 == 0)
		{
			C3 = 1;
			return '#';
		}
		
		//Coloum 4....A,B,C,D
		C2 = 1;
		C3 = 1;
		C4 = 0;
		C1 = 1;
		if(R1 == 0)
		{
			C4 = 1;
			return 'A';
		}
		else if(R2 == 0)
		{
			C4 = 1;
			return 'B';
		}
		else if(R3 == 0)
		{
			C4 = 1;
			return 'C';
		}
		else if(R4 == 0)
		{
			C4 = 1;
			return 'D';
		}
		else
		{
			C4 = 1;
		}
		
		return 0xff;
}