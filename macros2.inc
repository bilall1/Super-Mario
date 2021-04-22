printChar MACRO char ;Parameter in char form e.g. "a"
	PUSH ax
	PUSH dx
	
	mov ah, 02h
	mov dx, char
	int 21h
	
	POP dx
	POP ax
ENDm

setCursorPosition MACRO xAxis, yAxis
	PUSH ax
	PUSH bx
	PUSH dx
	
	mov ah, 02h
	mov dx, yAxis
	mov dh, dl
	mov bx, xAxis
	mov dl, bl
	mov bh, 0
	int 10h
	
	POP dx
	POP bx
	POP ax
ENDm

writeCharacterOnScreen MACRO char, attribute, count
	PUSH ax
	PUSH bx
	PUSH cx
	
	mov ah, 09h
	mov al, char
	mov bh, 0
	mov bl, attribute
	mov cx, count
	int 10h
	
	POP cx
	POP bx
	POP ax
ENDm

clearScreen MACRO
	PUSH ax
	
	mov ax, 10h
	int 10h
	
	POP ax
ENDm

changeBGcolor MACRO bgColor ;Parameter in binary
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	
	mov ah, 06h
	mov al, 0
	mov ch, 0
	mov cl, 0
	mov dh, 24
	mov dl, 79
	mov bh, bgColor
	int 10h
	
	POP dx
	POP cx
	POP bx
	POP ax
ENDm

timeDelay MACRO milliSec, microSec ;Parameters in hexa
	PUSH ax
	PUSH cx
	PUSH dx

	mov cx, milliSec
	mov dx, microSec
	mov ah, 86h
	mov al, 0
	int 15h
	
	POP dx
	POP cx
	POP ax
ENDm

drawHorizontalLine MACRO xAxis, yAxis, len, color ;color Parameter in binary
	LOCAL lineLoop
	PUSH ax
	PUSH cx
	PUSH dx

	mov cx, len
	mov dx, yAxis
	lineLoop:
		add cx, xAxis
		mov ah, 0ch
		mov al, color
		int 10h
		sub cx, xAxis
	LOOP lineLoop
		
	POP dx
	POP cx
	POP ax
ENDm

drawRectangle MACRO xAxis, yAxis, xLength, yLength, color ;color Parameter in binary
	LOCAL squareLoop
	PUSH cx
	PUSH dx
	
	mov cx, yLength
	mov dx, yAxis
	squareLoop:
		drawHorizontalLine xAxis, dx, xLength, color
		add dx, 1
	LOOP squareLoop
	
	POP dx
	POP cx
ENDm

drawFlag MACRO xAxis, yAxis ;color Parameter in binary
	LOCAL var1, var2
	PUSH ax
	PUSH bx
	.data
	var1 WORD ?
	var2 WORD ?
	.code
	drawRectangle xAxis, yAxis, 80, 45, 0010b
	drawRectangle xAxis, yAxis, 20, 45, 1111b
	mov ax, xAxis
	add ax, 40
	mov bl, 8
	div bl
	mov ah, 0
	mov var1, ax
	mov ax, yAxis
	add ax, 24
	mov bl, 14
	div bl
	mov ah, 0
	mov var2, ax
	setCursorPosition var1, var2
	writeCharacterOnScreen '(', 9h, 1
	mov ax, xAxis
	add ax, 48
	mov bl, 8
	div bl
	mov ah, 0
	mov var1, ax
	setCursorPosition var1, var2
	writeCharacterOnScreen '*', 7Fh, 1
	
	POP bx
	POP ax
ENDm

toMoveAhead MACRO xMario, yMario, xHurdleL,xHurdleR,yHurdle,xHurdle2L,xHurdle2R,yHurdle2, flagPos
	
	LOCAL compareHurdle1
	LOCAL moveAhead
	LOCAL subi
	mov ax, xMario
	add ax, 5

	cmp ax,flagPos
	JE nextLevel
	cmp ax,xHurdleR
	JL compareHurdle1

	mov cx,yHurdle2
	cmp cx,yMario
	JG moveAhead
	
	cmp ax,xHurdle2L
	JE subi	
compareHurdle1:
	cmp ax,xHurdleL ;this check is to stop mario from going ahead if there is an hurdle
	JNE moveAhead	
	mov cx, yHurdle
	cmp cx,yMario
	JG moveAhead
	
