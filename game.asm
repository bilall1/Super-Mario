.model small
.stack 200d
include macros2.inc

.data
xMario dw 0
yMario dw 270

xMarioCopy dw 0
yMarioCopy dw 270

hurdlePosxL dw 65
hurdle2PosxL dw 365
hurdlePosY dw 155
hurdle2PosxR dw 445
hurdle2PosY dw 175
hurdlePosxR dw 150
flagPos dw 580

isDrawFlagTrue dw 1
isDrawCastleTrue dw 1

whatLevel dw 0
xEnemy dw 270
yEnemy dw 290
xMonster dw 170
yMonter dw 0
yFire dw 80
xFire dw 0
myValue dw 0  ;used for storing distance
whereToMoveEnemy dw 0
whereToMoveMonster dw 1
toDrowFireOrNot dw 0
;somewhat used variables. To ensure gravity smoothness but doesn't work properly
downCheck dw 0    
upCheck dw 0
;checks to see if mario has eaten coin or not
coinEaten dw 0
diamondEaten dw 0
;score is added in this variable
;score dw 0

pcX dw ?
pcY dw ?
pcFound db ?

.code
main PROC
mov ax, @data
mov ds, ax

;Screen Mode Setting
mov ah,0h
mov al,10h
int 10h


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; EVERY RECTANGLE DRAWN IS WITH REFERENCE TO UPPER LEFT CORNER ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

call welcomeScreen
timeDelay 20, 0

changeBGcolor 1011b
drawAdvancedPlatform 
drawPipe 100, 200, 120
drawPipe 400, 220, 100
;call updateScore

