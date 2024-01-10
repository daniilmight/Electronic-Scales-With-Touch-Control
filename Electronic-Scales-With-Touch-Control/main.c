#define F_CPU 8000000UL
#define coord_x_y 0
#define base 5
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "glcd.h"
void init(void);
void timer_init(void);
void init_adc(void);
void get_mass(void);
void show_mass(void);
void scan_key(void);

volatile unsigned char sec_flag=1, new_temp_flag=0;
volatile unsigned int ms_counter=0;
unsigned long long int temp_code_ADC0=0;
volatile unsigned char temp_ADC=0;
volatile unsigned int t_ADC=0;
volatile unsigned char mode=0; // ���������� ������ ���������
unsigned char adc[4]={0,0,0,0};
// ���������� ������� ������ ������
volatile unsigned char gread_key=0, gkey_up_counter=0, gkey_status=0, gkey_down_counter=0;
volatile unsigned char gkey=0,isOn=0,isOn1=0,isOn2=0;
volatile int yl = 0, yr = 0, prov=0;
// ���������� ��� �������� ������� ��������� ����� �������
volatile unsigned int x_coordinate=0;
volatile unsigned int y_coordinate=0;


int main(void)
{
	asm ("cli");		//������ ���������� ���������
	init();				//������������� ������
	// ��������� ��������������� ������������������ ������������� �������
	_delay_ms(100);
	glcd_off();			//��������� �������
	_delay_ms(100);		//��������
	glcd_on();			//�������� ����� ��������
	glcd_clear();		//������� �������
	//
	timer_init();		//������������� ������� �������� (���������� ��� ����������, �������� 8 ��)
	init_adc();			//������������� ��� (��� � ���������� �������)

	// ����������� ��������
	rectangle(0,0,126,62,0,1);	//��������� �������� �������
	
	// ����� ��������������� ����������� "������"
	rectangle(86, 11, 121, 25, 0, 1); //kg
	rectangle(86, 28, 121, 42, 0, 1); //lbs
	rectangle(86, 45, 121, 59, 0, 1); //ct

	// ����� ��������� ��������
	glcd_puts("M=",base,1,0,1,1);

	//����� ��������� �������� ��������
	glcd_putchar((adc[3]+48),base+8*3,1,0,1);
	glcd_putchar((adc[2]+48),base+8*4,1,2,1);
	glcd_putchar((adc[1]+48),base+8*5,1,2,1);
	glcd_puts(".",base+8*5,1,0,1,1);
	glcd_putchar((adc[0]+48),base+8*7,1,2,1);
	glcd_puts("kg ",60,2,0,1,1);

	// ��������� ����� � ����� ����������� "������"
	glcd_puts("Kg",96,2,0,1,1);
	glcd_puts("Lbs",93,4,0,1,1);
	glcd_puts("St",96,6,0,1,1);
	
	
	
	line(45, 40, 45, 50, 0, 1);
	line(45, 50, 35, 58, 0, 1);
	line(45, 50, 55, 58, 0, 1);
	line(35, 58, 55, 58, 0, 1);

	asm ("sei");	//��������� ����������

	// �������� ���������
	while(1)
	{
		if ((new_temp_flag==1)){
			show_mass();
			new_temp_flag=0;
		}
		
		if ((gkey_status&0b10000000)!=0)
		{
			gkey_status=gkey_status&0b01000000;
			if (gkey==0){
				PORTB^=(1<<PB7);
				mode=0; // ����������
				glcd_puts("kg ",60,2,0,1,1);
			}
			if (gkey==1){
				PORTB^=(1<<PB6);
				mode=1; // �����
				glcd_puts("lbs",60,2,0,1,1);
			}
			if (gkey==2){
				PORTB^=(1<<PB5);
				mode=2; // ������
				glcd_puts("st ",60,2,0,1,1);
			}
		}
	}
	return 0;
}

ISR (TIMER0_COMP_vect)
{
	ms_counter++; // ������� ����������
	
	if(ms_counter==10)
	{
		ms_counter=0;
		sec_flag=1; //���������� ���� �������
		scan_key();
		if (new_temp_flag==0){ //��������� ���� ����������� ����������� �������� �����
			get_mass(); // ������� �������� ���� ��� ������ ��������� �����
			new_temp_flag=1;
		}
	}
}