subi:
	sub ax,5
	;changeBGcolor 1011b
	jmp backLoop
 
moveAhead:
	mov xMario,ax
	;changeBGcolor 1011b
	jmp backLoop

nextLevel:
	add whatLevel,1
	.if whatLevel == 1
		call levelTwoScreen
		timeDelay 20,0
		changeBGcolor 1011b
		drawAdvancedPlatform 
		drawPipe 100, 200, 120
		drawPipe 400, 220, 100
		;call updateScore
	.elseif whatLevel == 2
		call levelThreeScreen
		timeDelay 20,0
		changeBGcolor 1011b
		drawAdvancedPlatform 
		drawPipe 100, 200, 120
		drawPipe 400, 220, 100
		;call updateScore
	.elseif whatLevel == 3
		call youWonScreen
		timeDelay 20, 0
		jmp endgame
	.endif
	add isDrawFlagTrue, 1
	mov coinEaten,0
	mov diamondEaten,0
	mov xMario,0
	mov xMarioCopy, 0
	mov yMario,260
	mov yMarioCopy, 260
	;changeBGcolor 1011b
	jmp backLoop
	
endm

toMoveBack MACRO xMario,yMario,xHurdleR,yHurdle,xHurdle2R,yHurdle2

	LOCAL moveBack
	LOCAL addi
	LOCAL compareHurdle1
	mov ax,xMario
	sub ax,5
	cmp ax,0
	JL addi
	
	cmp ax,xHurdleR
	JLE compareHurdle1
	
	mov cx, yHurdle2
	cmp cx,yMario
	JG moveBack
	cmp ax,xHurdle2R
	JE addi

compareHurdle1:
	cmp ax,xHurdleR  ;this check is to stop mario from going back if there is an hurdle
	JNE moveBack
	mov cx,yHurdle
	cmp cx,yMario
	JG moveBack
	addi:
		add ax,5
		;changeBGcolor 1011b
		jmp backLoop
 moveBack:
	mov xMario,ax
	;changeBGcolor 1011b
	jmp backLoop

endm

toMoveDown MACRO xMario, yMario, xHurdleL,xHurdleR,yHurdle, xHurdle2L, xHurdle2R,yHurdle2

	LOCAL subt
	LOCAL moveDown
	LOCAL compareHurdle1
	mov downCheck,1
	.if yMario == 270
		mov downCheck,0
	.endif
	mov ax,yMario
	add ax,5
	cmp ax,275
	JE subt
	mov dx,xMario
	cmp dx,xHurdleR
	JL compareHurdle1
;hurdle2 checks
	mov cx,xHurdle2R
	cmp xMario,cx
	JG moveDown
	mov cx, xHurdle2L
	cmp xMario,cx
	JLE moveDown
	cmp ax,yHurdle2
	JE subt

;hurdle2 checks end

compareHurdle1:
	cmp ax,yHurdle
	JNE moveDown
	mov cx,xHurdleR
	cmp xMario,cx
	JG moveDown
	mov cx,xHurdleL
	cmp xMario,cx
	JL moveDown
	subt:
		sub ax,5
		;changeBGcolor 1011b
		jmp backLoop
 moveDown:
	mov yMario,ax
	;changeBGcolor 1011b
	jmp backLoop
endm