InfiniteLoop:

	
	;checks to draw coin. On level 2 screen flickers so much b/c of enemy movement that coins are hardly visible
	.if coinEaten ==0
		drawCoin  120, 170
	.endif	
	
	
	.if diamondEaten ==0
		drawCollectable 415, 190
	.endif
	
	
	

	;This is level 2. Everything you do in level 2 is done here
	.if whatLevel == 2
		;moves monster to right if "whereToMoveMonster" value is 0
		.if whereToMoveMonster ==0
			push ax
			mov ax,xMonster
			add ax,5			;speed of the monster at top
			mov xMonster,ax
			pop ax
		.else
			push ax
			mov ax,xMonster
			sub ax,5			;speed of the monster at top
			mov xMonster,ax
			pop ax
		.endif
		;switches direction of monster
		.if xMonster>= 500		
			push cx
			mov cx,1
			mov whereToMoveMonster,cx
			pop cx
		
		.elseif xMonster<=5
			push cx
			mov cx,whereToMoveMonster
			mov cx,0
			mov whereToMoveMonster,cx
			pop cx
		.endif	
	.endif
	
	;Everything common in level 1&2 in dealt here
		PUSH bx
		PUSH cx
		;front collectable check
		mov bx, xMario
		add bx, 41
		mov pcX, bx
		mov cx, 40
		collisionLoop0:
			mov pcY, cx
			mov bx, yMario
			add pcY, bx
			call getPixelColor		;for removing the first collectable and changing background
			.if pcFound == 1110b
				mov coinEaten, 1
				.elseif pcFound == 1111b
				mov diamondEaten, 1
				.endif
		LOOP collisionLoop0
		POP cx
		POP bx
	.if whatLevel>=1
		;Collision detection here
		PUSH cx
		PUSH bx
		
		.if whatLevel == 2
			; Detecting for fire
			mov bx, yMario
			sub bx, 1
			mov pcY, bx
			mov cx, 50
			collisionLoop1:
				mov pcX, cx
				mov bx, xMario
				add pcX, bx
				call getPixelColor					;compare pixel
				.if pcFound == 0100b				;color matches with fire in range
					call youLostScreen				;end the game
					timeDelay 20, 0
					jmp endgame
				.endif
			LOOP collisionLoop1
		.endif
		
		;front enemy check && collectable check
		mov bx, xMario
		add bx, 41
		mov pcX, bx
		mov cx, 40
		collisionLoop2:
			mov pcY, cx
			mov bx, yMario
			add pcY, bx
			call getPixelColor
			.if pcFound == 0110b
				call youLostScreen
				timeDelay 20, 0
				jmp endgame
			.elseif pcFound == 1110b
				mov coinEaten, 1
				.elseif pcFound == 1111b
				mov diamondEaten, 1
				.endif
		LOOP collisionLoop2
		
		;back enemy check
		mov bx, xMario
		sub bx, 1
		mov pcX, bx
		mov cx, 40
		collisionLoop3:
			mov pcY, cx
			mov bx, yMario
			add pcY, bx
			call getPixelColor
			.if pcFound == 0110b
				call youLostScreen
				timeDelay 20, 0
				jmp endgame
			.endif
		LOOP collisionLoop3

		POP bx
		POP cx

		;moves enemy to right when value is 0
		.if whereToMoveEnemy ==0
			push ax
			mov ax,xEnemy
			add ax,5
			mov xEnemy,ax
			mov ax,xMonster
			pop ax
		.else
			push ax
			mov ax,xEnemy
			sub ax,5
			mov xEnemy,ax
			pop ax
		.endif
		;****** TO use .if use c++ comparision operators and you can only compare one variable and a register or one variable and a constant value e.g 10 *******;
		;switches direction of enemy
		mov ax,hurdle2PosxL
		mov dx,hurdlePosxR
		.if xEnemy>=ax
			push ax
			;mov ax,whereToMoveEnemy
			mov ax,1
			mov whereToMoveEnemy,ax
			pop ax
		.elseif xEnemy<= dx 
			push ax
			;mov ax,whereToMoveEnemy
			mov ax,0
			mov whereToMoveEnemy,ax
			pop ax
		.endif
	.endif
	
	;redrawing bg after movement;
	.if whereToMoveEnemy == 0
		PUSH ax
		mov ax, xEnemy
		sub ax, 5
		mov xEnemy, ax
		drawRectangle xEnemy, yEnemy, 35, 30, 1011b		;placing rectangles of background color 
		add ax, 5										;where enemy was placed ,moving backward
		mov xEnemy, ax
		POP ax
	.else
		drawRectangle xEnemy, yEnemy, 35, 30, 1011b		;placing rectangles of background color
	.endif												;where enemy was placed ,moving forward
	;redrawing bg after movement;

	;redrawing bg after movement;
	.if whereToMoveMonster == 0
		PUSH ax
		mov ax, xMonster
		sub ax, 5
		mov xMonster, ax
		drawRectangle xMonster, yMonter, 65, 60, 1011b	;placing rectangles of background color
		add ax, 5										;where monster was placed ,moving backward	
		mov xMonster, ax
		POP ax
	.else
		drawRectangle xMonster, yMonter, 65, 60, 1011b	;placing rectangles of background color
	.endif												;where monster was placed ,moving forward
	;redrawing bg after movement;

	.if whatLevel==1
		;changeBGcolor 1011b
		drawEnemy xEnemy, yEnemy
	.endif
	.if whatLevel ==2
		;changeBGcolor 1011b
		.if isDrawCastleTrue == 1
			drawCastle 515,190
			sub isDrawCastleTrue, 1
		.endif
		drawEnemy xEnemy, yEnemy
		drawMonster xMonster,yMonter
			
		;Gives initital coordinates to fire when value is 0. Value is only set to 0 when the fire reaches the ground.
		.if toDrowFireOrNot == 0
			push bx
			mov bx,yMonter
			add bx,80
			mov yFire,bx
			.if xMonster > 145
				.if xMonster < 365
					mov bx, xMonster
					add bx,20
					mov xFire,bx
				.endif
			.endif