//	�������	 ����������� ��������� �������� �����
void show_mass (void){
	ADMUX=0b00000101;
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���
	
	// �������� ������ ������ � ���������� �������� ����� � ��������� �������� ���������
	if (mode==0) temp_code_ADC0=t_ADC * 0.9775171065493646;	//��������� kg
	
	if (mode==1) temp_code_ADC0=t_ADC * 2.155425219941349;	//��������� lbs
	
	if (mode==2) temp_code_ADC0=t_ADC * 0.15393970181879757;	//��������� st

	//����������� ����������� "�����"
	yl = 37 + t_ADC / 139;
	yr = 43 - t_ADC / 139;

	if (prov != yl) {
		for (int i = 1; i < 5; i++) {
			line(45, 40, 30, yl + i, 0, 0);
			line(45, 40, 30, yl - i, 0, 0);			line(45, 40, 60, yr + i, 0, 0);
			line(45, 40, 60, yr - i, 0, 0);
		}
		for (int w = 5; w < 13; w++) {
			line(27, yl + w, 33, yl + w, 0, 0);

			line(57, yr + w, 63, yr + w, 0, 0);
		}
		line(60, yr + 2, 60, yr + 10, 0, 0);

		line(30, yl + 2, 30, yl + 10, 0, 0);
	}
	line(47, 40, 47, 50, 0, 0);

	line(45, 40, 30, yl, 0, 1);

	line(45, 40, 60, yr, 0, 1);

	line(60, yr + 3, 60, yr + 9, 0, 1);

	line(30, yl + 3, 30, yl + 9, 0, 1);

	line(27, yl + 9, 33, yl + 9, 0, 1);

	line(57, yr + 9, 63, yr + 9, 0, 1);

	// ����������� ���� ���������� ��������� ("��������" �������� �������)
	glcd_putchar((temp_code_ADC0%10+48),base+8*7,1,0,1); //������� ���������� ������ ���������� ���������
	temp_code_ADC0/=10;
	glcd_putchar((temp_code_ADC0%10+48),base+8*6,1,0,1);
	temp_code_ADC0/=10;
	glcd_putchar((temp_code_ADC0%10+48),base+8*4,1,0,1);
	temp_code_ADC0/=10;
	glcd_putchar((temp_code_ADC0%10+48),base+8*3,1,0,1); //������� ���������� ������ ���������� ���������
	
	prov = yl;
}

void init()
{
	PORTA=0b00000100;
	DDRA=0b00001100;
	PORTB=0b00000000;
	DDRB=0b11100011;
	PORTD=0b10000000;
	DDRD=0b11000000;
	DDRC=0b11111111;
	
}

void timer_init(void)
{
	//��������� �� ������������ �/�0 � ���������� 8 ��
	TCNT0=0b00000000;	//������� �/�0
	OCR0=249;			//������ ��������� 249 � ������� ��������� �/�0
	TCCR0=0b00001100;	//����� "����� ��� ����������" � �������� �� 256
	TIMSK=0b00000010;  //���������� ���������� �� ���������� �� �/�0
	TIFR=0b00000011;   //������� ������ ���������� �/�0
}
void init_adc(void)
{
	ADMUX=0b00000000;	//
	ADCSRA=0b10000111;
}

void scan_key(void){
	PORTA=0b00000100;       //
	_delay_ms(2);			// �������� !!! (���������� ������� ������ ����������)
	ADMUX=0b00000000;		// �������� ������� ����� ���
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���

	// ���������� ���������� �� ����������� �������
	x_coordinate=ADC;

	PORTA=0b00001000;
	_delay_ms(2);
	ADMUX=0b00000001;
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���

	// ���������� ���������� �� ����������� �������
	y_coordinate=ADC;

	if (((y_coordinate>480)&&(y_coordinate<620))&&((x_coordinate>550)&&(x_coordinate<770))&&(gkey_status==0)){
		gkey_status=0b11000000;
		gkey=0;
	}
	if (((y_coordinate>320)&&(y_coordinate<460))&&((x_coordinate>550)&&(x_coordinate<770))&&(gkey_status==0)){
		gkey_status=0b11000000;
		gkey=1;
	}
	if (((y_coordinate>160)&&(y_coordinate<290))&&((x_coordinate>550)&&(x_coordinate<770))&&(gkey_status==0)){
		gkey_status=0b11000000;
		gkey=2;
	}
	if (((y_coordinate<20)&&(x_coordinate<20))&&(gkey_status&0b01000000)!=0){
		gkey_status=gkey_status&0b10000000;
	}

}
void get_mass (void){ //������� ������ ������ ���
	// ���������� Y
	ADMUX=0b00000101;
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���
	t_ADC=ADC;
}