drawPipe MACRO xAxis, yAxis, height	;width = 50
	LOCAL var1, var2, var3
	PUSH ax
	PUSH bx
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	.code
	mov ax, height
	mov bl, 3
	div bl
	mov ah, 0
	mov var3, ax
	mov ax, yAxis
	add ax, var3
	mov var2, ax
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	drawRectangle xAxis, yAxis, 50, var3, 1010b	;main box
	drawRectangle var1, yAxis, 3, var3, 0010b	;first color
	add ax, 10
	mov var1, ax
	drawRectangle var1, yAxis, 2, var3, 0010b	;second color
	add ax, 7
	mov var1, ax
	drawRectangle var1, yAxis, 10, var3, 0010b	;third color
	drawRectangle xAxis, yAxis, 2, var3, 0000b	;border left
	drawHorizontalLine xAxis, var2, 50, 0000b ;border bottom
	mov ax, xAxis
	add ax, 48
	mov var1, ax
	drawRectangle var1, yAxis, 2, var3, 0000b;border right
	drawHorizontalLine xAxis, yAxis, 50, 0000b ;border top

	mov bx, yAxis
	add bx, var3
	add bx, 1
	mov var2, bx
	mov ax, height
	sub ax, var3
	mov var3, ax
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	drawRectangle var1, var2, 40, var3, 1010b ;main box bottom
	mov ax, xAxis
	add ax, 13
	mov var1, ax
	drawRectangle var1, var2, 3, var3, 0010b ;first color
	add var1, 10
	drawRectangle var1, var2, 2, var3, 0010b ;second color
	add var1, 7
	drawRectangle var1, var2, 10, var3, 0010b;third color
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	drawRectangle var1, var2, 40, 3, 0000b	;shadow of top box
	drawRectangle var1, var2, 2, var3, 0000b	;border left
	mov ax, xAxis
	add ax, 43
	mov var1, ax
	drawRectangle var1, var2, 2, var3, 0000b	;border right
	mov ax, yAxis
	add ax, height
	sub ax, 1
	mov var2, ax
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	drawRectangle var1, var2, 40, 2, 0000b	;border bottom
	
	POP bx
	POP ax
ENDm
drawEnemy MACRO xAxis, yAxis
	LOCAL firstLoop, secondLoop, thirdLoop, len, startX, var1, var2
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	.data
	startX	WORD 14
	len		WORD 2
	var1		WORD ?
	var2		WORD ?
	.code

	mov ax, 14			;;;;MAIN UPPER TRIANGLE
	add ax, xAxis
	mov startX, ax
	mov len, 2
	mov cx, 7
	mov dx, yAxis
	firstLoop:
		sub startX, 2
		add len, 4
		PUSH cx
		mov cx, 2
		secondLoop:
			PUSH cx
			mov cx, len
			thirdLoop:
				add cx, startX
				mov ah, 0ch
				mov al, 0110b
				int 10h
				sub cx, startX
			LOOP thirdLoop
			add dx, 1
			POP cx
		LOOP secondLoop
		POP cx
	LOOP firstLoop			;;;;MAIN UPPER TRIANGLE

	mov ax, yAxis
	add ax, 14
	mov var2, ax
	drawRectangle xAxis, var2, 30, 2,0110b ;;;;TRIANGLE EXTENSION
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 8
	mov var2, ax
	drawRectangle var1, var2, 4, 6, 1111b	;;;;LEFT EYE SOCKET
	add var1, 1
	add var2, 2
	drawRectangle var1, var2, 3, 4, 0000b	;;;;LEFT EYE BALL
	add var1, 5
	sub var2, 2
	drawRectangle var1, var2, 4, 6, 1111b	;;;;RIGHT EYE SOCKET
	add var2, 2
	drawRectangle var1, var2, 3, 4, 0000b	;;;;RIGHT EYE BALL	
	mov ax, xAxis
	add ax, 2
	mov var1, ax
	mov ax, yAxis
	add ax, 16
	mov var2, ax
	drawRectangle var1, var2, 26, 2, 0110b	;;;;TRIANGLE EXTENSION 2
	add var2, 6
	drawRectangle xAxis, var2, 10, 8, 0000b	;;;;LEFT FOOT
	mov ax, xAxis
	add ax, 6
	mov var1, ax
	sub var2, 6
	drawRectangle var1, var2, 18, 2, 1110b	;;;;LOWER BODY
	sub var1, 2
	add var2, 2
	drawRectangle var1, var2, 22, 4, 1110b	;;;;LOWER BODY 2
	add var1, 2
	add var2, 4
	drawRectangle var1, var2, 18, 2, 1110b	;;;;LOWER BODY 3
	mov ax, xAxis
	add ax, 22
	mov var1, ax
	drawRectangle var1, var2, 8, 2, 0000b	;;;;RIGHT FOOT
	add var2, 2
	sub var1, 2
	drawRectangle var1, var2, 10, 6, 0000b	;;;;RIGHT FOOT

	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawCollectable MACRO xAxis, yAxis	;width = height = 20
	LOCAL loop1, loop2, var1, var2, var3
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	.code
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	
	mov ax, xAxis
	add ax, 10
	mov bx, yAxis
	mov cx, 10
	mov dx, 1
	loop1:
		mov var1, ax
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 1101b ; upperbody
		drawHorizontalLine var1, var2, 1, 1111b	;left border

		PUSH ax
		add ax, dx
		sub ax, 1
		mov var1, ax
		drawHorizontalLine var1, var2, 1, 1111b	;right border
		POP ax

		dec ax
		inc bx
		add dx, 2
	LOOP loop1
	