;			mov toDrowFireOrNot,0
			pop bx
		.endif

		mov toDrowFireOrNot,1    ;when 1: Draws fire

		push ax
		mov ax,yFire
		add ax,5
		mov yFire,ax
		pop ax
		.if yFire>=250           ;Decrease 250 to increase range of fire drop
			mov toDrowFireOrNot,0
		.endif
		
		;draws fire only within range of two hurdles
		;***** This just stops from drawing. Coordinates may still be changing. This information is necessary in order to implement collsion detection****;
		;***** You can use drawFireOrNot variable to deal with this *****;
		.if toDrowFireOrNot == 1
			sub yFire, 5
			drawRectangle xFire, yFire, 20, 35, 1011b 	;placing rectangle at prev pos of fire
			add yFire, 5								
			drawFire xFire,yFire						;drawing fire at new location
		.else
			sub yFire, 5
			drawRectangle xFire, yFire, 20, 35, 1011b	; for removing the fire from bottom
		.endif
		
	.endif
	
	;Checks to redraw Background after movement
	PUSH ax
	mov ax, xMario
	.if xMarioCopy < ax
		drawRectangle xMarioCopy, yMario, 45, 50, 1011b	;placing rect at mario prev pos when moving forward
		add xMarioCopy, 5
	.endif
	mov ax, xMario
	.if xMarioCopy > ax
		drawRectangle xMario, yMario, 45, 50, 1011b		;placing rect at mario prev pos when moving backward
		sub xMarioCopy, 5
	.endif
	mov ax, yMario
	.if yMarioCopy > ax
		drawRectangle xMario, yMario, 40, 60, 1011b		;placing rect at mario prev pos when moving upward
		mov yMarioCopy, ax
	.endif
	mov ax, yMario
	.if yMarioCopy < ax
		drawRectangle xMario, yMarioCopy, 40, 55, 1011b	;placing rect at mario prev pos when moving downward
		mov yMarioCopy, ax
	.endif
	POP ax
	drawMarioBro xMario, yMario						;drawing mario
	;Checks to redraw Background after movement

	.if whatLevel<2 && isDrawFlagTrue==1
		drawFlag 550,50
		sub isDrawFlagTrue, 1
	.endif
	.if whatLevel==2 && isDrawFlagTrue==1
		sub isDrawFlagTrue, 1
	.endif
	; PARAMETERS -->> xAxis, yAxis, xLength, yLength, color
	;drawPlatform 200, 270, 100, 10, 0110b

	
	mov ah,11h
	int 16h
	jnz keyIsPressed
	jz keyIsNotPressed  ; Label was created to smoothen the gravity. But on v high cycles. It doesn't do very well
	keyIsNotPressed:
;		drawRectangle xMario, yMario, 40, 50, 1011b
		toMoveDown xMario,yMario,hurdlePosxL,hurdlePosxR,hurdlePosY,hurdle2PosxL,hurdle2PosxR,hurdle2PosY		;setting gravity
	mov upCheck,0   ; The not so used variable
		

	backLoop:
jmp InfiniteLoop

keyIsPressed:
;clears screen
;call clear

;Checking what key is pressed
 mov ah,10h
 int 16h
 
 ;checking right key
 cmp ah,4dh
 jne leftKey
 mov ax,xMario
 add ax,5
 
 ;The not so necessary variables again
 .if upCheck ==1 || downCheck ==0
 	toMoveAhead xMario,yMario,hurdlePosxL,hurdlePosxR,hurdlePosY,hurdle2PosxL,hurdle2PosxR,hurdle2PosY, flagPos
.endif
  
 ;checking left key
  leftKey:
	cmp ah,4bh
	jne upKey
	.if upCheck ==1 || downCheck ==0
		toMoveBack xMario,yMario,hurdlePosxR,hurdlePosY,hurdle2PosxR,hurdle2PosY
	.endif

 ;check up key
 upKey:
	cmp ah,48h
	jne downKey
	mov upCheck,1
	mov ax,yMario
	sub ax,10
	mov yMario,ax
	;changeBGcolor 1011b
 jmp backLoop

 
 ;check down key
 downKey:
	cmp ah,50h
	jne escape
	;dont need to move down using keys b/c gravity
	;toMoveDown xMario,yMario,hurdlePosxL,hurdlePosxR,hurdlePosY,hurdle2PosxL,hurdle2PosxR,hurdle2PosY
;check escape key
escape:
cmp ah,01h
jne backLoop
endgame:
call clear
.exit
main ENDp
clear proc uses ax
mov ah,0h
mov al,10h
int 10h
ret
clear endp
getPixelColor PROC
	POP si
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx

	mov ah, 0dh
	mov bh, 0
	mov cx, pcX
	mov dx, pcY
	int 10h
	mov pcFound, al

	POP dx
	POP cx
	POP bx
	POP ax
	PUSH si
	ret
getPixelColor ENDp

