PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL  	; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF  	; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF   	; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF   	; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF  	; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF    	; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF	; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON   	; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF    	; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF  	; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF  	; Disable In-Circuit Debugger

GLOBAL student_id, sequence, flag, temp, paused
GLOBAL _t1, _t2, _t3
GLOBAL button_pressedE0, button_pressedE1, button_pressedE2, button_pressedE3, button_pressedE4,button_pressedE5, button_pressedE7
GLOBAL button_releasedE0, button_releasedE1, button_releasedE2, button_releasedE3, button_releasedE4,button_releasedE5, button_releasedE7

; Define space for the variables in RAM
PSECT udata_acs
student_id:
	DS 6
sequence:
	DS 1
flag:
	DS 1
temp:
	DS 1
paused:
	DS 1
_t1:
	DS 1
_t2:
	DS 1
_t3:
	DS 1
button_pressedE0:
	DS 1
button_pressedE1:
	DS 1
button_pressedE2:
	DS 1
button_pressedE3:
	DS 1
button_pressedE4:
	DS 1
button_pressedE5:
	DS 1
button_pressedE7:
	DS 1
button_releasedE0:
	DS 1
button_releasedE1:
	DS 1
button_releasedE2:
	DS 1
button_releasedE3:
	DS 1
button_releasedE4:
	DS 1
button_releasedE5:
	DS 1
button_releasedE7:
	DS 1


PSECT resetVec,class=CODE,reloc=2
resetVec:
	goto   	main

PSECT CODE
init:
	clrf TRISD
	clrf TRISC
	setf TRISE
    
	clrf sequence
	clrf flag
	clrf temp
	clrf paused
    
	clrf button_pressedE0
	clrf button_pressedE1
	clrf button_pressedE2
	clrf button_pressedE3
	clrf button_pressedE4    
	clrf button_pressedE5
	clrf button_pressedE7
    
	clrf button_releasedE0
	clrf button_releasedE1
	clrf button_releasedE2
	clrf button_releasedE3
	clrf button_releasedE4
	clrf button_releasedE5
	clrf button_releasedE7
    
	clrf LATC
	clrf LATD
    
	movlw 2
	movwf student_id  	; Store '2' in STUDENT_ID (First digit)
    
	movlw 5
	movwf student_id+1	; Store '5' in STUDENT_ID+1 (Second digit)
    
	movlw 4
	movwf student_id+2	; Store '4' in STUDENT_ID+2 (Third digit)
    
	movlw 7
	movwf student_id+3	; Store '7' in STUDENT_ID+3 (Fourth digit)
    
	movlw 6
	movwf student_id+4	; Store '6' in STUDENT_ID+4 (Fifth digit)
    
	movlw 5
	movwf student_id+5
    
    
	movlw 0xFF
	movwf LATC
	movwf LATD
    
	call busy_wait_1sec
	clrf LATC
	clrf LATD
    
	goto main
   

pause_loop:
    call blink_RD0       ; Ensure RD0 keeps blinking even when paused
    call check_buttons   ; Check for button presses
    btfsc paused, 0      ; Check if still paused
    goto pause_loop      ; If still paused, stay in pause loop
    goto main            ; If not paused, return to main loop
    
main:
	call blink_RD0 
	call check_buttons
    	 ; Ensure RD0 keeps blinking

   	btfsc paused, 0
	goto pause_loop
    	

    	call update_sequence
    	goto main



update_sequence:
    movf sequence, W   ; Load current index into W
    call get_sequence_value  ; Get the value at the current index
    movwf LATC         ; Display the value on PORTC

    ; Increment the sequence index
    incf sequence, F

    ; Check if sequence has exceeded the length of student_id (6 digits)
    movlw 6            ; Load 6 into W (since student_id has 6 digits)
    cpfslt sequence    ; Compare sequence with 6
    clrf sequence      ; If sequence >= 6, reset it to 0

    return

get_sequence_value:
    movf sequence, W     ; Load index into WREG
    call compute_address  ; Compute correct memory location
    movf INDF0, W        ; Fetch value from computed address
    return

compute_address:
    movlw HIGH student_id  ; Load high byte of student_id
    movwf FSR0H
    movlw LOW student_id   ; Load low byte of student_id
    addwf sequence, W      ; Add index (sequence) to base address
    movwf FSR0L            ; Store result in FSR0L
    return

    
blink_RD0:
    movf LATD,W
    xorlw 0x01
    movwf LATD
    call busy_wait_500msec
    return 


    