;	drawHorizontalLine var1, var2, 20, 1111b ; middle line
	
	mov ax, xAxis
	sub dx, 1
	mov cx, 10
	loop2:
		mov var1, ax
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 0101b ;lowerbody
		drawHorizontalLine var1, var2, 1, 1111b	;left body

		PUSH ax
		add ax, dx
		sub ax, 1
		mov var1, ax
		drawHorizontalLine var1, var2, 1, 1111b ;right body
		POP ax
		
		inc ax
		inc bx
		sub dx, 2
	LOOP loop2
	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawCoin MACRO xAxis, yAxis		;width = 20, height = 15
	LOCAL loop1, loop2, var1, var2, var3, label1, label2, label3, label4
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	.code
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	
	mov ax, xAxis
	add ax, 8
	mov bx, yAxis
	mov cx, 7
	mov dx, 3
	loop1:
		mov var1, ax
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 1110b
		inc bx
		cmp cx, 6
		JL label1
		sub ax, 2
		add dx, 4
		label1:
		cmp cx, 4
		JL label2
		sub ax, 1
		add dx, 2
		label2:
		sub ax, 0
		add dx, 0
	LOOP loop1
	
	mov ax, xAxis
	mov cx, 7
	loop2:
		mov var1, ax
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 1110b
		inc bx
		cmp cx, 2
		JG label3
		add ax, 2
		sub dx, 4
		label3:
		cmp cx, 4
		JG label4
		add ax, 1
		sub dx, 2
		label4:
		sub ax, 0
		sub dx, 0
	LOOP loop2

	mov dx, 3
	mov var1, ax
	mov var2, bx
	mov var3, dx
	drawHorizontalLine var1, var2, var3, 1110b
	mov ax, xAxis
	add ax, 19
	mov bx, yAxis
	add bx, 6
	mov var1, ax
	mov var2, bx
	drawRectangle var1, var2, 1, 4, 1110b
	
	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawMonster MACRO xAxis, yAxis		;width = height = 60
	LOCAL var1, var2, var3, var4
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	var4 WORD ?
	.code
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	
	mov bx, yAxis
	add bx, 15
	mov var2, bx
	drawRectangle xAxis, var2, 60, 30, 1110b	;main body
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	drawRectangle var1, yAxis, 10, 20, 0100b	;left ear
	add ax, 30
	mov var1, ax
	drawRectangle var1, yAxis, 10, 20, 0100b	;right ear
	sub ax, 5
	mov var1, ax
	add bx, 10
	mov var2, bx
	drawRectangle var1, var2, 10, 10, 0100b		;right eye socket
	add bx, 3
	mov var2, bx
	drawRectangle var1, var2, 7, 5, 1100b		;right eye ball
	sub ax, 20
	mov var1, ax
	sub bx, 3
	mov var2, bx
	drawRectangle var1, var2, 10, 10, 0100b		;left eye socket
	add ax, 3
	mov var1, ax
	add bx, 3
	mov var2, bx
	drawRectangle var1, var2, 7, 5, 1100b		;left eye ball
	mov bx, yAxis
	add bx, 45
	mov var2, bx
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	drawRectangle var1, var2, 50, 15, 0100b		;outer mouth
	add ax, 5
	mov var1, ax
	add bx, 5
	mov var2, bx
	drawRectangle var1, var2, 40, 5, 1100b		;inner mouth
	
	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawFire MACRO xAxis, yAxis  ;width = 20 height = 30
 LOCAL var1, var2, loop1, loop2
 PUSH ax
 PUSH bx
 .data
 var1 WORD ?
 var2 WORD ?
 .code
