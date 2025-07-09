// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))
#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

// Timer Related Definitions
#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)
// ============================ //
//             End              //
// ============================ //
#include <xc.h>

#include <stdint.h>

#define T_PRELOAD_LOW 0xB0
#define T_PRELOAD_HIGH 0x3C

// ============================ //
//        DEFINITIONS           //
// ============================ //

uint8_t prevD = 0;


// ============================ //
//          GLOBALS             //
// ============================ //


volatile unsigned int hippo_head_position = 7;
volatile unsigned int hippo_size = 1;
volatile unsigned int total_score=0;
volatile unsigned int round_score=100;
volatile unsigned char reset_flag=0;
volatile unsigned char prize_flag=0;
volatile unsigned int counter_for_led_of_prize = 0;
volatile unsigned int counter_for_gravitation = 0;
volatile unsigned int counter_for_round_score = 0;
volatile unsigned int bottom_limit = 7;
volatile unsigned int current_digit=0;
    unsigned char nums[10]={
    0x3F,0x06,0x5B,0x4F, 0x66,0x6D, 0x7D, 0x07, 0x7F, 0x6F};
    
volatile unsigned int soft_reset_flag = 0;
volatile unsigned int soft_reset_counter_until_5=1;
volatile unsigned int ticks_counter_400 = 0;

// ============================ //
//          FUNCTIONS           //
// ============================ //

void init_ports()
{
    TRISD = 0x00;// set d as output
    PORTD = 0x00;
    LATD = 0x00;//set d output pins with 0
    
    TRISH = 0x00;//set porth as output
    PORTH = 0x00;
    LATH = 0x00;
    
    TRISJ = 0x00;//set portj as output
    PORTJ = 0x00;
    LATJ = 0x00;
    
    TRISB = 0x01; //set PortB0 as input
    PORTB = 0x00;
    LATB = 0x00; 
    
    PORTH = 0x00;
    LATH = 0x00;
    
    PORTJ = 0x00;
    LATJ = 0x00;
}

void init_interrupt()
{
    
    INTCON = 0x00;
    INTCONbits.INT0IE = 1; // external interrupt from user press is enabled
    INTCON2bits.INTEDG0 = 1; // external interrupt from user press in raising edge
    INTCONbits.TMR0IE=1;
    INTCONbits.RBIE = 0;
    INTCONbits.GIE = 1;
    //INTCONbits.PEIE = 1;
    
    RCONbits.IPEN = 0;
    
    
    T0CON = 0x00;
    T0CONbits.TMR0ON = 1;
    T0CONbits.PSA = 1;
    // 40MHZ 25ns 100ns 5*10^4 ticks 2.5*10^6(2) 1.25*10^6(4) 6.25*10^5(8) 3.125*10^5(16) 1.5625*10^5(32) 78125(64) 39062.5(128)  
    // Pre-load the value
    TMR0H = T_PRELOAD_HIGH;
    TMR0L = T_PRELOAD_LOW;
    
}

void blink_led_for_prize()
{
    if (PORTDbits.RD0 == 1)
    {
        PORTDbits.RD0 = 0;
    }

    else
    {
        PORTDbits.RD0 = 1;
    }

}





void update_display(){    
    PORTDbits.RD1 = 0;
    PORTDbits.RD2 = 0;
    PORTDbits.RD3 = 0;
    PORTDbits.RD4 = 0;
    PORTDbits.RD5 = 0;
    PORTDbits.RD6 = 0;
    PORTDbits.RD7 = 0;
 
    
    for(unsigned int i = hippo_head_position ; i < hippo_head_position + hippo_size ; i++)
    {
        PORTD |= (1 << i);
    }
}


void soft_reset(){
    
    
    hippo_head_position = 7;
    hippo_head_position -= hippo_size;
    hippo_head_position += 1;

    counter_for_led_of_prize = 0;
    counter_for_gravitation = 0;
    counter_for_round_score = 0;

}

void hard_reset(){
    hippo_size = 1;
    soft_reset();
}


void check_if_at_prize()
{
    if (hippo_head_position==0)
    {
        total_score+=round_score;
        round_score = 100;
        
        if(hippo_size+1==6)
        {
            
            //add blinking
            soft_reset_flag=1;
            hard_reset();
 
        }
        else
        {
            
            hippo_size+=1;
            soft_reset_flag=1;
            soft_reset();
            LATDbits.LATD0 = 1; //sets prize bit to 1 again

        }

    }
}


void moveup()
{
    hippo_head_position--;
    check_if_at_prize();
    update_display();
}





void seven_segment_D0()
{
    PORTHbits.RH3=1;
    LATJ=nums[total_score%10];
}
void seven_segment_D1()
{
    PORTHbits.RH2=1;
    LATJ=nums[(total_score/10)%10];
}
void seven_segment_D2()
{
    PORTHbits.RH1=1;
    LATJ=nums[(total_score/100)%10];
}
void seven_segment_D3()
{
    PORTHbits.RH0=1;
    LATJ=nums[(total_score/1000)%10];
}
               




void gravititaion(){
    bottom_limit = 7 - hippo_size + 1;
    if (hippo_head_position < bottom_limit)
    {
        hippo_head_position += 1;
        update_display();
    } 
}



// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{
    

    // Timer overflowed (5 ms)
    if(INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        // Pre-load the value
        TMR0H = T_PRELOAD_HIGH;
        TMR0L = T_PRELOAD_LOW;
        if(soft_reset_flag==0){
            counter_for_led_of_prize ++;
            counter_for_gravitation ++;
            counter_for_round_score ++;
        }
        else{
        }
        if(counter_for_led_of_prize == 100)
        {
            counter_for_led_of_prize = 0;
            blink_led_for_prize();
        }
        if(counter_for_gravitation == 70)
        {
            counter_for_gravitation = 0;
            gravititaion();
        }
        if(counter_for_round_score == 200 && round_score>0)
        {
            counter_for_round_score = 0;
            round_score-=10;
        }
        
        
                PORTH=0x00;


        if(current_digit==0)
        {
            seven_segment_D0();
        }
        else if(current_digit==1)
        {
            seven_segment_D1();
        }
        else if(current_digit==2)
        {
            seven_segment_D2();
        }
        else if(current_digit==3)
        {
            seven_segment_D3();
        }

        current_digit=(current_digit+1)%4;

        
        if(soft_reset_flag == 1){
            


            ticks_counter_400++;
            if(soft_reset_counter_until_5%2==1){ //on state
                if(ticks_counter_400==80){
                    ticks_counter_400 = 0;
                    soft_reset_counter_until_5++;
                }
                LATD=0xFF;
                }
            else{
                if(ticks_counter_400==80){
                    ticks_counter_400 = 0;
                    soft_reset_counter_until_5++;
                }
                LATD=0x00;
            }
            if(soft_reset_counter_until_5==6){
                soft_reset_counter_until_5=0;
                update_display();
                soft_reset_flag = 0; 
                
            }

        }
        
        
        

    } 
    
    if(INTCONbits.INT0IF)
    {
        // Read the value to satisfy the interrupt
        //prevB = PORTB.RB0;
        INTCONbits.INT0IF = 0;
        
        if (soft_reset_flag == 0) { 
        moveup(); 
        }
        
    }
    


}






// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    
    
    init_ports();
    __delay_ms(1000); //can we use it? yah!
    init_interrupt();
    
    update_display();
    
    while(1){
        
    }
}