busy_wait_1sec:
	movlw 0x016
	movwf _t3   	; copy W into t3
	_loop3:
    	movlw 0x70  	; copy desired value to W
    	movwf _t2   	; copy W into t2
    	_loop2:
        	movlw 0x86  	; copy desired value to W
        	movwf _t1   	; copy W into t1
        	_loop1:
            	decfsz _t1, 1   ; decrement t1, if 0 skip next
            	goto _loop1 	; else keep counting down
            	decfsz _t2, 1   ; decrement t2, if 0 skip next
            	goto _loop2 	; else keep counting down
            	decfsz _t3, 1   ; decrement t3, if 0 skip next
            	goto _loop3 	; else keep counting down
            	return
    
	 
busy_wait_500msec:
	movlw 0x018
	movwf _t3   	; copy W into t3
	_loop6:
    	movlw 0x70
    	movwf _t2   	; copy W into t2
    	_loop5:
        	movlw 0x02
        	movwf _t1   	; copy W into t1
        	_loop4:
		call check_buttons
            	decfsz _t1, 1   ; decrement t1, if 0 skip next
            	goto _loop4 	; else keep counting down
            	decfsz _t2, 1   ; decrement t2, if 0 skip next
            	goto _loop5 	; else keep counting down
            	decfsz _t3, 1   ; decrement t3, if 0 skip next
            	goto _loop6 	; else keep counting down
            	return
   	 

check_buttons:
	movlw 0x00
	btfsc PORTE, 0
	movlw 0x01

	btfss PORTE, 0
	movff button_pressedE0, button_releasedE0
	movwf button_pressedE0
    
	btfsc button_releasedE0, 0
	call check_RE0

	movlw 0x00
	movwf button_releasedE0
    
    
	movlw 0x00
	btfsc PORTE, 1
	movlw 0x01

	btfss PORTE, 1
	movff button_pressedE1, button_releasedE1
	movwf button_pressedE1
    
	btfsc button_releasedE1, 0
	call check_RE1

	movlw 0x00
	movwf button_releasedE1
    
    
	movlw 0x00
	btfsc PORTE, 2
	movlw 0x01

	btfss PORTE, 2
	movff button_pressedE2, button_releasedE2
	movwf button_pressedE2
    
	btfsc button_releasedE2, 0
	call check_RE2

	movlw 0x00
	movwf button_releasedE2
    
    
	movlw 0x00
	btfsc PORTE, 3
	movlw 0x01

	btfss PORTE, 3
	movff button_pressedE3, button_releasedE3
	movwf button_pressedE3
    
	btfsc button_releasedE3, 0
	call check_RE3

	movlw 0x00
	movwf button_releasedE3
    
    
	movlw 0x00
	btfsc PORTE, 4
	movlw 0x01

	btfss PORTE, 4
	movff button_pressedE4, button_releasedE4
	movwf button_pressedE4
    
	btfsc button_releasedE4, 0
	call check_RE4

	movlw 0x00
	movwf button_releasedE4
    
	movlw 0x00
	btfsc PORTE, 5
	movlw 0x01

	btfss PORTE, 5
	movff button_pressedE5, button_releasedE5
	movwf button_pressedE5
    
	btfsc button_releasedE5, 0
	call check_RE5

	movlw 0x00
	movwf button_releasedE5
    
	movlw 0x00
	btfsc PORTE, 7
	movlw 0x01

	btfss PORTE, 7
	movff button_pressedE7, button_releasedE7
	movwf button_pressedE7
    
	btfsc button_releasedE7, 0
	call check_RE7

	movlw 0x00
	movwf button_releasedE7
    
	return
    
    
check_RE7:
	movlw 0x00
	btfss paused, 0
	movlw 0x01
	movwf paused
	btfsc paused, 0
	goto pause_loop
	return
    
check_RE0:
   movf student_id, W  ; Load the first digit
    incf WREG
    movwf student_id
    movlw 10
    cpfsgt student_id
    return
    clrf student_id
    return
	
check_RE1:
movf student_id+1, W
    incf WREG
    movwf student_id+1
    movlw 10
    cpfsgt student_id+1
    return
    clrf student_id+1
    return

check_RE2:
    movf student_id, W  ; Load the first digit
    incf WREG
    movwf student_id+2
    movlw 10
    cpfsgt student_id+2
    return
    clrf student_id+2
    return
	    
check_RE3:
    movf student_id, W  ; Load the first digit
    incf WREG
    movwf student_id+3
    movlw 10
    cpfsgt student_id+3
    return
    clrf student_id+3
    return
	
check_RE4:
    movf student_id, W  ; Load the first digit
    incf WREG
    movwf student_id+4
    movlw 10
    cpfsgt student_id+4
    return
    clrf student_id+4
    return

check_RE5:
    movf student_id, W  ; Load the first digit
    incf WREG
    movwf student_id+5
    movlw 10
    cpfsgt student_id+5
    return
    clrf student_id+5
    return



end resetVec


