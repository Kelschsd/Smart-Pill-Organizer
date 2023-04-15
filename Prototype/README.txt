Prototype created as a final project for ECE 303. This project will serve as a starting point for our new project by expanding the code, and 
adding more features through both hardware and software. This project used 2 SN74HC595N shift registers in series to address each LED individually with an address.

Arduino Pin | Register 1 	| Register 2		| LCD Pin	| Buzzer	| Push Button	
------------+---------------+-------------------+-----------+-----------+---------------
	D2		|				|					|	RS		|			|
	D3		|				|					|	E		|			|
	D4		|				|					|	D4		|			|
	D5		|				|					|	D5		|			|
	D6		|				|					|	D6		|			|
	D7		|				|					|	D7		|			|
------------+---------------+-------------------+-----------+-----------+---------------
	D8		|	RCLK (12)	|	RCLK (12)		|			|			|
------------+---------------+-------------------+-----------+-----------+---------------
	D9		|				|					|			|			|	HIGH IN
------------+---------------+-------------------+-----------+-----------+---------------
	D10		|				|	  SER (14)		|			|			|
------------+---------------+-------------------+-----------+-----------+---------------
	D11		|	SER (14)	|					|			|			|	
	D12		| SRCLK	(11)	|	SRCLK (11)		|			|			|
------------+---------------+-------------------+-----------+-----------+---------------
	D13		|				|					|			|POSITIVE IN|
------------+---------------+-------------------+-----------+-----------+---------------
