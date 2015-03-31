# arduino-humidity-eco-ventilation
Controllo umidità ecologico con arduino, DHT11 o DHT22, 2 relay e uno shield LCD con pulsanti su pin analogico.

# Impostazione di massima del sistema
Il locale deve essere ermeticamente chiuso
Il sensore di umidità esterno va posto possibilmente in alto e nello stesso punto va installata una ventola con flusso da fuori a dentro.
Il sensore di umidità interno va posto possibilmente in basso e nello stesso punto va installata una ventola con flusso da dentro a fuori.

# Logica
Se l'umidità interna rilevata è maggiore di quella impostata come soglia
e se l'umidità rilevata esternamente è minore di quella interna
allora "asciuga", pompa aria più secca nel locale.

# Dipendendenze
Questo codice utilizza la libreria OSEPP LCDKeypad  http://osepp.com/products/shield-arduino-compatible/16x2-lcd-display-keypad-shield/