; drawRectangle xAxis, yAxis, 20, 30, 0000b
 mov ax, xAxis
 mov bx, yAxis
 mov var1, ax
 mov var2, bx
 
 ;lower
 add var2, 10
 drawRectangle xAxis, var2, 20, 20, 0100b
 drawHorizontalLine var1, var2, 20, 0000b
 
 ;middle
 sub var2, 5
 add var1, 2
 drawRectangle var1, var2, 16, 20, 1100b 
 drawHorizontalLine var1, var2, 16, 0000b
 
 ;upper
 sub var2, 5
 add var1, 2
 drawRectangle var1, var2, 12, 20, 1110b
 drawHorizontalLine var1, var2, 12, 0000b
 
 POP bx
 POP ax
ENDm

drawTower MACRO xAxis, yAxis		;height = 60, width = 30
	LOCAL var1, var2, var3, var4, loop1, loop2, loop3
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	var4 WORD ?
	.code
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	
	mov ax, xAxis
	add ax, 14
	mov var1, ax
	drawRectangle var1, yAxis, 2, 15, 0100b	;FlagPole
	mov bx, yAxis
	add bx, 3
	add ax, 1
	mov dx, 3
	mov cx, 4
	loop1:
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 0100b ;FlagUp
		inc bx
		add dx, 3
	LOOP loop1
	mov cx, 4
	loop2:
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 0100b ;FlagDown
		inc bx
		sub dx, 3
	LOOP loop2
	
	mov ax, xAxis
	add ax, 14
	mov bx, yAxis
	add bx, 15
	mov dx, 1
	mov cx, 15
	loop3:
		mov var1, ax
		mov var2, bx
		mov var3, dx
		drawHorizontalLine var1, var2, var3, 0100b ;Roof
		sub ax, 1
		add bx, 1
		add dx, 2
	LOOP loop3
	
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	mov bx, yAxis
	add bx, 30
	mov var2, bx
	drawRectangle var1, var2, 20, 30, 0111b	;tower
	drawRectangle var1, var2, 20, 2, 0000b ;roof shadow
	add ax, 5
	mov var1, ax
	add bx, 7
	mov var2, bx
	drawRectangle var1, var2, 10, 10, 1110b ;Window
	drawHorizontalLine var1, var2, 10, 0000b ;window shadow
	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawCastle MACRO xAxis, yAxis		;height = 100, width = 120
	LOCAL var1, var2, var3, var4, loop1, loop2
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD ?
	var4 WORD ?
	.code
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
		
	mov ax, xAxis
	add ax, 8
	mov var1, ax
	drawTower var1, yAxis		;first tower
	add ax, 37
	mov var1, ax
	drawTower var1, yAxis		;second tower
	add ax, 37
	mov var1, ax
	drawTower var1, yAxis		;third tower
	mov bx, yAxis
	add bx, 60
	mov var2, bx
	drawRectangle xAxis, var2, 120, 40, 1000b
	
	mov ax, xAxis
	add ax, 7
	mov bx, yAxis
	add bx, 53
	mov var2, bx
	mov cx, 5
	loop1:
		mov var1, ax
		drawRectangle var1, var2, 10, 6, 0110b
		add ax, 24
	LOOP loop1
	mov ax, xAxis
	mov var1, ax
	add bx, 6
	mov var2, bx
	drawRectangle var1, var2, 120, 3, 0000b

	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawAdvancedPlatform MACRO	; xAxis->0 yAxis->320 (640x15)
	LOCAL var1, var2, var3, loop1, loop2, keepMoving1, keepMoving2
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	.data
	var1 WORD ?
	var2 WORD ?
	var3 WORD 0
	.code			 
	drawRectangle 0, 320, 640, 30, 0100b	;for drawing red background for bricks
	drawRectangle 0, 320, 640, 3, 1100b		;for drawing top line of bricks
	
	mov cx, 2
	mov dx, 326
	loop1:
		PUSH cx
		mov cx, 17
		mov ax, 20
		mov var2, dx
		loop2:
			mov var1, ax
			drawRectangle var1, var2, 20, 3, 1100b		;loops for drawing bricks
			add ax, 37
		LOOP loop2
		POP cx
		add dx, 12
	LOOP loop1
	
	mov cx, 2
	mov dx, 332
	keepMoving1:
		PUSH cx
		mov cx, 17
		mov ax, 10
		mov var2, dx
		keepMoving2:
			mov var1, ax
			drawRectangle var1, var2, 20, 3, 1100b    ;;loops for drawing bricks
			add ax, 37
		LOOP keepMoving2
		POP cx
		add dx, 12
	LOOP keepMoving1	

	POP dx
	POP cx
	POP bx
	POP ax