welcomeScreen PROC
	changeBGcolor 0000b
	setCursorPosition 36, 11
	writeCharacterOnScreen "W", 00001110b, 1
	setCursorPosition 37, 11
	writeCharacterOnScreen "E", 00001110b, 1
	setCursorPosition 38, 11
	writeCharacterOnScreen "L", 00001110b, 1
	setCursorPosition 39, 11
	writeCharacterOnScreen "C", 00001110b, 1
	setCursorPosition 40, 11
	writeCharacterOnScreen "O", 00001110b, 1
	setCursorPosition 41, 11
	writeCharacterOnScreen "M", 00001110b, 1
	setCursorPosition 42, 11
	writeCharacterOnScreen "E", 00001110b, 1
	setCursorPosition 36, 13
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 37, 13
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 38, 13
	writeCharacterOnScreen "V", 00001011b, 1
	setCursorPosition 39, 13
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 40, 13
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 42, 13
	writeCharacterOnScreen "1", 00001011b, 1
	ret
welcomeScreen ENDp

youLostScreen PROC
	changeBGcolor 0000b
	setCursorPosition 35, 12
	writeCharacterOnScreen "Y", 00000100b, 1
	setCursorPosition 36, 12
	writeCharacterOnScreen "O", 00000100b, 1
	setCursorPosition 37, 12
	writeCharacterOnScreen "U", 00000100b, 1
	setCursorPosition 39, 12
	writeCharacterOnScreen "L", 00000100b, 1
	setCursorPosition 40, 12
	writeCharacterOnScreen "O", 00000100b, 1
	setCursorPosition 41, 12
	writeCharacterOnScreen "S", 00000100b, 1
	setCursorPosition 42, 12
	writeCharacterOnScreen "T", 00000100b, 1
	ret
youLostScreen ENDp

levelTwoScreen PROC
	changeBGcolor 0000b
	setCursorPosition 36, 12
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 37, 12
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 38, 12
	writeCharacterOnScreen "V", 00001011b, 1
	setCursorPosition 39, 12
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 40, 12
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 42, 12
	writeCharacterOnScreen "2", 00001011b, 1
	ret
levelTwoScreen ENDp

levelThreeScreen PROC
	changeBGcolor 0000b
	setCursorPosition 36, 12
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 37, 12
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 38, 12
	writeCharacterOnScreen "V", 00001011b, 1
	setCursorPosition 39, 12
	writeCharacterOnScreen "E", 00001011b, 1
	setCursorPosition 40, 12
	writeCharacterOnScreen "L", 00001011b, 1
	setCursorPosition 42, 12
	writeCharacterOnScreen "3", 00001011b, 1
	ret
levelThreeScreen ENDp

youWonScreen PROC
	changeBGcolor 0000b
	setCursorPosition 36, 12
	writeCharacterOnScreen "H", 00001011b, 1
	setCursorPosition 37, 12
	writeCharacterOnScreen "U", 00001011b, 1
	setCursorPosition 38, 12
	writeCharacterOnScreen "R", 00001011b, 1
	setCursorPosition 39, 12
	writeCharacterOnScreen "R", 00001011b, 1
	setCursorPosition 40, 12
	writeCharacterOnScreen "A", 00001011b, 1
	setCursorPosition 41, 12
	writeCharacterOnScreen "Y", 00001011b, 1
	setCursorPosition 42, 12
	writeCharacterOnScreen "!", 00001011b, 1
	ret
youWonScreen ENDp

; updateScore PROC
	; setCursorPosition 5, 0
	; writeCharacterOnScreen "S", 00001111b, 1
	; setCursorPosition 6, 0
	; writeCharacterOnScreen "C", 00001111b, 1
	; setCursorPosition 7, 0
	; writeCharacterOnScreen "O", 00001111b, 1
	; setCursorPosition 8, 0
	; writeCharacterOnScreen "R", 00001111b, 1
	; setCursorPosition 9, 0
	; writeCharacterOnScreen "E", 00001111b, 1
	; setCursorPosition 10, 0
	; writeCharacterOnScreen ":", 00001111b, 1
	; setCursorPosition 12, 0
	; writeCharacterOnScreen "0", 00001111b, 1
	; ret
; updateScore ENDp

end main