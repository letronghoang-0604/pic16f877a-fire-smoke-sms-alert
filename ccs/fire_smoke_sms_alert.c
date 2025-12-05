#include <16F887.h>
#device ADC=10
#fuses      HS
#use delay(crystal=20000000)

#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)

#define LCD_ENABLE_PIN PIN_b2
#define LCD_RS_PIN PIN_b4
#define LCD_RW_PIN PIN_b3

#define LCD_DATA4 PIN_D7
#define LCD_DATA5 PIN_D6
#define LCD_DATA6 PIN_D5
#define LCD_DATA7 PIN_D4

#include <lcd.c>        
 
#define     loa         pin_d2         // 
#define     mq2         pin_a1

void doc_lm35();
int1  tt,tt1,tt2;
unsigned int8 i;
unsigned int32 kq0,nd;

void xoa_tin_nhan()
{
     printf("AT+CMGD=1,4\r\n");
     delay_ms(500);
}

void gui_tin_nhan(unsigned char tn)
{
   printf("AT+CMGS=\"+84903240604\"\r\n");
   delay_ms(500);
   switch(tn)
   {
      case 1:     printf("HE THONG BAT DAU HOAT DONG");
                  break;
      case 2:     printf("CANH BAO QUA NHIET: %ld - CANH BAO KHI GAS",nd);
                  break;
      case 3:     printf("CANH BAO QUA NHIET: %ld", nd);
                  break;
      case 4:     printf("CANH BAO KHI GAS");
                  break;             
      case 5:     printf("NHIET DO:%ld", nd);          
                  break;           
   }
     delay_ms(500);
     putc(26);
     delay_ms(500);
}

void khoi_dong_sim()
{
   lcd_gotoxy(1,1);
   lcd_putc("KHOI DONG SIM...");
   lcd_gotoxy(1,2);
   lcd_putc("  VUI LONG CHO  ");
   delay_ms(10000);
   
   printf("AT\r\n");                  delay_ms(1000);
   printf("ATE1\r\n");                delay_ms(1000); 
   printf("AT+IPR=9600\r\n");         delay_ms(1000);
   printf("AT&W\r\n");                delay_ms(1000); 
   printf("AT+CMGF=1\r\n");           delay_ms(1000);
   printf("AT+CNMI=2,2,0,0,0\r\n");   delay_ms(1000);
   printf("AT+CMGD=1,4\r\n");         delay_ms(1000); 
   
   delay_ms(200);
   gui_tin_nhan(1);
}

void doc_lm35()
{
   kq0=0;              
   for(i=0;i<100;i++)
   {
      kq0 = kq0 + READ_ADC();       
      delay_ms(1);
   }
   kq0 = kq0/100;            
   kq0=kq0*5000/1023/10;     
   nd=kq0;
   lcd_gotoxy(1,1);
   lcd_putc("NHIET DO:");
   lcd_putc(nd/10+0x30);        
   lcd_putc(nd%10+0x30);         
   lcd_putc(0xdf);         
   lcd_putc("C    ");
}


void main()
{
   SET_TRIS_A(0Xff);       
   SET_TRIS_e(0Xff);
   SET_TRIS_d(0X00);
   set_tris_b(0x00);   
   set_tris_c(0x80);         
   
   lcd_init();               
   output_low(loa);          
   lcd_gotoxy(1,1);
   lcd_putc(" MACH CANH BAO  ");
   lcd_gotoxy(1,2);
   lcd_putc("NHIET DO-KHI GAS");
   delay_ms(4000);
   lcd_gotoxy(1,1);
   lcd_putc(" NAME  ");
   lcd_gotoxy(1,2);
   lcd_putc("   ID     ");
   delay_ms(4000);
   
   khoi_dong_sim();                
   
   SETUP_ADC(ADC_CLOCK_DIV_32);
   SETUP_ADC_PORTS(sAN0);
   SET_ADC_CHANNEL(0);
  
   delay_ms(20);
   tt=tt1=tt2=1;       
   
   output_low(loa);
   while(true)
   {    
      doc_lm35();
      if((nd>40)&&(input(mq2)==0))       
      {
         output_high(loa);      
         lcd_gotoxy(1,2);
         lcd_putc("QUA NHIET-KHI GA");
         tt1=tt2=1;           
         if(tt==1)                
         {
            tt=0;                         
            gui_tin_nhan(2);             
         }
      }
      else if(nd>40)      
      {
         output_high(loa);
         lcd_gotoxy(1,2);
         lcd_putc("    QUA NHIET    ");
         tt=tt2=1;
         if(tt1==1)
         {             
            gui_tin_nhan(3);               
            tt1=0;
         }
      }
      else if(input(mq2)==0)    
      {
         output_high(loa);
         lcd_gotoxy(1,2);
         lcd_putc("    KHI GAS    ");
         tt=tt1=1;
         if(tt2==1)
         {          
            gui_tin_nhan(4);               
            tt2=0;
         }
      }
      else           
      {
         output_low(loa);
         lcd_gotoxy(1,2);
         lcd_putc("  BINH THUONG   ");
         tt=tt1=tt2=1;
      }
   }
}
/***************************************************************/