ENDm

drawMarioBro MACRO xAxis, yAxis	;height = 50, width = 40
	LOCAL var1, var2
	PUSH ax
	
	.data
	var1 WORD ?
	var2 WORD ?
	.code
	;Draw Hat
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	drawRectangle var1, yAxis, 20, 5, 1100b ;Hat Top
	mov ax, yAxis
	add ax, 5
	mov var2, ax	
	drawRectangle var1, var2, 30, 5, 1100b ;Hat Bottom
	
	;draw FACE
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 10
	mov var2, ax	
	drawRectangle var1, var2, 20, 10, 1110b ;Face
	mov ax, xAxis
	add ax, 25
	mov var1, ax
	mov ax, yAxis
	add ax, 10
	mov var2, ax
	drawRectangle var1, var2, 3, 3, 1000b ;Eye
	mov ax, yAxis
	add ax, 17
	mov var2, ax	
	drawRectangle var1, var2, 5, 2, 1000b ;Mouth
	mov ax, xAxis
	add ax, 30
	mov var1, ax
	mov ax, yAxis
	add ax, 13
	mov var2, ax	
	drawRectangle var1, var2, 3, 4, 1110b ;Nose
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 8
	mov var2, ax	
	drawRectangle var1, var2, 5, 5, 0110b ;Top Hair
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	mov ax, yAxis
	add ax, 12
	mov var2, ax	
	drawRectangle var1, var2, 5, 5, 0110b ;Middle Hair
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 15
	mov var2, ax	
	drawRectangle var1, var2, 5, 5, 0110b ;Bottom Hair
	
	;draw BODY
	mov ax, xAxis
	mov var1, ax
	mov ax, yAxis
	add ax, 20
	mov var2, ax
	drawRectangle var1, var2, 40, 20, 1100b ;BODY
	mov ax, xAxis
	mov var1, ax
	mov ax, yAxis
	add ax, 27
	mov var2, ax
	drawRectangle var1, var2, 10, 13, 1110b ;Hand Left
	mov ax, xAxis
	add ax, 30
	mov var1, ax
	mov ax, yAxis
	add ax, 27
	mov var2, ax	
	drawRectangle var1, var2, 10, 13, 1110b ;Hand Right
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 20
	mov var2, ax	
	drawRectangle var1, var2, 5, 7, 1001b ;Upper Left
	mov ax, xAxis
	add ax, 25
	mov var1, ax
	mov ax, yAxis
	add ax, 20
	mov var2, ax	
	drawRectangle var1, var2, 5, 7, 1001b ;Upper Right
	mov ax, xAxis
	add ax, 8
	mov var1, ax
	mov ax, yAxis
	add ax, 27
	mov var2, ax
	drawRectangle var1, var2, 24, 15, 1001b ;Middle
	mov ax, xAxis
	add ax, 10
	mov var1, ax
	mov ax, yAxis
	add ax, 30
	mov var2, ax
	drawRectangle var1, var2, 5, 3, 0110b ;Button Left
	mov ax, xAxis
	add ax, 25
	mov var1, ax
	mov ax, yAxis
	add ax, 30
	mov var2, ax
	drawRectangle var1, var2, 5, 3, 0110b ;Button Right
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	mov ax, yAxis
	add ax, 40
	mov var2, ax	
	drawRectangle var1, var2, 10, 5, 1001b ;Bottom Left
	mov ax, xAxis
	add ax, 25
	mov var1, ax
	mov ax, yAxis
	add ax, 40
	mov var2, ax
	drawRectangle var1, var2, 10, 5, 1001b ;Bottom Right

	;Draw SHOES
	mov ax, xAxis
	add ax, 5
	mov var1, ax
	mov ax, yAxis
	add ax, 45
	mov var2, ax
	drawRectangle var1, var2, 10, 5, 0110b ;Left
	mov ax, xAxis
	add ax, 25
	mov var1, ax
	mov ax, yAxis
	add ax, 45
	mov var2, ax
	drawRectangle var1, var2, 10, 5, 0110b ;Right
	
	POP ax
ENDm
