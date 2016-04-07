# NetTest
NetTest provides network send/recv for test

While developing, I used to need temporary simple network sender or receiver. That was annoying.

Build environment:<br>
Qt 4.8.6 & Qt static 4.8.6 with QtCreator 3.4.2

Function:<br>
TCP client send/recv<br>
UDP send/recv<br>
Multicast send/recv<br>
with custom packets. This style is my usual job.

Rule:<br>
less comments as possible in order to variable & function name show meaning<br>
all custom signals contain 'signal' prefix and slots also 'slot' preifx.<br>
most of all debuging output should show via textbox if possible<br>
